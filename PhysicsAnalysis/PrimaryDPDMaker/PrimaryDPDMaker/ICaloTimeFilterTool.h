/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryDPDMaker_ICaloTimeFilterTool_H
#define PrimaryDPDMaker_ICaloTimeFilterTool_H

#include "PrimaryDPDMaker/TimingFilterInformation.h"
#include "AsgTools/IAsgTool.h"
#include "StoreGate/ReadHandle.h"
#include "LArRecEvent/LArCollisionTime.h"


class ICaloTimeFilterTool : virtual public asg::IAsgTool {

     ASG_TOOL_INTERFACE (ICaloTimeFilterTool)

   virtual  ~ICaloTimeFilterTool()= default;
   
    
   virtual StatusCode getTimeDifference(TimingFilterInformation& time_info, const SG::ReadHandleKey<LArCollisionTime>& read_key,
                                         const EventContext& ctx) const = 0;

    virtual StatusCode getTimeDifference(TimingFilterInformation& time_info) = 0;


};

   


#endif

