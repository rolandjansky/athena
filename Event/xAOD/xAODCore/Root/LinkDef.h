// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LinkDef.h 579782 2014-01-23 15:40:31Z krasznaa $
#ifndef XAODCORE_LINKDEF_H
#define XAODCORE_LINKDEF_H

// Local include(s):
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/PerfStats.h"

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

#pragma link C++ class vector<vector<float> >+;
#pragma link C++ class vector<vector<int> >+;

#endif // __CINT__
#endif // XAODCORE_LINKDEF_H
