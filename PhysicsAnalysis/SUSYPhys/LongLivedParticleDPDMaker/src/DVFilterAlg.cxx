/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LongLivedParticleDPDMaker/DVFilterAlg.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "VxVertex/VxContainer.h"
#include "muonEvent/MuonContainer.h"
#include "ITrackToVertex/ITrackToVertex.h"

/// Standard Service Constructor
DVFilterAlg::DVFilterAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator), 
    m_nProcessed(0),
    m_nPassMuonCut(0),
    m_trackToVertexTool("Reco::TrackToVertex")
{
  declareProperty("passAll", m_passAll = false); 
  declareProperty("primVtxColl",m_primVtxName="VxPrimaryCandidate");
  declareProperty("MuonCollection",m_muonCollName="MuidMuonCollection");
  declareProperty("MuonPtCut",m_muonPtCut=20000.);
  declareProperty("MuonEtaCut",m_muonEtaCut=2.);
  declareProperty("MuonD0Cut",m_muonD0Cut=1.);
}

/// Destructor
DVFilterAlg::~DVFilterAlg() {}

StatusCode DVFilterAlg::initialize()
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);
  sc = m_trackToVertexTool.retrieve();
  if(sc.isFailure()){
    msg(MSG::INFO) << "Could not retrieve TrackToVertexTool";
    return StatusCode::FAILURE;
  }

  return sc;
}

StatusCode DVFilterAlg::execute()
{

  StatusCode sc(StatusCode::SUCCESS);

  m_nProcessed += 1;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "==> execute " << name() << " on " << m_nProcessed << " event..." << endreq;

  ///If passAll is selected, accept all events
  if(m_passAll){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << name() << " event passed because passAll is true" << endreq;
    setFilterPassed(true);
    return sc;
  }

  const VxContainer* primVertices(0);
  sc = evtStore()->retrieve(primVertices,m_primVtxName);
  if (sc.isFailure()) 
    msg(MSG::ERROR)<<"Failed to retrieve VxPrimaryCandidate collection "<<m_primVtxName<<endreq;
  else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"retrieved VxPrimaryCandidate size "<<primVertices->size()<<endreq;  

  if (primVertices->size() == 0) {
    msg(MSG::WARNING)<<"Primary vertex not found, will skip this event"<<endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }


  const  Analysis::MuonContainer*  muon_reco=0;
  sc=evtStore()->retrieve(muon_reco , m_muonCollName);
  if( sc.isFailure()  ||  !muon_reco ) {
    msg(MSG::WARNING) << "No muon container found, will skip this event" << endreq; 
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  } 


  Analysis::MuonContainer::const_iterator muonItr = muon_reco->begin();  
  Analysis::MuonContainer::const_iterator muonItrE = muon_reco->end(); 
  
  for (; muonItr!=muonItrE; ++muonItr) { 
    
    if (fabs((*muonItr)->pt()) < m_muonPtCut ) continue;
    if (fabs((*muonItr)->eta()) > m_muonEtaCut ) continue;
    Rec::TrackParticle* muTrack = const_cast<Rec::TrackParticle*>((*muonItr)->track());
    if (muTrack==0) continue;
    /// Find d0 wrt all PVs in the event
    bool muGoodD0=true;

    for (unsigned int i_vtx=0; i_vtx<primVertices->size(); ++i_vtx) {
      const Trk::RecVertex pVtx = (primVertices->at(i_vtx))->recVertex();
      const Trk::Perigee* perigeeAtPV = m_trackToVertexTool->perigeeAtVertex(*muTrack, pVtx.position());
      if (perigeeAtPV == 0) {
	msg(MSG::DEBUG)<<"perigee at PV is null"<<endreq;
	continue;
      }
      double d0 = perigeeAtPV->parameters()[Trk::d0];
      if (fabs(d0)< m_muonD0Cut ) muGoodD0=false;
      break;
    }
    if (muGoodD0) { 
      m_nPassMuonCut++;
      setFilterPassed(true);
      return sc;
    }
  }
  setFilterPassed(false);
  return sc;  
}

StatusCode DVFilterAlg::finalize()
{

  StatusCode sc(StatusCode::SUCCESS);


  if (msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "Finalize " << name() << endreq;
    msg(MSG::INFO) <<"nProcessed "<<m_nProcessed<<endreq;
    msg(MSG::INFO) <<"nPassMuonCut "<<m_nPassMuonCut<<endreq;
    msg(MSG::INFO) <<"Efficiency =  "<<100.0*(float)m_nPassMuonCut/ (float)m_nProcessed<<"%"<<endreq;
  }
  return sc;  
}
