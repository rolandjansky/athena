// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/hexdump.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2021
 * @brief Helpers to make a nice dump of a region of memory.
 */


#ifndef CXXUTILS_HEXDUMP_H
#define CXXUTILS_HEXDUMP_H


#include <ostream>


namespace CxxUtils {


/**
 * @brief Make a hex dump of memory.
 * @param s Stream to which to write the output.
 * @param addr Address at which to start dumping.
 * @param n Number of byte to dump.
 * @param offset Offset by which to shift the printed address
 *               (mostly for testing).
 */
void hexdump (std::ostream& s, const void* addr, size_t n, size_t offset = 0);


/**
 * @brief Make a hex dump of memory, protected against bad reads.
 * @param s Stream to which to write the output.
 * @param addr Address at which to start dumping.
 * @param n Number of byte to dump.
 * @param offset Offset by which to shift the printed address
 *               (mostly for testing).
 *
 * This function will skip dumping memory pages that are not readable.
 * It may also start dumping slightly before the requested address,
 * in order to start with an alignment of 16.
 */
void safeHexdump (std::ostream& s, const void* addr, size_t n, size_t offset = 0);


} // namespace CxxUtils


#endif // not CXXUTILS_HEXDUMP_H
