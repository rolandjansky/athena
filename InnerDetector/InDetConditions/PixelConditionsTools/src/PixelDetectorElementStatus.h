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
         const InDetDD::PixelModuleDesign &pixel_design=dynamic_cast<const InDetDD::PixelModuleDesign &>(design);
         // number of circuits reported for FEI3 is only the number of circuits per half-module
         return pixel_design.numberOfCircuits() * (pixel_design.getReadoutTechnology() ==  InDetDD::PixelReadoutTechnology::FEI3 ? 2 : 1);
      }

   };

}
#endif
