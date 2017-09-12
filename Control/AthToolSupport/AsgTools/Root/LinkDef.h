/// Dear emacs, this is -*- c++ -*-
/// $Id: LinkDef.h 804869 2017-05-15 20:14:34Z krumnack $
///
/// @file LinkDef.h
/// @brief CINT dictionary description for standalone compilation
///
/// This file is used exclusively in standalone compilation to generate
/// dictionaries for types that in Athena are defined in other packages, and
/// hence get their dictionaries from elsewhere. As a result, the Athena
/// compilation must not use this file to generate a dictionary.
///
#ifndef ASGTOOLS_LINKDEF_H
#define ASGTOOLS_LINKDEF_H

// Local include(s):
#include "AsgTools/MsgLevel.h"
#include "AsgTools/MsgStream.h"
#include "AsgTools/INamedInterface.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace MSG+;
#pragma link C++ struct MSG::DummyStruct+;
#pragma link C++ enum MSG::Level+;
#pragma link C++ function MSG::name+;
#pragma link C++ class MsgStream+;
#pragma link C++ class INamedInterface+;

#endif // __CINT__
#endif // not ASGTOOLS_LINKDEF_H
