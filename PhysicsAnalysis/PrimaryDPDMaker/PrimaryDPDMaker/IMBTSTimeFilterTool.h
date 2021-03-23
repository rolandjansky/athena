/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryDPDMaker_IMBTSTimeFilterTool_H
#define PrimaryDPDMaker_IMBTSTimeFilterTool_H

#include "PrimaryDPDMaker/TimingFilterInformation.h"
#include "AsgTools/IAsgTool.h"
#include "StoreGate/ReadHandle.h"
#include "TileEvent/TileCellContainer.h"
   
class IMBTSTimeFilterTool : virtual public asg::IAsgTool   {
 public:    
  virtual ~IMBTSTimeFilterTool()= default;

  ASG_TOOL_INTERFACE (IMBTSTimeFilterTool);
  
  /// Caching method using the ReadHandles
  virtual StatusCode getTimeDifference(TimingFilterInformation& time_info,
                                        const SG::ReadHandleKey<TileCellContainer>& key,
                                        const EventContext& ctx) const = 0;
  /// Caching method using the old store gate mechanism. Attention: Not MT safe
  virtual StatusCode getTimeDifference(TimingFilterInformation& time_info) = 0;

};

#endif

