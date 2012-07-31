// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LinkDef.h 457108 2011-09-05 09:14:09Z krasznaa $
#ifdef __CINT__

#include "../TrigAnalysisInterfaces/IIBunchCrossingTool.h"

// This pretty empty LinkDef file is here to create a small RootCore
// library out of this interface package. This is just a hack,
// in the future a better solution might be found. (This dictionary
// is not necessarily needed...)

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

#pragma link C++ class Trig::IIBunchCrossingTool+;

#endif // __CINT__
