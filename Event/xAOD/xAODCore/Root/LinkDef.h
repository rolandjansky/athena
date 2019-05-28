// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCORE_LINKDEF_H
#define XAODCORE_LINKDEF_H
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

#pragma link C++ class xAOD::BranchStats+;
#pragma link C++ class xAOD::ReadStats+;
#pragma link C++ class unordered_map<string,xAOD::BranchStats>+;
#pragma link C++ class pair<string,xAOD::BranchStats>+;
#pragma link C++ class xAOD::PerfStats+;

#ifdef XAOD_STANDALONE
#pragma link C++ class vector<vector<float> >+;
#pragma link C++ class vector<vector<int> >+;
#endif

#endif // __CINT__
#endif // XAODCORE_LINKDEF_H
