
#include "../include/io.h"

static unsigned short int OYAMO_VERBOSE_FLAG = 0;

char *oyamo_read_file(const char *file_path)
{
    // https://github.com/sebbekarlsson/hello/blob/master/src/io.c

    char *buffer = 0;
    long length;

    FILE *f = fopen(file_path, "rb");

    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);

        buffer = (char *)calloc(length, length);

        if (buffer)
        {
            fread(buffer, 1, length, f);
        }

        fclose(f);
        return (char *)buffer;
    }

    return (char *)NULL;
}

void oyamo_log(char *file, char *function_name, char *message)
{
    if (OYAMO_VERBOSE_FLAG & OYAMO_VERBOSE_LOG)
    {
        printf(
            "\n***OYAMO-Log***\n"
            "Message: %s\n"
            "Function: %s\n"
            "In: %s\n"
            "***OYAMO-Log***\n",
            message, function_name, file);
    }
}

static void oyamo_print(char *message, oyamo_print_status_flag flag, char *color)
{
    printf(COLOR_RST);
    printf("%s", color);

    switch (flag)
    {
    case OYAMO_PRINT_SUCCESS:
        if (OYAMO_VERBOSE_FLAG & OYAMO_VERBOSE_SUCCESS)
        {
            printf("[+] %s\n", message);
        }
        break;
    case OYAMO_PRINT_ERROR:
        if (OYAMO_VERBOSE_FLAG & OYAMO_VERBOSE_ERROR)
        {
            printf("[-] %s\n", message);
        }
        break;
    case OYAMO_PRINT_WARNING:
        if (OYAMO_VERBOSE_FLAG & OYAMO_VERBOSE_WARNING)
        {
            printf("[!] %s\n", message);
        }
        break;
    case OYAMO_PRINT_INFO:
        if (OYAMO_VERBOSE_FLAG & OYAMO_VERBOSE_INFO)
        {
            printf("[*] %s\n", message);
        }
        break;
    case OYAMO_PRINT_WHITE:
        if (OYAMO_VERBOSE_FLAG & OYAMO_VERBOSE_WHITE)
        {
            printf("[>] %s\n", message);
        }
        break;
    }

    printf(COLOR_RST);
}

void oyamo_print_success(char *message, ...)
{
    char new_message[256];
    va_list args;
    va_start(args, message);
    vsnprintf(new_message, 255, message, args);
    va_end(args);

    oyamo_print(new_message, OYAMO_PRINT_SUCCESS, COLOR_GRN);
}

void oyamo_print_error(char *message, ...)
{
    char new_message[256];
    va_list args;
    va_start(args, message);
    vsnprintf(new_message, 255, message, args);
    va_end(args);

    oyamo_print(new_message, OYAMO_PRINT_ERROR, COLOR_RED);
}

void oyamo_print_warning(char *message, ...)
{
    char new_message[256];
    va_list args;
    va_start(args, message);
    vsnprintf(new_message, 255, message, args);
    va_end(args);

    oyamo_print(new_message, OYAMO_PRINT_WARNING, COLOR_YEL);
}

void oyamo_print_info(char *message, ...)
{
    char new_message[256];
    va_list args;
    va_start(args, message);
    vsnprintf(new_message, 255, message, args);
    va_end(args);

    oyamo_print(new_message, OYAMO_PRINT_INFO, COLOR_BLU);
}

void oyamo_print_white(char *message, ...)
{
    char new_message[256];
    va_list args;
    va_start(args, message);
    vsnprintf(new_message, 255, message, args);
    va_end(args);

    oyamo_print(new_message, OYAMO_PRINT_WHITE, COLOR_WHT);
}

void oyamo_set_verbose(unsigned short int flag)
{
    OYAMO_VERBOSE_FLAG = flag;
}