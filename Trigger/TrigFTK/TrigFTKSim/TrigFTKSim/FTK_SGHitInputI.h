/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_SGHITINPUTI_H
#define FTK_SGHITINPUTI_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigFTKSim/FTKPMap.h"

static const InterfaceID IID_FTK_SGHitInputI("FTK_SGHitInputI",1,0);

class FTKDataInput;

class FTK_SGHitInputI : virtual public IAlgTool 
{      
 public:
  /** access to tool interface */
  static const InterfaceID& interfaceID();

  //functions that can be accessed by a tool handle
  //and will be implimented in class(es) that inherit
  //from IMyAlgTool
  virtual int readData()=0;  //is a pure virtual class
  virtual FTKDataInput* reference()=0; // get the reference to the FTKDataInput object
};

inline const InterfaceID& FTK_SGHitInputI::interfaceID()
{
  return IID_FTK_SGHitInputI;
}

#endif // FTK_SGHITINPUTI_H
