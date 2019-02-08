/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <EventLoopGrid/GridWorker.h>
#include <EventLoopGrid/GridDriver.h>
#include <EventLoopGrid/PrunDriver.h>
#include <EventLoopGrid/PandaRootTools.h>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace EL;

#pragma link C++ class EL::GridWorker+;
#pragma link C++ class EL::GridDriver+;
#pragma link C++ class EL::PrunDriver+;
#pragma link C++ class PandaRootTools+;
#pragma link C++ function EL::getRootCoreConfig ();

#endif
