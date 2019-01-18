//==================================================================================
//
//  ZMM_Event.cxx :       Class designed to reconstruct di-boson events
//                        in particular Z0 -> mu+ mu- events.
//  Author :              Kyle Stevenson, QMUL
//  Date :                7th December 2007
//
//==================================================================================

//==================================================================================
// Include files...
//==================================================================================

// This files header
#include "InDetPerformanceMonitoring/MuonSelectorForJpsi.h"
// Package Headers
#include "InDetPerformanceMonitoring/PerfMonServices.h"
#include <sstream>
// ATLAS headers
#include "StoreGate/StoreGateSvc.h"
//#include "muonEvent/MuonParamDefs.h"
#include "CLHEP/Random/RandFlat.h"

#include "GaudiKernel/IToolSvc.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"




// Local debug variables. Global scope, but only accessible by this file.
static const float CGeV              =  1.0e-3;  // Conversion factor to remove evil MeV
                                                 // nonsense.

// Static declarations
unsigned int MuonSelectorForJpsi::s_uNumInstances;

//==================================================================================
// Public Methods
//==================================================================================

MuonSelectorForJpsi::MuonSelectorForJpsi()
{
  ++s_uNumInstances;

  std::stringstream xTmp;  xTmp << s_uNumInstances;
  m_xSampleName     = "MuID_" + xTmp.str();

  m_pxMuon = NULL;
  m_bLock  = false;

  m_coneSize        = 0.4;
  m_ucJMuon_Cut     = 0; //not used
  m_IsoCut          = 0.2;

  m_bCutOnCombKine  = false; //not used
  m_fEtaCut         = 2.5;
  //m_fEtaCut         = 1.9;
  m_combPtCut       = 1.0*CLHEP::GeV; // GeV/c //Lower than for the Zmumu case

  //m_ptMSCut         = 10.0*CLHEP::GeV;
  m_ptMSCut         = 0.0*CLHEP::GeV;
  m_diffZCut        = 10.0*CLHEP::mm;  // mm
  m_diffPtCut       = 15.0*CLHEP::GeV;  // not used
  m_pVZCut          = 150.0*CLHEP::mm; // mm

  m_fIDPtCut        = 0.0;     // GeV/c
  m_fIDChiPerDofCut = 4.0;
  m_IDSiHitsCut     = 8;
  m_ucID_PIXCut     = 1;        // Hits
  m_ucID_SCTCut     = 4;        // Hits
  m_ucID_TRTCut     = 0;        // Hits

  m_doDebug         = false;
  m_doQualSelection = true;
  m_doIsoSelection  = false;
  m_doPtSelection   = true;
  m_doIPSelection   = true;

  m_msgStream =  new MsgStream(PerfMonServices::getMessagingService(), "InDetPerformanceMonitoring" );

}




MuonSelectorForJpsi::~MuonSelectorForJpsi()
{
  --s_uNumInstances;
  delete m_msgStream;
}

bool MuonSelectorForJpsi::passSelection( const xAOD::Muon* pxMuon )
{

  std::vector<bool> passes;
  bool pass = true;
  if ( pxMuon )
    {
      // Save local copy of muon address if it's ok.
      m_pxMuon = pxMuon;
      // Test muon pass conditions in turn
      if(m_doQualSelection){
	pass = passQualCuts();
	passes.push_back(pass);
      }

      if (m_doIsoSelection){
        pass = passIsolCuts();
        passes.push_back(pass);
      }

      if (m_doPtSelection){
        pass = passPtCuts();
	passes.push_back(pass);
      }

      if (m_doIPSelection){
        pass = passIPCuts();
        passes.push_back(pass);
      }

      for (int i=0; i < int(passes.size()); i++)
	if (false == passes[i]){
	  if(m_doDebug) std::cout << " haven't passed the " << i <<"th selection " << std::endl;
	  return false;
	}

    }
  return true;
}



void MuonSelectorForJpsi::Init()
{
  ISvcLocator* serviceLocator = Gaudi::svcLocator();
  IToolSvc* toolSvc;
  StatusCode sc = serviceLocator->service("ToolSvc", toolSvc, true);

  if ( sc.isFailure() || toolSvc == 0 ) {
    (*m_msgStream) << MSG::ERROR << "Unable to retrieve ToolSvc " << endreq;
    return;
  }
  PARENT::Init();
}



bool MuonSelectorForJpsi::Reco()
{
  return true;
}

//==================================================================================
// Protected Methods
//==================================================================================
void MuonSelectorForJpsi::BookHistograms( )
{
}


