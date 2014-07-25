/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGINDETROADMAKERTOOL_H__
#define __ITRIGINDETROADMAKERTOOL_H__ 

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"

class IRoiDescriptor;
class TrigInDetRoad;


static const InterfaceID IID_ITrigInDetRoadMakerTool("ITrigInDetRoadMakerTool", 1 , 0); 


class ITrigInDetRoadMakerTool: virtual public IAlgTool {

 public:
  
  static const InterfaceID& interfaceID() {
    return IID_ITrigInDetRoadMakerTool;
  }
  
  virtual StatusCode makeRoads(std::vector<TrigInDetRoad>&, const IRoiDescriptor* ) = 0;
  
};

#endif


