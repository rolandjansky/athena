// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LinkDef.h 618129 2014-09-23 11:37:00Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_LINKDEF_H
#define TRIGBUNCHCROSSINGTOOL_LINKDEF_H

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingToolBase.h"
#include "TrigBunchCrossingTool/D3PDBunchCrossingTool.h"
#include "TrigBunchCrossingTool/StaticBunchCrossingTool.h"
#include "TrigBunchCrossingTool/WebBunchCrossingTool.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

// Extra dictionary for reading the configuration data:
#pragma link C++ class vector<float>+;

// Dictionaries for the concrete tools:
#pragma link C++ class Trig::BunchCrossingToolBase+;
#pragma link C++ class Trig::D3PDBunchCrossingTool+;
#pragma link C++ class Trig::StaticBunchCrossingTool+;
#pragma link C++ class Trig::WebBunchCrossingTool+;

#endif // __CINT__
#endif // TRIGBUNCHCROSSINGTOOL_LINKDEF_H
