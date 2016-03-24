/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "v40_util.h"
#include "eformat/Issue.h"

namespace offline_eformat {
namespace v40 {

uint32_t convert_to_40(const uint32_t* src, uint32_t* dest, uint32_t max,
                       eformat::CheckSum /*event_checksum*/,
                       eformat::CheckSum /*rob_checksum*/)
{

    // just copy it to destination
    auto size = src[1];
    if (size > max) {
        throw EFORMAT_BLOCK_SIZE_TOO_SMALL(size, max);
    }
    std::copy(src, src+size, dest);
    return size;
}

}} // namespace offline_eformat::v40
