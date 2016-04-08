// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LinkDef.h 715479 2015-12-18 15:12:11Z mnowak $
#ifndef XAODCORE_LINKDEF_H
#define XAODCORE_LINKDEF_H

#ifdef ROOTCORE
// MN: includes needed by RootCore but posing problems in ROOT 6.6.1. remove protection when ROOT-7879 fixed
// Local include(s):
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/PerfStats.h"
//#include "xAODCore/tools/PrintHelpers.h"
#endif

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
