/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "InDetTruthAlgs/InDetDetailedTrackTruthMaker.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTrack/TrackCollection.h"

#include <iterator>

namespace InDet {

//================================================================
InDetDetailedTrackTruthMaker::InDetDetailedTrackTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_truthTool("Trk::DetailedTrackTruthBuilder")
{  
  declareProperty("TruthNamePixel",          m_PRDTruthNamePixel      = "PRD_MultiTruthPixel");
  declareProperty("TruthNameSCT",            m_PRDTruthNameSCT        = "PRD_MultiTruthSCT");
  declareProperty("TruthNameTRT",            m_PRDTruthNameTRT        = "PRD_MultiTruthTRT");
  declareProperty("TrackCollectionName",     m_trackCollectionName    = "Tracks");
  declareProperty("DetailedTrackTruthName",  m_detailedTrackTruthName = "DetailedTrackTruth");
  declareProperty("TruthTool",               m_truthTool);
}

// Initialize method
// -----------------------------------------------------------------------------------------------------
StatusCode InDetDetailedTrackTruthMaker::initialize()
{
  ATH_MSG_DEBUG ("InDetDetailedTrackTruthMaker::initialize()");
  
  //----------------
  if ( m_truthTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL ("Failed to retrieve tool " << m_truthTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ("Retrieved tool " << m_truthTool);
  }

  //----------------
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode InDetDetailedTrackTruthMaker::finalize() 
{
  ATH_MSG_DEBUG ("InDetDetailedTrackTruthMaker finalized");
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode InDetDetailedTrackTruthMaker::execute()
{
  ATH_MSG_DEBUG ("InDetDetailedTrackTruthMaker::execute()");

  StatusCode sc;

  //----------------------------------------------------------------
  // Retrieve track collection
  const TrackCollection *tracks = 0;
  sc = evtStore()->retrieve(tracks, m_trackCollectionName);
  if (sc.isFailure()){
    ATH_MSG_ERROR ("TrackCollection "<<m_trackCollectionName<<" NOT found");
    return sc;
  } else {
    ATH_MSG_DEBUG ("Got TrackCollection "<<m_trackCollectionName);
  }

  //----------------------------------------------------------------
  // Retrieve prep raw data truth
  std::vector<const PRD_MultiTruthCollection*> prdCollectionVector(3);

  if(!m_PRDTruthNamePixel.empty()) {
    sc = evtStore()->retrieve(prdCollectionVector[0], m_PRDTruthNamePixel);
    if (sc.isFailure()){
      ATH_MSG_WARNING ("Pixel PRD_MultiTruthCollection "<<m_PRDTruthNamePixel<<" NOT found");
    } else {
      ATH_MSG_DEBUG ("Got Pixel PRD_MultiTruthCollection "<<m_PRDTruthNamePixel);
    }
  }

  if(!m_PRDTruthNameSCT.empty()) {
    sc = evtStore()->retrieve(prdCollectionVector[1], m_PRDTruthNameSCT);
    if (sc.isFailure()){
      ATH_MSG_WARNING ("SCT PRD_MultiTruthCollection "<<m_PRDTruthNameSCT<<" NOT found");
    } else {
      ATH_MSG_DEBUG ("Got SCT PRD_MultiTruthCollection "<<m_PRDTruthNameSCT);
    }
  }

  if(!m_PRDTruthNameTRT.empty()) {
    sc = evtStore()->retrieve(prdCollectionVector[2], m_PRDTruthNameTRT);
    if (sc.isFailure()){
      ATH_MSG_WARNING ("TRT PRD_MultiTruthCollection "<<m_PRDTruthNameTRT<<" NOT found");
    } else {
      ATH_MSG_DEBUG ("Got TRT PRD_MultiTruthCollection "<<m_PRDTruthNameTRT);
    }
  }

  //----------------------------------------------------------------
  // Produce and store the output.

  DetailedTrackTruthCollection *dttc = new DetailedTrackTruthCollection(tracks);
  m_truthTool->buildDetailedTrackTruth(dttc, *tracks, prdCollectionVector);

  sc=evtStore()->record(dttc, m_detailedTrackTruthName, false);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("DetailedTrackTruthCollection '" << m_detailedTrackTruthName << "' could not be registered in StoreGate !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ("DetailedTrackTruthCollection '" << m_detailedTrackTruthName	<< "' is registered in StoreGate, size="<<dttc->size());
  }
  
  return StatusCode::SUCCESS;
}

//================================================================
} // namespace InDet


//================================================================
//EOF
