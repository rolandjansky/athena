/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1GeoFlags               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: October 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1GeometrySystems/VP1GeoFlags.h"

//____________________________________________________________________
QString VP1GeoFlags::toString(const VOLSTATE& s)
{
  switch (s) {
  case CONTRACTED: return "CONTRACTED";
  case EXPANDED: return "EXPANDED";
  case ZAPPED: return "ZAPPED";
  default:
    return "<UNKNOWN>";
  }
}
