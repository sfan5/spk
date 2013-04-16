#include <spk/spk.h>
#include <stdlib.h>

short create_spk(char *outfile, int inlen, char *in[])
{
    return create_spk_ex(outfile, inlen, in, false, false);
}

short create_spk_ex(char *outfile, int inlen, char *in[], bool no_gid_uid, bool no_mode)
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
        if(no_mode) fh->mode = 0xffff; else fh->mode = (uint16_t) s.st_mode;
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
        if(s.st_mode & S_IFDIR)
        { // Directory
           memset(fh->name, 0, 255);
           strcpy(fh->name, in[i]);
           fh->type = SPK_T_DIR;
           fh->length = 0;
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
