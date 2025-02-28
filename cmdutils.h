/*
 * Various utilities for command line tools
 * copyright (c) 2003 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
 * FOR TEST (MJPG PROCESS)
 */

#ifndef FFMPEG_CMDUTILS_H
#define FFMPEG_CMDUTILS_H

#include <stdint.h>

#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

#ifdef __MINGW32__
#undef main /* We don't want SDL to override our main() */
#endif

/**
 * program name, defined by the program for show_version().
 */
extern const char program_name[];

/**
 * program birth year, defined by the program for show_banner()
 */
extern const int program_birth_year;

extern AVCodecContext *avcodec_opts[AVMEDIA_TYPE_NB];
extern AVFormatContext *avformat_opts;
extern struct SwsContext *sws_opts;
extern AVDictionary *format_opts, *codec_opts;

/**
 * Initialize the cmdutils option system, in particular
 * allocate the *_opts contexts.
 */
void init_opts(void);
/**
 * Uninitialize the cmdutils option system, in particular
 * free the *_opts contexts and their contents.
 */
void uninit_opts(void);

/**
 * Trivial log callback.
 * Only suitable for opt_help and similar since it lacks prefix handling.
 */
void log_callback_help(void* ptr, int level, const char* fmt, va_list vl);

/**
 * Fallback for options that are not explicitly handled, these will be
 * parsed through AVOptions.
 */
int opt_default(const char *opt, const char *arg);

/**
 * Set the libav* libraries log level.
 */
int opt_loglevel(const char *opt, const char *arg);

int opt_codec_debug(const char *opt, const char *arg);

/**
 * Limit the execution time.
 */
int opt_timelimit(const char *opt, const char *arg);

/**
 * Parse a string and return its corresponding value as a double.
 * Exit from the application if the string cannot be correctly
 * parsed or the corresponding value is invalid.
 *
 * @param context the context of the value to be set (e.g. the
 * corresponding commandline option name)
 * @param numstr the string to be parsed
 * @param type the type (OPT_INT64 or OPT_FLOAT) as which the
 * string should be parsed
 * @param min the minimum valid accepted value
 * @param max the maximum valid accepted value
 */
double parse_number_or_die(const char *context, const char *numstr, int type, double min, double max);

/**
 * Parse a string specifying a time and return its corresponding
 * value as a number of microseconds. Exit from the application if
 * the string cannot be correctly parsed.
 *
 * @param context the context of the value to be set (e.g. the
 * corresponding commandline option name)
 * @param timestr the string to be parsed
 * @param is_duration a flag which tells how to interpret timestr, if
 * not zero timestr is interpreted as a duration, otherwise as a
 * date
 *
 * @see parse_date()
 */
int64_t parse_time_or_die(const char *context, const char *timestr, int is_duration);

typedef struct SpecifierOpt {
    char *specifier;    /**< stream/chapter/program/... specifier */
    union {
        uint8_t *str;
        int        i;
        int64_t  i64;
        float      f;
        double   dbl;
    } u;
} SpecifierOpt;

typedef struct {
    const char *name;
    int flags;
#define HAS_ARG    0x0001
#define OPT_BOOL   0x0002
#define OPT_EXPERT 0x0004
#define OPT_STRING 0x0008
#define OPT_VIDEO  0x0010
#define OPT_AUDIO  0x0020
#define OPT_GRAB   0x0040
#define OPT_INT    0x0080
#define OPT_FLOAT  0x0100
#define OPT_SUBTITLE 0x0200
#define OPT_INT64  0x0400
#define OPT_EXIT   0x0800
#define OPT_DATA   0x1000
#define OPT_FUNC2  0x2000
#define OPT_OFFSET 0x4000       /* option is specified as an offset in a passed optctx */
#define OPT_SPEC   0x8000       /* option is to be stored in an array of SpecifierOpt.
                                   Implies OPT_OFFSET. Next element after the offset is
                                   an int containing element count in the array. */
#define OPT_TIME  0x10000
#define OPT_DOUBLE 0x20000
     union {
        void *dst_ptr;
        int (*func_arg)(const char *, const char *);
        int (*func2_arg)(void *, const char *, const char *);
        size_t off;
    } u;
    const char *help;
    const char *argname;
} OptionDef;

void show_help_options(const OptionDef *options, const char *msg, int mask, int value);

/**
 * Show help for all options with given flags in class and all its
 * children.
 */
void show_help_children(const AVClass *class, int flags);

/**
 * Parse the command line arguments.
 *
 * @param optctx an opaque options context
 * @param options Array with the definitions required to interpret every
 * option of the form: -option_name [argument]
 * @param parse_arg_function Name of the function called to process every
 * argument without a leading option name flag. NULL if such arguments do
 * not have to be processed.
 */
void parse_options(void *optctx, int argc, char **argv, const OptionDef *options,
                   void (* parse_arg_function)(void *optctx, const char*));

/**
 * Parse one given option.
 *
 * @return on success 1 if arg was consumed, 0 otherwise; negative number on error
 */
int parse_option(void *optctx, const char *opt, const char *arg, const OptionDef *options);

/**
 * Find the '-loglevel' option in the commandline args and apply it.
 */
void parse_loglevel(int argc, char **argv, const OptionDef *options);

