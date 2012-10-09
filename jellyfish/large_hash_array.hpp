/*  This file is part of Jellyfish.

    Jellyfish is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Jellyfish is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Jellyfish.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __JELLYFISH_LARGE_HASH_ARRAY_HPP__
#define __JELLYFISH_LARGE_HASH_ARRAY_HPP__

#include <jellyfish/storage.hpp>
#include <jellyfish/atomic_gcc.hpp>
#include <jellyfish/allocators_mmap.hpp>
#include <jellyfish/offsets_key_value.hpp>
#include <jellyfish/misc.hpp>
#include <jellyfish/err.hpp>
#include <jellyfish/mer_dna.hpp>
#include <jellyfish/rectangular_binary_matrix.hpp>

namespace jellyfish { namespace large_hash {
/* Contains an integer, the reprobe limit. It is capped based on the
 * reprobe strategy to not be bigger than the size of the hash
 * array. Also, the length to encode reprobe limit must not be larger
 * than the length to encode _size.
 */
class reprobe_limit_t {
  uint_t limit;
public:
  reprobe_limit_t(uint_t _limit, size_t *_reprobes, size_t _size) :
    limit(_limit)
  {
    while(_reprobes[limit] >= _size && limit >= 1)
      limit--;
  }
  inline uint_t val() const { return limit; }
};

// Key is any type with the following two methods: get_bits(unsigned
// int start, unsigned int len); and set_bits(unsigned int start,
// unsigned int len, uint64_t bits). These methods get and set the
// bits [start, start + len). Start and len may not be aligned to word
// boundaries. On the other hand, len is guaranteed to be <
// sizeof(uint64_t). I.e. never more than 1 word is fetched or set.
template<typename Key, typename word = uint64_t, typename atomic_t = ::atomic::gcc, typename mem_block_t = ::allocators::mmap>
class array {
  define_error_class(ErrorAllocation);

  static const int  wsize = std::numeric_limits<word>::digits; // Word size in bits
  // Can't be done. Resort to an evil macro!
  //  static const word fmask = std::numeric_limits<word>::max(); // Mask full of ones
#define fmask (std::numeric_limits<word>::max())

  typedef typename Offsets<word>::offset_t offset_t;
  typedef typename offset_t::key key_offsets;
  typedef typename offset_t::val val_offsets;
  uint16_t                 lsize_; // log of size
  size_t                   size_, size_mask_;
  reprobe_limit_t          reprobe_limit_;
  uint16_t                 key_len_; // Length of key in bits
  uint16_t                 raw_key_len_; // Length of key stored raw (i.e. complement of implied length)
  Offsets<word>            offsets_; // key len reduced by size of hash array
  mem_block_t              mem_block_;
  word                    *data_;
  atomic_t                 atomic_;
  size_t                  *reprobes_;
  RectangularBinaryMatrix  hash_matrix_;
  RectangularBinaryMatrix  hash_inverse_matrix_;


public:
  typedef Key               key_type;
  typedef uint64_t          mapped_type;
  typedef std::pair<const   Key, uint64_t> value_type;
  // TODO: iterators
  typedef value_type&       reference;
  typedef const value_type& const_reference;
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;

  array(size_t size, // Size of hash. To be rounded up to a power of 2
        uint16_t key_len, // Size of key in bits
        uint16_t val_len, // Size of val in bits
        uint16_t reprobe_limit, // Maximum reprobe
        size_t* reprobes = jellyfish::quadratic_reprobes) : // Reprobing policy
    lsize_(ceilLog2(size)),
    size_((size_t)1 << lsize_),
    size_mask_(size_ - 1),
    reprobe_limit_(reprobe_limit, reprobes, size_),
    key_len_(key_len),
    raw_key_len_(key_len_ - lsize_),
    offsets_(key_len_ - lsize_ + bitsize(reprobe_limit_.val() + 1), val_len, reprobe_limit_.val() + 1),
    mem_block_(div_ceil(size_, (size_t)offsets_.block_len()) * offsets_.block_word_len() * sizeof(word)),
    data_((word *)mem_block_.get_ptr()),
    reprobes_(reprobes),
    hash_matrix_(lsize_, key_len_),
    hash_inverse_matrix_(hash_matrix_.randomize_pseudo_inverse(random_bits))
  {
    if(!data_)
      eraise(ErrorAllocation) << "Failed to allocate "
                              << (div_ceil(size, (size_t)offsets_.block_len()) * offsets_.block_word_len() * sizeof(word))
                              << " bytes of memory";
  }

