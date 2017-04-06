/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//==================================================================================

//==================================================================================
// Include files...
//==================================================================================

// This files header
#include "InDetPerformanceMonitoring/ZmumuEvent.h"

// Standard headers

// Package Headers
#include "InDetPerformanceMonitoring/PerfMonServices.h"

// ATLAS headers
#include "StoreGate/StoreGateSvc.h"

//#include "muonEvent/MuonParamDefs.h"

#include "CLHEP/Random/RandFlat.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
//==================================================================================
// Public Methods
//==================================================================================

ZmumuEvent::ZmumuEvent()
{
  m_xSampleName = "ZMM";

  m_container = PerfMonServices::MUID_COLLECTION;

  m_doDebug = false;

  // Setup the muon tags
  m_uMuonTags   = 2;
  m_uTrackMatch = 0;
  m_bLooseMatch = true;  // will use combined fit otherwise.
  m_etaCut      = 1.05;
}

ZmumuEvent::~ZmumuEvent()
{
}

void ZmumuEvent::Init()
{
  m_xMuonID.Init();
  PARENT::Init();
}



const std::string ZmumuEvent::getRegion() const{

  const double eta1 = fabs(m_pxRecMuon[MUON1]->eta());
  const double eta2 = fabs(m_pxRecMuon[MUON2]->eta());
  //std::cout << "  eta1: " << eta1 << std::endl;
  //std::cout << "  eta2: " << eta2 << std::endl;

  if ( eta1 < m_etaCut && eta2 < m_etaCut )
    return "BB";

  else if( (eta1 < m_etaCut && eta2 > m_etaCut) || (eta1 > m_etaCut && eta2 < m_etaCut) )
    return "BE";

  else return "EE";
}



bool ZmumuEvent::Reco()
{
  // Clear out the previous events record.
  Clear();
  //  const Analysis::MuonContainer* pxMuonContainer = PerfMonServices::getContainer<Analysis::MuonContainer>( m_container );
  const xAOD::MuonContainer* pxMuonContainer = PerfMonServices::getContainer<xAOD::MuonContainer>( m_container );
  if (!pxMuonContainer){
    //     std::cout << "Can't retrieve combined muon collection" << std::endl;
    return false;
  }
  else{
    if(m_doDebug){     std::cout << pxMuonContainer->size() << " combined muon "<<std::endl; }
    xAOD::MuonContainer::const_iterator xMuonItr  = pxMuonContainer->begin();
    xAOD::MuonContainer::const_iterator xMuonItrE  = pxMuonContainer->end();
      while ( xMuonItr != xMuonItrE )
	{
	  const xAOD::Muon* pxCMuon = *xMuonItr;
	  // Apply muon cuts
	  if ( m_xMuonID.passSelection( pxCMuon ) ) {
	       RecordMuon( pxCMuon );
	  }
	  xMuonItr++;
	}
  }


  // Reconstruct the invariant mass ( based on mu-sys pt ).
  ReconstructKinematics();

  m_passedSelectionCuts = EventSelection();
  return m_passedSelectionCuts;
}



//==================================================================================
// Protected Methods
//==================================================================================
void ZmumuEvent::BookHistograms()
{
}

//==================================================================================
// Private Methods
//==================================================================================
bool ZmumuEvent::EventSelection()
{

  if(m_doDebug){  std::cout <<" m_uNumberOfFullPassMuons: " << m_numberOfFullPassMuons << std::endl;}
  // First require two muon-id's with cuts pre-applied.
  if ( m_numberOfFullPassMuons != 2 )    return false;

  if ( !((m_pxRecMuon[MUON1]->pt() > 20.0*CLHEP::GeV &&  m_pxRecMuon[MUON2]->pt() > 15.0*CLHEP::GeV ) ||
         (m_pxRecMuon[MUON1]->pt() > 15.0*CLHEP::GeV &&  m_pxRecMuon[MUON2]->pt() > 20.0*CLHEP::GeV )) )
    return false;

  if(m_doDebug){   std::cout <<" m_fInvariantMass[ID]:  "<< m_fInvariantMass[ID] << std::endl;}

  if ( -1 != (m_pxRecMuon[MUON1]->charge() * m_pxRecMuon[MUON2]->charge()) ){
        std::cout << "same sign event!!!" << std::endl;
       return false;
  }



  if ( m_fInvariantMass[ID]  < 60.0f  )         return false;
  if ( m_fInvariantMass[ID]  > 120.0f )         return false;
  if ( m_fMuonDispersion[ID] <  0.2f  )         return false;
  if ( getZCharge(ID) != 0            )         return false;

  return true;
}

