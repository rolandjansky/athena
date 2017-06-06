/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/SystematicEvent.h"
#include "TopEvent/KLFitterResult.h"
#include "TopEvent/PseudoTopResult.h"

#ifdef __CINT__
#pragma extra_include "TopEvent/SystematicEvent.h";
#pragma extra_include "TopEvent/KLFitterResult.h";
#pragma extra_include "TopEvent/PseudoTopResult.h";

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class xAOD::SystematicEvent+;
#pragma link C++ class xAOD::SystematicEventContainer+;
#pragma link C++ class xAOD::SystematicEventAuxContainer+;

#pragma link C++ class xAOD::KLFitterResult+;
#pragma link C++ class xAOD::KLFitterResultContainer+;
#pragma link C++ class xAOD::KLFitterResultAuxContainer+;

#pragma link C++ class xAOD::PseudoTopResult+;
#pragma link C++ class xAOD::PseudoTopResultContainer+;
#pragma link C++ class xAOD::PseudoTopResultAuxContainer+;

#endif