  size_t size() const { return size_; }
  size_t lsize() const { return lsize_; }
  size_t size_mask() const { return size_mask_; }
  uint_t key_len() const { return key_len_; }
  uint_t val_len() const { return offsets_.val_len(); }

  uint_t max_reprobe() const { return reprobe_limit_.val(); }
  size_t max_reprobe_offset() const { return reprobes_[reprobe_limit_.val()]; }

  const RectangularBinaryMatrix& matrix() const { return hash_matrix_; }
  const RectangularBinaryMatrix& inverse_matrix() const { return hash_inverse_matrix_; }

  /**
   * Clear hash table. Not thread safe.
   */
  void clear() {
    memset(data_, '\0', mem_block_.get_size());
  }

  /**
   * Use hash values as counters.
   *
   * The matrix multiplication gets only a uint64_t. The lsb of the
   * matrix product, the hsb are assume to be equal to the key itself
   * (the matrix has a partial identity on the first rows).
   */
  inline bool add(const key_type& key, mapped_type val, bool* is_new, size_t* id) {
    uint64_t hash = hash_matrix_.times(key);
    return add_rec(hash & size_mask_, key, val, false, is_new, id);
  }
  inline bool add(const key_type& key, mapped_type val) {
    bool is_new;
    size_t id;
    return add(key, val, &is_new, &id);
  }

  inline bool set(const key_type& key) {
    bool   is_new;
    size_t id;
    return set(key, &is_new, &id);
  }
  bool set(const key_type& key, bool* is_new, size_t* id) {
    word*           w;
    const offset_t* o;

    *id = hash_matrix_.times(key) & size_mask_;
    return claim_key(key, is_new, id, &o, &w);
  }

  inline bool get_val_for_key(const key_type& key, mapped_type* val, bool carry_bit = false) const {
    key_type tmp_key;
    size_t   id;
    return get_val_for_key(key, val, tmp_key, &id, carry_bit);
  }

  bool get_val_for_key(const key_type& key, mapped_type* val, key_type& tmp_key,
                       size_t* id, bool carry_bit = false) const {
    const word*     w;
    const offset_t* o;
    if(!get_key_id(key, id, tmp_key, &w, &o))
      return false;
    *val = get_val_at_id(*id, w, o, true, carry_bit);
    return true;
  }

  inline bool get_key_id(const key_type& key, size_t* id) const {
    key_type        tmp_key;
    const word*     w;
    const offset_t* o;
    return get_key_id(key, id, tmp_key, &w, &o);
  }

  inline bool get_key_id(const key_type& key, size_t* id, key_type& tmp_key) const {
    const word*     w;
    const offset_t* o;
    return get_key_id(key, id, tmp_key, &w, &o);
  }


  bool get_key_id(const key_type& key, size_t* id, key_type& tmp_key, const word** w, const offset_t** o) const {
    const size_t oid = hash_matrix_.times(key) & size_mask_;
    size_t       cid = oid;

    for(uint_t reprobe = 0; reprobe < reprobe_limit_.val(); ++reprobe, cid = (oid + reprobes_[reprobe]) & size_mask_) {
      key_status st = get_key_at_id(cid, tmp_key, w, o);
      switch(st) {
      case EMPTY:
        return false;
      case FILLED:
        if(oid != tmp_key.get_bits(0, lsize_))
          break;
        tmp_key.set_bits(0, lsize_, key.get_bits(0, lsize_));
        if(tmp_key != key)
          break;
        *id = cid;
        return true;
      default:
        break;
      }
    }

    return false;
  }

  //////////////////////////////
  // Iterator
  //////////////////////////////
  class iterator {
  protected:
    const array	*ary_;
    size_t	 start_id_, id_, end_id_;

    key_type    key_;
    mapped_type val_;

  public:
    iterator(const array *ary, size_t start, size_t end) :
      ary_(ary),
      start_id_(start > ary->size() ? ary->size() : start),
      id_(start),
      end_id_(end > ary->size() ? ary->size() : end)
    {}

    uint64_t start() const { return start_id_; }
    uint64_t end() const { return end_id_; }
    const key_type& key() const { return key_; }
    const mapped_type& val() const { return val_; }
    size_t id() const { return id_ - 1; }

