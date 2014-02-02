#include <spk/spk.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

#ifndef _WIN32
char *readlink_malloc(const char *filename)
{
    int size = 10;
    char *buffer = NULL;

    while(1)
    {
        buffer = (char*) realloc(buffer, size);
        int nchars = readlink(filename, buffer, size);
        if(nchars < 0)
        {
            free(buffer);
            return NULL;
        }
        if(nchars < size)
            return buffer;
        size += 10;
    }
}
#endif

short create_spk(char *outfile, int inlen, char *in[])
{
    return create_spk_ex(outfile, inlen, in, false, false, false);
}

short create_spk_ex(char *outfile, int inlen, char *in[], bool verbose, bool no_gid_uid, bool no_mode)
{
    FILE *of, *f;
    of = fopen(outfile, "wb");
    if(of == NULL) return SPK_E_FAILEDOPEN;
    const char* temp1 = SPK_MAGIC;
    struct stat s;
    int i, j;
    spk_fileheader_t *fh = malloc(sizeof(spk_fileheader_t));
    if(fh == NULL) return SPK_E_UNKNOWN;

    fwrite(temp1, SPK_MAGIC_LEN, 1, of);
    for(i = 0; i < inlen; i++)
    {
#ifdef _WIN32
        stat(in[i], &s);
#else
        lstat(in[i], &s);
#endif
        if(no_mode) fh->mode = 0xffff; else fh->mode = (uint16_t) s.st_mode;
#ifndef _WIN32
        if(no_gid_uid)
        {
            fh->uid = 0xffff;
            fh->gid = 0xffff;
        }
        else
        {
            fh->uid = (uint16_t) s.st_uid;
            fh->gid = (uint16_t) s.st_gid;
        }
#else
        fh->uid = 0xffff;
        fh->gid = 0xffff;
#endif
        printf("%s\n", in[i]);
        if(s.st_mode & S_IFDIR)
        { // Directory
           memset(fh->name, 0, 255);
           strncpy(fh->name, in[i], 254);
           fh->type = SPK_T_DIR;
           fh->length = 0;
           fwrite(fh, sizeof(spk_fileheader_t), 1, of);
        }
#ifndef _WIN32
        else if(S_ISLNK(s.st_mode))
        { // Symlink
            memset(fh->name, 0, 255);
            strncpy(fh->name, in[i], 254);
            fh->type = SPK_T_SYMLINK;
            char *dest = readlink_malloc(in[i]);
            if(dest == NULL) return SPK_E_FAILEDOPEN;
            fh->length = (uint32_t) strlen(dest);
            fwrite(fh, sizeof(spk_fileheader_t), 1, of);
            fwrite(dest, strlen(dest), 1, of);
            free(dest);
        }
#endif
        else if(s.st_mode & S_IFREG)
        { // File
            memset(fh->name, 0, 255);
            strncpy(fh->name, in[i], 254);
            fh->type = SPK_T_FILE;
            f = fopen(in[i], "rb");
            if(f == NULL) return SPK_E_FAILEDOPEN;
            fh->length = (uint32_t) s.st_size;
            fwrite(fh, sizeof(spk_fileheader_t), 1, of);
            char buf[BUFFER_SIZE];
            while(1)
            {
                j = fread(buf, 1, BUFFER_SIZE, f);
                if(j == 0) break;
                fwrite(buf, j, 1, of);
            }
            fclose(f);
        }
    }
    fclose(of);
    free(fh);
    return SPK_E_OK;
}
