/*
  File autogenerated by gengetopt version 2.22.4
  generated with the following command:
  /genome4/raid/gus/Source/gengetopt-2.22.4/src/gengetopt --show-required --default-option -c cc -H hpp -F rhisto_main_cmdline -f rhisto_main_cmdline -a rhisto_main_args --unamed-opts=database.jf

  The developers of gengetopt consider the fixed text that goes in all
  gengetopt output files to be in the public domain:
  we make no copyright claims on it.
*/

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FIX_UNUSED
#define FIX_UNUSED(X) (void) (X) /* avoid warnings for unused params */
#endif

#include <getopt.h>

#include "rhisto_main_cmdline.hpp"

const char *rhisto_main_args_purpose = "Create an histogram of k-mer occurences";

const char *rhisto_main_args_usage = "Usage: jellyfish rhisto [OPTIONS]... [database.jf]...";

const char *rhisto_main_args_description = "";

const char *rhisto_main_args_help[] = {
  "      --help            Print help and exit",
  "  -V, --version         Print version and exit",
  "  -l, --low=LONG        Low count value  (default=`1')",
  "  -h, --high=LONG       High count value  (default=`10000')",
  "  -i, --increment=LONG  Increment  (default=`1')",
  "  -t, --threads=INT     Nuber of threads  (default=`1')",
    0
};

typedef enum {ARG_NO
  , ARG_INT
  , ARG_LONG
} rhisto_main_cmdline_arg_type;

static
void clear_given (struct rhisto_main_args *args_info);
static
void clear_args (struct rhisto_main_args *args_info);

static int
rhisto_main_cmdline_internal (int argc, char **argv, struct rhisto_main_args *args_info,
                        struct rhisto_main_cmdline_params *params, const char *additional_error);


static char *
gengetopt_strdup (const char *s);

static
void clear_given (struct rhisto_main_args *args_info)
{
  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->low_given = 0 ;
  args_info->high_given = 0 ;
  args_info->increment_given = 0 ;
  args_info->threads_given = 0 ;
}

static
void clear_args (struct rhisto_main_args *args_info)
{
  FIX_UNUSED (args_info);
  args_info->low_arg = 1;
  args_info->low_orig = NULL;
  args_info->high_arg = 10000;
  args_info->high_orig = NULL;
  args_info->increment_arg = 1;
  args_info->increment_orig = NULL;
  args_info->threads_arg = 1;
  args_info->threads_orig = NULL;
  
}

static
void init_args_info(struct rhisto_main_args *args_info)
{


  args_info->help_help = rhisto_main_args_help[0] ;
  args_info->version_help = rhisto_main_args_help[1] ;
  args_info->low_help = rhisto_main_args_help[2] ;
  args_info->high_help = rhisto_main_args_help[3] ;
  args_info->increment_help = rhisto_main_args_help[4] ;
  args_info->threads_help = rhisto_main_args_help[5] ;
  
}

void
rhisto_main_cmdline_print_version (void)
{
  printf ("%s %s\n",
     (strlen(RHISTO_MAIN_CMDLINE_PACKAGE_NAME) ? RHISTO_MAIN_CMDLINE_PACKAGE_NAME : RHISTO_MAIN_CMDLINE_PACKAGE),
     RHISTO_MAIN_CMDLINE_VERSION);
}

static void print_help_common(void) {
  rhisto_main_cmdline_print_version ();

  if (strlen(rhisto_main_args_purpose) > 0)
    printf("\n%s\n", rhisto_main_args_purpose);

  if (strlen(rhisto_main_args_usage) > 0)
    printf("\n%s\n", rhisto_main_args_usage);

  printf("\n");

  if (strlen(rhisto_main_args_description) > 0)
    printf("%s\n\n", rhisto_main_args_description);
}

void
rhisto_main_cmdline_print_help (void)
{
  int i = 0;
  print_help_common();
  while (rhisto_main_args_help[i])
    printf("%s\n", rhisto_main_args_help[i++]);
}