    bool next() {
      key_status success = EMPTY;
      while(success != FILLED && id_ < end_id_)
        success = ary_->get_key_val_at_id(id_++, key_, val_);
      if(success == FILLED)
        key_.set_bits(0, ary_->lsize_, ary_->hash_inverse_matrix_.times(key_));

      return success == FILLED;
    }
  };
  friend class iterator;
  iterator iterator_all() const { return iterator(this, 0, size()); }
  iterator iterator_slice(size_t slice_number, size_t number_of_slice) const {
    std::pair<size_t, size_t> res = slice(slice_number, number_of_slice, size());
    return iterator(this, res.first, res.second);
  }

protected:
  // Claim a key with the large bit not set. I.e. first entry for a key.
  //
  // id is input/output. Equal to hash & size_maks on input. Equal to
  // actual id where key was set on output. key is already hash
  // shifted and masked to get higher bits. (>> lsize & key_mask)
  // is_new is set on output to true if key did not exists in hash
  // before. *ao points to the actual offsets object and w to the word
  // holding the value.
  bool claim_key(const key_type& key, bool* is_new, size_t* id, const offset_t** _ao, word** _w) {
    uint_t	    reprobe        = 0;
    const offset_t *o, *lo;
    word	   *w, *kw, nkey;
    bool	    key_claimed    = false;
    size_t	    cid            = *id;

    // Akey contains first word of what to store in the key
    // field. I.e. part of the original key (the rest is encoded in
    // the original position) and the reprobe value to substract from
    // the actual position to get to the original position.
    //
    //    MSB                     LSB
    //   +--------------+-------------+
    //   |  MSB of key  |  reprobe    |
    //   + -------------+-------------+
    //     raw_key_len    reprobe_len
    //
    // Akey is updated at every operation to reflect the current
    // reprobe value. nkey is the temporary word containing the part
    // to be stored in the current word kw (+ some offset).
    word      akey          = 1; // start reprobe value == 0. Store reprobe value + 1
    const int to_copy       = std::min((int)(wsize - offsets_.reprobe_len()), key_len_ - lsize_);
    akey                   |= key.get_bits(lsize_, to_copy) << offsets_.reprobe_len();
    const int abits_copied  = lsize_ + to_copy; // Bits from original key already copied, explicitly or implicitly

    do {
      int bits_copied = abits_copied;

      w  = offsets_.word_offset(cid, &o, &lo, data_);
      kw = w + o->key.woff;

      if(o->key.sb_mask1) { // key split on multiple words
        nkey = akey << o->key.boff;
        nkey |= o->key.sb_mask1;
        nkey &= o->key.mask1;

        key_claimed = set_key(kw, nkey, o->key.mask1, o->key.mask1, is_new);
        if(key_claimed) {
          nkey = akey >> o->key.shift;
          if(o->key.full_words) {
            // Copy full words. First one is special 
            nkey                  |= key.get_bits(bits_copied, o->key.shift - 1) << (wsize - o->key.shift);
            bits_copied           += o->key.shift - 1;
            nkey                  |= o->key.sb_mask1; // Set bit is MSB
            int copied_full_words  = 1;
            key_claimed            = set_key(kw + copied_full_words, nkey, fmask, fmask, is_new);
            // Copy more full words if needed
            while(bits_copied + wsize - 1 <= key_len_ && key_claimed) {
              nkey               = key.get_bits(bits_copied, wsize - 1);
              bits_copied       += wsize - 1;
              nkey              |= o->key.sb_mask1;
              copied_full_words += 1;
              key_claimed        = set_key(kw + copied_full_words, nkey, fmask, fmask, is_new);
            }
            assert(!key_claimed || (bits_copied < key_len_) == (o->key.sb_mask2 != 0));
            if(o->key.sb_mask2 && key_claimed) { // Copy last word
              nkey               = key.get_bits(bits_copied, key_len_ - bits_copied);
              nkey              |= o->key.sb_mask2;
              copied_full_words += 1;
              key_claimed        = set_key(kw + copied_full_words, nkey, o->key.mask2, o->key.mask2, is_new);
            }
          } else if(o->key.sb_mask2) { // if bits_copied + wsize - 1 < key_len
            // Copy last word, no full words copied
            nkey        |= key.get_bits(bits_copied, key_len_ - bits_copied) << (wsize - o->key.shift);
            nkey        |= o->key.sb_mask2;
            nkey        &= o->key.mask2;
            key_claimed  = set_key(kw + 1, nkey, o->key.mask2, o->key.mask2, is_new);
          }
        } // if(key_claimed)
      } else { // key on one word
        nkey = akey << o->key.boff;
        nkey &= o->key.mask1;
        key_claimed = set_key(kw, nkey, o->key.mask1, o->key.mask1, is_new);
      }
      if(!key_claimed) { // reprobe
        if(++reprobe > reprobe_limit_.val())
          return false;
        cid = (*id + reprobes_[reprobe]) & size_mask_;
        akey = (akey & ~offsets_.reprobe_mask()) | (reprobe + 1); // TODO: msb_hash_key | ((reprobe + 1) << key_off);
      }
    } while(!key_claimed);

    *id  = cid;
    *_w  = w;
    *_ao = o;
    return true;
  }

