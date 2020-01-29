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
#include "AthContainers/tools/getThinnedFlags.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainersInterfaces/IAuxStoreCompression.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaKernel/ThinningDecisionBase.h"
#include "CxxUtils/no_sanitize_undefined.h"
#include <vector>


namespace SG {


/**
 * @brief Helper to copy an aux store while applying thinning.
 * @param orig Source aux store from which to copy.
 * @param copy Destination aux store to which to copy.
 * @param svc The thinning service.
 *
 * @c orig and @c copy are both auxiliary store objects.
 * The data from @c orig will be copied to @c copy, with individual
 * elements removed according to thinning recorded for @c orig in @c svc.
 *
 * [To be removed as part of MT thinning work; use the variant below instead.]
 */
void copyAuxStoreThinned NO_SANITIZE_UNDEFINED
   (const SG::IConstAuxStore& orig,
    SG::IAuxStore& copy,
    IThinningSvc* svc)
{
  size_t size = orig.size();
  if (size == 0) {
    copy.resize(0);
    return;
  }
  size_t nremaining = 0;
  std::vector<unsigned char> flags;
  bool thinned = getThinnedFlags (svc, orig, nremaining, flags);

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

  // Get the auxiliary IDs of the variables that should be compressed
  SG::auxid_set_t comp_auxids_high;
  SG::auxid_set_t comp_auxids_low;
  const IAuxStoreCompression* icomp = dynamic_cast<const IAuxStoreCompression*> (&orig);
  if (icomp != nullptr) {
    comp_auxids_high = icomp->getCompressedAuxIDs(true);  // High compression
    comp_auxids_low  = icomp->getCompressedAuxIDs(false); // Low compression
  }

  copy.resize (nremaining);
  
  // Loop over all the variables of the original container:
  for (SG::auxid_t auxid : auxids) {
    // Skip null auxids (happens if we don't have the dictionary)
    if(auxid == SG::null_auxid) continue;

    // Skip non-selected dynamic variables.
    if (dyn_auxids.test(auxid) && !sel_auxids.test(auxid))
      continue;

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

    // Get the element size and the type name for the current auxid
    const size_t eltSize{r.getEltSize(auxid)};
    const std::string typeName{r.getTypeName(auxid)};
    const bool compressed_high{comp_auxids_high.test(auxid)};
    const bool compressed_low{comp_auxids_low.test(auxid)};
    const bool compressed{compressed_high || compressed_low};

    // Create the target variable:
    void* dst = copy.getData (auxid, nremaining, nremaining);

    // Copy over all elements, with thinning.
    for (std::size_t isrc = 0, idst = 0; isrc < size; ++isrc) {
      if (!thinned || !flags[isrc]) {
        r.copyForOutput (auxid, dst, idst, src, isrc);
        // Compression BEGINS
        if (compressed) {
          // Get the pointer to the memory
          void* eltPtr = reinterpret_cast<char*>(dst) + idst*eltSize;

          // Here comes the actual compression
          // By now we should only have either float or std::vector<float>
          if(typeName.find("vector") == std::string::npos) {
            *(float*) eltPtr = icomp->getCompressedValue(*(float*) eltPtr, compressed_high);
          } else {
            std::vector<float> &vals = *(reinterpret_cast<std::vector<float>*>(eltPtr));
            for(auto &val: vals) {
              val = icomp->getCompressedValue(val, compressed_high);
            }
          }
        } // Compression ENDS
        ++idst;
      }
    }
  }
}


/**
 * @brief Helper to copy an aux store while applying thinning.
 * @param orig Source aux store from which to copy.
 * @param copy Destination aux store to which to copy.
 * @param dec The thinning decision for this object.
 *
 * @c orig and @c copy are both auxiliary store objects.
 * The data from @c orig will be copied to @c copy, with individual
 * elements removed according to thinning recorded in @c dec.
 */
void copyAuxStoreThinned NO_SANITIZE_UNDEFINED
   (const SG::IConstAuxStore& orig,
    SG::IAuxStore& copy,
    const SG::ThinningDecisionBase* dec)
{
  size_t size = orig.size();
  if (size == 0) {
    copy.resize(0);
    return;
  }

  size_t nremaining = dec ? dec->thinnedSize() : size;
  if (!dec) {
    // Temporary: If no new thinning, try old thinning instead.
    copyAuxStoreThinned (orig, copy, IThinningSvc::instance());
    return;
  }
  
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

  // Get the auxiliary IDs of the variables that should be compressed
  SG::auxid_set_t comp_auxids_high;
  SG::auxid_set_t comp_auxids_low;
  const IAuxStoreCompression* icomp = dynamic_cast<const IAuxStoreCompression*> (&orig);
  if (icomp != nullptr) {
    comp_auxids_high = icomp->getCompressedAuxIDs(true);  // High compression
    comp_auxids_low  = icomp->getCompressedAuxIDs(false); // Low compression
  }

  copy.resize (nremaining);
  
  // Loop over all the variables of the original container:
  for (SG::auxid_t auxid : auxids) {
    // Skip null auxids (happens if we don't have the dictionary)
    if(auxid == SG::null_auxid) continue;

    // Skip non-selected dynamic variables.
    if (dyn_auxids.test(auxid) && !sel_auxids.test(auxid))
      continue;

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

    // Get the element size and the type name for the current auxid
    const size_t eltSize{r.getEltSize(auxid)};
    const std::string typeName{r.getTypeName(auxid)};
    const bool compressed_high{comp_auxids_high.test(auxid)};
    const bool compressed_low{comp_auxids_low.test(auxid)};
    const bool compressed{compressed_high || compressed_low};

    // Create the target variable:
    void* dst = copy.getData (auxid, nremaining, nremaining);

    // Copy over all elements, with thinning.
    for (std::size_t isrc = 0, idst = 0; isrc < size; ++isrc) {
      if (!dec || !dec->thinned(isrc)) {
        r.copyForOutput (auxid, dst, idst, src, isrc);
        // Compression BEGINS
        if (compressed) {
          // Get the pointer to the memory
          void* eltPtr = reinterpret_cast<char*>(dst) + idst*eltSize;

          // Here comes the actual compression
          // By now we should only have either float or std::vector<float>
          if(typeName.find("vector") == std::string::npos) {
            *(float*) eltPtr = icomp->getCompressedValue(*(float*) eltPtr, compressed_high);
          } else {
            std::vector<float> &vals = *(reinterpret_cast<std::vector<float>*>(eltPtr));
            for(auto &val: vals) {
              val = icomp->getCompressedValue(val, compressed_high);
            }
          }
        } // Compression ENDS
        ++idst;
      }
    }
  }
}


/**
 * @brief For compatibility with old code, that passed `0' as the third arg.
 *        (Can be removed when old thinning is removed.)
 */
void copyAuxStoreThinned (const SG::IConstAuxStore& orig,
                          SG::IAuxStore& copy,
                          int)
{
  copyAuxStoreThinned (orig, copy,
                       static_cast<const SG::ThinningDecisionBase*> (nullptr) );
}


} // namespace SG
