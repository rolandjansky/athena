/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopAnalysis/ObjectLoaderStandardCuts.h"

#include "TopAnalysis/EventSaverFlatNtuple.h"
#include "TopAnalysis/EventSaverxAOD.h"
#include "TopAnalysis/EventSaverxAODNext.h"

#ifdef __CINT__

#pragma extra_include "TopAnalysis/ObjectLoaderStandardCuts.h";
#pragma extra_include "TopAnalysis/EventSaverFlatNtuple.h";
#pragma extra_include "TopAnalysis/EventSaverxAOD.h";
#pragma extra_include "TopAnalysis/EventSaverxAODNext.h";

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

//for loading the object selection at run time
#pragma link C++ class top::ObjectLoaderBase+;
#pragma link C++ class top::ObjectLoaderStandardCuts+;

//for loading the event saver at run time
#pragma link C++ class top::EventSaverBase+;
#pragma link C++ class top::EventSaverFlatNtuple+;
#pragma link C++ class top::EventSaverxAOD+;
#pragma link C++ class top::EventSaverxAODNext+;

#endif
