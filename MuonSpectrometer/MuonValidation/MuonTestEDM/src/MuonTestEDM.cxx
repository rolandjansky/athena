/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
AmbiguityProcessor Algorithm 
***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>
#include "MuonTestEDM.h"


# include <iostream>
# include <iomanip>
#include <vector>
#include <string>

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"


#include "TrkTrack/TrackCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkSegment/SegmentCollection.h"


//Muon DD manager
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscDetectorElement.h"
#include "MuonReadoutGeometry/MdtDetectorElement.h"
#include "MuonReadoutGeometry/RpcDetectorElement.h"
#include "MuonReadoutGeometry/TgcDetectorElement.h"

//Muon helpers
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

// PRDs
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"

// RDOs
#include "MuonRDO/CscRawDataContainer.h" 
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/MdtCsmContainer.h"

#include <typeinfo>

//#include "DataModel/DataHeader.h"


MuonTestEDM::MuonTestEDM(const std::string& name, ISvcLocator* pSvcLocator) :
  TrkEDMTestAlg(name, pSvcLocator) {
  declareProperty("TrackCollectionNames",          m_trackCollNames);
  declareProperty("SegmentCollectionNames",        m_segmentCollNames);
  declareProperty("trackParticleCollNames",        m_trackParticleCollNames);
  declareProperty("RdoDumpFileName",               m_rdoDumpFileName = "MuonTestEDM.rdo.log" );
  declareProperty("PrdDumpFileName",               m_prdDumpFileName = "MuonTestEDM.prd.log" );    
  declareProperty("TrackDumpFileName",             m_trackDumpFileName =  "MuonTestEDM.track.log");
  declareProperty("TrackParticleDumpFileName",     m_trackPartDumpFileName = "MuonTestEDM.trackParticle.log");
  declareProperty("SegmentDumpFileName",           m_segmentDumpFileName = "MuonTestEDM.segment.log");
  declareProperty("DoDumpRDOs",                    m_dumpRDOs = false);
  declareProperty("DoDumpPRDs",                    m_dumpPRDs = false);
  declareProperty("DoDumpTracks",                  m_dumpTracks = true);
  declareProperty("DoDumpSegments",                m_dumpSegments = false);
  declareProperty("DoDumpTrackParticles",          m_dumpTrackParticles = true);
  m_segmentCollNames.push_back("MuonSegments");
  m_trackCollNames.push_back("MuonSpectrometerTracks");
  m_trackParticleCollNames.push_back("MuonSpectrometerParticles");
  m_summaryDumpFileName = "MuonTestEDM.summary.log";
}

//--------------------------------------------------------------------------
MuonTestEDM::~MuonTestEDM(void)
{}

//-----------------------------------------------------------------------
StatusCode MuonTestEDM::initialize() {

  if( TrkEDMTestAlg::initialize().isFailure() ) return StatusCode::FAILURE;

  // open file to dump objects to.
  if (m_dumpToFile){
    if (m_dumpRDOs)     {m_rdoFileOutput.open(m_rdoDumpFileName.c_str()); ATH_MSG_VERBOSE("RDO dump="<<m_rdoDumpFileName);m_rdoFileOutput.precision(6);}
    if (m_dumpPRDs)     {m_prdFileOutput.open(m_prdDumpFileName.c_str());ATH_MSG_VERBOSE("PRD dump="<<m_prdDumpFileName);m_prdFileOutput.precision(6);}
    if (m_dumpTracks)   {m_trackFileOutput.open(m_trackDumpFileName.c_str());ATH_MSG_VERBOSE("TRK dump="<<m_trackDumpFileName);m_trackFileOutput.precision(6);}
    if (m_dumpTrackParticles)   {m_trackPartFileOutput.open(m_trackPartDumpFileName.c_str());ATH_MSG_VERBOSE("TRK Part dump="<<m_trackPartDumpFileName);
      m_trackPartFileOutput.precision(6);}
    if (m_dumpSegments) {m_segmentFileOutput.open(m_segmentDumpFileName.c_str());ATH_MSG_VERBOSE("SEG dump="<<m_segmentDumpFileName);
      m_segmentFileOutput.precision(6);}
  }

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
StatusCode
MuonTestEDM::runTest()  {

  ATH_MSG_VERBOSE("Execute");
  using namespace Trk;

  if (m_dumpToFile){
    if (m_dumpRDOs)     m_rdoFileOutput<<"+~+~ Event number: " << m_eventNum << std::endl;
    if (m_dumpPRDs)     m_prdFileOutput<<"+~+~ Event number: " << m_eventNum << std::endl;
    if (m_dumpTracks)   m_trackFileOutput<<"+~+~ Event number: " << m_eventNum << std::endl;
    if (m_dumpTrackParticles)   m_trackPartFileOutput<<"+~+~ Event number: " << m_eventNum << std::endl;
    if (m_dumpSegments) m_segmentFileOutput<<"+~+~ Event number: " << m_eventNum << std::endl;
  } 

  //tracks
  if (m_dumpTracks){
    loadAndDumpMultipleContainers<TrackCollection>(m_trackCollNames, m_trackFileOutput);
  } 

  // if (m_dumpTrackParticles){
  //     loadAndDumpMultipleContainers<Rec::TrackParticleContainer>(m_trackPartCollNames, m_trackPartFileOutput);
  // }

  // segments
  if (m_dumpSegments) {
    loadAndDumpMultipleContainers<SegmentCollection>(m_segmentCollNames, m_segmentFileOutput);
  }

  //RDOs
  if (m_dumpRDOs) {
    loadAndDumpIdentifiableContainer<CscRawDataContainer>("CSCRDO", m_rdoFileOutput);
    loadAndDumpIdentifiableContainer<RpcPadContainer>("RPCPAD", m_rdoFileOutput);
    loadAndDumpIdentifiableContainer<TgcRdoContainer>("TGCRDO", m_rdoFileOutput);
    loadAndDumpIdentifiableContainer<MdtCsmContainer>("MDTCSM", m_rdoFileOutput);
  }

  if (m_dumpPRDs) {
    loadAndDumpIdentifiableContainer<Muon::CscPrepDataContainer>("CSC_Clusters", m_prdFileOutput);
    loadAndDumpIdentifiableContainer<Muon::RpcPrepDataContainer>("RPC_Measurements", m_prdFileOutput);
    loadAndDumpIdentifiableContainer<Muon::TgcPrepDataContainer>("TGC_Measurements", m_prdFileOutput);
    loadAndDumpIdentifiableContainer<Muon::MdtPrepDataContainer>("MDT_DriftCircles", m_prdFileOutput);
  }
    
  return StatusCode::SUCCESS;
}

StatusCode MuonTestEDM::finalize() {

  if( TrkEDMTestAlg::finalize().isFailure() ) return StatusCode::FAILURE;

  if (m_dumpToFile) {
    if (m_dumpRDOs)             m_rdoFileOutput.close();
    if (m_dumpPRDs)             m_prdFileOutput.close();
    if (m_dumpTracks)           m_trackFileOutput.close();
    if (m_dumpTrackParticles)   m_trackPartFileOutput.close();
    if (m_dumpSegments)         m_segmentFileOutput.close();    
  }
  return StatusCode::SUCCESS;
}
