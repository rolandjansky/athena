/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIG_L2_LAYER_SET_PREDICTOR_TOOL_H__
#define __ITRIG_L2_LAYER_SET_PREDICTOR_TOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetPattRecoEvent/TrigL2LayerSetLUT.h"

static const InterfaceID IID_ITrigL2LayerSetPredictorTool("ITrigL2LayerSetPredictorTool",1,0);  
  
class ITrigL2LayerSetPredictorTool: virtual public IAlgTool { 
 public: 
  static const InterfaceID& interfaceID() { 
    return IID_ITrigL2LayerSetPredictorTool; 
  } 
  virtual StatusCode createLUT() = 0;
  virtual const TrigL2LayerSetLUT* getLUT() = 0;
};

#endif
