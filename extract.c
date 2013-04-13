#include <spk/spk.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

short extract_spk(char *filename, char *outdir)
{
    return extract_spk_ex(filename, outdir, 0);
}

short extract_spk_ex(char *filename, char *outdir, short verbose)
{
    FILE *f, *of;
    spk_fileheader_t *fh = malloc(sizeof(spk_fileheader_t));
    char *outpath = malloc(512);
    uint32_t i;
    char temp1[SPK_MAGIC_LEN];
    const char* temp2 = SPK_MAGIC;
    
    f = fopen(filename, "rb");
    if(f == NULL) return SPK_E_FAILEDOPEN;
    fread(temp1, SPK_MAGIC_LEN, 1, f);
    if(memcmp(temp1, temp2, SPK_MAGIC_LEN)) return SPK_E_NOTANSPKFILE;
    while(1)
    {
        i = (uint32_t) fgetc(f);
        if(feof(f)) break; else ungetc((int) i, f);
        
        fread(fh, sizeof(spk_fileheader_t), 1, f);
        if(fh->type != SPK_T_FILE && fh->type != SPK_T_DIR) return SPK_E_CORRUPTFILE;
        if(fh->type == SPK_T_DIR && fh->length != 0) return SPK_E_CORRUPTFILE;
        memset(outpath, 0, 512);
        strcat(outpath, outdir);
        strcat(outpath, "/");
        strcat(outpath, fh->name);
        if(fh->type == SPK_T_FILE)
        {
            of = fopen(outpath, "wb");
            if(of == NULL) return SPK_E_FAILEDOPEN;
            i = 0;
            while(i < fh->length)
            {
                if(feof(f)) return SPK_E_CORRUPTFILE;
                fputc(fgetc(f), of);
                i++;
            }
            fclose(of);
            chmod(outpath, (mode_t) fh->mode);
            chown(outpath, (uid_t) fh->uid, (gid_t) fh->gid);
        }
        else if(fh->type == SPK_T_DIR)
        {
            mkdir(outpath, (mode_t) fh->mode);
            chown(outpath, (uid_t) fh->uid, (gid_t) fh->gid);
        }
        
        if(verbose) printf("%s\n", fh->name);
    }
    return SPK_E_OK;
}
