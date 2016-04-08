/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : MuonSpShowerBuilder.cxx
Package : offline/PhysicsAnalysis/EventViewBuilder/EventViewBuilderAlgs
Author  : Frank Paige, Stephane Willocq
Created : August 2006

DESCRIPTION:

See MuonSpShowerBuilder.h. 


*****************************************************************************/

// Include files

#include "MuonSpShowerBuilderAlgs/MuonSpShowerBuilder.h"
#include "muonEvent/MuonSpShower.h"
#include "muonEvent/MuonSpShowerContainer.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODJet/JetContainer.h"

#include <math.h>

/////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////

MuonSpShowerBuilder::MuonSpShowerBuilder(const std::string& name, 
  ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator),
  m_JetCollectionKey("Cone7H1TowerJets"),	// input jets
  m_jetPtCut(50*CLHEP::GeV),				// pt cut on jets
  m_RpcPrepDataKey("RPC_Measurements"),		// RPC hits in ESD
  m_TgcPrepDataKey("TGC_Measurements"),		// TGC hits in ESD
  m_MdtPrepDataKey("MDT_DriftCircles"),		// MDT hits in ESD
  m_MuonSegmentKey("MuonSegments"),	        // Muon segments in ESD
  m_etaCut(M_PI/8),				// eta cut for hits
  m_phiCut(M_PI/8),				// phi cut for hits
  m_rCut(0.4),					// R cut for segments
  m_triggerHitCut(0),				// minimum hits
  m_mdtHitCut(0),				// minimum hits
  m_muonSegmentCut(0),				// minimum segments
  m_MuonSpShowerKey("MuonSpShowers"),		// Storegate key
  m_MuonMgr(0),
  m_muonHelper(0)
{
  declareProperty("JetCollectionKey", m_JetCollectionKey);
  declareProperty("jetPtCut", m_jetPtCut);
  declareProperty("RpcPrepDataKey", m_RpcPrepDataKey);
  declareProperty("TgcPrepDataKey", m_TgcPrepDataKey);
  declareProperty("MdtPrepDataKey", m_MdtPrepDataKey);
  declareProperty("MuonSegmentKey", m_MuonSegmentKey);
  declareProperty("etaHitCut", m_etaCut);
  declareProperty("phiHitCut", m_phiCut);
  declareProperty("rSegmentCut", m_rCut);
  declareProperty("triggerHitCut", m_triggerHitCut);
  declareProperty("mdtHitCut", m_mdtHitCut);
  declareProperty("muonSegmentCut", m_muonSegmentCut);

}

/////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////

MuonSpShowerBuilder::~MuonSpShowerBuilder() {}

/////////////////////////////////////////////////////////////////
// Initialize
/////////////////////////////////////////////////////////////////

StatusCode MuonSpShowerBuilder::initialize() {

  ATH_MSG_INFO("Initializing MuonSpShowerBuilder");

  StatusCode sc = detStore()->retrieve( m_MuonMgr );
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR(" Cannot retrieve MuonDetDescrMgr ");
  } else {
    m_muonHelper = m_MuonMgr->mdtIdHelper();
  }

  return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////
// Finalize
/////////////////////////////////////////////////////////////////

