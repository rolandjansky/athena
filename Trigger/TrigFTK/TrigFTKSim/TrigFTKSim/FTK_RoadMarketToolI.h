/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_ROADMARKETTOOLI_H
#define FTK_ROADMARKETTOOLI_H

#include "GaudiKernel/IAlgTool.h"

#include <vector>

static const InterfaceID IID_FTK_RoadMarketToolI("FTK_RoadMarketToolI",1,0);

class FTKRoadOutput;
class FTKRoadInput;
class TTree;

class FTK_RoadMarketToolI : virtual public IAlgTool 
{      
 public:
  /** access to tool interface */
  static const InterfaceID& interfaceID();

  //functions that can be accessed by a tool handle
  virtual FTKRoadOutput* outputReference()=0; // get the reference to the FTKDataInput object
  virtual FTKRoadInput* inputReference()=0; // get the reference to the FTKDataInput object
  virtual void ConnectRoads(TTree*,const char *,const std::vector<int>&, int bs=0)=0;
};

inline const InterfaceID& FTK_RoadMarketToolI::interfaceID()
{
  return IID_FTK_RoadMarketToolI;
}

#endif // FTK_SGROADOUTPUTI_H
