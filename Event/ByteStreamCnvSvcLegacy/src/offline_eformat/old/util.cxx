/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamCnvSvcLegacy/offline_eformat/old/util.h"
#include "eformat/util.h"
#include "eformat/Version.h"
#include "v40_util.h"
#include "v50_util.h"

namespace {

// This should have been defined in eformat itself but it's not
const uint16_t MAJOR_V50_VERSION = 0x0500;

}

namespace offline_eformat {
namespace old {

uint32_t convert_to_40(const uint32_t* src, uint32_t* dest, uint32_t max,
                       eformat::CheckSum event_checksum,
                       eformat::CheckSum rob_checksum)
{
    switch (eformat::peek_type(src)) {
    case eformat::FULL_EVENT:
        switch (eformat::peek_major_version(src)) {
        case ::MAJOR_V50_VERSION:
            return offline_eformat::v50::convert_to_40(src, dest, max, event_checksum, rob_checksum);
        case eformat::MAJOR_V40_VERSION:
            // 40 -> to 40 works (makes a byte-level copy and ignores checksum flags)
            return offline_eformat::v40::convert_to_40(src, dest, max, event_checksum, rob_checksum);
        default:
            throw EFORMAT_BAD_VERSION(eformat::peek_major_version(src), ::MAJOR_V50_VERSION);
        }
    default:
        throw EFORMAT_UNSUPPORTED_OPERATION("convert to v40 format", src[0]);
    }
    return 0; //this cannot not happen...
}

}} // namespace offline_eformat::old
