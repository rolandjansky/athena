/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGTRTTRACKFOLLOWING_H__ 
#define __ITRIGTRTTRACKFOLLOWING_H__ 

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

static const InterfaceID IID_ITrigTRT_TrackExtensionTool("ITrigTRT_TrackExtensionTool", 1 , 0); 

class ITrigTRT_TrackExtensionTool: virtual public IAlgTool 
{
 public:

  static const InterfaceID& interfaceID() {
    return IID_ITrigTRT_TrackExtensionTool;
  }
  virtual StatusCode propagate(TrigInDetTrackCollection*) = 0;

  virtual const std::vector<int>* fillTRT_DataErrors() = 0;

  virtual double getTRTTotalTime() const = 0;
  virtual double getTRTTrtCnvTime() const = 0;
  virtual double getTRTRobProvTime() const = 0;
  virtual double getTRT_DataProviderTime(void) const = 0;
  virtual double getTRT_RobPreloaderTime(void) const = 0;
  virtual double getTRT_RoadBuilderTime(void) const = 0;
  virtual double getTRT_PDAF_TrackingTime(void) const = 0;
};

#endif
