/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_HLTRESULTMTMAKERTOOL_H
#define TRIGOUTPUTHANDLING_HLTRESULTMTMAKERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/HLTResultMT.h"
/**
 * @class HLTResultMTMakerTool
 * @brief 
 **/
class HLTResultMTMakerTool : virtual public IAlgTool {
public: 
  DeclareInterfaceID(HLTResultMTMakerTool, 1, 0);

  virtual StatusCode fill( HLT::HLTResultMT& resultToFill ) const = 0;
  
  virtual ~HLTResultMTMakerTool() override {}
}; 

#endif // TRIGOUTPUTHANDLING_HLRESULTMTMAKERTOOL_H
