// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LinkDef.h 617991 2014-09-22 13:59:57Z krasznaa $
#ifndef TRIGANALYSISINTERFACES_LINKDEF_H
#define TRIGANALYSISINTERFACES_LINKDEF_H

// Local include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "TrigAnalysisInterfaces/IBunchCrossingConfProvider.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

#pragma link C++ class Trig::IBunchCrossingTool+;
#pragma link C++ class Trig::IBunchCrossingConfProvider+;

#endif // __CINT__
#endif // not TRIGANALYSISINTERFACES_LINKDEF_H
