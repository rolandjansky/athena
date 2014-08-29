/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UserProcessDefinition_H
#define UserProcessDefinition_H

#include "FadsPhysics/UserPhysicsDefinitionProxy.h"

#define USERPROCESSDEF(CLTYPE) static UserPhysicsDefinitionProxy<#CLTYPE> plugin;

#endif // UserProcessDefinition_H