void
rhisto_main_cmdline_init (struct rhisto_main_args *args_info)
{
  clear_given (args_info);
  clear_args (args_info);
  init_args_info (args_info);

  args_info->inputs = 0;
  args_info->inputs_num = 0;
}

void
rhisto_main_cmdline_params_init(struct rhisto_main_cmdline_params *params)
{
  if (params)
    { 
      params->override = 0;
      params->initialize = 1;
      params->check_required = 1;
      params->check_ambiguity = 0;
      params->print_errors = 1;
    }
}

struct rhisto_main_cmdline_params *
rhisto_main_cmdline_params_create(void)
{
  struct rhisto_main_cmdline_params *params = 
    (struct rhisto_main_cmdline_params *)malloc(sizeof(struct rhisto_main_cmdline_params));
  rhisto_main_cmdline_params_init(params);  
  return params;
}

static void
free_string_field (char **s)
{
  if (*s)
    {
      free (*s);
      *s = 0;
    }
}


static void
rhisto_main_cmdline_release (struct rhisto_main_args *args_info)
{
  unsigned int i;
  free_string_field (&(args_info->low_orig));
  free_string_field (&(args_info->high_orig));
  free_string_field (&(args_info->increment_orig));
  free_string_field (&(args_info->threads_orig));
  
  
  for (i = 0; i < args_info->inputs_num; ++i)
    free (args_info->inputs [i]);

  if (args_info->inputs_num)
    free (args_info->inputs);

  clear_given (args_info);
}


static void
write_into_file(FILE *outfile, const char *opt, const char *arg, const char *values[])
{
  FIX_UNUSED (values);
  if (arg) {
    fprintf(outfile, "%s=\"%s\"\n", opt, arg);
  } else {
    fprintf(outfile, "%s\n", opt);
  }
}


int
rhisto_main_cmdline_dump(FILE *outfile, struct rhisto_main_args *args_info)
{
  int i = 0;

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot dump options to stream\n", RHISTO_MAIN_CMDLINE_PACKAGE);
      return EXIT_FAILURE;
    }

  if (args_info->help_given)
    write_into_file(outfile, "help", 0, 0 );
  if (args_info->version_given)
    write_into_file(outfile, "version", 0, 0 );
  if (args_info->low_given)
    write_into_file(outfile, "low", args_info->low_orig, 0);
  if (args_info->high_given)
    write_into_file(outfile, "high", args_info->high_orig, 0);
  if (args_info->increment_given)
    write_into_file(outfile, "increment", args_info->increment_orig, 0);
  if (args_info->threads_given)
    write_into_file(outfile, "threads", args_info->threads_orig, 0);
  

  i = EXIT_SUCCESS;
  return i;
}

int
rhisto_main_cmdline_file_save(const char *filename, struct rhisto_main_args *args_info)
{
  FILE *outfile;
  int i = 0;

  outfile = fopen(filename, "w");

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot open file for writing: %s\n", RHISTO_MAIN_CMDLINE_PACKAGE, filename);
      return EXIT_FAILURE;
    }

  i = rhisto_main_cmdline_dump(outfile, args_info);
  fclose (outfile);

  return i;
}

void
rhisto_main_cmdline_free (struct rhisto_main_args *args_info)
{
  rhisto_main_cmdline_release (args_info);
}

/** @brief replacement of strdup, which is not standard */
char *
gengetopt_strdup (const char *s)
{
  char *result = 0;
  if (!s)
    return result;

  result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
}

int
rhisto_main_cmdline (int argc, char **argv, struct rhisto_main_args *args_info)
{
  return rhisto_main_cmdline2 (argc, argv, args_info, 0, 1, 1);
}

int
rhisto_main_cmdline_ext (int argc, char **argv, struct rhisto_main_args *args_info,
                   struct rhisto_main_cmdline_params *params)
{
  int result;
  result = rhisto_main_cmdline_internal (argc, argv, args_info, params, 0);

