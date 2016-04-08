/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VertexCommonFlags                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: August 2008                              //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1VertexSystems/VertexCommonFlags.h"

//____________________________________________________________________
QString VertexCommonFlags::toString(const QUANTITY& q)
{
  switch (q) {
  case ENERGY: return "ENERGY";
  case MOMENTUM: return "MOMENTUM";
  case TRANSVERSE_MOM: return "TRANSVERSE_MOM";
  case MASS: return "MASS";
  default: return "Unknown (ERROR)";
  }
}
