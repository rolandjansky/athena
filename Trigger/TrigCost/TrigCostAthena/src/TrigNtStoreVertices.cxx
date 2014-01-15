/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <string>
#include <iostream>
#include <algorithm>

// ROOT
#include "TFile.h"
#include "TTree.h"

// Framework
#include "GaudiKernel/MsgStream.h"

// Reconstruction and MC
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "TrigInDetEvent/TrigVertex.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/RecVertex.h"
#include "TrkEventPrimitives/TrkEventPrimitivesDict.h"

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostAthena/TrigNtStoreVertices.h"

//---------------------------------------------------------------------------------------
Trig::TrigNtStoreVertices::TrigNtStoreVertices(const std::string &name,
					       const std::string &type,
					       const IInterface  *parent)
  :AlgTool(name, type, parent),
   m_log(0),
   m_storeGate("StoreGateSvc", name),
   m_config        (0),
   m_file          (0),
   m_treeEvent     (0),
   m_treeConfig    (0),
   m_runNumber     (0),
   m_lumiBlock     (0),
   m_eventNumber   (0),
   m_bunchId       (0),
   m_timestamp     (0),
   m_timestamp_ns  (0),
   m_nVtx          (0),
   m_vtx_x         (0),
   m_vtx_y         (0),
   m_vtx_z         (0),
   m_vtx_chi2      (0),
   m_vtx_ndof      (0),
   m_event_lv1_ptr (0),
   m_event_hlt_ptr (0),
   m_HLTPrescaleKey(0),
   m_LV1PrescaleKey(0),
   m_masterKey     (0),
   m_chainNames    (0),
   m_chainIds      (0)
{
  declareInterface<Trig::ITrigNtTool>(this);

  declareProperty("offvtxContainerName", m_offvtxContainerName = "VxPrimaryCandidate");
  declareProperty("outputNtuple"       , m_outputNtuple        = "");
  declareProperty("debug"              , m_debug               = false);
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtStoreVertices::initialize()
{
  //
  // Get services
  //
  m_log = new MsgStream(msgSvc(), name());
      
  if(m_storeGate.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
    return StatusCode::FAILURE;
  }  

  //
  // Create output file
  //
  if(m_outputNtuple.empty()) {
    log() << MSG::ERROR << "Empty output ntuple name" << endreq;
    return StatusCode::FAILURE;
  }

  m_file = new TFile(m_outputNtuple.c_str(), "RECREATE") ;

  if(m_file && m_file->IsOpen() && !m_file->IsZombie()) {
    log() << MSG::INFO << "Output file: " << m_outputNtuple.c_str() << endreq;
  }
  else {
    log() << MSG::ERROR << "Failed to open output file: " << m_outputNtuple.c_str() << endreq;
    return StatusCode::FAILURE;
  }

  //
  // Create trees
  //
  m_treeEvent  = new TTree("eb_event",  "Vertex and trigger decision information") ;
  m_treeConfig = new TTree("eb_config", "Trigger configuration information") ;  
  
  m_treeEvent ->SetDirectory(m_file);
  m_treeConfig->SetDirectory(m_file);

  //
  // Create and initialize event variables
  //
  m_vtx_x    = new std::vector<Float_t>();
  m_vtx_y    = new std::vector<Float_t>();
  m_vtx_z    = new std::vector<Float_t>();
  m_vtx_chi2 = new std::vector<Float_t>();
  m_vtx_ndof = new std::vector<Int_t>();

  m_event_lv1_ptr = new std::vector<uint16_t>();
  m_event_hlt_ptr = new std::vector<uint32_t>();   
      
  m_treeEvent->Branch("runNumber"      , &m_runNumber        , "runNumber/I"      ) ;
  m_treeEvent->Branch("lumiBlock"      , &m_lumiBlock        , "lumiBlock/I"      ) ;
  m_treeEvent->Branch("eventNumber"    , &m_eventNumber      , "eventNumber/I"    ) ;
  m_treeEvent->Branch("bunchId"        , &m_bunchId          , "bunchId/I"        ) ;
  m_treeEvent->Branch("timestamp"      , &m_timestamp        , "timestamp/I"      ) ;
  m_treeEvent->Branch("timestamp_ns"   , &m_timestamp_ns     , "timestamp_ns/I"   ) ;
  m_treeEvent->Branch("nVtx"           , &m_nVtx             , "nVtx/I"           ) ;    
    
  const Int_t buffer_size = 10000000 ;
  const Int_t split_level = 0 ;
  
  m_treeEvent->Branch("vtx_x",     "std::vector<Float_t>",  &m_vtx_x,         buffer_size, split_level);
  m_treeEvent->Branch("vtx_y",     "std::vector<Float_t>",  &m_vtx_y,         buffer_size, split_level);
  m_treeEvent->Branch("vtx_z",     "std::vector<Float_t>",  &m_vtx_z,         buffer_size, split_level);
  m_treeEvent->Branch("vtx_chi2",  "std::vector<Float_t>",  &m_vtx_chi2,      buffer_size, split_level);
  m_treeEvent->Branch("vtx_ndof",  "std::vector<Int_t>",    &m_vtx_ndof,      buffer_size, split_level);
  m_treeEvent->Branch("event_lv1", "std::vector<uint16_t>", &m_event_lv1_ptr, buffer_size, split_level);
  m_treeEvent->Branch("event_hlt", "std::vector<uint32_t>", &m_event_hlt_ptr, buffer_size, split_level);

  //
  // Create and initialize config variables
  //
  m_chainNames = new std::vector<std::string>();
  m_chainIds   = new std::vector<uint32_t>();

  m_treeConfig->Branch("HTLPrescaleKey" , &m_HLTPrescaleKey , "HLTPrescaleKey/I" ) ;
  m_treeConfig->Branch("LV1PrescaleKey" , &m_LV1PrescaleKey , "LV1PrescaleKey/I" ) ;
  m_treeConfig->Branch("masterKey"      , &m_masterKey      , "masterKey/I"      ) ;

  m_treeConfig->Branch("trig_names", "std::vector<std::string>", &m_chainNames, buffer_size, split_level);
  m_treeConfig->Branch("trig_ids",   "std::vector<uint32_t>",    &m_chainIds,   buffer_size, split_level);

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtStoreVertices::finalize()
{
  //
  // Clean up internal state
  //
  log() << MSG::DEBUG << "finalize()" << endreq;

  delete m_log; m_log = 0;

  m_file->Write() ;
  m_file->Close() ;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtStoreVertices::Fill(TrigMonConfig *confg)
{
  //
  // Copy configuration
  //
  if(!confg) {
    log() << MSG::WARNING << "Null TrigMonConfig pointer" << endreq;
    return false;
  }
  
  m_config         = confg;
  m_masterKey      = confg->getMasterKey() ;
  m_HLTPrescaleKey = confg->getHLTPrescaleKey() ;
  m_LV1PrescaleKey = confg->getLV1PrescaleKey() ;

  m_chainNames->clear();
  m_chainIds  ->clear();

  for(unsigned i = 0; i < m_config->size<TrigConfChain>(); i++) {
    const TrigConfChain &chain = m_config->at<TrigConfChain>(i);
    
    m_chainNames->push_back(chain.getName());
    m_chainIds  ->push_back(chain.getEncodedId());
  }
  
  m_treeConfig->Fill();
  
  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtStoreVertices::Fill(TrigMonEvent &event)
{
  //
  // Process current event
  //
  if(m_debug && outputLevel() <= MSG::DEBUG) {
    log() << MSG::INFO << "dumpStore--------------------------------------------" << endreq;
    m_storeGate->dump();
  }

  //
  // Get L1 and HLT trigger decision objects
  //
  const std::vector<uint16_t>& event_lv1 = event.getL1Item() ;
  const std::vector<uint32_t>& event_hlt = event.getChain() ;
    
  *m_event_lv1_ptr = event_lv1 ;
  *m_event_hlt_ptr = event_hlt ;

  // Event info
  m_runNumber    = event.getRun() ;
  m_lumiBlock    = event.getLumi() ;
  m_eventNumber  = event.getEvent();
  m_bunchId      = event.getBunchId();
  m_timestamp    = event.getSec() ;
  m_timestamp_ns = event.getNanoSec() ;
  m_nVtx         = 0 ;
  
  // Reset vtx varianles
  m_vtx_x    -> clear();
  m_vtx_y    -> clear();
  m_vtx_z    -> clear();
  m_vtx_chi2 -> clear();
  m_vtx_ndof -> clear();  

  const DataHandle<VxContainer> offvtxColl;

  if(m_storeGate->contains<VxContainer>(m_offvtxContainerName) &&
     m_storeGate->retrieve(offvtxColl, m_offvtxContainerName).isSuccess()) {
    //
    // Read offline vertices 
    //
    log() << MSG::DEBUG << "VxContainer size is " << offvtxColl->size() << endreq;

    VxContainer::const_iterator aOffVtxIterator = offvtxColl->begin();
    VxContainer::const_iterator offvtxCollEnd   = offvtxColl->end();

    for (; aOffVtxIterator != offvtxCollEnd; aOffVtxIterator++) {
      Trk::VxCandidate* aOffVtx = (*aOffVtxIterator);
      const Trk::RecVertex recvtx = aOffVtx->recVertex();
      
      const float chi2 = recvtx.fitQuality().chiSquared() ;
      const int   ndof = recvtx.fitQuality().numberDoF()  ;

      // Reject dummy vertex
      if(ndof==0) continue ;

      m_vtx_x    -> push_back(recvtx.position().x()) ;
      m_vtx_y    -> push_back(recvtx.position().y()) ;
      m_vtx_z    -> push_back(recvtx.position().z()) ;
      m_vtx_chi2 -> push_back(chi2) ;
      m_vtx_ndof -> push_back(ndof) ;
      m_nVtx++ ;
    }
  }
  else {
    log() << MSG::DEBUG << "No VxContainer with key " << m_offvtxContainerName  << endreq;
  }
  
  //
  // Finally save branch data
  //
  m_treeEvent -> Fill() ;

  return true;
}


