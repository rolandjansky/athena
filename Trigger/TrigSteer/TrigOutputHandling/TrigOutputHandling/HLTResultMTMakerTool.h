/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_HLTRESULTMTMAKERTOOL_H
#define TRIGOUTPUTHANDLING_HLTRESULTMTMAKERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

#include "TrigSteeringEvent/HLTResultMT.h"

/**
 * @class HLTResultMTMakerTool
 * @brief Base class for AlgTools filling information in an HLTResultMT object
 **/
class HLTResultMTMakerTool : virtual public IAlgTool {
public: 
  DeclareInterfaceID(HLTResultMTMakerTool, 1, 0);

  virtual StatusCode fill( HLT::HLTResultMT& resultToFill, const EventContext& ctx ) const = 0;
  
  virtual ~HLTResultMTMakerTool() override = default;
}; 

#endif // TRIGOUTPUTHANDLING_HLTRESULTMTMAKERTOOL_H
