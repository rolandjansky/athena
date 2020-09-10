/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

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
// ATLAS headers
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include <vector>
#include <cmath>

// Static declarations
std::atomic<unsigned int> MuonSelector::s_uNumInstances;

//==================================================================================
// Public Methods
//==================================================================================

MuonSelector::MuonSelector()
{
  unsigned int inst = ++s_uNumInstances;
  m_xSampleName     = "MuID_" + std::to_string(inst);
  m_pxMuon = nullptr;
  m_bLock  = false;
  m_coneSize        = 0.4;
  m_ucJMuon_Cut     = 0; //not used
  m_IsoCut          = 0.2;
  m_bCutOnCombKine  = false; //not used
  m_fEtaCut         = 2.5;
  m_combPtCut       = 15.0*CLHEP::GeV; // GeV/c
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
  m_doDebug     = false;
  m_doQualSelection = true;
  m_doIsoSelection  = false;
  m_doPtSelection   = true;
  m_doIPSelection   = true;
  ServiceHandle<IMessageSvc> msgSvc ("MessageSvc", "MuonSelector");
  m_msgStream =  new MsgStream(msgSvc.get(), "InDetPerformanceMonitoring" );
}



MuonSelector::~MuonSelector()
{
  delete m_msgStream;
}

bool MuonSelector::passSelection( const xAOD::Muon* pxMuon,
                                  const xAOD::VertexContainer& vxContainer )
{ 
  auto issueCutMessage = [this](const std::string & cutName){
   auto msg= std::string("Haven't passed the ")+cutName +std::string(" selection.");
   (*m_msgStream)<<MSG::INFO<<msg<<endmsg;
  };
  if ( pxMuon ){
    // Save local copy of muon address if it's ok.
    m_pxMuon = pxMuon;
    // Test muon pass conditions in turn
    if(m_doQualSelection and not passQualCuts()){
      issueCutMessage("quality");
      return false;
    }
    if (m_doIsoSelection and not passIsolCuts()){
      issueCutMessage("isolation");
      return false;
    }
    if (m_doPtSelection and not passPtCuts()){
      issueCutMessage("pT");
      return false;
    }
    if (m_doIPSelection and not passIPCuts(vxContainer)){
      issueCutMessage("impact parameter");
      return false;
    }
  }
  return true;
}



void MuonSelector::Init()
{
  ISvcLocator* serviceLocator = Gaudi::svcLocator();
  IToolSvc* toolSvc;
  StatusCode sc = serviceLocator->service("ToolSvc", toolSvc, true);
  if ( sc.isFailure() || toolSvc == nullptr ) {
    (*m_msgStream) << MSG::ERROR << "Unable to retrieve ToolSvc " << endmsg;
    return;
  }
  PARENT::Init();
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
  // First get the muon track, then the summaries
  const xAOD::TrackParticle* IDTrk = m_pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  if( xAOD::Muon::Combined ) return false;
  if (IDTrk) {
    unsigned char dummy(255);
    bool eBLhits = IDTrk->summaryValue( dummy, xAOD::expectInnermostPixelLayerHit )? dummy :false;
    int  nBLhits = IDTrk->summaryValue( dummy, xAOD::numberOfInnermostPixelLayerHits )? dummy :-1;

    int nhitsPIX = IDTrk->summaryValue( dummy, xAOD::numberOfPixelHits )? dummy :-1;
    int nhitsSCT = IDTrk->summaryValue( dummy, xAOD::numberOfSCTHits )? dummy :-1;

    int nPIXDS = IDTrk->summaryValue( dummy, xAOD::numberOfPixelDeadSensors )? dummy :-1;
    int nSCTDS = IDTrk->summaryValue( dummy, xAOD::numberOfSCTDeadSensors )? dummy :-1;

    int nPIXH = IDTrk->summaryValue( dummy, xAOD::numberOfPixelHoles )? dummy :-1;
    int nSCTH = IDTrk->summaryValue( dummy, xAOD::numberOfSCTHoles )? dummy :-1;

    if(m_doDebug)  (*m_msgStream)<<MSG::INFO << " eBLhits: " << eBLhits << " nBLhits:  " << nBLhits  << " nhitsPIX: " << nhitsPIX << endmsg;

    if (   ((!eBLhits) || (nBLhits > 0))
	   &&   (nhitsPIX + nPIXDS > 1 )
	   &&   (nhitsSCT + nSCTDS >=6 )
	   &&   (nPIXH    + nSCTH  < 2 ) )
      return true;
  }
  if(m_doDebug) (*m_msgStream)<<MSG::INFO << "this muon not pass the hits number QualCuts:  "  << endmsg;
  return false;
}

