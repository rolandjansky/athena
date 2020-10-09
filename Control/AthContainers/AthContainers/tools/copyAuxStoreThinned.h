// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017, 2019, 2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthContainers/tools/copyAuxStoreThinned.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Helper to copy an aux store while applying thinning.
 */


#ifndef ATHCONTAINERS_COPYAUXSTORETHINNED_H
#define ATHCONTAINERS_COPYAUXSTORETHINNED_H


#include "AthenaKernel/ThinningCache.h"


namespace SG {


class IConstAuxStore;
class IAuxStore;


/**
 * @brief Helper to copy an aux store while applying thinning.
 * @param orig Source aux store from which to copy.
 * @param copy Destination aux store to which to copy.
 * @param info Thinning information for this object (or nullptr).
 *
 * @c orig and @c copy are both auxiliary store objects.
 * The data from @c orig will be copied to @c copy, with individual
 * elements / variables removed according @c info.
 */
void copyAuxStoreThinned (const SG::IConstAuxStore& orig,
                          SG::IAuxStore& copy,
                          const SG::ThinningInfo* info);

/**
 * @brief Helper method to apply lossy float compression
 * @param dst Pointer to the start of the vector's data
 * @param idst Index of element in vector
 * @param eltSize Element size for the auxid
 * @param typeName Type name for the auxid
 * @param nmantissa Compression level to be used for the auxid
 */
void lossyFloatCompress (void* dst, std::size_t dst_index,
                         const std::size_t& eltSize,
                         const std::string& typeName,
                         const unsigned int& nmantissa);

} // namespace SG


#endif // not ATHCONTAINERS_COPYAUXSTORETHINNED_H
