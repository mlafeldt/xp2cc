#ifndef _CHKSUM_H_
#define _CHKSUM_H_

#include "mytypes.h"

u32 get_chksum32(u8 *blk, u32 len);
u16 get_crc16(u8 *blk, u32 len);
u16 get_crc16ccitt(u8 *blk, u32 len);
u32 get_crc32(u8 *blk, u32 len);

#endif // _CHKSUM_H_
