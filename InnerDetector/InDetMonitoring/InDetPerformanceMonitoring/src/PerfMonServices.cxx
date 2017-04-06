#include "InDetPerformanceMonitoring/PerfMonServices.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

// NULL the static pointers
ISvcLocator*    PerfMonServices::s_pxSvcLocator   = NULL;
ITHistSvc*      PerfMonServices::s_pxTHistSvc     = NULL;
IMessageSvc*    PerfMonServices::s_pxMessageSvc   = NULL;
StoreGateSvc*   PerfMonServices::s_pxStoreGateSvc = NULL;
INTupleSvc*     PerfMonServices::s_pxNtupleSvc    = NULL;
IAtRndmGenSvc*  PerfMonServices::s_pxAtRndmGenSvc = NULL;

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

  //MsgStream log( s_pxMessageSvc, "Services" );
  if ( !bRegisterSuccess )
  {
    //log << MSG::WARNING
    //<< "Problem starting at least one of the Athena services : this may be cause problems downstream"
    //<< endreq;
  }

  return StatusCode::SUCCESS;
}

//===========================================================================
// Private Members
//===========================================================================

// Set of initialisation functions
bool PerfMonServices::InitialiseMessaging()
{
  StatusCode xStatus = s_pxSvcLocator->service( "MessageSvc", s_pxMessageSvc );
  return CheckServiceInit( xStatus, !s_pxMessageSvc /* , "messaging" */ );
}

bool PerfMonServices::InitialiseStoreGate()
{
  StatusCode xStatus = s_pxSvcLocator->service("StoreGateSvc", s_pxStoreGateSvc );
  return CheckServiceInit( xStatus, !s_pxStoreGateSvc /* , "StoreGateSvc" */ );
}

bool PerfMonServices::InitialiseRootTuple()
{
  StatusCode xStatus = s_pxSvcLocator->service( "NTupleSvc", s_pxNtupleSvc );
  return CheckServiceInit( xStatus, !s_pxNtupleSvc /* , "NTuple" */ );
}

bool PerfMonServices::InitialiseHistograms()
{
  StatusCode xStatus = s_pxSvcLocator->service("THistSvc", s_pxTHistSvc);
  return CheckServiceInit( xStatus, !s_pxTHistSvc /* , "Histogram" */ );
}

bool PerfMonServices::InitialiseRandom()
{
  StatusCode xStatus = s_pxSvcLocator->service( "AtRndmGenSvc", s_pxAtRndmGenSvc, true);
  return CheckServiceInit( xStatus, !s_pxAtRndmGenSvc /* , "random number" */ );
}

// Small status check utility function for the initialisers above
bool PerfMonServices::CheckServiceInit( const StatusCode& xSC, bool bNullPointer /*, const std::string& sType */ )
{
  //MsgStream log( s_pxMessageSvc, "Services" );
  if ( !xSC.isSuccess() )
  {
    //log << MSG::WARNING
    //<< "Problem starting " << sType << " service : Error Code."
    //<< endreq;
  }
  if ( bNullPointer )
  {
    //log << MSG::WARNING
    //<< "Problem starting " << sType << " service : NULL pointer."
    //<< endreq;
  }
  return ( xSC.isSuccess() && !bNullPointer ) ? true : false;
}

//===========================================================================