void ZmumuEvent::Clear()
{
  m_numberOfFullPassMuons = 0;
  m_passedSelectionCuts   = false;

  for ( unsigned int u = 0; u < NUM_MUONS; ++u )
    {
      m_pxRecMuon[u]      = NULL;
      m_pxMSTrack[u]     = NULL;
      m_pxMETrack[u] = NULL;
      m_pxIDTrack[u]      = NULL;
    }
  for ( unsigned int v = 0; v < NUM_TYPES; ++v )
    {
      m_fZPt[v]            = -999.9f;
      m_fZEtaDir[v]        = -999.9f;
      m_fZPhiDir[v]        = -999.9f;
      m_fInvariantMass[v]  = -999.9f;
      m_fMuonDispersion[v] = -999.9f;
    }
}

void ZmumuEvent::RecordMuon( const xAOD::Muon* pxMuon )
{
  // This shouldn't really ever happen but just in case.
  if ( !pxMuon ) return;

  if ( m_numberOfFullPassMuons < NUM_MUONS )
    {
      // The main Muon
      m_pxRecMuon[m_numberOfFullPassMuons] = pxMuon;
      // Tracking Muon Spectrometer ( raw )
      const xAOD::TrackParticle* pxMSTrack   = pxMuon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
      m_pxMSTrack[m_numberOfFullPassMuons] = pxMSTrack;

      // Tracking ID ( fix later to include loose match track conditions )
      const xAOD::TrackParticle*  pxIDTrack  = pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
      m_pxIDTrack[m_numberOfFullPassMuons] = pxIDTrack;
    }
  ++m_numberOfFullPassMuons;
}




void ZmumuEvent::ReconstructKinematics()
{
  // Three ways. No checks here so make sure the pointers are ok before this.
  if ( m_numberOfFullPassMuons == 2 )
    {

      // Note that all the util. functions will check the pointers & return -999.9f on failure.
      m_fInvariantMass[MS]      = EvalDiMuInvMass( m_pxMSTrack[MUON1], m_pxMSTrack[MUON2] );
      m_fMuonDispersion[MS]     = EvaluateAngle(   m_pxMSTrack[MUON1], m_pxMSTrack[MUON2] );
      m_fZPt[MS]                = EvalPt(          m_pxMSTrack[MUON1], m_pxMSTrack[MUON2] );
      m_fZEtaDir[MS]            = EvalEta(         m_pxMSTrack[MUON1], m_pxMSTrack[MUON2] );
      m_fZPhiDir[MS]            = EvalPhi(         m_pxMSTrack[MUON1], m_pxMSTrack[MUON2] );

      m_fInvariantMass[CB]      = EvalDiMuInvMass( m_pxRecMuon[MUON1], m_pxRecMuon[MUON2] );
      m_fMuonDispersion[CB]     = EvaluateAngle(   m_pxRecMuon[MUON1], m_pxRecMuon[MUON2] );
      m_fZPt[CB]                = EvalPt(          m_pxRecMuon[MUON1], m_pxRecMuon[MUON2] );
      m_fZEtaDir[CB]            = EvalEta(         m_pxRecMuon[MUON1], m_pxRecMuon[MUON2] );
      m_fZPhiDir[CB]            = EvalPhi(         m_pxRecMuon[MUON1], m_pxRecMuon[MUON2] );

      m_fInvariantMass[ID]      = EvalDiMuInvMass( m_pxIDTrack[MUON1], m_pxIDTrack[MUON2]);
      m_fMuonDispersion[ID]     = EvaluateAngle(   m_pxIDTrack[MUON1], m_pxIDTrack[MUON2] );
      m_fZPt[ID]                = EvalPt(          m_pxIDTrack[MUON1], m_pxIDTrack[MUON2] );
      m_fZEtaDir[ID]            = EvalEta(         m_pxIDTrack[MUON1], m_pxIDTrack[MUON2] );
      m_fZPhiDir[ID]            = EvalPhi(         m_pxIDTrack[MUON1], m_pxIDTrack[MUON2] );
    }
}

float ZmumuEvent::getPtImbalance( ZTYPE eType )
{
  // First determine what's positive
  if ( m_numberOfFullPassMuons == 2 )
    {
      switch ( eType )
	{
	case MS :
	  {
	    return EvalPtDiff( m_pxMSTrack[MUON1], m_pxMSTrack[MUON2] );
	  }
	case ME:
	  {
	    return EvalPtDiff( m_pxMETrack[MUON1], m_pxMETrack[MUON2] );
	  }
	case CB:
	  {
	    return EvalPtDiff( m_pxRecMuon[MUON1], m_pxRecMuon[MUON2] );
	  }
	case ID:
	  {
	    return EvalPtDiff( m_pxIDTrack[MUON1], m_pxIDTrack[MUON2] );
	  }
	default:
	  return -999.0;
	}
    }
  else
    {
      return -999.0;
    }
}

