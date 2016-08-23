/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "MuonTruthAlgs/MuonDetailedTrackTruthMaker.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTrack/TrackCollection.h"
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
  m_trackCollectionNames.push_back("MuonSpectrometerTracks");
  m_trackCollectionNames.push_back("MooreTracks");
  m_trackCollectionNames.push_back("ConvertedMBoyMuonSpectroOnlyTracks");
  m_trackCollectionNames.push_back("ConvertedMBoyTracks");
  m_trackCollectionNames.push_back("MuidExtrapolatedTracks");
  m_trackCollectionNames.push_back("ExtrapolatedMuonSpectrometerTracks");
  m_trackCollectionNames.push_back("Combined_Tracks");
  m_trackCollectionNames.push_back("CombinedFitMuonTracks");
  m_trackCollectionNames.push_back("ConvertedMuIdCBTracks");
  m_trackCollectionNames.push_back("ConvertedMuIdExtrTracks");
  m_trackCollectionNames.push_back("ConvertedStacoTracks");
  m_trackCollectionNames.push_back("MuGirlRefittedTracks");

  declareProperty("PRD_TruthNames",          m_PRD_TruthNames);
  m_PRD_TruthNames.push_back("CSC_TruthMap");
  m_PRD_TruthNames.push_back("RPC_TruthMap");
  m_PRD_TruthNames.push_back("TGC_TruthMap");
  m_PRD_TruthNames.push_back("MDT_TruthMap");


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
  std::vector<std::string >::const_iterator it = m_trackCollectionNames.begin(), itEnd = m_trackCollectionNames.end();
  for ( ; it!=itEnd ; ++it)
    m_detailedTrackTruthNames.push_back(*it+"Truth");
  
  msg(MSG::INFO) << " processing: ";
  std::vector<std::string>::const_iterator ikeyt = m_detailedTrackTruthNames.begin();
  for(std::vector<std::string>::const_iterator ikey = m_trackCollectionNames.begin(); ikey != m_trackCollectionNames.end(); ikey++,++ikeyt) {
    msg(MSG::INFO) << "  " << *ikey << "  " << *ikeyt;
  }
  msg(MSG::INFO) << endmsg;
  
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

  StatusCode sc;

  //----------------------------------------------------------------
  // Retrieve prep raw data truth
  std::vector<const PRD_MultiTruthCollection*> prdCollectionVector;
  for(std::vector<std::string>::const_iterator ikey = m_PRD_TruthNames.begin(); ikey != m_PRD_TruthNames.end(); ikey++) {
    prdCollectionVector.push_back(0);
    sc = evtStore()->retrieve(*prdCollectionVector.rbegin(), *ikey);
    if (!sc.isSuccess()){
      ATH_MSG_WARNING(  "PRD_MultiTruthCollection "<<*ikey<<" NOT found");
    } else {
      ATH_MSG_DEBUG( "Got PRD_MultiTruthCollection "<<*ikey);
    }
  }

  //----------------------------------------------------------------
  // Retrieve track collections
  unsigned int i=0;
  std::vector<std::string>::const_iterator it = m_trackCollectionNames.begin(), itEnd = m_trackCollectionNames.end();
  for ( ; it!=itEnd ; ++it, ++i) {

    const TrackCollection *tracks = 0;
    if (evtStore()->contains<TrackCollection>(*it) && evtStore()->retrieve(tracks, *it).isSuccess()){
      ATH_MSG_DEBUG( "Got TrackCollection "<<*it );
    } else {
      ATH_MSG_VERBOSE( "TrackCollection "<<*it<<" NOT found");
      continue;
    }

    //----------------------------------------------------------------
    // Produce and store the output.

    DetailedTrackTruthCollection *dttc = new DetailedTrackTruthCollection(tracks);
    m_truthTool->buildDetailedTrackTruth(dttc, *tracks, prdCollectionVector);

    sc=evtStore()->record(dttc, m_detailedTrackTruthNames[i], false);
    if (sc.isFailure()) {
      ATH_MSG_WARNING( "DetailedTrackTruthCollection '" << m_detailedTrackTruthNames[i] << "' could not be registered in StoreGate !" );
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_DEBUG("DetailedTrackTruthCollection '" << m_detailedTrackTruthNames[i]
        << "' is registered in StoreGate, size="<<dttc->size() );
    }
  }
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
