/** @file mer_counter_cmdline.hpp
 *  @brief The header file for the command line option parser
 *  generated by GNU Gengetopt version 2.22.4
 *  http://www.gnu.org/software/gengetopt.
 *  DO NOT modify this file, since it can be overwritten
 *  @author GNU Gengetopt by Lorenzo Bettini */

#ifndef MER_COUNTER_CMDLINE_H
#define MER_COUNTER_CMDLINE_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for FILE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef MER_COUNTER_CMDLINE_PACKAGE
/** @brief the program name (used for printing errors) */
#define MER_COUNTER_CMDLINE_PACKAGE "jellyfish count"
#endif

#ifndef MER_COUNTER_CMDLINE_PACKAGE_NAME
/** @brief the complete program name (used for help and version) */
#define MER_COUNTER_CMDLINE_PACKAGE_NAME "jellyfish count"
#endif

#ifndef MER_COUNTER_CMDLINE_VERSION
/** @brief the program version */
#define MER_COUNTER_CMDLINE_VERSION VERSION
#endif

/** @brief Where the command line options are stored */
struct mer_counter_args
{
  const char *help_help; /**< @brief Print help and exit help description.  */
  const char *full_help_help; /**< @brief Print help, including hidden options, and exit help description.  */
  const char *version_help; /**< @brief Print version and exit help description.  */
  int mer_len_arg;	/**< @brief Length of mer.  */
  char * mer_len_orig;	/**< @brief Length of mer original value given at command line.  */
  const char *mer_len_help; /**< @brief Length of mer help description.  */
  long size_arg;	/**< @brief Hash size.  */
  char * size_orig;	/**< @brief Hash size original value given at command line.  */
  const char *size_help; /**< @brief Hash size help description.  */
  int threads_arg;	/**< @brief Number of threads (default='1').  */
  char * threads_orig;	/**< @brief Number of threads original value given at command line.  */
  const char *threads_help; /**< @brief Number of threads help description.  */
  char * output_arg;	/**< @brief Output prefix (default='mer_counts').  */
  char * output_orig;	/**< @brief Output prefix original value given at command line.  */
  const char *output_help; /**< @brief Output prefix help description.  */
  int counter_len_arg;	/**< @brief Length of counting field (default='7').  */
  char * counter_len_orig;	/**< @brief Length of counting field original value given at command line.  */
  const char *counter_len_help; /**< @brief Length of counting field help description.  */
  int out_counter_len_arg;	/**< @brief Length of counter field in output (default='4').  */
  char * out_counter_len_orig;	/**< @brief Length of counter field in output original value given at command line.  */
  const char *out_counter_len_help; /**< @brief Length of counter field in output help description.  */
  int both_strands_flag;	/**< @brief Count both strand, canonical representation (default=off).  */
  const char *both_strands_help; /**< @brief Count both strand, canonical representation help description.  */
  int reprobes_arg;	/**< @brief Maximum number of reprobes (default='62').  */
  char * reprobes_orig;	/**< @brief Maximum number of reprobes original value given at command line.  */
  const char *reprobes_help; /**< @brief Maximum number of reprobes help description.  */
  int raw_flag;	/**< @brief Write raw database (default=off).  */
  const char *raw_help; /**< @brief Write raw database help description.  */
  int quake_flag;	/**< @brief Quake compatibility mode (default=off).  */
  const char *quake_help; /**< @brief Quake compatibility mode help description.  */
  int quality_start_arg;	/**< @brief Starting ASCII for quality values (default='64').  */
  char * quality_start_orig;	/**< @brief Starting ASCII for quality values original value given at command line.  */
  const char *quality_start_help; /**< @brief Starting ASCII for quality values help description.  */
  char * matrix_arg;	/**< @brief Hash function binary matrix.  */
  char * matrix_orig;	/**< @brief Hash function binary matrix original value given at command line.  */
  const char *matrix_help; /**< @brief Hash function binary matrix help description.  */
  char * timing_arg;	/**< @brief Print timing information.  */
  char * timing_orig;	/**< @brief Print timing information original value given at command line.  */
  const char *timing_help; /**< @brief Print timing information help description.  */
  int no_write_flag;	/**< @brief Don't write database (default=off).  */
  const char *no_write_help; /**< @brief Don't write database help description.  */
  int measure_flag;	/**< @brief Write usage statistics (default=off).  */
  const char *measure_help; /**< @brief Write usage statistics help description.  */
  long buffers_arg;	/**< @brief Number of buffers per thread.  */
  char * buffers_orig;	/**< @brief Number of buffers per thread original value given at command line.  */
  const char *buffers_help; /**< @brief Number of buffers per thread help description.  */
  long buffer_size_arg;	/**< @brief Size of buffers (default='8192').  */
  char * buffer_size_orig;	/**< @brief Size of buffers original value given at command line.  */
  const char *buffer_size_help; /**< @brief Size of buffers help description.  */
  long out_buffer_size_arg;	/**< @brief Size of output buffer per thread (default='20000000').  */
  char * out_buffer_size_orig;	/**< @brief Size of output buffer per thread original value given at command line.  */
  const char *out_buffer_size_help; /**< @brief Size of output buffer per thread help description.  */
  
