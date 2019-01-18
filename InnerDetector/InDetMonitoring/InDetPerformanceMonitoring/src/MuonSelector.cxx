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
#include "InDetPerformanceMonitoring/MuonSelector.h"
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
unsigned int MuonSelector::s_uNumInstances;

//==================================================================================
// Public Methods
//==================================================================================

MuonSelector::MuonSelector()
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
  m_combPtCut       = 15.0*CLHEP::GeV; // GeV/c
  //    m_combPtCut       = .01*CLHEP::GeV; // GeV/c

  //m_ptMSCut         = 10.0*CLHEP::GeV;
  m_ptMSCut         = 0.0*CLHEP::GeV;
  m_diffZCut        = 1.5*CLHEP::mm;  // mm --> used to be 10.0*CLHEP::mm // Changed by Salva 26/january/2016
  m_diffPtCut       = 15.0*CLHEP::GeV;  // not used
  m_pVZCut          = 150.0*CLHEP::mm; // mm

  m_fIDPtCut        = 0.0;     // GeV/c
  m_fIDChiPerDofCut = 4.0;
  m_IDSiHitsCut     = 8;
  m_ucID_PIXCut     = 1;        // Hits
  m_ucID_SCTCut     = 4;        // Hits
  m_ucID_TRTCut     = 0;        // Hits

  m_doDebug     = false;
  m_doQualSelection = false;
  m_doIsoSelection  = false;
  m_doPtSelection   = true;
  m_doIPSelection   = true;

  m_msgStream =  new MsgStream(PerfMonServices::getMessagingService(), "InDetPerformanceMonitoring" );

}



MuonSelector::~MuonSelector()
{
  --s_uNumInstances;
  delete m_msgStream;
}

//bool MuonSelector::passSelection( const Analysis::Muon* pxMuon )
bool MuonSelector::passSelection( const xAOD::Muon* pxMuon)
{
  if(m_doDebug){ std::cout << "  * MuonSelector * passSelection() * starting on new muon " << pxMuon << std::endl; }

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
	if (m_doDebug && !pass)			
	  std::cout<<"Muon Fails QualSelection"<<std::endl;
      }

      if (m_doIsoSelection){
        pass = passIsolCuts();
        passes.push_back(pass);
	if (m_doDebug && !pass)			
	  std::cout<<"Muon Fails Iso Selection"<<std::endl;
      }

      if (m_doPtSelection){
        pass = passPtCuts();
	passes.push_back(pass);
	if (m_doDebug && !pass)			
	  std::cout<<"Muon Fails pT Selection"<<std::endl;
      }

      if (m_doIPSelection){
        pass = passIPCuts();
        passes.push_back(pass);
	if (m_doDebug && !pass)			
	  std::cout<<"Muon Fails IP Selection"<<std::endl;
      }

      for (int i=0; i < int(passes.size()); i++)
	if (false == passes[i]){
	  if(m_doDebug) std::cout << "  * MuonSelector * passSelection() * BAD MUON * muon haven't passed the " << i+1 <<"th selection " << std::endl;
	  return false;
	}

    }
  if(m_doDebug){ std::cout << "  * MuonSelector * passSelection() * completed. GOOD MUON " << std::endl; }
  return true;
}



void MuonSelector::Init()
{
  ISvcLocator* serviceLocator = Gaudi::svcLocator();
  IToolSvc* toolSvc;
  StatusCode sc = serviceLocator->service("ToolSvc", toolSvc, true);

  if ( sc.isFailure() || toolSvc == 0 ) {
    (*m_msgStream) << MSG::ERROR << "  * MuonSelector * Init() * Unable to retrieve ToolSvc " << endreq;
    return;
  }
  PARENT::Init();
}



bool MuonSelector::Reco()
{
  return true;
}

//==================================================================================
// Protected Methods
//==================================================================================
void MuonSelector::BookHistograms( )
{
}


