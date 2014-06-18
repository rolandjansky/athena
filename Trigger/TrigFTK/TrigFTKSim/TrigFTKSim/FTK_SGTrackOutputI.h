/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_SGTRACKOUTPUTI_H
#define FTK_SGTRACKOUTPUTI_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_FTK_SGTrackOutputI("FTK_SGTrackOutputI",1,0);

class FTKTrackOutput;

class FTK_SGTrackOutputI : virtual public IAlgTool 
{      
 public:
  /** access to tool interface */
  static const InterfaceID& interfaceID();

  //functions that can be accessed by a tool handle
  virtual FTKTrackOutput* reference()=0; // get the reference to the FTKDataInput object
};

inline const InterfaceID& FTK_SGTrackOutputI::interfaceID()
{
  return IID_FTK_SGTrackOutputI;
}

#endif // FTK_SGTRACKOUTPUTI_H