/**
 * Check if the given stream matches a stream specifier.
 *
 * @param s  Corresponding format context.
 * @param st Stream from s to be checked.
 * @param spec A stream specifier of the [v|a|s|d]:[<stream index>] form.
 *
 * @return 1 if the stream matches, 0 if it doesn't, <0 on error
 */
int check_stream_specifier(AVFormatContext *s, AVStream *st, const char *spec);

/**
 * Filter out options for given codec.
 *
 * Create a new options dictionary containing only the options from
 * opts which apply to the codec with ID codec_id.
 *
 * @param s Corresponding format context.
 * @param st A stream from s for which the options should be filtered.
 * @return a pointer to the created dictionary
 */
AVDictionary *filter_codec_opts(AVDictionary *opts, enum CodecID codec_id, AVFormatContext *s, AVStream *st);

/**
 * Setup AVCodecContext options for avformat_find_stream_info().
 *
 * Create an array of dictionaries, one dictionary for each stream
 * contained in s.
 * Each dictionary will contain the options from codec_opts which can
 * be applied to the corresponding stream codec context.
 *
 * @return pointer to the created array of dictionaries, NULL if it
 * cannot be created
 */
AVDictionary **setup_find_stream_info_opts(AVFormatContext *s, AVDictionary *codec_opts);

/**
 * Print an error message to stderr, indicating filename and a human
 * readable description of the error code err.
 *
 * If strerror_r() is not available the use of this function in a
 * multithreaded application may be unsafe.
 *
 * @see av_strerror()
 */
void print_error(const char *filename, int err);

/**
 * Print the program banner to stderr. The banner contents depend on the
 * current version of the repository and of the libav* libraries used by
 * the program.
 */
void show_banner(void);

/**
 * Print the version of the program to stdout. The version message
 * depends on the current versions of the repository and of the libav*
 * libraries.
 * This option processing function does not utilize the arguments.
 */
int opt_version(const char *opt, const char *arg);

/**
 * Print the license of the program to stdout. The license depends on
 * the license of the libraries compiled into the program.
 * This option processing function does not utilize the arguments.
 */
int opt_license(const char *opt, const char *arg);

/**
 * Print a listing containing all the formats supported by the
 * program.
 * This option processing function does not utilize the arguments.
 */
int opt_formats(const char *opt, const char *arg);

/**
 * Print a listing containing all the codecs supported by the
 * program.
 * This option processing function does not utilize the arguments.
 */
int opt_codecs(const char *opt, const char *arg);

/**
 * Print a listing containing all the filters supported by the
 * program.
 * This option processing function does not utilize the arguments.
 */
int opt_filters(const char *opt, const char *arg);

/**
 * Print a listing containing all the bit stream filters supported by the
 * program.
 * This option processing function does not utilize the arguments.
 */
int opt_bsfs(const char *opt, const char *arg);

/**
 * Print a listing containing all the protocols supported by the
 * program.
 * This option processing function does not utilize the arguments.
 */
int opt_protocols(const char *opt, const char *arg);

/**
 * Print a listing containing all the pixel formats supported by the
 * program.
 * This option processing function does not utilize the arguments.
 */
int opt_pix_fmts(const char *opt, const char *arg);

/**
 * Print a listing containing all the sample formats supported by the
 * program.
 */
int show_sample_fmts(const char *opt, const char *arg);

/**
 * Return a positive value if a line read from standard input
 * starts with [yY], otherwise return 0.
 */
int read_yesno(void);

/**
 * Read the file with name filename, and put its content in a newly
 * allocated 0-terminated buffer.
 *
 * @param bufptr location where pointer to buffer is returned
 * @param size   location where size of buffer is returned
 * @return 0 in case of success, a negative value corresponding to an
 * AVERROR error code in case of failure.
 */
int read_file(const char *filename, char **bufptr, size_t *size);

/**
 * Get a file corresponding to a preset file.
 *
 * If is_path is non-zero, look for the file in the path preset_name.
 * Otherwise search for a file named arg.ffpreset in the directories
 * $FFMPEG_DATADIR (if set), $HOME/.ffmpeg, and in the datadir defined
 * at configuration time or in a "ffpresets" folder along the executable
 * on win32, in that order. If no such file is found and
 * codec_name is defined, then search for a file named
 * codec_name-preset_name.ffpreset in the above-mentioned directories.
 *
 * @param filename buffer where the name of the found filename is written
 * @param filename_size size in bytes of the filename buffer
 * @param preset_name name of the preset to search
 * @param is_path tell if preset_name is a filename path
 * @param codec_name name of the codec for which to look for the
 * preset, may be NULL
 */
FILE *get_preset_file(char *filename, size_t filename_size,
                      const char *preset_name, int is_path, const char *codec_name);

/**
 * Do all the necessary cleanup and abort.
 * This function is implemented in the avtools, not cmdutils.
 */
void exit_program(int ret);

/**
 * Realloc array to hold new_size elements of elem_size.
 * Calls exit_program() on failure.
 *
 * @param elem_size size in bytes of each element
 * @param size new element count will be written here
 * @return reallocated array
 */
void *grow_array(void *array, int elem_size, int *size, int new_size);

#endif /* CMDUTILS_H */
