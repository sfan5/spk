#include <spk/spk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

short create_spk(char *outfile, int inlen, char *in[])
{
    FILE *of, *f;
    of = fopen(outfile, "wb");
    const char* temp1 = SPK_MAGIC;
    struct stat s;
    int i;
    uint32_t j;
    spk_fileheader_t *fh = malloc(sizeof(spk_fileheader_t));
    
    fwrite(temp1, SPK_MAGIC_LEN, 1, of);
    for(i = 0; i < inlen; i++)
    {
        stat(in[i], &s);
        if(s.st_mode & S_IFDIR)
        { // Directory
           memset(fh->name, 0, 255);
           strcpy(fh->name, in[i]);
           fh->type = SPK_T_DIR;
           fh->length = 0;
           fh->mode = (uint16_t) s.st_mode;
           fh->uid = (uint16_t) s.st_uid;
           fh->gid = (uint16_t) s.st_gid;
           fwrite(fh, sizeof(spk_fileheader_t), 1, of);
        }
        else if(s.st_mode & S_IFREG)
        { // File
            memset(fh->name, 0, 255);
            strcpy(fh->name, in[i]);
            fh->type = SPK_T_FILE;
            f = fopen(in[i], "rb");
            if(f == NULL) return SPK_E_FAILEDOPEN;
            fh->length = (uint32_t) s.st_size;
            fh->mode = (uint16_t) s.st_mode;
            fh->uid = (uint16_t) s.st_uid;
            fh->gid = (uint16_t) s.st_gid;
            fwrite(fh, sizeof(spk_fileheader_t), 1, of);
            for(j = 0; j < fh->length; j++)
            {
                fputc(fgetc(f), of);
            }
            fclose(f);
        }
    }
    fclose(of);
    return SPK_E_OK;
}
