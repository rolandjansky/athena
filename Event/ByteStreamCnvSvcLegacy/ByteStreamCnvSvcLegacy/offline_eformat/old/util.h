/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_OFFLINE_EFORMAT_OLD_UTIL_H
#define BYTESTREAMCNVSVC_OFFLINE_EFORMAT_OLD_UTIL_H 1

/**
 * @file offline_eformat/old/util.h
 *
 * @brief Declarations of methods for working with old eformat versions.
 *
 * @author Andy Salnikov
 */

#include <stdint.h>
#include "eformat/checksum.h"

namespace offline_eformat {
namespace old {

/**
 * Converts a full event fragment or a ROS fragment, from @b some format
 * to v4.0 format, using the space of contiguous memory storage area given.
 * If the event given is already on the current format, no conversion takes
 * place.
 *
 * @param src A pointer to the first word of the event, lying in a @b
 * contiguous area of memory.
 * @param dest The destination area of memory, preallocated
 * @param max The maximum number of words that fit in the preallocated
 * memory area "dest".
 * @param event_checksum What type of checksum to deploy for full events
 * @param rob_checksum What type of checksum to deploy for rob fragments
 *
 * @return A counter, for the number of words copied from the source to the
 * destination. If that number is zero, something wrong happened.
 */
uint32_t convert_to_40(const uint32_t* src, uint32_t* dest, uint32_t max,
                       eformat::CheckSum event_checksum = eformat::NO_CHECKSUM,
                       eformat::CheckSum rob_checksum = eformat::NO_CHECKSUM);

}} // namespace offline_eformat::old

#endif // BYTESTREAMCNVSVC_OFFLINE_EFORMAT_OLD_UTIL_H
