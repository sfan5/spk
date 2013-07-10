#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <spk/spk.h>

void print_usage();
void recursive_add_to_array(char *array[], int *inlen, int *inmaxlen, char *path, bool verbose);

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
        bool verbose = (strchr(argv[1], 'v') != NULL);
        bool no_gid_uid = false, no_mode = false;
        int i, archiven;
        char *archivep;
        for(i = 2; i < argc; i++)
        {
            if(argv[i][0] != '-')
            {
                archivep = argv[i];
                archiven = i;
                break;
            }

            if(!strcmp(argv[i], "--no-uid-gid")) { no_gid_uid = true; continue; }
            if(!strcmp(argv[i], "--no-mode")) { no_mode = true; continue; }
            fprintf(stderr, "Unknown argument %s\n", argv[i]);
        }
        if(archivep == NULL) { fprintf(stderr, "No archive specified\n"); return EXIT_FAILURE; }
        switch(argv[1][0])
        {
            case 'c':
                if(argc < 4)
                {
                    print_usage();
                    return EXIT_SUCCESS;
                }
                int i, inlen = 0, maxlen = 64;
                char *in[64];
                for(i = archiven + 1; i < argc; i++)
                {
                    recursive_add_to_array(in, &inlen, &maxlen, argv[i], verbose);
                }
                co = create_spk_ex(archivep, inlen, in, no_gid_uid, no_mode);
                if(co != SPK_E_OK) fprintf(stderr, "%s\n", strerror_spk(co));
                break;
            case 'x':
                co = extract_spk_ex(archivep, ".", verbose, no_gid_uid, no_mode);
                if(co != SPK_E_OK) fprintf(stderr, "%s\n", strerror_spk(co));
                break;
            default:
                fprintf(stderr, "Unknown operation %c\n", argv[1][0]);
                break;
        }
    }
    return (co == 0)?EXIT_SUCCESS:EXIT_FAILURE;
}

void recursive_add_to_array(char *array[], int *inlen, int *inmaxlen, char *path, bool verbose)
{
    DIR *d;
    struct dirent *entry;
    struct stat s;
    lstat(path, &s);
    if(path[strlen(path) - 1] == '.') return;
    if(path[strlen(path) - 1] == '/') path[strlen(path) - 1] = 0;
    if(inlen == inmaxlen)
    {
        *inmaxlen += 10;
        array = realloc(array, *inmaxlen);
    }
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
            recursive_add_to_array(array, inlen, inmaxlen, newpath, verbose);
        }
        closedir(d);
    }
    else if(s.st_mode & S_IFREG)
    { // File
        if(verbose) printf("%s\n", path);
        array[*inlen] = path;
        *inlen += 1;
    }
#ifndef _WIN32
    else if(s.st_mode & S_IFLNK)
    { // Symlink
        if(verbose) printf("%s\n", path);
        array[*inlen] = path;
        *inlen += 1;
    }
#endif
    else
    {
        fprintf(stderr, "Warning: cannot archive %s\n", path);
    }
}

void print_usage()
{
#if defined(__clang__)
    const char *cv = "Clang";
#elif defined(__GNUC__)
    const char *cv = "gcc";
#elif defined(_MSC_VER)
    const char *cv = "MS Visual Studio";
#else
    const char *cv = "unknown compiler";
#endif
    printf("spk version " SPK_VERSION_S " (compiled %s %s with %s %s)\n", __DATE__, __TIME__, cv, __VERSION__); 
    printf("Usage: spk (cx)[v] [options] <archive> [files] ...\n");
    printf("spk packs files and directories in a developer-friendly format.\n\n");
    printf("Valid options:\n");
    printf("  --no-uid-gid          Doesn't save User ID and Group ID to archive\n");
    printf("  --no-mode             Doesn't save Access mode to archive\n");
    printf("\n");
    printf("Examples:\n");
    printf("  spk cv example.spk foo bar        # Create example.spk with the files foo and bar\n");
    printf("  spk x --no-uid-gid archive.spk    # Extract archive.spk without setting UID and GID\n");
}
