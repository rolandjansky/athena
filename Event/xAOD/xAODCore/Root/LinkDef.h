// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LinkDef.h 666726 2015-05-13 02:19:48Z ssnyder $
#ifndef XAODCORE_LINKDEF_H
#define XAODCORE_LINKDEF_H

// Local include(s):
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/PerfStats.h"
//#include "xAODCore/tools/PrintHelpers.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

#pragma link C++ class xAOD::BranchStats+;
#pragma link C++ class xAOD::ReadStats+;
#pragma link C++ class map<string,xAOD::BranchStats>+;
#pragma link C++ class pair<string,xAOD::BranchStats>+;
#pragma link C++ class xAOD::PerfStats+;

#ifdef XAOD_STANDALONE
#pragma link C++ class vector<vector<float> >+;
#pragma link C++ class vector<vector<int> >+;
#endif

//#pragma link C++ function xAOD::dump+;

#endif // __CINT__
#endif // XAODCORE_LINKDEF_H
