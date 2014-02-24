#ifndef SPK_FORMAT_H
#define SPK_FORMAT_H

#include <inttypes.h>

#define SPK_MAGIC "SPK\xFF" // 'S' 'P' 'K' \xFF NUL
#define SPK_MAGIC_LEN 5

#define SPK_T_FILE 0x01
#define SPK_T_DIR  0x02
#define SPK_T_SYMLINK  0x03

typedef struct
{
	char name[255];
	uint8_t type;
	uint16_t mode;
	uint16_t uid;
	uint16_t gid;
	uint32_t length;
} __attribute__((packed)) spk_fileheader_t;

#endif