  // Claim large key. Enter an entry for a key when it is not the
  // first entry. Only encode the number of reprobe hops back to the
  // first entry of the key in the hash table. It is simpler as can
  // takes less than one word in length.
  bool claim_large_key(size_t* id, const offset_t** _ao, word** _w) {
    uint_t          reprobe     = 0;
    size_t          cid         = *id;
    const offset_t *o, *lo;
    word           *w, *kw, nkey;
    bool            key_claimed = false;

    do {
      w = offsets_.word_offset(cid, &o, &lo, data_);
      kw = w + lo->key.woff;

      if(lo->key.sb_mask1) { // key split on multiple words
        nkey = (reprobe << lo->key.boff) | lo->key.sb_mask1 | lo->key.lb_mask;
        nkey &= lo->key.mask1;

        // Use o->key.mask1 and not lo->key.mask1 as the first one is
        // guaranteed to be bigger. The key needs to be free on its
        // longer mask to claim it!
        key_claimed = set_key(kw, nkey, o->key.mask1, lo->key.mask1);
        if(key_claimed) {
          nkey         = (reprobe >> lo->key.shift) | lo->key.sb_mask2;
          nkey        &= lo->key.mask2;
          key_claimed  = set_key(kw + 1, nkey, o->key.full_words ? fmask : o->key.mask2, lo->key.mask2);
        }
      } else { // key on 1 word
        nkey  = (reprobe << lo->key.boff) | lo->key.lb_mask;
        nkey &= lo->key.mask1;
        key_claimed = set_key(kw, nkey, o->key.mask1, lo->key.mask1);
      }
      if(!key_claimed) { //reprobe
        if(++reprobe > reprobe_limit_.val())
          return false;
        cid  = (*id + reprobes_[reprobe]) & size_mask_;
      }
    } while(!key_claimed);

    *id  = cid;
    *_w  = w;
    *_ao = lo;
    return true;
  }

  // Add val to key. id is the starting place (result of hash
  // computation). eid is set to the effective place in the
  // array. large is set to true is setting a large key (upon
  // recurrence if there is a carry).
  bool add_rec(size_t id, const key_type& key, word val, bool large, bool* is_new, size_t* eid) {
    const offset_t	*ao;
    word		*w;

    bool claimed = false;
    if(large)
      claimed = claim_large_key(&id, &ao, &w);
    else
      claimed = claim_key(key, is_new, &id, &ao, &w);
    if(!claimed)
      return false;
    *eid = id;

    // Increment value
    word *vw = w + ao->val.woff;
    word cary = add_val(vw, val, ao->val.boff, ao->val.mask1);
    cary >>= ao->val.shift;
    if(cary && ao->val.mask2) { // value split on two words
      cary = add_val(vw + 1, cary, 0, ao->val.mask2);
      cary >>= ao->val.cshift;
    }
    if(cary) {
      id = (id + reprobes_[0]) & size_mask_;
      size_t ignore_eid;
      bool   ignore_is_new;
      if(add_rec(id, key, cary, true, &ignore_is_new, &ignore_eid))
        return true;

      // Adding failed, table is full. Need to back-track and
      // substract val.
      cary = add_val(vw, ((word)1 << offsets_.val_len()) - val,
                     ao->val.boff, ao->val.mask1);
      cary >>= ao->val.shift;
      if(cary && ao->val.mask2) {
        // Can I ignore the cary here? Table is known to be full, so
        // not much of a choice. But does it leave the table in a
        // consistent state?
        add_val(vw + 1, cary, 0, ao->val.mask2);
      }
      return false;
    }
    return true;
  }

