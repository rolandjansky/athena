/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIG_L2_LAYER_NUMBER_TOOL_H__
#define __ITRIG_L2_LAYER_NUMBER_TOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetPattRecoEvent/TrigInDetSiLayer.h"
#include <vector>


/** @class ITrigL2LayerNumberTool    
provides the abstract interface for the silicon layer number tool
@author D.Emeliyanov <http://consult.cern.ch/xwho>
*/

static const InterfaceID IID_ITrigL2LayerNumberTool("ITrigL2LayerNumberTool",1,0);

class ITrigL2LayerNumberTool : virtual public IAlgTool {
 public:
  /** other standard AlgTool methods */
  
  static const InterfaceID& interfaceID () {  //!< the Tool's interface
    return IID_ITrigL2LayerNumberTool; 
  }  	
  
  virtual int maxSiliconLayerNum() = 0;
  virtual int offsetEndcapPixels() = 0;
  virtual int offsetBarrelSCT()    = 0;
  virtual int offsetEndcapSCT()    = 0;
  virtual void report()            = 0;//prints out the above 

  virtual int maxNumberOfUniqueLayers()  = 0;
  virtual const std::vector<short>* pixelLayers()  = 0; // hashId addressable arrays of layer numbers
  virtual const std::vector<short>* sctLayers()  = 0; // hashId addressable arrays of layer numbers
  virtual const std::vector<TRIG_INDET_SI_LAYER>* layerGeometry() = 0;
};

#endif
