// $Id: LinkDef.h 618090 2014-09-23 06:56:52Z sfarrell $
// Dear emacs, this is -*- c++ -*-

#include <vector>

#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/StreamTestWrapper.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace CP;
#pragma link C++ class CP::SystematicVariation+;
#pragma link C++ class std::set<CP::SystematicVariation>+;
#pragma link C++ class CP::SystematicSet+;
#pragma link C++ class std::vector<CP::SystematicSet>+;

// For the streaming unit-test
#pragma link C++ namespace SysStreamTest;
#pragma link C++ class SysStreamTest::StreamTestWrapper+;

#endif
