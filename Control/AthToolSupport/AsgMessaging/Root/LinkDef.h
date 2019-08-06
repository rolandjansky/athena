/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @file LinkDef.h
/// @brief CINT dictionary description for standalone compilation
///
/// This file is used exclusively in standalone compilation to generate
/// dictionaries for types that in Athena are defined in other packages, and
/// hence get their dictionaries from elsewhere. As a result, the Athena
/// compilation must not use this file to generate a dictionary.
///
#ifndef ASGMESSAGING_LINKDEF_H
#define ASGMESSAGING_LINKDEF_H

// Local include(s):
#include "AsgMessaging/MsgLevel.h"
#include "AsgMessaging/MsgStream.h"
#include "AsgMessaging/INamedInterface.h"

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
#endif // not ASGMESSAGING_LINKDEF_H
