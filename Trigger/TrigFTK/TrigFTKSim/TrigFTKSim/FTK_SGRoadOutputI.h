/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_SGROADOUTPUTI_H
#define FTK_SGROADOUTPUTI_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_FTK_SGRoadOutputI("FTK_SGRoadOutputI",1,0);

class FTKRoadOutput;

class FTK_SGRoadOutputI : virtual public IAlgTool 
{      
 public:
  /** access to tool interface */
  static const InterfaceID& interfaceID();

  //functions that can be accessed by a tool handle
  virtual FTKRoadOutput* reference()=0; // get the reference to the FTKDataInput object
};

inline const InterfaceID& FTK_SGRoadOutputI::interfaceID()
{
  return IID_FTK_SGRoadOutputI;
}

#endif // FTK_SGROADOUTPUTI_H
