// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENSHAPEINTERFACE_STATICCONSTANTS_H
#define EVENSHAPEINTERFACE_STATICCONSTANTS_H

#include <string>

namespace EventShapeConstants_Values
{
  static double _invalidValue(-1.);
  static double _nullValue(0.);
  static const std::string _invalidKey("INVALID");
  static const std::string _defaultStateKey("CALIBRATED");
}

struct EventShapeConstants
{
  static double invalidValue() 
  { return EventShapeConstants_Values::_invalidValue; }
  static double nullValue()    
  { return EventShapeConstants_Values::_nullValue; }
  static const std::string& invalidKey() 
  { return EventShapeConstants_Values::_invalidKey;}
  static const std::string& defaultStateKey() 
  { return EventShapeConstants_Values::_defaultStateKey; }
};

#endif
