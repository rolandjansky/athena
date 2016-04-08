// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENSHAPEINTERFACE_STATICCONSTANTS_H
#define EVENSHAPEINTERFACE_STATICCONSTANTS_H

#include <string>

namespace EventShapeConstants_Values
{
  static double invalidValue(-1.);
  static double nullValue(0.);
  static const std::string invalidKey("INVALID");
  static const std::string defaultStateKey("CALIBRATED");
}

struct EventShapeConstants
{
  static double invalidValue() 
  { return EventShapeConstants_Values::invalidValue; }
  static double nullValue()    
  { return EventShapeConstants_Values::nullValue; }
  static const std::string& invalidKey() 
  { return EventShapeConstants_Values::invalidKey;}
  static const std::string& defaultStateKey() 
  { return EventShapeConstants_Values::defaultStateKey; }
};

#endif
