/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAY_RTTMONITORING_RTTMON_H
#define OVERLAY_RTTMONITORING_RTTMON_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : rttmon.h
/// Package : offline/Event/EventOverlay/OverlayRTTMonitoring
/// Author  : Piyali Banerjee; UdeM
///           Updated by G.Redlinger May 2012 to be compatible with
///            deprecation of CBNT.  Use AthAlgorithm instead.
///            For now, rttmon is an empty algorithm....
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"


class rttmon : public AthAlgorithm {
  
 public:

  rttmon(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode beginRun();
  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
  StatusCode initEvent();


 private:


	 int m_nevt;
	 int m_NInTimeEvents;
	 int m_NInTimePileup;
	 
};

#endif