StatusCode MuonSpShowerBuilder::finalize() {
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// Execute
/////////////////////////////////////////////////////////////////

StatusCode MuonSpShowerBuilder::execute() {

  ATH_MSG_DEBUG("Executing MuonSpShowerBuilder");

  StatusCode sc;  

  // Retrieve jet collection
  const xAOD::JetContainer* jets = 0;
  sc = evtStore()->retrieve(jets, m_JetCollectionKey);
  if(  sc.isFailure() || !jets ) {
    ATH_MSG_WARNING("Could not retrieve JetCollection with name " << m_JetCollectionKey );
    return StatusCode::SUCCESS;
  }
  auto jetItr = jets->begin();
  auto jetEnd = jets->end();

  // Retrieve trigger hits
  const Muon::RpcPrepDataContainer* rpcs = 0;
  sc = evtStore()->retrieve(rpcs, m_RpcPrepDataKey);
  if(  sc.isFailure() || !rpcs ) {
    ATH_MSG_WARNING("Could not retrieve RpcPrepDataCollection with name " << m_RpcPrepDataKey );
    return StatusCode::SUCCESS;
  }
  Muon::RpcPrepDataContainer::const_iterator rpcColItr  = rpcs->begin();
  Muon::RpcPrepDataContainer::const_iterator rpcColItrB = rpcs->begin();
  Muon::RpcPrepDataContainer::const_iterator rpcColItrE = rpcs->end();

  const Muon::TgcPrepDataContainer* tgcs = 0;
  sc = evtStore()->retrieve(tgcs, m_TgcPrepDataKey);
  if(  sc.isFailure() || !tgcs ) {
    ATH_MSG_WARNING("Could not retrieve TgcPrepDataCollection with name " << m_TgcPrepDataKey );
    return StatusCode::SUCCESS;
  }
  Muon::TgcPrepDataContainer::const_iterator tgcColItr  = tgcs->begin();
  Muon::TgcPrepDataContainer::const_iterator tgcColItrB = tgcs->begin();
  Muon::TgcPrepDataContainer::const_iterator tgcColItrE = tgcs->end();

  // Retrieve MDT hits
  const Muon::MdtPrepDataContainer* mdts = 0;
  sc = evtStore()->retrieve(mdts, m_MdtPrepDataKey);
  if(  sc.isFailure() || !mdts ) {
    ATH_MSG_WARNING("Could not retrieve MdtPrepDataCollection with name " << m_MdtPrepDataKey );
    return StatusCode::SUCCESS;
  }
  Muon::MdtPrepDataContainer::const_iterator mdtColItr  = mdts->begin();
  Muon::MdtPrepDataContainer::const_iterator mdtColItrB = mdts->begin();
  Muon::MdtPrepDataContainer::const_iterator mdtColItrE = mdts->end();

  // Retrieve muon segments. These are saved as Trk::Segments; there is no
  // separate container package
  xAOD::MuonSegmentContainer* segments = 0;
  if(evtStore()->contains<xAOD::MuonSegmentContainer>(m_MuonSegmentKey)) {
    if(evtStore()->retrieve(segments,m_MuonSegmentKey).isFailure()) {
      ATH_MSG_WARNING("Unable to retrieve " << m_MuonSegmentKey );
      return StatusCode::FAILURE;
    }
  }

  if(!segments) return StatusCode::FAILURE;

  auto segItr = segments->begin();
  auto segBeg = segments->begin();
  auto segEnd = segments->end();
  ATH_MSG_DEBUG("MDT segments size = " <<segments->size() );

  // Make new MuonSpShowerContainer
  Rec::MuonSpShowerContainer* showers = new Rec::MuonSpShowerContainer;
  sc = evtStore()->record(showers, m_MuonSpShowerKey);
  if( sc.isFailure() || !showers ) {
    ATH_MSG_WARNING("Could not record MuonSpShowerContainer with name " << m_MuonSpShowerKey );
    return StatusCode::SUCCESS;
  }


  ////////////////////////////////////////
  // Loop over jets
  for(; jetItr != jetEnd; ++jetItr) {
    if( (*jetItr)->pt() < m_jetPtCut ) continue;
    double etaj = (*jetItr)->eta();
    double phij = (*jetItr)->phi();

    // Loop over trigger hits
    unsigned short ntrig = 0;

    for(rpcColItr=rpcColItrB; rpcColItr!=rpcColItrE; ++rpcColItr) {
      Muon::RpcPrepDataCollection::const_iterator rpcItr = (*rpcColItr)->begin();
      Muon::RpcPrepDataCollection::const_iterator rpcEnd = (*rpcColItr)->end();
      for(; rpcItr != rpcEnd; ++rpcItr) {
        double etah =(*rpcItr)->detectorElement()->globalPosition().eta();
        double phih =(*rpcItr)->detectorElement()->globalPosition().phi();
        double dphihj = fabs(phih-phij); 
        if( dphihj>M_PI ) dphihj = 2*M_PI - dphihj; 
        double detahj = etah - etaj;
        if( sqrt(dphihj*dphihj + detahj*detahj) < m_rCut ) ++ntrig;
      }
    }

    for(tgcColItr=tgcColItrB; tgcColItr!=tgcColItrE; ++tgcColItr) {
      Muon::TgcPrepDataCollection::const_iterator tgcItr = (*tgcColItr)->begin();
      Muon::TgcPrepDataCollection::const_iterator tgcEnd = (*tgcColItr)->end();
      for(; tgcItr != tgcEnd; ++tgcItr) {
        double etah =(*tgcItr)->detectorElement()->globalPosition().eta();
        double phih =(*tgcItr)->detectorElement()->globalPosition().phi();
        double dphihj = fabs(phih-phij); 
        if( dphihj>M_PI ) dphihj = 2*M_PI - dphihj; 
        double detahj = etah - etaj;
        if( sqrt(dphihj*dphihj + detahj*detahj) < m_rCut ) ++ntrig;
      }
    }

    // Loop over MDT hits
    unsigned short nHit[3];
    nHit[0] = 0;
    nHit[1] = 0;
    nHit[2] = 0;

    for(mdtColItr=mdtColItrB; mdtColItr!=mdtColItrE; ++mdtColItr) {
      Muon::MdtPrepDataCollection::const_iterator mdtItr = (*mdtColItr)->begin();
      Muon::MdtPrepDataCollection::const_iterator mdtEnd = (*mdtColItr)->end();
      for(; mdtItr != mdtEnd; ++mdtItr) {
        double etah =(*mdtItr)->detectorElement()->globalPosition().eta();
        double phih =(*mdtItr)->detectorElement()->globalPosition().phi();
        double dphihj = fabs(phih-phij); 
        if( dphihj>M_PI ) dphihj = 2*M_PI - dphihj; 
        if( fabs(etah-etaj)>m_etaCut || dphihj>m_phiCut ) continue; 
        // 0 = inner, 1 = EE, 2 = middle, 3 = outer
        // We combine 1 and 2
        int layer = m_muonHelper->stationRegion( (*mdtItr)->identify() );
        if( layer > 1 ) --layer;
        ++nHit[layer];
      }
    }

    // Muon segments 

    unsigned short nSeg[3];
    nSeg[0] = 0;
    nSeg[1] = 0;
    nSeg[2] = 0;

    for(segItr=segBeg; segItr!=segEnd; ++segItr) {
      Amg::Vector3D pos( (**segItr).x(),(**segItr).y(),(**segItr).z());
      double etas = pos.eta();
      double phis = pos.phi();
      double dr = fabs(phij-phis);
      if( dr > M_PI ) dr = 2*M_PI - dr;
      dr = sqrt( dr*dr + (etas-etaj)*(etas-etaj) );
      if( dr > m_rCut ) continue;

      Muon::MuonStationIndex::ChIndex chIndex = static_cast<Muon::MuonStationIndex::ChIndex>((**segItr).chamberIndex());
      if( chIndex == Muon::MuonStationIndex::ChUnknown ) continue;
      Muon::MuonStationIndex::LayerIndex layer = Muon::MuonStationIndex::toLayerIndex(chIndex);
      if( layer < 0 || layer == Muon::MuonStationIndex::LayerUnknown ) continue;
      // 0 = inner, 1 = middle,EE,BEE, 2 = outer
      if( layer > Muon::MuonStationIndex::Outer ) layer = Muon::MuonStationIndex::Middle;
      ++nSeg[layer];
    }

    ATH_MSG_DEBUG("Found hits/segments = " << ntrig <<" " 
         <<nHit[0] <<" " <<nHit[1] <<" " <<nHit[2] <<" " 
         <<nSeg[0] <<" " <<nSeg[1] <<" " <<nSeg[2] );

    // Check against cuts
    if( ntrig < m_triggerHitCut ) continue;
    if( nHit[0]+nHit[1]+nHit[2] < m_mdtHitCut ) continue;
    if( nSeg[0]+nSeg[1]+nSeg[2] < m_muonSegmentCut ) continue;
    Rec::MuonSpShower* shower = new Rec::MuonSpShower(etaj,phij,
      ntrig, nHit[0], nHit[1], nHit[2], nSeg[0], nSeg[1], nSeg[2]);
    showers->push_back(shower);
  }

  return StatusCode::SUCCESS;  
}

