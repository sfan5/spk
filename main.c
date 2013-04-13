#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <spk/spk.h>

void print_usage();
void recursive_add_to_array(char *array[], int *inlen, char *path, short verbose);

int main(int argc, char *argv[])
{
    short co = 1;
    if(argc < 3)
    {
        print_usage();
        return EXIT_SUCCESS;
    }
    else
    {
        short verbose = (strchr(argv[1], 'v') != NULL);
        switch(argv[1][0])
        {
            case 'c':
                if(argc < 4)
                {
                    print_usage();
                    return EXIT_SUCCESS;
                }
                int i, inlen = 0;
                char *in[4096];
                for(i = 3; i < argc; i++)
                {
                    recursive_add_to_array(in, &inlen, argv[i], verbose);
                }
                co = create_spk(argv[2], inlen, in);
                if(co != SPK_E_OK) printf("%s\n", strerror_spk(co));
                break;
            case 'x':
                co = extract_spk_ex(argv[2], ".", verbose);
                if(co != SPK_E_OK) printf("%s\n", strerror_spk(co));
                break;
            default:
                printf("Unknown operation %c\n", argv[1][0]);
                break;
        }
    }
    return (co == 0)?EXIT_SUCCESS:EXIT_FAILURE;
}

void recursive_add_to_array(char *array[], int *inlen, char *path, short verbose)
{
    DIR *d;
    struct dirent *entry;
    struct stat s;
    stat(path, &s);
    if(path[strlen(path) - 1] == '.') return;
    if(s.st_mode & S_IFDIR)
    { // Directory
        if(verbose) printf("%s\n", path);
        array[*inlen] = path;
        *inlen += 1;
        d = opendir(path);
        if(d == NULL) { fprintf(stderr, "Warning: Could not open %s\n", path); return; }
        while((entry = readdir(d)) != NULL)
        {
            char *newpath = malloc(255);
            memset(newpath, 0, 255);
            strcat(newpath, path);
            strcat(newpath, "/");
            strcat(newpath, entry->d_name);
            recursive_add_to_array(array, inlen, newpath, verbose);
        }
        closedir(d);
    }
    else if(s.st_mode & S_IFREG)
    { // File
        if(verbose) printf("%s\n", path);
        array[*inlen] = path;
        *inlen += 1;
    }
    else
    {
        fprintf(stderr, "Warning: cannot archive %s\n", path);
    }
}

void print_usage()
{
#ifdef __clang__
    const char *cv = "Clang";
#elif defined(__GNUC__)
    const char *cv = "gcc";
#elif defined(_MSC_VER)
    const char *cv = "MS Visual Studio";
#else
    const char *cv = "unknown compiler";
#endif
    printf("spk version 0.1 (compiled %s %s with %s %s)\n", __DATE__, __TIME__, cv, __VERSION__); 
    printf("Usage: spk [cx][v] <archive> [files] ...\n");
    printf("spk packs files and directories in a developer-friendly format.\n\n");
    printf("Examples:\n");
    printf("  spk cv example.spk foo bar    # Create example.spk with the files foo and bar\n");
    printf("  spk x  archive.spk            # Extract archive.spk\n");
}