bool MuonSelector::passPtCuts()
{
  const xAOD::TrackParticle* pxMuonID = m_pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  const xAOD::TrackParticle* pxMuonMS = m_pxMuon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
  if ( !(pxMuonID && pxMuonMS)){
    if(m_doDebug) (*m_msgStream)<<MSG::INFO << "NO inDetTrackParticle && muonSpectrometerTrackParticle: " << endmsg;
  }else {
    double pt    = m_pxMuon->pt();
    double ptID  = pxMuonID->pt();
    double ptMS  = pxMuonMS->pt();
    double fMEta = fabs( m_pxMuon->eta() );

    if(m_doDebug) (*m_msgStream)<<MSG::INFO <<" in passPtCuts: "<< " pt: "  << pt << " ptID: " << ptID <<" ptMS:  " << ptMS << " fMEta: "<< fMEta << endmsg;


    if (  fMEta < m_fEtaCut    &&
    pt    > m_combPtCut  &&
    ptMS  > m_ptMSCut
    ){
             if(m_doDebug) (*m_msgStream)<<MSG::INFO << " this muon passed the PtCuts:" << endmsg;
             return true;
    }
  }

  (*m_msgStream) << MSG::DEBUG << " this muon did not pass the PtCuts:" << endmsg;
  return false;
}

bool MuonSelector::passIsolCuts()
{
  const  xAOD::TrackParticle* pxMuonID = m_pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  if (pxMuonID) {
    float iso_pt40(0);
    if( !m_pxMuon->isolation(iso_pt40, xAOD::Iso::ptcone40) ) {
      std::cout << "No isolation variable stored on the muon" << std::endl;
      if(m_doDebug) (*m_msgStream)<<MSG::INFO << " this muon did not pass the IsoCuts:" << endmsg;
      return false;
    }
    else {
      double pt = m_pxMuon->pt();
      double ptSum = xAOD::Iso::ptcone40;
      if(m_doDebug) (*m_msgStream)<<MSG::INFO <<" in passIsolCuts , pt :" << pt <<" ptSum: "<< ptSum << endmsg;
        if (ptSum/pt < m_IsoCut ){
         if(m_doDebug) (*m_msgStream)<<MSG::INFO << " this muon passed the IsoCuts:" << endmsg;
       return true;
     }
    }
  }
  if(m_doDebug) (*m_msgStream)<<MSG::INFO << " this muon did not pass the IsoCuts:" << endmsg;
  return false;
}


bool MuonSelector::passIPCuts(const xAOD::VertexContainer& vxContainer)
{
  float extd0 = 0.0 ;
  float extz0 = 0.0 ;
  if (m_pxMuon->inDetTrackParticleLink().isValid()) {
    const xAOD::TrackParticle* IDTrk = m_pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    extd0 = IDTrk->d0();
    extz0 = IDTrk->z0()+IDTrk->vz();
    if(m_doDebug){
      (*m_msgStream)<<MSG::INFO << " the IDTrack muon d0:  " << extd0 << endmsg;
      (*m_msgStream)<<MSG::INFO << " the IDTrack muon z0:  " << extz0 << endmsg;
    }
  }
  else
    return false;
  if ( vxContainer.size()>1 ) {
    const xAOD::Vertex* PV  = vxContainer[0];
    Amg::Vector3D newPos = PV->position();
    if(m_doDebug) (*m_msgStream)<<MSG::INFO << " the PV of this event: " << newPos << endmsg;

    if ( PV->nTrackParticles() > 2         &&
     std::fabs(newPos.z())              < m_pVZCut   &&
     std::fabs(extz0 - newPos.z())      < m_diffZCut    ){
      if(m_doDebug) (*m_msgStream)<<MSG::INFO <<" this muon passed the IPCuts: " << endmsg;
      return true;
    }
  }
  if(m_doDebug) (*m_msgStream)<<MSG::INFO <<" this muon not passed the IPCuts: " << endmsg;
  return false;
}
