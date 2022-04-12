/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef PIXELFEUTILS_H
#define PIXELFEUTILS_H

#include "InDetReadoutGeometry/SiDetectorElementStatus.h"
#include "PixelReadoutGeometry/IPixelReadoutManager.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetIdentifier/PixelID.h"
#include <algorithm>
#include <tuple>
#include <limits>
#include <cassert>

namespace Pixel {

   /** Create a tuple of the given values in increasing order.
    */
   template <typename T>
   inline std::tuple<T,T> sorted_tuple(T a, T b) {
      return (a<b ? std::make_tuple(a,b) : std::make_tuple(b,a) );
   }

   /**  Get The fraction of good chips the pixel of a pixel array is connected to.
    * @param pixelDetElStatus helper class which provides the front-end module status information.
    * @param pixelReadout the pixel readout manager needed to determine the front-end chip of a pixel.
    * @param moduleId the identifier of the corresponding module.
    * @param moduleIdHash the identifier hash of the corresponding module.
    * @param startId the identifier of one corner of the pixel array.
    * @param endId the identifier of the opposing corner of the pixel array which must be on the same pixel module.
    */
   inline double getGoodFraction(const InDet::SiDetectorElementStatus &pixelDetElStatus,
                                 const InDetDD::IPixelReadoutManager &pixelReadout,
                                 const PixelID &pixelID,
                                 const Identifier &moduleId,
                                 const IdentifierHash &moudlIdHash,
                                 const Identifier &startId,
                                 const Identifier &endId) {
      auto [phiStart,phiEnd] = sorted_tuple( pixelID.phi_index(startId), pixelID.phi_index(endId) );
      auto [etaStart,etaEnd] = sorted_tuple( pixelID.eta_index(startId), pixelID.eta_index(endId) );

      double nTotal = (phiEnd-phiStart+1.0)*(etaEnd-etaStart+1.0);

      unsigned int nGood=0;
      for (int i=phiStart; i<=phiEnd; i++) {
         for (int j=etaStart; j<=etaEnd; j++) {
            int chFE = pixelReadout.getFE(pixelID.pixel_id(moduleId,i,j), moduleId);
            if (pixelDetElStatus.isChipGood(moudlIdHash,chFE)) {++nGood;}
         }
      }
      return nGood/nTotal;
   }

   /** Create a word with a bit representing the given readout technology to the given value.
    * @param technology the readout technology
    * @param the value to which the bit should be set to.
    * @return word with readout technology bit set to given value
    */
   inline unsigned int makeReadoutTechnologyBit( InDetDD::PixelReadoutTechnology technology, unsigned int bit_val=1 ) {
      assert(static_cast<unsigned int>(technology) < 31);
      return bit_val << static_cast<unsigned int>(technology);
   }

   /** Check whether the readout technology of the specified module is contained in the given readout technology mask.
    * @param elementStatus the detector element status information.
    * @param moduleIdHash the module hash of the module in question.
    * @param readOutTechnologyMask a mask which contains a bits per readout technology.
    */
   inline bool matchingReadoutTechnology(const InDet::SiDetectorElementStatus &elementStatus,
                                         const IdentifierHash &moduleIdHash,
                                         unsigned int readOutTechnologyMask = (    Pixel::makeReadoutTechnologyBit(InDetDD::PixelReadoutTechnology::FEI4)
                                                                               | ( Pixel::makeReadoutTechnologyBit(InDetDD::PixelReadoutTechnology::FEI3) ) ))
   {
      const InDetDD::SiDetectorElement *element = elementStatus.getDetectorElement(moduleIdHash);
      const InDetDD::PixelModuleDesign &p_design = dynamic_cast<const InDetDD::PixelModuleDesign &>(element->design());
      return ( static_cast<uint64_t>(p_design.getReadoutTechnology()) < 31ul && readOutTechnologyMask & Pixel::makeReadoutTechnologyBit(p_design.getReadoutTechnology()) );
   }

}
#endif