  // Atomic methods to set the key. Attempt to set nkey in word w. All
  // bits matching free_mask must be unset and the bits matching
  // equal_mask must be equal for a success in setting the key. Set
  // is_new to true if the spot was previously empty. Otherwise, if
  // is_new is false but true is returned, the key was already present
  // at that spot.
  inline bool set_key(word *w, word nkey, word free_mask, word equal_mask, bool *is_new) {
    word ow = *w, nw, okey;

    okey = ow & free_mask;
    while(okey == 0) { // large bit not set && key is free
      nw = atomic_.cas(w, ow, ow | nkey);
      if(nw == ow) {
        *is_new = true;
        return true;
      }
      ow = nw;
      okey = ow & free_mask;
    }
    *is_new = false;
    return (ow & equal_mask) == nkey;
  }

  inline bool set_key(word *w, word nkey, word free_mask, word equal_mask) {
    bool is_new;
    return set_key(w, nkey, free_mask, equal_mask, &is_new);
  }

  // Add val the value in word w, with shift and mask giving the
  // particular part of the word in which the value is stored. The
  // return value is the carry.
  inline word add_val(word *w, word val, uint_t shift, word mask) {
    word now = *w, ow, nw, nval;

    do {
      ow = now;
      nval = ((ow & mask) >> shift) + val;
      nw = (ow & ~mask) | ((nval << shift) & mask);
      now = atomic_.cas(w, ow, nw);
    } while(now != ow);

    return nval & (~(mask >> shift));
  }


  enum key_status { FILLED, EMPTY, LBSET};
  // Return the key and value at position id. If the slot at id is
  // empty or has the large bit set, returns false. Otherwise, returns
  // the key and the value is the sum of all the entries in the hash
  // table for that key. I.e., the table is search forward for entries
  // with large bit set pointing back to the key at id, and all those
  // values are summed up.
  key_status get_key_val_at_id(size_t id, key_type& key, word& val, const bool carry_bit = false) const {
    const word*     w;
    const offset_t* o;

    key_status st = get_key_at_id(id, key, &w, &o);
    if(st != FILLED)
       return st;

    val = get_val_at_id(id, w, o, true, carry_bit);

    return FILLED;
  }

  key_status get_key_at_id(size_t id, key_type& key, const word** w_, const offset_t** o_) const {
    const offset_t *o, *lo;
    const word*     w        = offsets_.word_offset(id, &o, &lo, data_);
    const word*     kvw      = w + o->key.woff;
    word            key_word = *kvw;
    word            kreprobe = 0;

    const key_offsets& key_o = o->key;
    if(key_word & key_o.lb_mask)
      return LBSET;
    int bits_copied = lsize_;
    if(key_o.sb_mask1) {
      if((key_word & key_o.sb_mask1) == 0)
        return EMPTY;
      kreprobe = (key_word & key_o.mask1 & ~key_o.sb_mask1) >> key_o.boff;
      if(key_o.full_words) {
        // Copy full words. First one is special
        key_word = *(kvw + 1);
        if(offsets_.reprobe_len() < key_o.shift) {
          key.set_bits(bits_copied, key_o.shift - offsets_.reprobe_len(), kreprobe >> offsets_.reprobe_len());
          bits_copied += key_o.shift - offsets_.reprobe_len();
          kreprobe    &= offsets_.reprobe_mask();
          key.set_bits(bits_copied, wsize - 1, key_word & ~key_o.sb_mask1);
          bits_copied += wsize - 1;
        } else {
          int reprobe_left  = offsets_.reprobe_len() - key_o.shift;
          kreprobe         |= (key_word & (((word)1 << reprobe_left) - 1)) << key_o.shift;
          key.set_bits(bits_copied, wsize - 1 - reprobe_left, (key_word & ~key_o.sb_mask1) >> reprobe_left);
          bits_copied += wsize - 1 - reprobe_left;
        }
        int word_copied = 2;
        while(bits_copied + wsize - 1 <= key_len_) {
          key.set_bits(bits_copied, wsize - 1, *(kvw + word_copied++) & (fmask >> 1));
          bits_copied += wsize - 1;
        }
        if(key_o.sb_mask2)
          key.set_bits(bits_copied, key_len_ - bits_copied, *(kvw + word_copied) & key_o.mask2 & ~key_o.sb_mask2);
      } else if(key_o.sb_mask2) { // if(bits_copied + wsize - 1 < key_len
        // Two words but no full words
        key_word = *(kvw + 1) & key_o.mask2 & ~key_o.sb_mask2;
        if(offsets_.reprobe_len() < key_o.shift) {
          key.set_bits(bits_copied, key_o.shift - offsets_.reprobe_len(), kreprobe >> offsets_.reprobe_len());
          bits_copied += key_o.shift - offsets_.reprobe_len();
          kreprobe    &= offsets_.reprobe_mask();
          key.set_bits(bits_copied, key_len_ - bits_copied, key_word);
        } else {
          int reprobe_left  = offsets_.reprobe_len() - key_o.shift;
          kreprobe         |= (key_word & (((word)1 << reprobe_left) - 1)) << key_o.shift;
          key.set_bits(bits_copied, key_len_ - bits_copied, key_word >> reprobe_left);
        }
      }
    } else { // if(key_o.sb_mask1
      // Everything in 1 word
      key_word = (key_word & key_o.mask1) >> key_o.boff;
      if(key_word == 0)
        return EMPTY;
      kreprobe = key_word & offsets_.reprobe_mask();
      key.set_bits(bits_copied, raw_key_len_, key_word >> offsets_.reprobe_len());
    }
    // Compute missing oid so that the original key can be computed
    // back through the inverse matrix.
    size_t oid = id; // Original id
    if(kreprobe > 1)
      oid -= reprobes_[kreprobe - 1];
    oid &= size_mask_;
    key.set_bits(0, lsize_, oid);

    *w_ = w;
    *o_ = o;

    return FILLED;
  }

