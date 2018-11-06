/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerformanceMonitoring/PerfMonServices.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

// nullptr the static pointers
ISvcLocator*    PerfMonServices::s_pxSvcLocator   = nullptr;
ITHistSvc*      PerfMonServices::s_pxTHistSvc     = nullptr;
IMessageSvc*    PerfMonServices::s_pxMessageSvc   = nullptr;
StoreGateSvc*   PerfMonServices::s_pxStoreGateSvc = nullptr;
INTupleSvc*     PerfMonServices::s_pxNtupleSvc    = nullptr;
IAtRndmGenSvc*  PerfMonServices::s_pxAtRndmGenSvc = nullptr;

// Names of the various containers.
const std::string PerfMonServices::s_sContainerNames[NUM_CONTAINERS] = { "MuidMuonCollection",
									  "StacoMuonCollection",
									  "Muons",
									  "ElectronCollection",
									  "PhotonCollection",
									  "MET_Final",
									  "TrackParticleCandidate",
									 //	 "VxPrimaryCandidate"};
									 "PrimaryVertices"};
const std::string PerfMonServices::s_sAtlfastContainerNames[NUM_CONTAINERS] = { "AtlfastMuonCollection",
										 "AtlfastMuonCollection",
										 "AtlfastMuons",
										 "AtlfastElectronCollection",
										 "AtlfastPhotonCollection",
										 "AtlfastMissingEt",
										 "Placeholder"                };

//===========================================================================
// Constructors & destructors
//===========================================================================

PerfMonServices::PerfMonServices()
{
}

PerfMonServices::~PerfMonServices()
{
}

//===========================================================================
// Public Members
//===========================================================================

StatusCode PerfMonServices::InitialiseServices( ISvcLocator* pxSvcLocator )
{
  s_pxSvcLocator = pxSvcLocator;

  // Init. the Athena services that will be used in PhysAna.
  // Init. Messaging first (the others use it for output).
  bool bRegisterSuccess = true;
  bRegisterSuccess &= InitialiseMessaging();
  bRegisterSuccess &= InitialiseStoreGate();
  bRegisterSuccess &= InitialiseRootTuple();
  bRegisterSuccess &= InitialiseHistograms();
  bRegisterSuccess &= InitialiseRandom();

  return StatusCode::SUCCESS;
}

//===========================================================================
// Private Members
//===========================================================================

// Set of initialisation functions
bool PerfMonServices::InitialiseMessaging()
{
  StatusCode xStatus = s_pxSvcLocator->service( "MessageSvc", s_pxMessageSvc );
  return CheckServiceInit( xStatus, !s_pxMessageSvc );
}

bool PerfMonServices::InitialiseStoreGate()
{
  StatusCode xStatus = s_pxSvcLocator->service("StoreGateSvc", s_pxStoreGateSvc );
  return CheckServiceInit( xStatus, !s_pxStoreGateSvc );
}

bool PerfMonServices::InitialiseRootTuple()
{
  StatusCode xStatus = s_pxSvcLocator->service( "NTupleSvc", s_pxNtupleSvc );
  return CheckServiceInit( xStatus, !s_pxNtupleSvc );
}

bool PerfMonServices::InitialiseHistograms()
{
  StatusCode xStatus = s_pxSvcLocator->service("THistSvc", s_pxTHistSvc);
  return CheckServiceInit( xStatus, !s_pxTHistSvc );
}

bool PerfMonServices::InitialiseRandom()
{
  StatusCode xStatus = s_pxSvcLocator->service( "AtRndmGenSvc", s_pxAtRndmGenSvc, true);
  return CheckServiceInit( xStatus, !s_pxAtRndmGenSvc);
}

// Small status check utility function for the initialisers above
bool PerfMonServices::CheckServiceInit( const StatusCode& xSC, bool bNullPointer )
{
  return ( xSC.isSuccess() and not bNullPointer );
}

//===========================================================================
