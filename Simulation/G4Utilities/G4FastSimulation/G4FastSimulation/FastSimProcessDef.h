/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FastSimProcessDef_h
#define FastSimProcessDef_h

#include "FadsPhysics/UserPhysicsDefinition.h"

class FastSimProcessDef: public UserPhysicsDefinition 
{
 public:

  FastSimProcessDef();

  void ConstructProcess();

};

#endif

