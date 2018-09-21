/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UPGRADEPERFORMANCEFUNCTIONS_LINKDEF_H
#define UPGRADEPERFORMANCEFUNCTIONS_LINKDEF_H

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace Upgrade;
#pragma link C++ class Upgrade::UpgradePerformanceFunctions+;

#endif 

#endif
