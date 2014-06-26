/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TAUDISCRIMINANT__
#define __TAUDISCRIMINANT__

#include "TauDiscriminant/MethodBase.h"
#include "TauDiscriminant/MethodBDT.h"
#include "TauDiscriminant/MethodCuts.h"
#include "TauDiscriminant/MethodDummy.h"
#include "TauDiscriminant/MethodLLH.h"
#include "TauDiscriminant/MethodTransform.h"
#include "TauDiscriminant/Types.h"
#include "TauDiscriminant/TauIDReader.h"
#include "TauDiscriminant/TauDetailsManagerStandalone.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace TauID;
#pragma link C++ namespace TauID::Types;

#pragma link C++ enum TauID::Types::MethodType;
#pragma link C++ class TauID::TauIDReader;
#pragma link C++ class TauID::TauDetailsManagerStandalone;
#pragma link C++ class TauID::MethodBase;
#pragma link C++ class TauID::MethodDummy;
#pragma link C++ class TauID::MethodTransform;
#pragma link C++ class TauID::MethodBDT;
#pragma link C++ class TauID::MethodCuts;
#pragma link C++ class TauID::MethodLLH;

#endif
#endif
