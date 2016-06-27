/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <libgen.h>
#include <errno.h>
#include <stdint.h>

#define NOOP "0xDEADBEEF"

#define returniffail(x) do { typeof(x) __x = (x); if (__x < 0) return (__x); } while(0)

typedef enum {
        little_endian,
        big_endian,
        none_endian
} endian_t;

/* Endianness */
static union {
	char s[4];
	uint32_t u;
} endian_data;


/* Platform NOOPS */
typedef enum {
        intel_x86 = 0,
        arm_a32,
        arm_t32_16bit,
        arm_t32_32bit,
        arm_a64_64bit,
} PType;

typedef struct {
        PType type;
        char *type_str;
        char *desc;
        char *noop;
} plat_t;

plat_t supported_platforms[] = {
        {intel_x86, "intel_x86", "Intel X86", ""},
        {arm_a32, "arm_a32", "ARM A32", ""},
};

#define PLATFORM_COUNT (sizeof(supported_platforms) / sizeof(supported_platforms[0]))
#define IS_BIG_ENDIAN (endian_data.u == (uint32_t)0x01020304)

static void print_help(const char *name)
{
        printf("\n");
        printf("USAGE:\n");
        printf("%s [OPTION]\n", basename((char *)name));
        printf("Create a binary!\n");
        printf("    -e, --endian      endianess[Either 'little' or 'big']\n");
        printf("    -s, --size        size of the binary\n");
        printf("    -n, --noop        fill the binary with noops\n");
        printf("    -f, --file        name of the output binary file\n");
        printf("    -l, --list-plat   list all supported platforms\n");
        printf("    -p, --platform    Generate binary for a specified platform\n");
        printf("    -h, --help        print this help\n");
        printf("\n");
}

/* Command line arguments */
struct arguments {
        endian_t endian;
        size_t size;
        char *fname;
        int noop;
        FILE *fp;
};

static void list_platforms(void)
{
        unsigned int i;

        printf("     List of supported platform types\n");
        printf("   ====================================\n");
        for (i = 0; i < PLATFORM_COUNT; i++) {
                printf("%12s  =>  %20s\n", supported_platforms[i].type_str,
                       supported_platforms[i].desc);
        }
}

static int parse_arguments(int argc, char **argv, struct arguments *args)
{
        static const struct option long_options[] = {
                { "endian", required_argument, NULL, 'e' },
                { "size", required_argument, NULL, 's' },
                { "file", required_argument, NULL, 'f' },
                { "noop", no_argument, NULL, 'n' },
                { "list-plat", no_argument, NULL, 'l' },
                { "platform", optional_argument, NULL, 'p' },
                { "help", no_argument, NULL, 'h' },
                {0, 0, 0, 0}
        };

        memset(args, 0, sizeof(*args));
        args->endian = none_endian;
        args->size = 0;
        args->noop = 0;
        args->fname = NULL;

        while (1) {
                int c;

                c = getopt_long(argc, argv, "e:s:f:p:lhn?", long_options, &optind);

                if (c == -1)
                        break;

                switch (c) {
                case 'e':
                        if (strncmp(optarg, "big", 3) == 0)
                                args->endian = big_endian;
                        else if (strncmp(optarg, "little", 6) == 0)
                                args->endian = little_endian;
                        else
                                args->endian = none_endian;
                        break;
                case 's':
                        args->size = atoi(optarg);
                        break;
                case 'f':
                        args->fname = strdup(optarg);
                        break;
                case 'n':
                        args->noop = 1;
                        break;
                case 'l':
                        list_platforms();
                        exit(EXIT_SUCCESS);
                case '?':
                case 'h':
                        print_help(argv[0]);
                        exit(EXIT_SUCCESS);
                }
        }

        if (args->endian == none_endian ||
            args->size == 0 ||
            args->fname == NULL) {
                fprintf(stderr, "Missing arguments!\n");
                print_help(argv[0]);
                return -EINVAL;
        }

        return 0;
}

static void mk_bin(struct arguments *args)
{
        size_t siz = 0;
        unsigned char *buf;

        buf = malloc(args->size);
        if (buf == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
        }

        memset(buf, 0, args->size);

        while (siz+4 <= args->size) {
                uint32_t d = strtoul(NOOP, 0, 16);
                unsigned char *bin = (unsigned char *)&d;
                if (args->endian == little_endian) {
                        buf[siz++] = bin[0];
                        buf[siz++] = bin[1];
                        buf[siz++] = bin[2];
                        buf[siz++] = bin[3];
                } else if (args->endian == big_endian) {
                        buf[siz++] = bin[3];
                        buf[siz++] = bin[2];
                        buf[siz++] = bin[1];
                        buf[siz++] = bin[0];
                }
        }

        fwrite(buf, args->size, 1, args->fp);
        fflush(args->fp);
        free(buf);
        buf = NULL;
}

int main(int argc, char **argv)
{
        struct arguments args;

        returniffail(parse_arguments(argc, argv, &args));

        (void)memcpy(endian_data.s, "\01\02\03\04", 4);

        /* Round it up to the nearest 4 byte multiple */
        args.size = (args.size | 0x03) + 1;

        args.fp = fopen(args.fname, "w");
        if (args.fp == NULL) {
                perror("fopen:");
                exit(EXIT_FAILURE);
        }

        mk_bin(&args);

        fclose(args.fp);

        exit(EXIT_SUCCESS);
}