  unsigned int help_given ;	/**< @brief Whether help was given.  */
  unsigned int full_help_given ;	/**< @brief Whether full-help was given.  */
  unsigned int version_given ;	/**< @brief Whether version was given.  */
  unsigned int mer_len_given ;	/**< @brief Whether mer-len was given.  */
  unsigned int size_given ;	/**< @brief Whether size was given.  */
  unsigned int threads_given ;	/**< @brief Whether threads was given.  */
  unsigned int output_given ;	/**< @brief Whether output was given.  */
  unsigned int counter_len_given ;	/**< @brief Whether counter-len was given.  */
  unsigned int out_counter_len_given ;	/**< @brief Whether out-counter-len was given.  */
  unsigned int both_strands_given ;	/**< @brief Whether both-strands was given.  */
  unsigned int reprobes_given ;	/**< @brief Whether reprobes was given.  */
  unsigned int raw_given ;	/**< @brief Whether raw was given.  */
  unsigned int quake_given ;	/**< @brief Whether quake was given.  */
  unsigned int quality_start_given ;	/**< @brief Whether quality-start was given.  */
  unsigned int matrix_given ;	/**< @brief Whether matrix was given.  */
  unsigned int timing_given ;	/**< @brief Whether timing was given.  */
  unsigned int no_write_given ;	/**< @brief Whether no-write was given.  */
  unsigned int measure_given ;	/**< @brief Whether measure was given.  */
  unsigned int buffers_given ;	/**< @brief Whether buffers was given.  */
  unsigned int buffer_size_given ;	/**< @brief Whether buffer-size was given.  */
  unsigned int out_buffer_size_given ;	/**< @brief Whether out-buffer-size was given.  */

  char **inputs ; /**< @brief unamed options (options without names) */
  unsigned inputs_num ; /**< @brief unamed options number */
} ;

/** @brief The additional parameters to pass to parser functions */
struct mer_counter_cmdline_params
{
  int override; /**< @brief whether to override possibly already present options (default 0) */
  int initialize; /**< @brief whether to initialize the option structure mer_counter_args (default 1) */
  int check_required; /**< @brief whether to check that all required options were provided (default 1) */
  int check_ambiguity; /**< @brief whether to check for options already specified in the option structure mer_counter_args (default 0) */
  int print_errors; /**< @brief whether getopt_long should print an error message for a bad option (default 1) */
} ;

/** @brief the purpose string of the program */
extern const char *mer_counter_args_purpose;
/** @brief the usage string of the program */
extern const char *mer_counter_args_usage;
/** @brief all the lines making the help output */
extern const char *mer_counter_args_help[];
/** @brief all the lines making the full help output (including hidden options) */
extern const char *mer_counter_args_full_help[];

/**
 * The command line parser
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int mer_counter_cmdline (int argc, char **argv,
  struct mer_counter_args *args_info);

/**
 * The command line parser (version with additional parameters - deprecated)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use mer_counter_cmdline_ext() instead
 */
int mer_counter_cmdline2 (int argc, char **argv,
  struct mer_counter_args *args_info,
  int override, int initialize, int check_required);

/**
 * The command line parser (version with additional parameters)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int mer_counter_cmdline_ext (int argc, char **argv,
  struct mer_counter_args *args_info,
  struct mer_counter_cmdline_params *params);

/**
 * Save the contents of the option struct into an already open FILE stream.
 * @param outfile the stream where to dump options
 * @param args_info the option struct to dump
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int mer_counter_cmdline_dump(FILE *outfile,
  struct mer_counter_args *args_info);

/**
 * Save the contents of the option struct into a (text) file.
 * This file can be read by the config file parser (if generated by gengetopt)
 * @param filename the file where to save
 * @param args_info the option struct to save
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int mer_counter_cmdline_file_save(const char *filename,
  struct mer_counter_args *args_info);

/**
 * Print the help
 */
void mer_counter_cmdline_print_help(void);
/**
 * Print the full help (including hidden options)
 */
void mer_counter_cmdline_print_full_help(void);
/**
 * Print the version
 */
void mer_counter_cmdline_print_version(void);

/**
 * Initializes all the fields a mer_counter_cmdline_params structure 
 * to their default values
 * @param params the structure to initialize
 */
void mer_counter_cmdline_params_init(struct mer_counter_cmdline_params *params);

/**
 * Allocates dynamically a mer_counter_cmdline_params structure and initializes
 * all its fields to their default values
 * @return the created and initialized mer_counter_cmdline_params structure
 */
struct mer_counter_cmdline_params *mer_counter_cmdline_params_create(void);

/**
 * Initializes the passed mer_counter_args structure's fields
 * (also set default values for options that have a default)
 * @param args_info the structure to initialize
 */
void mer_counter_cmdline_init (struct mer_counter_args *args_info);
/**
 * Deallocates the string fields of the mer_counter_args structure
 * (but does not deallocate the structure itself)
 * @param args_info the structure to deallocate
 */
void mer_counter_cmdline_free (struct mer_counter_args *args_info);

/**
 * Checks that all the required options were specified
 * @param args_info the structure to check
 * @param prog_name the name of the program that will be used to print
 *   possible errors
 * @return
 */
int mer_counter_cmdline_required (struct mer_counter_args *args_info,
  const char *prog_name);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* MER_COUNTER_CMDLINE_H */
