/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_HLRESULTMTMAKERTOOL_H
#define TRIGOUTPUTHANDLING_HLRESULTMTMAKERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/HLTResultMT.h"
/**
 * @class HLResultMTMakerTool
 * @brief 
 **/
class HLResultMTMakerTool : virtual public IAlgTool {
public: 
  DeclareInterfaceID(HLResultMTMakerTool, 1, 0);

  virtual StatusCode fill( HLT::HLTResultMT& resultToFill ) const = 0;
  
  virtual ~HLResultMTMakerTool() override {}
}; 

#endif // TRIGOUTPUTHANDLING_HLRESULTMTMAKERTOOL_H