//==================================================================================
// Private Methods
//==================================================================================
bool MuonSelector::passQualCuts()
{
  // Apply muon hit criteria
  // First get the muon track, then the summarys
  
  //const xAOD::TrackParticle* IDTrk = m_pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);  // Always use combined track
  const xAOD::TrackParticle* IDTrk = m_pxMuon->trackParticle(xAOD::Muon::CombinedTrackParticle);

  //    if ( !m_pxMuon->isCombinedMuon()) return false;
  //  if ( m_pxMuon->trackParticle(xAOD::Muon::CombinedTrackParticle) ) return false;
  //???? if( xAOD::Muon::Combined ) return false;  //PF: ??
  
  
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

    if(m_doDebug) std::cout << "   * MuonSelector * passQualCuts() * eBLhits: " << eBLhits
			    << "  nBLhits:  " << nBLhits
			    << "  nhitsPIX: " << nhitsPIX
			    << "  nhitsSCT: " << nhitsSCT
			    << "  holes: " << nPIXH + nSCTH
			    << std::endl;

    if (((!eBLhits) || (nBLhits > 0))
	&&   (nhitsPIX + nPIXDS > 1 )
	&&   (nhitsSCT + nSCTDS >=6 )
	&&   (nPIXH    + nSCTH  < 2 ) ) {
      if (m_doDebug) std::cout << "   * MuonSelector * passQualCuts() * this muon satisfies the hits number QualCuts  "  << std::endl;
      return true;
    }
  }

  if(m_doDebug) std::cout << "   * MuonSelector * passQualCuts() * this muon did not pass the hits number QualCuts  "  << std::endl;
  return false;
}

bool MuonSelector::passPtCuts()
{

  const xAOD::TrackParticle* pxMuonID = m_pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  const xAOD::TrackParticle* pxMuonMS = m_pxMuon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
  const xAOD::TrackParticle* pxMuonCB = m_pxMuon->trackParticle(xAOD::Muon::CombinedTrackParticle);

  double pt, ptID, ptMS,ptCB;

  if ( !(pxMuonID || pxMuonMS || pxMuonCB)){
         if(m_doDebug) std::cout << "   * MuonSelector * passPtCuts() * NO inDetTrackParticle && muonSpectrometerTrackParticle && CombinedTrackParticle: " << std::endl;
  }

  else {

    pt    = m_pxMuon->pt();
    ptID  = pxMuonID ? pxMuonID->pt() : 0.0 ;
    ptMS  = pxMuonMS ? pxMuonMS->pt() : 0.0 ;
    ptMS  = pxMuonCB ? pxMuonCB->pt() : 0.0 ;
    double fMEta = fabs( m_pxMuon->eta() );

    if(m_doDebug) std::cout <<"   * MuonSelector * passPtCuts() * pt of segments "  << pt << " ptID: " << ptID <<" ptMS:  " << ptMS << " ptCB:" << ptCB << " fMEta: "<< fMEta << std::endl;

    
    if (  fMEta < m_fEtaCut    &&
	  pt    > m_combPtCut  &&
	  (ptMS  > m_ptMSCut || !pxMuonMS) //PF: This is kinda silly as m_ptMSCut is 0.0 by default and not changed anywhere at setup. Anyway kept the logic for future proper implementation
	  //fabs(ptMS - ptID) < m_diffPtCut
	  ){
      if(m_doDebug) std::cout << "   * MuonSelector * passPtCuts() * this muon passed the PtCuts (" << m_combPtCut <<") "<< std::endl;
             return true;
    }
  }
  if(m_doDebug) std::cout << "   * MuonSelector * passPtCuts() * this muon did not pass the PtCuts (reco pt=" << pt << ") " << std::endl;
  return false;
}

bool MuonSelector::passIsolCuts()
{
  float iso_pt40(0);
  if( !m_pxMuon->isolation(iso_pt40, xAOD::Iso::ptcone40) ) {
    std::cout << " * MuonSelector * WARNING * No isolation variable stored on the muon" << std::endl;
    if(m_doDebug) std::cout << "   * MuonSelector * passIsolCuts() * this muon did not pass the IsoCuts " << endmsg;
    return false;
  }
  
  else {
    double pt = m_pxMuon->pt();
    double ptSum = xAOD::Iso::ptcone40;
    if(m_doDebug) std::cout <<" in passIsolCuts , pt :" << pt <<" ptSum: "<< ptSum << std::endl;
    if (ptSum/pt < m_IsoCut ){
      if(m_doDebug) std::cout << "   * MuonSelector * passIsolCuts() * this muon passed the IsoCuts:" << std::endl;
      return true;
      
    }
  }
  
  if(m_doDebug) std::cout << "   * MuonSelector * passIsolCuts() * this muon did not pass the IsoCuts:" << std::endl;
  return false;
}


