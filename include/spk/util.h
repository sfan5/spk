#ifndef SPK_UTIL_H
#define SPK_UTIL_H

#include <stdbool.h>

#define SPK_E_OK            0x00 // OK
#define SPK_E_NOTANSPKFILE  0x01 // Not an .spk file
#define SPK_E_CORRUPTFILE   0x02 // Corrupt .spk file
#define SPK_E_FAILEDOPEN    0x03 // Could not open file/directory
#define SPK_E_UNKNOWN       0x04 // Unknown error

const char* strerror_spk(short e);
short extract_spk(char *filename, char *outdir);
short extract_spk_ex(char *filename, char *outdir, bool verbose, bool ignore_gid_uid, bool ignore_mode);
short create_spk(char *outfile, int inlen, char *in[]);
short create_spk_ex(char *outfile, int inlen, char *in[], bool verbose, bool no_gid_uid, bool no_mode);

#endif
