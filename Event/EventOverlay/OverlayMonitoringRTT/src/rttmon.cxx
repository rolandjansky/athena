/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////
/// Author: Piyali Banerjee; UdeM
/// Date:   April 2009  (Updated by G.Redlinger May 2012)
/// See rttmon.h for details
/////////////////////////////////////////////////////////////

#include "OverlayMonitoringRTT/rttmon.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"



//Constructor

rttmon::rttmon(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_nevt(0),
  m_NInTimeEvents(0),
  m_NInTimePileup(0)
  {
}

StatusCode rttmon::beginRun()
{
  StatusCode sc = StatusCode::SUCCESS;

  return sc;
}

StatusCode rttmon::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;

  return sc;
}

StatusCode rttmon::finalize()
{
  StatusCode sc = StatusCode::SUCCESS;

  return sc;
}

StatusCode rttmon::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  return sc;
}

StatusCode rttmon::initEvent()
{
  StatusCode sc = StatusCode::SUCCESS;

  return sc;
}


