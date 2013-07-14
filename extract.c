#include <spk/spk.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

short extract_spk(char *filename, char *outdir)
{
    return extract_spk_ex(filename, outdir, false, false, false);
}

short extract_spk_ex(char *filename, char *outdir, bool verbose, bool ignore_gid_uid, bool ignore_mode)
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
        if(fh->type == SPK_T_DIR && fh->length != 0) return SPK_E_CORRUPTFILE;
        if(fh->type == SPK_T_SYMLINK && fh->length == 0) return SPK_E_CORRUPTFILE;
        if(fh->name[0] == '/') return SPK_E_CORRUPTFILE;
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
            if(fh->mode != 0xffff && !ignore_mode) chmod(outpath, (mode_t) fh->mode);
#ifndef _WIN32
            if(fh->uid != 0xffff && !ignore_gid_uid) chown(outpath, (uid_t) fh->uid, (gid_t) fh->gid);
#endif
        }
        else if(fh->type == SPK_T_DIR)
        {
#ifdef _WIN32
            mkdir(outpath);
#else
            mkdir(outpath, 0777);
#endif
            if(fh->mode != 0xffff && !ignore_mode) chmod(outpath, (mode_t) fh->mode);
#ifndef _WIN32
            if(fh->uid != 0xffff && !ignore_gid_uid) chown(outpath, (uid_t) fh->uid, (gid_t) fh->gid);
#endif
        }
#ifndef _WIN32
        else if(fh->type == SPK_T_SYMLINK)
        {
            char *dest = (char*) malloc(fh->length);
            fread(dest, fh->length, 1, f);
            symlink(dest, outpath);
            if(fh->mode != 0xffff && !ignore_mode) chmod(outpath, (mode_t) fh->mode);
            if(fh->uid != 0xffff && !ignore_gid_uid) chown(outpath, (uid_t) fh->uid, (gid_t) fh->gid);
        }
#else
        else if(fh->type == SPK_T_SYMLINK)
        {
            fprintf(stderr, "WARN: Symlinks are not supported on Windows\n");
        }
#endif
        else return SPK_E_CORRUPTFILE;
        
        
        if(verbose) printf("%s\n", fh->name);
    }
    return SPK_E_OK;
}
