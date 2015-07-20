/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __JER__
#define __JER__

#include "JetResolution/JERProvider.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all defined_ines; // What does this do??
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class JERProvider+;
// Can't currently generate dictionaries for CP tools with EDM dependencies
//#pragma link C++ class JERTool+;
//#pragma link C++ class JERSmearingTool+;

#endif

#endif

