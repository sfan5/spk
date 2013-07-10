#ifndef _SPK_H
#define _SPK_H

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

#include <spk/format.h>
#include <spk/util.h>

#define _STR_HELPER(x) #x
#define _STR(x) _STR_HELPER(x)

#define SPK_VERSION_MAJOR 0
#define SPK_VERSION_MINOR 1
#define SPK_VERSION_BUILD 8
#define SPK_VERSION SPK_VERSION_MAJOR ## SPK_VERSION_MINOR ## SPK_VERSION_BUILD
#define SPK_VERSION_S _STR(SPK_VERSION_MAJOR) "." _STR(SPK_VERSION_MINOR) "." _STR(SPK_VERSION_BUILD)

#endif
