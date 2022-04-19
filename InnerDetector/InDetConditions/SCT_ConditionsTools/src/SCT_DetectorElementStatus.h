/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/* Dear emacs, this is -*-c++-*- */
#ifndef SCT_DETECTORELEMENTSTATUS_H
#define SCT_DETECTORELEMENTSTATUS_H
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"

namespace InDet {
   /** Event data to hold the status information for SCT modules.
    */
   class SCT_DetectorElementStatus : public InDet::SiDetectorElementStatus
   {
      static constexpr unsigned int NUM_CHIPS_PER_WAFER = 6;
   public:
      SCT_DetectorElementStatus(const InDetDD::SiDetectorElementCollection &detector_elements)
         : InDet::SiDetectorElementStatus(detector_elements) {}
    protected:
      virtual unsigned int numberOfChips([[maybe_unused]] const InDetDD::SiDetectorDesign& design) const override {
         // @TODO eliminate the dynamic cast when not in debug build
         return NUM_CHIPS_PER_WAFER;
      }

   };

}
#endif
