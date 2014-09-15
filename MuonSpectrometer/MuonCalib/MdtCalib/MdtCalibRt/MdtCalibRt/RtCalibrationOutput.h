/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RTCALIBRATIONOUTPUT_H
#define RTCALIBRATIONOUTPUT_H

#include "MdtCalibInterfaces/IMdtCalibrationOutput.h"
#include "MdtCalibData/RtFullInfo.h"
#include "MdtCalibData/IRtRelation.h"

namespace MuonCalib{
/** 
@class RtCalibrationOutput
Class for communication between event loop and rt calibration algorithm
contains only a rt relation for now.
    
*/

class RtCalibrationOutput : public IMdtCalibrationOutput {
 public:
  RtCalibrationOutput( const IRtRelation* rt_rel, const RtFullInfo* fi ) : 
  IMdtCalibrationOutput("RtCalibrationOutput"), m_rtRelation(rt_rel), m_fullInfo(fi) {}

  /** access to private attributes */
  const IRtRelation* rt() const { return m_rtRelation; }  
  const RtFullInfo* fullInfo() const { return m_fullInfo; } 	 

 private:
  // pointer to a IRtRelation instance
  const IRtRelation* m_rtRelation;
  /** additonal info for validation */ 	 
  const RtFullInfo* m_fullInfo;  
};
}
#endif
