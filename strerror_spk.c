#include <spk/util.h>

const char* strerror_spk(short e)
{
    switch(e)
    {
        case SPK_E_OK:
            return "OK";
            break;
        case SPK_E_NOTANSPKFILE:
            return "Not an .spk file";
            break;
        case SPK_E_CORRUPTFILE:
            return "Corrupt .spk file";
            break;
        case SPK_E_FAILEDOPEN:
            return "Failed to open file/directory";
            break;
        case SPK_E_UNKNOWN:
        default:
            return "Unknown error";
            break;
    }
}
