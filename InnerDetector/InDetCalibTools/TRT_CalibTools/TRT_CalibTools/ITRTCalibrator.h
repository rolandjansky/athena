/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBTOOLS__ITRTCALIBRATOR_H
#define TRT_CALIBTOOLS__ITRTCALIBRATOR_H
/********************************************************************

NAME:     ITRTCalibrator
PACKAGE:  TRT_CalibTools

AUTHORS:  Johan Lundquist
CREATED:  March 2009

PURPOSE:  

********************************************************************/

#include "GaudiKernel/IAlgTool.h"

namespace Trk{
  class Track;
}
namespace TRT{
class TrackInfo;
}

static const InterfaceID IID_ITRTCalibrator("ITRTCalibrator", 1, 0);

class ITRTCalibrator : virtual public IAlgTool {
public:

  virtual bool fill(const Trk::Track* aTrack, TRT::TrackInfo* output) = 0;
  virtual bool calibrate() = 0;

  static const InterfaceID& interfaceID();
};

inline const InterfaceID& ITRTCalibrator::interfaceID()
{ 
        return IID_ITRTCalibrator; 
}

#endif // TRT_CALIBTOOLS__ITRTCALIBRATOR_H