  word get_val_at_id(const size_t id, const word* w, const offset_t* o, const bool reprobe = true,
                     const bool carry_bit = false) const {
    word            val = 0;
    // First part of value
    const word* kvw = w + o->val.woff;
    val = ((*kvw) & o->val.mask1) >> o->val.boff;
    if(o->val.mask2)
      val |= ((*(kvw+1)) & o->val.mask2) << o->val.shift;

    // Do we want to get the large value
    bool do_reprobe = reprobe;
    if(carry_bit && do_reprobe) {
      do_reprobe   = do_reprobe && (val & 0x1);
      val        >>= 1;
    }
    if(!do_reprobe)
      return val;

    return resolve_val_rec((id + reprobes_[0]) & size_mask_, val, carry_bit);
  }

  word resolve_val_rec(const size_t id, word val, const bool carry_bit, const uint_t overflows = 0) const {
    uint_t          reprobe = 0;
    size_t          cid     = id;

    while(reprobe <= reprobe_limit_.val()) {
      const offset_t     *o, *lo;
      const word*         w    = offsets_.word_offset(cid, &o, &lo, data_);
      const word*         kw   = w + o->key.woff;
      word                nkey = *kw;
      const key_offsets&  lkey = lo->key;

      if(nkey & lkey.lb_mask) {
        // If the large bit is set, the size of the key (reprobe_len)
        // is guaranteed to have a length of at most 1 word.
        if(lkey.sb_mask1) {
          nkey  = (nkey & lkey.mask1 & ~lkey.sb_mask1) >> lkey.boff;
          nkey |= ((*(kw+1)) & lkey.mask2 & ~lkey.sb_mask2) << lkey.shift;
        } else {
          nkey = (nkey & lkey.mask1) >> lkey.boff;
        }
        if(nkey == reprobe) {
          const val_offsets& lval = lo->val;
          const word*        vw   = w + lval.woff;
          word               nval = ((*vw) & lval.mask1) >> lval.boff;
          if(lval.mask2)
            nval |= ((*(vw+1)) & lval.mask2) << lval.shift;

          bool do_reprobe = true;
          if(carry_bit) {
            do_reprobe   = nval & 0x1;
            nval       >>= 1;
          }

          nval <<= offsets_.val_len();
          nval <<= offsets_.lval_len() * overflows;
          val   += nval;

          if(!do_reprobe)
            return val;

          return resolve_val_rec((cid + reprobes_[0]) & size_mask_, val, carry_bit, overflows + 1);
        }
      } else if((nkey & o->key.mask1) == 0) {
        break;
      }

      cid  = (id + reprobes_[++reprobe]) & size_mask_;
    }

    return val;
  }

};

} } // namespace jellyfish { namespace large_hash_array

#endif /* __JELLYFISH_LARGE_HASH_ARRAY_HPP__ */