//==================================================================================
// Private Methods
//==================================================================================
bool MuonSelectorForJpsi::passQualCuts()
{
  // Apply muon hit criteria
  // First get the muon track, then the summarys
  const xAOD::TrackParticle* IDTrk = m_pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);

  //    if ( !m_pxMuon->isCombinedMuon()) return false;
  //  if ( m_pxMuon->trackParticle(xAOD::Muon::CombinedTrackParticle) ) return false;
  if( xAOD::Muon::Combined ) return false;
  if (IDTrk) {
    uint8_t dummy(-1);
    bool eBLhits = IDTrk->summaryValue( dummy, xAOD::expectBLayerHit )? dummy :-1;
    int  nBLhits = IDTrk->summaryValue( dummy, xAOD::numberOfBLayerHits )? dummy :-1;

    int nhitsPIX = IDTrk->summaryValue( dummy, xAOD::numberOfPixelHits )? dummy :-1;
    int nhitsSCT = IDTrk->summaryValue( dummy, xAOD::numberOfSCTHits )? dummy :-1;

    int nPIXDS = IDTrk->summaryValue( dummy, xAOD::numberOfPixelDeadSensors )? dummy :-1;
    int nSCTDS = IDTrk->summaryValue( dummy, xAOD::numberOfSCTDeadSensors )? dummy :-1;

    int nPIXH = IDTrk->summaryValue( dummy, xAOD::numberOfPixelHoles )? dummy :-1;
    int nSCTH = IDTrk->summaryValue( dummy, xAOD::numberOfSCTHoles )? dummy :-1;

    if(m_doDebug) std::cout << " eBLhits: " << eBLhits << " nBLhits:  " << nBLhits  << " nhitsPIX: " << nhitsPIX << std::endl;

    if (   ((!eBLhits) || (nBLhits > 0))
	   &&   (nhitsPIX + nPIXDS > 1 )
	   &&   (nhitsSCT + nSCTDS >=6 )
	   &&   (nPIXH    + nSCTH  < 2 ) )
      return true;
  }


if(m_doDebug) std::cout << "this muon not pass the hits number QualCuts:  "  << std::endl;
return false;
}

bool MuonSelectorForJpsi::passPtCuts()
{

  const xAOD::TrackParticle* pxMuonID = m_pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  const xAOD::TrackParticle* pxMuonMS = m_pxMuon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
  if ( !(pxMuonID && pxMuonMS)){
         if(m_doDebug) std::cout << "NO inDetTrackParticle && muonSpectrometerTrackParticle: " << std::endl;
  }

  else {

    double pt    = m_pxMuon->pt();
    double ptID  = pxMuonID->pt();
    double ptMS  = pxMuonMS->pt();
    double fMEta = fabs( m_pxMuon->eta() );

    if(m_doDebug) std::cout <<" in passPtCuts: "<< " pt: "  << pt << " ptID: " << ptID <<" ptMS:  " << ptMS << " fMEta: "<< fMEta << std::endl;


    if (  fMEta < m_fEtaCut    &&
    	  pt    > m_combPtCut  &&
    	  ptMS  > m_ptMSCut
    	  //fabs(ptMS - ptID) < m_diffPtCut
    	  ){
             if(m_doDebug) std::cout << " this muon passed the PtCuts:" << std::endl;
             return true;
    }
  }

  (*m_msgStream) << MSG::DEBUG << " this muon did not pass the PtCuts:" << endreq;
  return false;
}

bool MuonSelectorForJpsi::passIsolCuts()
{
  const  xAOD::TrackParticle* pxMuonID = m_pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  if (pxMuonID) {
    float iso_pt40(0);
    if( !m_pxMuon->isolation(iso_pt40, xAOD::Iso::ptcone40) ) {
      std::cout << "No isolation variable stored on the muon" << std::endl;
      if(m_doDebug) std::cout << " this muon did not pass the IsoCuts:" << std::endl;
      return false;
    }
    else {
      double pt = m_pxMuon->pt();
      double ptSum = xAOD::Iso::ptcone40;
      if(m_doDebug) std::cout <<" in passIsolCuts , pt :" << pt <<" ptSum: "<< ptSum << std::endl;
        if (ptSum/pt < m_IsoCut ){
         if(m_doDebug) std::cout << " this muon passed the IsoCuts:" << std::endl;
       return true;
     }
    }
  }
  if(m_doDebug) std::cout << " this muon did not pass the IsoCuts:" << std::endl;
  return false;
}


bool MuonSelectorForJpsi::passIPCuts()
{
  float extd0 = 0.0 ;
  float extz0 = 0.0 ;

  if (m_pxMuon->inDetTrackParticleLink().isValid()) {
    const xAOD::TrackParticle* IDTrk = m_pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    extd0 = IDTrk->d0();
    extz0 = IDTrk->z0()+IDTrk->vz();
    if(m_doDebug){
      std::cout << " the IDTrack muon d0:  " << extd0 << std::endl;
      std::cout << " the IDTrack muon z0:  " << extz0 << std::endl;
    }
  }
  else
    return false;

  const xAOD::VertexContainer *  vxContainer(0);
  vxContainer = PerfMonServices::getContainer<xAOD::VertexContainer>( PerfMonServices::VTX_COLLECTION );
  if (!vxContainer){
    if(m_doDebug) std::cout << " NO vertex collection "<< std::endl;
    return false;
  }

  if ( vxContainer->size()>1 ) {
    const xAOD::Vertex* PV  = (*vxContainer)[0];
    Amg::Vector3D newPos = PV->position();
    if(m_doDebug) std::cout << " the PV of this event: " << newPos << std::endl;

    if ( PV->nTrackParticles() > 2         &&
	 fabs(newPos.z())              < m_pVZCut   &&
    	 fabs(extz0 - newPos.z())      < m_diffZCut    ){
      if(m_doDebug) std::cout <<" this muon passed the IPCuts: " << std::endl;
      return true;
    }
  }
  if(m_doDebug) std::cout <<" this muon not passed the IPCuts: " << std::endl;
  return false;
}
