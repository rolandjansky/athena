/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackCommonFlags                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackCommonFlags.h"

//____________________________________________________________________
QString TrackCommonFlags::toString(const SELECTIONMODE& sm)
{
  switch (sm) {
  case SINGLEOBJECT: return "SINGLEOBJECT";
  case MULTITRACK: return "MULTITRACK";
  case TRACKFIT: return "TRACKFIT";
  default: return "Unknown (ERROR)";
  }
}

QString TrackCommonFlags::toString(const FITTERMODE& sm)
{
  switch (sm) {
  case FROMPRDS: return "FROMPRDS";
  case REFITSINGLETRACK: return "REFITSINGLETRACK";
  case EXTENDTRACKWITHPRDS: return "EXTENDTRACKWITHPRDS";
  case COMBINETWOTRACKS: return "COMBINETWOTRACKS";
  default: return "Unknown (ERROR)";
  }
}


//____________________________________________________________________
QString TrackCommonFlags::toString(const DETAILLEVEL& dl )
{
  switch (dl) {
  case AUTO: return "AUTO";
  case SIMPLE: return "SIMPLE";
  case DETAILED: return "DETAILED";
  default: return "Unknown (ERROR)";
  }
}