  if (result == EXIT_FAILURE)
    {
      rhisto_main_cmdline_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
rhisto_main_cmdline2 (int argc, char **argv, struct rhisto_main_args *args_info, int override, int initialize, int check_required)
{
  int result;
  struct rhisto_main_cmdline_params params;
  
  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;

  result = rhisto_main_cmdline_internal (argc, argv, args_info, &params, 0);

  if (result == EXIT_FAILURE)
    {
      rhisto_main_cmdline_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
rhisto_main_cmdline_required (struct rhisto_main_args *args_info, const char *prog_name)
{
  FIX_UNUSED (args_info);
  FIX_UNUSED (prog_name);
  return EXIT_SUCCESS;
}


static char *package_name = 0;

/**
 * @brief updates an option
 * @param field the generic pointer to the field to update
 * @param orig_field the pointer to the orig field
 * @param field_given the pointer to the number of occurrence of this option
 * @param prev_given the pointer to the number of occurrence already seen
 * @param value the argument for this option (if null no arg was specified)
 * @param possible_values the possible values for this option (if specified)
 * @param default_value the default value (in case the option only accepts fixed values)
 * @param arg_type the type of this option
 * @param check_ambiguity @see rhisto_main_cmdline_params.check_ambiguity
 * @param override @see rhisto_main_cmdline_params.override
 * @param no_free whether to free a possible previous value
 * @param multiple_option whether this is a multiple option
 * @param long_opt the corresponding long option
 * @param short_opt the corresponding short option (or '-' if none)
 * @param additional_error possible further error specification
 */
static
int update_arg(void *field, char **orig_field,
               unsigned int *field_given, unsigned int *prev_given, 
               char *value, const char *possible_values[],
               const char *default_value,
               rhisto_main_cmdline_arg_type arg_type,
               int check_ambiguity, int override,
               int no_free, int multiple_option,
               const char *long_opt, char short_opt,
               const char *additional_error)
{
  char *stop_char = 0;
  const char *val = value;
  int found;
  FIX_UNUSED (field);

  stop_char = 0;
  found = 0;

  if (!multiple_option && prev_given && (*prev_given || (check_ambiguity && *field_given)))
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: `--%s' (`-%c') option given more than once%s\n", 
               package_name, long_opt, short_opt,
               (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: `--%s' option given more than once%s\n", 
               package_name, long_opt,
               (additional_error ? additional_error : ""));
      return 1; /* failure */
    }

  FIX_UNUSED (default_value);
    
  if (field_given && *field_given && ! override)
    return 0;
  if (prev_given)
    (*prev_given)++;
  if (field_given)
    (*field_given)++;
  if (possible_values)
    val = possible_values[found];

  switch(arg_type) {
  case ARG_INT:
    if (val) *((int *)field) = strtol (val, &stop_char, 0);
    break;
  case ARG_LONG:
    if (val) *((long *)field) = (long)strtol (val, &stop_char, 0);
    break;
  default:
    break;
  };

  /* check numeric conversion */
  switch(arg_type) {
  case ARG_INT:
  case ARG_LONG:
    if (val && !(stop_char && *stop_char == '\0')) {
      fprintf(stderr, "%s: invalid numeric value: %s\n", package_name, val);
      return 1; /* failure */
    }
    break;
  default:
    ;
  };

  /* store the original value */
  switch(arg_type) {
  case ARG_NO:
    break;
  default:
    if (value && orig_field) {
      if (no_free) {
        *orig_field = value;
      } else {
        if (*orig_field)
          free (*orig_field); /* free previous string */
        *orig_field = gengetopt_strdup (value);
      }
    }
  };

  return 0; /* OK */
}


int
rhisto_main_cmdline_internal (
  int argc, char **argv, struct rhisto_main_args *args_info,
                        struct rhisto_main_cmdline_params *params, const char *additional_error)
{
  int c;	/* Character of the parsed option.  */

  int error = 0;
  struct rhisto_main_args local_args_info;
  
  int override;
  int initialize;
  int check_required;
  int check_ambiguity;
  
  package_name = argv[0];
  
  override = params->override;
  initialize = params->initialize;
  check_required = params->check_required;
  check_ambiguity = params->check_ambiguity;

  if (initialize)
    rhisto_main_cmdline_init (args_info);

  rhisto_main_cmdline_init (&local_args_info);

  optarg = 0;
  optind = 0;
  opterr = params->print_errors;
  optopt = '?';

  while (1)
    {
      int option_index = 0;

      static struct option long_options[] = {
        { "help",	0, NULL, 0 },
        { "version",	0, NULL, 'V' },
        { "low",	1, NULL, 'l' },
        { "high",	1, NULL, 'h' },
        { "increment",	1, NULL, 'i' },
        { "threads",	1, NULL, 't' },
        { 0,  0, 0, 0 }
      };

      c = getopt_long (argc, argv, "Vl:h:i:t:", long_options, &option_index);

      if (c == -1) break;	/* Exit from `while (1)' loop.  */

      switch (c)
        {
        case 'V':	/* Print version and exit.  */
          rhisto_main_cmdline_print_version ();
          rhisto_main_cmdline_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'l':	/* Low count value.  */
        
        
          if (update_arg( (void *)&(args_info->low_arg), 
               &(args_info->low_orig), &(args_info->low_given),
              &(local_args_info.low_given), optarg, 0, "1", ARG_LONG,
              check_ambiguity, override, 0, 0,
              "low", 'l',
              additional_error))
            goto failure;
        
          break;
        case 'h':	/* High count value.  */
        
        
          if (update_arg( (void *)&(args_info->high_arg), 
               &(args_info->high_orig), &(args_info->high_given),
              &(local_args_info.high_given), optarg, 0, "10000", ARG_LONG,
              check_ambiguity, override, 0, 0,
              "high", 'h',
              additional_error))
            goto failure;
        
          break;
        case 'i':	/* Increment.  */
        
        
          if (update_arg( (void *)&(args_info->increment_arg), 
               &(args_info->increment_orig), &(args_info->increment_given),
              &(local_args_info.increment_given), optarg, 0, "1", ARG_LONG,
              check_ambiguity, override, 0, 0,
              "increment", 'i',
              additional_error))
            goto failure;
        
          break;
        case 't':	/* Nuber of threads.  */
        
        
          if (update_arg( (void *)&(args_info->threads_arg), 
               &(args_info->threads_orig), &(args_info->threads_given),
              &(local_args_info.threads_given), optarg, 0, "1", ARG_INT,
              check_ambiguity, override, 0, 0,
              "threads", 't',
              additional_error))
            goto failure;
        
          break;

        case 0:	/* Long option with no short option */
          if (strcmp (long_options[option_index].name, "help") == 0) {
            rhisto_main_cmdline_print_help ();
            rhisto_main_cmdline_free (&local_args_info);
            exit (EXIT_SUCCESS);
          }

        case '?':	/* Invalid option.  */
          /* `getopt_long' already printed an error message.  */
          goto failure;

        default:	/* bug: option not considered.  */
          fprintf (stderr, "%s: option unknown: %c%s\n", RHISTO_MAIN_CMDLINE_PACKAGE, c, (additional_error ? additional_error : ""));
          abort ();
        } /* switch */
    } /* while */




  rhisto_main_cmdline_release (&local_args_info);

  if ( error )
    return (EXIT_FAILURE);

  if (optind < argc)
    {
      int i = 0 ;
      int found_prog_name = 0;
      /* whether program name, i.e., argv[0], is in the remaining args
         (this may happen with some implementations of getopt,
          but surely not with the one included by gengetopt) */

      i = optind;
      while (i < argc)
        if (argv[i++] == argv[0]) {
          found_prog_name = 1;
          break;
        }
      i = 0;

      args_info->inputs_num = argc - optind - found_prog_name;
      args_info->inputs =
        (char **)(malloc ((args_info->inputs_num)*sizeof(char *))) ;
      while (optind < argc)
        if (argv[optind++] != argv[0])
          args_info->inputs[ i++ ] = gengetopt_strdup (argv[optind-1]) ;
    }

  return 0;

failure:
  
  rhisto_main_cmdline_release (&local_args_info);
  return (EXIT_FAILURE);
}
