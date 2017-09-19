/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "MuonTruthAlgs/MuonDetailedTrackTruthMaker.h"
#include <iterator>

//================================================================
MuonDetailedTrackTruthMaker::MuonDetailedTrackTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_truthTool("Trk::DetailedTrackTruthBuilder")
{  
  declareProperty("TruthTool",               m_truthTool);

  // Inputs
  declareProperty("TrackCollectionNames",     m_trackCollectionNames );
  m_trackCollectionNames.reserve(9);
  m_trackCollectionNames.emplace_back("MuonSpectrometerTracks");
  m_trackCollectionNames.emplace_back("MooreTracks");
  m_trackCollectionNames.emplace_back("ConvertedMBoyMuonSpectroOnlyTracks");
  m_trackCollectionNames.emplace_back("ConvertedMBoyTracks");
  m_trackCollectionNames.emplace_back("MuidExtrapolatedTracks");
  m_trackCollectionNames.emplace_back("ExtrapolatedMuonSpectrometerTracks");
  m_trackCollectionNames.emplace_back("Combined_Tracks");
  m_trackCollectionNames.emplace_back("CombinedFitMuonTracks");
  m_trackCollectionNames.emplace_back("ConvertedMuIdCBTracks");
  m_trackCollectionNames.emplace_back("ConvertedMuIdExtrTracks");
  m_trackCollectionNames.emplace_back("ConvertedStacoTracks");
  m_trackCollectionNames.emplace_back("MuGirlRefittedTracks");

  declareProperty("PRD_TruthNames",          m_PRD_TruthNames);
  m_PRD_TruthNames.emplace_back("CSC_TruthMap");
  m_PRD_TruthNames.emplace_back("RPC_TruthMap");
  m_PRD_TruthNames.emplace_back("TGC_TruthMap");
  m_PRD_TruthNames.emplace_back("MDT_TruthMap");


  // Output
  declareProperty("DetailedTrackTruthNames",  m_detailedTrackTruthNames);
}

// Initialize method
// -----------------------------------------------------------------------------------------------------
StatusCode MuonDetailedTrackTruthMaker::initialize()
{
  ATH_MSG_DEBUG( "MuonDetailedTrackTruthMaker::initialize()");
    
  //----------------
  if ( m_truthTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_truthTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved tool " << m_truthTool );
  }

  m_detailedTrackTruthNames.reserve ( m_trackCollectionNames.size());
  for(unsigned int i=0;i<m_trackCollectionNames.size();i++){
    m_detailedTrackTruthNames.emplace_back(m_trackCollectionNames.at(i).key()+"Truth");
    ATH_MSG_INFO("process "<<m_trackCollectionNames.at(i).key()<<" for detailed truth collection "<<m_detailedTrackTruthNames.at(i).key());
  }

  ATH_CHECK(m_trackCollectionNames.initialize());
  ATH_CHECK(m_PRD_TruthNames.initialize());
  ATH_CHECK(m_detailedTrackTruthNames.initialize());
  
  //----------------
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode MuonDetailedTrackTruthMaker::finalize() 
{
  ATH_MSG_DEBUG( "MuonDetailedTrackTruthMaker::finalize()");
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode MuonDetailedTrackTruthMaker::execute() {
  ATH_MSG_DEBUG( "MuonDetailedTrackTruthMaker::execute()");

  //----------------------------------------------------------------
  // Retrieve prep raw data truth
  std::vector<const PRD_MultiTruthCollection*> prdCollectionVector;
  for(SG::ReadHandle<PRD_MultiTruthCollection>& col : m_PRD_TruthNames.makeHandles()){
    if(!col.isPresent()) continue;
    if(!col.isValid()){
      ATH_MSG_WARNING("invalid PRD_MultiTruthCollection "<<col.name());
      return StatusCode::FAILURE;
    }
    prdCollectionVector.push_back(col.cptr());
  }

  //----------------------------------------------------------------
  // Retrieve track collections
  int i=0;
  for(SG::ReadHandle<TrackCollection>& tcol : m_trackCollectionNames.makeHandles()){
    if(!tcol.isValid()){
      ATH_MSG_WARNING("invalid TrackCollection "<<tcol.name());
      return StatusCode::FAILURE;
    }
    if(!tcol.isPresent()) continue;

    //----------------------------------------------------------------
    // Produce and store the output.

    SG::WriteHandle<DetailedTrackTruthCollection> dttc(m_detailedTrackTruthNames.at(i));
    ATH_CHECK(dttc.record(std::make_unique<DetailedTrackTruthCollection>()));
    dttc->setTrackCollection(tcol.cptr());
    m_truthTool->buildDetailedTrackTruth(dttc.ptr(), *(tcol.cptr()), prdCollectionVector);
    i++;
  }
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
