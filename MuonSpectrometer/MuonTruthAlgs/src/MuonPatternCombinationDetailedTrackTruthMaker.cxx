/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "MuonTruthAlgs/MuonPatternCombinationDetailedTrackTruthMaker.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "MuonPattern/DetailedMuonPatternTruthCollection.h"

#include <iterator>

//================================================================
MuonPatternCombinationDetailedTrackTruthMaker::MuonPatternCombinationDetailedTrackTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{  
  if (m_detailedTrackTruthName.empty()) m_detailedTrackTruthName = (m_trackCollectionName + "Truth");
}

// -----------------------------------------------------------------------------------------------------
StatusCode MuonPatternCombinationDetailedTrackTruthMaker::initialize()
{
  ATH_MSG_DEBUG( "MuonPatternCombinationDetailedTrackTruthMaker::initialize()");
  ATH_CHECK(m_truthTool.retrieve());
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode MuonPatternCombinationDetailedTrackTruthMaker::execute() {
  ATH_MSG_DEBUG( "MuonPatternCombinationDetailedTrackTruthMaker::execute()");

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

  const TrackCollection *tracks = 0;
  if (evtStore()->contains<TrackCollection>(m_trackCollectionName) && evtStore()->retrieve(tracks, m_trackCollectionName).isSuccess()){
    ATH_MSG_DEBUG( "Got TrackCollection " << m_trackCollectionName );
  } else {
    ATH_MSG_VERBOSE( "TrackCollection "<< m_trackCollectionName <<" NOT found");
    return StatusCode::FAILURE;
  }

  const MuonPatternCombinationCollection *muPatternCombinations = 0;
  if (evtStore()->contains<MuonPatternCombinationCollection>(m_collection) && 
      evtStore()->retrieve(muPatternCombinations, m_collection).isSuccess()){
    ATH_MSG_DEBUG( "Got MuonPatternCombinationCollection "<< m_collection );
  } else {
    ATH_MSG_VERBOSE( "MuonpatternCombinationCollection "<< m_collection <<" NOT found");
      return StatusCode::FAILURE;
  }

  //----------------------------------------------------------------
  // Produce and store the output.

//    DetailedTrackTruthCollection *dttc = new DetailedTrackTruthCollection(tracks);
//    m_truthTool->buildDetailedTrackTruth(dttc, *tracks, prdCollectionVector);

//(DetailedMuonPatternTruthCollection *output, const MuonPatternCombinationCollection& tracks, const std::vector<const PRD_MultiTruthCollection*>& prdTruth);

  DetailedMuonPatternTruthCollection *dmptc = new DetailedMuonPatternTruthCollection();
  m_truthTool->buildDetailedMuonPatternTruth(dmptc, *muPatternCombinations, prdCollectionVector);

  sc=evtStore()->record(dmptc, m_detailedTrackTruthName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "DetailedMuonPatternTrackTruthCollection '" << m_detailedTrackTruthName << "' could not be registered in StoreGate !" );
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("DetailedMuonPatternTrackTruthCollection '" << m_detailedTrackTruthName
      << "' is registered in StoreGate, size="<<dmptc->size() );
    std::cout << "Debug 7.0: DetailedTrackTruthCollection is registered in storegate." << std::endl;
  }

  return StatusCode::SUCCESS;
}

//================================================================
//EOF
