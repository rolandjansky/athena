/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef PIXELBSUTILS_H
#define PIXELBSUTILS_H
#include "PixelReadoutGeometry/PixelFEUtils.h"

namespace InDet {
   /** Retrieve the bytestream error word for the given module if the readout technology of the module is contained in in the mask.
    * @param elementStatus the detector element status information.
    * @param bsErrorContainer the container with bytestream error words.
    * @param the hash of the module in question.
    * @param the index of the error word where the index of the module error is equal to the hash.
    * @param readOutTechnologyMask a mask which contains a bit for each readout technology.
    */
   inline unsigned int getBSErrorWord(const InDet::SiDetectorElementStatus &elementStatus,
                                      const IDCInDetBSErrContainer &bsErrorContainer,
                                      const IdentifierHash &moduleIdHash,
                                      unsigned int index,
                                      unsigned int readOutTechnologyMask = (    Pixel::makeReadoutTechnologyBit(InDetDD::PixelReadoutTechnology::FEI4)
                                                                            | ( Pixel::makeReadoutTechnologyBit(InDetDD::PixelReadoutTechnology::FEI3) ) ))
   {
      if ( Pixel::matchingReadoutTechnology(elementStatus, moduleIdHash, readOutTechnologyMask )) {
         constexpr uint64_t missingErrorInfo{std::numeric_limits<uint64_t>::max()-3000000000};
         uint64_t word = static_cast<uint64_t>(bsErrorContainer.retrieve(index));
         return word < missingErrorInfo ? word : 0;
      }
      else {
         return 0;
      }
   }
}
#endif
