/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "version.h"

const char* TrigT2BeamSpot_version::operator()() const
{
  static const char* version = "$Id";
  return version;
}
