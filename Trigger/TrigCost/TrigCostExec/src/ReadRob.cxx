/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/NtupleSvc.h"

// Local
#include "TrigCostExec/RobService.h"
#include "TrigCostExec/ReadRob.h"

REGISTER_ANP_OBJECT(AlgEvent,ReadRob)

using namespace std;

//-----------------------------------------------------------------------------
Anp::ReadRob::ReadRob()
{
}

//-----------------------------------------------------------------------------
Anp::ReadRob::~ReadRob()
{
}

//-----------------------------------------------------------------------------
void Anp::ReadRob::Config(const Registry &reg)
{
  //
  // Read configuration and setup ntuple branches
  //
  reg.Get("ReadRob", "Debug", fDebug);

  //
  // This is needed by StudyTime for ROBID<-> SUBDET map is read from ntuple
  //
  RobService::Instance().Config(reg);
  
  //
  // Book input data for trigger configuration
  //
  fConfig = NtupleSvc::Instance("config").Branch<TrigMonConfig>("conf"); 
}

//-----------------------------------------------------------------------------
bool Anp::ReadRob::Init()
{  
  return true;
}

//-----------------------------------------------------------------------------
void Anp::ReadRob::Exec()
{
  //
  // Process one event from ntuple
  //
  if(fConfig.valid() && fConfig.GetState() == Ntuple::kREAD) {
    RobService::Instance().AddConfig(fConfig.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::ReadRob::Done()
{
}

