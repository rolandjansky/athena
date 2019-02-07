/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PATCORE_LINKDEF_H
#define PATCORE_LINKDEF_H


#include <vector>
#include <string>
#include <bitset>
#include "TString.h"
#include "PATCore/TSelectorToolBase.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace Root ;

#pragma link C++ class Root::TSelectorToolBase+ ;

#endif

#endif

