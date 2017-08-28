/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBSVC_MDTCALIBRATIONSVCINPUT_H
#define MDTCALIBSVC_MDTCALIBRATIONSVCINPUT_H

/*
 * 
 * Authors Niels van Eldik & Martin Woudstra
 *
 */

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
  class StraightLineSurface;
}

/**
  @class MdtCalibrationSvcInput
  This class contains all possible input for the MdtCalibrationSvc. 
  Not all fields have to be filled to perform a calibration but if some fields 
  are missing one might not get the optimal resolution.
  @author Martin Woudstra, Niels van Eldik
*/
class MdtCalibrationSvcInput {
public:
  MdtCalibrationSvcInput() :
    tof(0.),triggerOffset(0.),pointOfClosestApproach(0),trackDirection(0),
    nominalWireSurface(0),wireSurface(0) {
  }  
  double tof;
  double triggerOffset;
  const Amg::Vector3D *pointOfClosestApproach;
  const Amg::Vector3D *trackDirection;
  const Trk::StraightLineSurface *nominalWireSurface;
  const Trk::StraightLineSurface *wireSurface;
};

#endif
