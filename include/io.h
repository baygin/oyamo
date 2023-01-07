// https://github.com/sebbekarlsson/hello/blob/master/src/include/io.h

#ifndef OYAMO_IO_H
#define OYAMO_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#define oyamo_clear_terminal() printf("\033[H\033[J")

#define COLOR_RED "\x1B[31m"
#define COLOR_GRN "\x1B[32m"
#define COLOR_YEL "\x1B[33m"
#define COLOR_BLU "\x1B[34m"
#define COLOR_MAG "\x1B[35m"
#define COLOR_CYN "\x1B[36m"
#define COLOR_WHT "\x1B[37m"
#define COLOR_RST "\x1B[0m"

#define OYAMO_VERBOSE_LOG 0b00000001
#define OYAMO_VERBOSE_SUCCESS 0b00000010
#define OYAMO_VERBOSE_ERROR 0b00000100
#define OYAMO_VERBOSE_WARNING 0b00001000
#define OYAMO_VERBOSE_INFO 0b00010000
#define OYAMO_VERBOSE_WHITE 0b00100000
#define OYAMO_VERBOSE_ALL (                       \
    OYAMO_VERBOSE_LOG | OYAMO_VERBOSE_SUCCESS |   \
    OYAMO_VERBOSE_ERROR | OYAMO_VERBOSE_WARNING | \
    OYAMO_VERBOSE_INFO | OYAMO_VERBOSE_WHITE)

typedef enum
{
    OYAMO_PRINT_SUCCESS,
    OYAMO_PRINT_ERROR,
    OYAMO_PRINT_WARNING,
    OYAMO_PRINT_INFO,
    OYAMO_PRINT_WHITE
} oyamo_print_status_flag;

char *oyamo_read_file(const char *file_path);
void oyamo_set_verbose(unsigned short int flag);
void oyamo_log(char *file, char *function_name, char *message);
static void oyamo_print(char *message, oyamo_print_status_flag flag, char *color);
void oyamo_print_success(char *message, ...);
void oyamo_print_error(char *message, ...);
void oyamo_print_warning(char *message, ...);
void oyamo_print_info(char *message, ...);
void oyamo_print_white(char *message, ...);

#endif // !OYAMO_IO_H