bool MuonSelector::passIPCuts()
{
  float extd0 = 0.0 ;
  float extz0 = 0.0 ;  

  //I'm not really sure of this logic. 
  if (m_pxMuon->inDetTrackParticleLink().isValid()) {
    const xAOD::TrackParticle* IDTrk = m_pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);  //Always use combined track
    if (!IDTrk) 
      {
	return false;
      }
    extd0 = IDTrk->d0();
    extz0 = IDTrk->z0()+IDTrk->vz();
    if(m_doDebug){
      std::cout << "   * MuonSelector * passIPCuts() *"
		<< " the IDTrack muon d0:  " << extd0
		<< " the IDTrack muon z0:  " << extz0 << std::endl;
    }
  }
  else {
    if(m_doDebug) std::cout << "   * MuonSelector * passIPCuts() * no valid inDetTrackParticleLink(). Will use the combined muon IPs" << std::endl;
    
    const xAOD::TrackParticle* CBTrk = m_pxMuon->trackParticle(xAOD::Muon::CombinedTrackParticle);
    if (!CBTrk) {
      if(m_doDebug) std::cout << "   * MuonSelector * passIPCuts() * no valid CombinedTrackParticle. Giving up." << std::endl;
      return false;
    }
    else { 
      extd0 = CBTrk->d0();
      extz0 = CBTrk->z0()+CBTrk->vz();
      if(m_doDebug){
	std::cout << " * MuonSelector * passIPCuts() *"
		  << " the CBTrack muon d0:  " << extd0
		  << " the CBTrack muon z0:  " << extz0 << std::endl;
      }
    }
  }
  
  const xAOD::VertexContainer *  vxContainer(0);
  vxContainer = PerfMonServices::getContainer<xAOD::VertexContainer>( PerfMonServices::VTX_COLLECTION );
  if (!vxContainer){
    if(m_doDebug) std::cout << "   * MuonSelector * passIPCuts() ** fails because NO vertex collection "<< std::endl;
    return false;
  }

  if ( vxContainer->size()>1 ) {
    if(m_doDebug) {
      std::cout << "   * MuonSelector * passIPCuts() ** vertex container is filled with " << vxContainer->size() << " vertices" <<  std::endl;

      // loop on vertices to check their coordinates:
      for (int ivtx=0; ivtx < (int) vxContainer->size(); ivtx++) {
	const xAOD::Vertex* thisVtx  = (*vxContainer)[ivtx];
	std::cout << "                                   vertex " << ivtx+1 << " (x,y,z) = (" << thisVtx->position().x()
		  << ", " << thisVtx->position().y()
		  << ", " << thisVtx->position().z()
		  << ")  type= " << thisVtx->vertexType()
		  << "  Npart= " << thisVtx->nTrackParticles()
		  <<  std::endl;
      }
    }
        
    // new method
    if (true) {
      bool goodmuonIP = false; // by default think the IP is not good
      // loop on vertices and check the muon may come from one of them
      for (int ivtx=0; ivtx < (int) vxContainer->size(); ivtx++) {
	const xAOD::Vertex* thisVtx  = (*vxContainer)[ivtx];
	if ( (thisVtx->vertexType() == 1) && thisVtx->nTrackParticles()>2 ) {
	  // check the vertex is in range and the muon is not far from the vertex
	  if (fabs(thisVtx->position().z()) < m_pVZCut && fabs(extz0 - thisVtx->position().z()) < m_diffZCut){
	    goodmuonIP = true;
	    if(m_doDebug) std::cout <<"   * MuonSelector * passIPCuts() * this muon has passed the IPCuts for vertex " << ivtx+1
				    << "  pVZcut= " << fabs(extz0 - thisVtx->position().z()) << " < " << m_diffZCut << std::endl;
	  }
	}
      }
      if (goodmuonIP) {
	if(m_doDebug) std::cout <<"   * MuonSelector * passIPCuts() * this muon has passed the IPCuts. Zcut: "<<  m_pVZCut << "  m_diffZCut " << m_diffZCut << std::endl;
	return true;
      }
    }
  }
  if(m_doDebug) std::cout <<"   * MuonSelector * passIPCuts() * this muon has not passed the IPCuts: " << std::endl;
  return false;
}
