// $Id: LinkDef.h 614132 2014-08-29 17:16:45Z sfarrell $
// Dear emacs, this is -*- c++ -*-

#include <vector>

#include "PATInterfaces/SystematicSet.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class std::vector<CP::SystematicSet>+;

#endif
