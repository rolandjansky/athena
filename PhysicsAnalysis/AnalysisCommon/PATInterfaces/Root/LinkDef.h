// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef PATINTERFACES_LINKDEF_H
#define PATINTERFACES_LINKDEF_H

// Local include(s).
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicsUtil.h"
#include "PATInterfaces/StreamTestWrapper.h"

// System include(s).
#include <set>
#include <vector>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace CP;
#pragma link C++ class CP::CorrectionCode+;
#pragma link C++ class CP::SystematicVariation+;
#pragma link C++ class std::set<CP::SystematicVariation>+;
#pragma link C++ class CP::SystematicSet+;
#pragma link C++ class std::vector<CP::SystematicSet>+;
#pragma link C++ class CP::SystematicSet::iterator;
#pragma link C++ class CP::SystematicSet::const_iterator;
#pragma link C++ class CP::SystematicRegistry;

// For the streaming unit-test
#pragma link C++ namespace SysStreamTest;
#pragma link C++ class SysStreamTest::StreamTestWrapper+;

#endif // __CINT__
#endif // not PATINTERFACES_LINKDEF_H
