#ifndef SPK_FORMAT_H
#define SPK_FOMART_H

#include <inttypes.h>

#define SPK_MAGIC "SPK\xFF" // 'S' 'P' 'K' \xFF NUL
#define SPK_MAGIC_LEN 5

typedef enum
{
    SPK_T_FILE,
    SPK_T_DIR
} spk_filetype_t;

typedef struct
{
	char name[255];
	spk_filetype_t type;
	uint16_t mode;
	uint16_t uid;
	uint16_t gid;
	uint32_t length;
} spk_fileheader_t;

#endif
