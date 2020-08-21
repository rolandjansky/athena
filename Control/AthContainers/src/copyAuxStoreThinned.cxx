/*
  Copyright (C) 2002-2017, 2019-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthContainers/Root/copyAuxStoreThinned.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Helper to copy an aux store while applying thinning.
 */


#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainersInterfaces/IAuxStoreCompression.h"
#include "AthenaKernel/ThinningDecisionBase.h"
#include "CxxUtils/no_sanitize_undefined.h"
#include "CxxUtils/FloatCompressor.h"
#include <vector>


namespace SG {


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
void copyAuxStoreThinned NO_SANITIZE_UNDEFINED
   (const SG::IConstAuxStore& orig,
    SG::IAuxStore& copy,
    const SG::ThinningInfo* info)
{
  size_t size = orig.size();
  if (size == 0) {
    copy.resize(0);
    return;
  }

  const ThinningDecisionBase* dec = info ? info->m_decision : nullptr;

  size_t nremaining = dec ? dec->thinnedSize() : size;

  // Access the auxiliary type registry:
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  // The auxiliary IDs that the original container has:
  SG::auxid_set_t auxids = orig.getAuxIDs();

  SG::auxid_set_t dyn_auxids;
  SG::auxid_set_t sel_auxids;
  if (const IAuxStoreIO* iio = dynamic_cast<const IAuxStoreIO*> (&orig)) {
    dyn_auxids = iio->getDynamicAuxIDs();
    sel_auxids = iio->getSelectedAuxIDs();
  }

  copy.resize (nremaining);

  // Loop over all the variables of the original container:
  for (SG::auxid_t auxid : auxids) {
    // Skip null auxids (happens if we don't have the dictionary)
    if(auxid == SG::null_auxid) continue;

    // Skip non-selected dynamic variables.
    if (dyn_auxids.test(auxid)) {
      if (info) {
        if (info->vetoed(auxid)) continue;
      }
      if (!sel_auxids.test(auxid)) continue;
    }

    // Access the source variable:
    const void* src = orig.getData (auxid);

    if (!src) continue;

    // FIXME: Do this via proper interfaces.
    if (const IAuxStoreIO* iio = dynamic_cast<const IAuxStoreIO*> (&orig))
    {
      const std::type_info* typ = iio->getIOType (auxid);
      if (strstr (typ->name(), "PackedContainer") != nullptr) {
        // This cast gets a warning from the undefined behavior sanitizer
        // in gcc6.  Done like this deliberately for now, so suppress ubsan
        // checking for this function.
        const PackedParameters& parms =
          reinterpret_cast<const PackedContainer<int>* > (iio->getIOData (auxid))->parms();
        copy.setOption (auxid, AuxDataOption ("nbits", parms.nbits()));
        copy.setOption (auxid, AuxDataOption ("float", parms.isFloat()));
        copy.setOption (auxid, AuxDataOption ("signed", parms.isSigned()));
        copy.setOption (auxid, AuxDataOption ("rounding", parms.rounding()));
        copy.setOption (auxid, AuxDataOption ("nmantissa", parms.nmantissa()));
        copy.setOption (auxid, AuxDataOption ("scale", parms.scale()));
      }
    }

    // Get the element size, type name and compression info for the current auxid
    // This information is later passed to the helper function lossyFloatCompress
    const size_t eltSize{ r.getEltSize(auxid) };
    const std::string typeName{ r.getTypeName(auxid) };
    const unsigned int nmantissa{ info != nullptr ? info->compression(auxid) : 0 };

    // Create the target variable:
    void* dst = copy.getData (auxid, nremaining, nremaining);

    // Copy over all elements, with thinning.
    for (std::size_t isrc = 0, idst = 0; isrc < size; ++isrc) {
      if (!dec || !dec->thinned(isrc)) {
        r.copyForOutput (auxid, dst, idst, src, isrc);
        // Apply lossy float compression here (in-place)
        // Maybe it would be better to do this via the registry during copy
        lossyFloatCompress(dst, idst, eltSize, typeName, nmantissa);
        ++idst;
      }
    }
  }
}

/**
 * @brief Helper method to apply lossy float compression
 * @param dst Pointer to the start of the vector's data
 * @param idst Index of element in vector
 * @param eltSize Element size for the auxid
 * @param typeName Type name for the auxid
 * @param nmantissa Compression level to be used for the auxid
 */
void lossyFloatCompress (void* dst, std::size_t idst,
                         const std::size_t& eltSize,
                         const std::string& typeName,
                         const unsigned int& nmantissa)
{
  // Check if there is anything to be done
  // Total number of explicit mantissa bits for a 32 bit float is 23
  // 0 is used to denote no compression should be applied to the variable
  static const unsigned int NMANTISSA_MAX = 23;
  if (nmantissa == 0 || nmantissa >= NMANTISSA_MAX) return;

  // Setup all possible compressors: done only once
  static const std::vector<CxxUtils::FloatCompressor> compressors = [] {
    std::vector<CxxUtils::FloatCompressor> result;
    result.reserve(NMANTISSA_MAX);
    for (std::size_t idx = 0; idx < NMANTISSA_MAX; ++idx)
      result.emplace_back(idx);
    return result;
  }();

  // Get the pointer to the memory
  void* eltPtr = reinterpret_cast<char*>(dst) + idst*eltSize;

  // This is where we apply in-place lossy float compression
  if(typeName == "float") {
    *(float*) eltPtr = compressors[nmantissa].reduceFloatPrecision(*(float*) eltPtr);
  } else if (typeName == "std::vector<float>"){
    std::vector<float> &vals = *(reinterpret_cast<std::vector<float>*>(eltPtr));
    for(auto &val: vals) {
      val = compressors[nmantissa].reduceFloatPrecision(val);
    }
  }
}

} // namespace SG
