/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/IAlgTool.h"

namespace Trk{
  class Track;
}
namespace TRT{
class TrackInfo;
}

class ATLAS_NOT_THREAD_SAFE ITRTCalibrator : virtual public IAlgTool {
public:
  DeclareInterfaceID( ITRTCalibrator, 1, 0 );
  virtual bool fill(const Trk::Track* aTrack, TRT::TrackInfo* output) = 0;
  // uses thread-unsafe methods from Calibrator.
  virtual bool calibrate ATLAS_NOT_THREAD_SAFE () = 0;
};

#endif // TRT_CALIBTOOLS__ITRTCALIBRATOR_H