int ZmumuEvent::getZCharge( ZTYPE eType )
{
  switch ( eType )
    {
    case MS :
      {
	return ( static_cast<int>( EvalCharge( m_pxMSTrack[MUON1], m_pxMSTrack[MUON2] ) ) );
      }
    case ME:
      {
	return ( static_cast<int>( EvalCharge( m_pxMETrack[MUON1], m_pxMETrack[MUON2] ) ) );
      }
    case CB:
      {
	return ( static_cast<int>( EvalCharge( m_pxRecMuon[MUON1], m_pxRecMuon[MUON2] ) ) );
      }
    case ID:
      {
	return ( static_cast<int>( EvalCharge( m_pxIDTrack[MUON1], m_pxIDTrack[MUON2] ) ) );
      }
    default:
      return -999;
    }
}

unsigned int ZmumuEvent::getPosMuon( ZTYPE eType )
{
  if ( getNumberOfTaggedMuons() != 2 ) return 999;
  if ( getZCharge(eType) != 0        ) return 999;

  switch ( eType )
    {
    case MS :
      {
	if ( !m_pxMSTrack[MUON1] || !m_pxMSTrack[MUON2] ) return 999;
	return ( static_cast<int>( m_pxMSTrack[MUON1]->charge() ) == 1  ? MUON1 : MUON2 );
      }
    case ME:
      {
	if ( !m_pxMETrack[MUON1] || !m_pxMETrack[MUON2] ) return 999;
	return ( static_cast<int>( m_pxMETrack[MUON1]->charge() ) == 1  ? MUON1 : MUON2 );
      }
    case CB:
      {
	if ( !m_pxRecMuon[MUON1] || !m_pxRecMuon[MUON2] ) return 999;
	return ( static_cast<int>( m_pxRecMuon[MUON1]->charge() ) == 1  ? MUON1 : MUON2 );
      }
    case ID:
      {
	if ( !m_pxIDTrack[MUON1] || !m_pxIDTrack[MUON2] ) return 999;
	return ( static_cast<int>( m_pxIDTrack[MUON1]->charge() ) == 1 ? MUON1 : MUON2 );
      }
    default:
      return 999;
    }
}

unsigned int ZmumuEvent::getNegMuon( ZTYPE eType )
{
  unsigned int uTmp = getPosMuon( eType );
  if ( uTmp == 999 )
    {
      return 999;
    }
  else
    {
      return ( ( uTmp == MUON1 ) ? MUON2 : MUON1 );
    }
}

const xAOD::TrackParticle*  ZmumuEvent::getLooseIDTk( unsigned int /*uPart*/ )
{
  const xAOD::TrackParticleContainer*  pxTrackContainer =
    PerfMonServices::getContainer<xAOD::TrackParticleContainer>( PerfMonServices::TRK_COLLECTION );

  if ( pxTrackContainer )
    {
      xAOD::TrackParticleContainer::const_iterator xTrkItr  = pxTrackContainer->begin();
      xAOD::TrackParticleContainer::const_iterator xTrkItrE  = pxTrackContainer->end();
      while ( xTrkItr != xTrkItrE )
	{
	  const xAOD::TrackParticle* pxTrack = *xTrkItr;
	  if(!(pxTrack->track())) continue;
	  const Trk::Track* pxTrkTrack = pxTrack->track();
	  if ( !pxTrack ) continue;
	  const Trk::Perigee* pxPerigee = pxTrkTrack->perigeeParameters() ;
	  if ( !pxPerigee ) continue;

	  // const float fTrkPt    = pxPerigee->pT()*1.0e-3;
	  // const float fTrkPtErr = fabs( pxPerigee->localErrorMatrix().error(Trk::qOverP) );
	  // const float fPtSig    = ( 1.0f / pxPerigee->pT() ) /  fTrkPtErr;  // Potential problem.
	  const float fTrkPhi   = pxPerigee->parameters()[Trk::phi];
	  const float fTrkEta   = pxPerigee->eta();

	  float fDPhi = fabs( fTrkPhi -  m_pxMETrack[MUON1]->phi() );
	  float fDEta = fabs( fTrkEta -  m_pxMETrack[MUON2]->eta() );
	  float fDR = sqrt( fDPhi*fDPhi + fDEta*fDEta );

	  if ( fDR < 0.3f )
	    {
	      return pxTrack;
	    }

	  xTrkItr++;
	}
    }
  // if ()
  return NULL;
}
