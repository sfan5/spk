#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <spk/spk.h>

typedef struct ll_node {
    const char *v;
    struct ll_node *next;
} ll_node;

void ll_add(ll_node *array, const char *v);

void print_usage();
void recursive_add_to_array(ll_node *array, const char *path);

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
                int i, len;
                ll_node *in, *cur, *tmp;
                char **arr;

                in = malloc(sizeof(ll_node));
                if(in == NULL)
                {
                    fprintf(stderr, "Not enough memory available\n");
                    return EXIT_FAILURE;
                }
                in->next = NULL;
                in->v = (const char*) NULL;

                for(i = archiven + 1; i < argc; i++)
                {
                    if(argv[i][strlen(argv[i]) - 1] == '/') argv[i][strlen(argv[i]) - 1] = 0;
                    recursive_add_to_array(in, argv[i]);
                }

                cur = in;
                len = 0;
                while((cur = cur->next) != NULL)
                    len++;
                len++;

                arr = malloc(len * sizeof(char*));
                if(arr == NULL)
                {
                    fprintf(stderr, "Not enough memory available\n");
                    return EXIT_FAILURE;
                }
                cur = in;
                i = 0;
                while(cur != NULL)
                {
                    arr[i] = (char*) cur->v;
                    i++;
                    tmp = cur;
                    cur = cur->next;
                    free(tmp);
                }

                co = create_spk_ex(archivep, len, arr, verbose, no_gid_uid, no_mode);
                free(arr);
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
    return (co == SPK_E_OK)?EXIT_SUCCESS:EXIT_FAILURE;
}

void ll_add(ll_node *array, const char *v)
{
    ll_node *c, *n;
    c = array;
    while(1)
    {
        if(c->v == NULL)
        {
            c->v = v;
            return;
        }
        if(c->next == NULL)
            break;
        c = c->next;
    }
    n = malloc(sizeof(ll_node));
    if(n == NULL)
        fprintf(stderr, "Not enough memory available!\n");
    n->v = v;
    n->next = NULL;
    c->next = n;
}

void recursive_add_to_array(ll_node *array, const char *path)
{
    DIR *d;
    struct dirent *entry;
    struct stat s;
#ifdef _WIN32
    stat(path, &s);
#else
    lstat(path, &s);
#endif
    if(path[strlen(path) - 1] == '.') return;
    if(s.st_mode & S_IFDIR)
    { // Directory
        ll_add(array, path);
        d = opendir(path);
        if(d == NULL) { fprintf(stderr, "Warning: Could not open %s\n", path); return; }
        while((entry = readdir(d)) != NULL)
        {
            if(entry->d_name[0] == '.') continue;
            char *newpath = malloc(255);
            if(newpath == NULL) { fprintf(stderr, "Not enough memory available!\n"); continue; }
            memset(newpath, 0, 255);
            strncat(newpath, path, 254);
            strncat(newpath, "/", 254 - strlen(newpath));
            strncat(newpath, entry->d_name, 254 - strlen(newpath));
            recursive_add_to_array(array, newpath);
        }
        closedir(d);
    }
    else if(s.st_mode & S_IFREG) // File
        ll_add(array, path);
#ifndef _WIN32
    else if(s.st_mode & S_IFLNK) // Symlink
        ll_add(array, path);
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
