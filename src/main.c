#include <stdlib.h>
#include <stdio.h>

static FILE* out_stream;
static char* program;
static char* target;
static int format;

static inline void __attribute((noreturn)) __usage_error(int error_code, char* restrict message)
{
    fprintf(stderr,
        "Error %d: %s.\n" // error_code, message
        "\n"
        "Usage:\n"
        "  %s <path/to/executable> [ optional switches ]\n" // program
        "\n"
        "positional arguments:\n"
        "  path/to/executable\n"
        "\n"
        "optional switches:\n"
        "  -c,-C\t\toutput as c-style char array\n"
        "  -s,-S\t\toutput as double quoted string\n"
        "example :\n"
        "  %s path/to/executable\n" // program
        ,error_code, message, program, program
    );

    exit(error_code);
}

static inline void verify_requirements(void)
{
    if (system("type objdump > /dev/null"))
    {
        __usage_error(-1, "'objdump' not found");
    }
    if (system("type grep > /dev/null"))
    {
        __usage_error(-1, "'grep' not found");
    }
    if (system("type cut > /dev/null"))
    {
        __usage_error(-1, "'cut' not found");
    }
}

static void handle_command_line(int argc, char** argv)
{
    if (argc < 2)
    {
        __usage_error(1, "Missing positional arguments");
    }

    for (int i = 2; i < argc; i++)
    {
        switch (argv[i][0])
        {
            case '-':
            {
                switch (argv[i][1])
                {
                    case 'C':
                    case 'c':
                    {
                        format = 'c';
                        break;
                    }
                    case 'S':
                    case 's':
                    {
                        format = 's';
                        break;
                    }
                    case 'O':
                    case 'o':
                    {
                        if (i+1 >= argc)
                        {
                            __usage_error(2, "No file specified");
                        }
                        if (!(out_stream = fopen(argv[i+1], "w")))
                        {
                            char buffer[256] = { 0 };
                            snprintf(buffer, sizeof(buffer), "Failed to open: %s", argv[i+1]);
                            __usage_error(4, "Failed to open file");
                        }
                        break;
                    }
                    default:
                    {
                        __usage_error(3, "Unknown option supplied");
                    }
                }
            }
        }
    }

    if (!format)
    {
        format = 'c';
    }

    if (!out_stream)
    {
        out_stream = stdout;
    }
}

static inline void verify_target_exists(void)
{
    FILE* fp;
    if (!(fp = fopen(target, "rb")))
    {
        char buffer[256] = { 0 };
        snprintf(buffer, sizeof(buffer), "Failed to find executable '%s'", target);
        __usage_error(6, buffer);
    }
    fclose(fp);
}

int main(int argc, char** argv)
{
    FILE*   fp = NULL;
    char    disassembler[256] = { 0 };

    program = argv[0];
    handle_command_line(argc, argv);

    target = argv[1];
    verify_requirements();
    verify_target_exists();

    snprintf(disassembler, sizeof(disassembler), "objdump -d --section=.text %s | grep '^ ' | cut -f2", target);
    if (!(fp = popen(disassembler, "r")))
    {
        fprintf(stderr, "Failed to disassemble %s\n", target);
        return 5;
    }

    if (format == 'c')
    {
        unsigned size = 0;
        for (int c; (c = fgetc(fp)) != EOF;)
        {
            if (c != ' ' && c != '\n' && c != '\t')
            {
                ++size;
            }
        }
        pclose(fp);

        if (!(fp = popen(disassembler, "r")))
        {
            fprintf(stderr, "Failed to disassemble %s\n", target);
            return 5;
        }

        fprintf(out_stream, "unsigned char shellcode[%u] = {\n    ", size);
        unsigned position = 0;
        for (int c = 0; (c = fgetc(fp)) != EOF;)
        {
            if (c != ' ' && c != '\n' && c != '\t')
            {
                ++position;
                fprintf(out_stream, "0x%2X%c%s", c, (position < size ? ',' : '\0'), (position % 10 == 0  && position != 0 ? "\n    " : " "));
            }
        }
        fprintf(out_stream, "\n}\n");
    }
    else if (format == 's')
    {
        fputc('"', out_stream);
        for (int c = 0; (c = fgetc(fp)) != EOF;)
        {
            if (c != ' ' && c != '\n' && c != '\t')
            {
                fprintf(out_stream, "\\x%2X", c);
            }
        }
        fputc('"', out_stream);
        pclose(fp);
    }

    fclose(out_stream);
    return 0;
}
