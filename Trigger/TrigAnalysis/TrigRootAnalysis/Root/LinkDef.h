// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LinkDef.h 747438 2016-05-16 09:56:25Z tamartin $
#ifndef TRIGROOTANALYSIS_LINKDEF_H
#define TRIGROOTANALYSIS_LINKDEF_H

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

// Dictionaries for the interface classes:
#pragma link C++ class D3PD::IITrigConfigSvcD3PD+;
#pragma link C++ class D3PD::IITrigDecisionToolD3PD+;
#pragma link C++ class D3PD::Trig::IDataAccess+;
#pragma link C++ class D3PD::Trig::IConfigAccess+;

// Dictionaries for the classes implemented in this library:
#pragma link C++ class D3PD::TrigConfigSvcD3PD+;
#pragma link C++ class D3PD::Trig::DataAccess+;
#pragma link C++ class D3PD::Trig::ConfigAccess+;
#pragma link C++ class D3PD::Trig::ChainGroupHandling+;
#pragma link C++ class D3PD::Trig::TrigDecisionFunctions+;
#pragma link C++ class D3PD::TrigDecisionToolD3PD+;
#pragma link C++ class D3PD::PyTrigDecisionToolD3PD+;
#pragma link C++ class D3PD::ChainGroup+;

// Other constructs:
#pragma link C++ enum D3PD::TrigDefs::DecisionTypes+;

// Additional dictionaries for reading the D3PDs. ROOT says that it doesn't
// need some of these, but I beg to differ. I've seen some weird crashes without
// these in some test jobs...
#ifdef XAOD_STANDALONE
// In AtlasSTLAddReflex
#pragma link C++ class map<string,string>+;
#endif

// [TrigMonConfig] addind some more dictionaries for additional structures used saving the full trig conf.
// Will not run without these.
#ifdef XAOD_STANDALONE
// In AtlasSTLAddReflex
#pragma link C++ class vector<vector<float> >+;
#pragma link C++ class vector<vector<string> >+;
#elif defined ROOTCORE
#pragma link C++ class vector<vector<float> >+;
#pragma link C++ class vector<vector<string> >+;
#endif

#endif // __CINT__
#endif // TRIGROOTANALYSIS_LINKDEF_H
