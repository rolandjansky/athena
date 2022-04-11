/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/* Dear emacs, this is -*-c++-*- */
#ifndef PIXELDETECTORELEMENTSTATUS_H
#define PIXELDETECTORELEMENTSTATUS_H
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"

namespace InDet {
   /** Event data to hold the status information for Pixel modules.
    */
   class PixelDetectorElementStatus : public InDet::SiDetectorElementStatus
   {
   public:
      PixelDetectorElementStatus(const PixelDetectorElementStatus &si_detector_element_status) = default;
      PixelDetectorElementStatus(const InDetDD::SiDetectorElementCollection &detector_elements)
         : InDet::SiDetectorElementStatus(detector_elements) {}
    protected:
      virtual unsigned int numberOfChips(const InDetDD::SiDetectorDesign& design) const override {
         // @TODO eliminate the dynamic cast when not in debug build
         return dynamic_cast<const InDetDD::PixelModuleDesign &>(design).numberOfCircuits();
      }

   };

}
#endif
