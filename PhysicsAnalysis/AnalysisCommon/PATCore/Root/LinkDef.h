/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __PATCORE__
#define __PATCORE__


#include <vector>
#include <string>
#include <bitset>
#include "TString.h"
#include "PATCore/TAccept.h"
#include "PATCore/TSelectorToolBase.h"
#include "PATCore/TResult.h"
#include "PATCore/TCalculatorToolBase.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace Root ;

#pragma link C++ class Root::TAccept+ ;
#pragma link C++ class Root::TSelectorToolBase+ ;
#pragma link C++ class Root::TResult+ ;
#pragma link C++ class Root::TCalculatorToolBase+ ; 

#endif

#endif

