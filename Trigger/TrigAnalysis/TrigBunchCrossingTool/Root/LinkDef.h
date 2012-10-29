// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LinkDef.h 513171 2012-08-10 09:41:46Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_LINKDEF_H
#define TRIGBUNCHCROSSINGTOOL_LINKDEF_H

// Local include(s):
#include "../TrigBunchCrossingTool/BunchCrossingToolBase.h"
#include "../TrigBunchCrossingTool/D3PDBunchCrossingToolSA.h"
#include "../TrigBunchCrossingTool/StaticBunchCrossingToolSA.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

// Extra dictionary for reading the configuration data:
#pragma link C++ class vector<float>+;

// Dictionaries for the concrete tools:
#pragma link C++ class Trig::BunchCrossingToolBase+;
#pragma link C++ class Trig::D3PDBunchCrossingToolSA+;
#pragma link C++ class Trig::StaticBunchCrossingToolSA+;

#endif // __CINT__
#endif // TRIGBUNCHCROSSINGTOOL_LINKDEF_H
