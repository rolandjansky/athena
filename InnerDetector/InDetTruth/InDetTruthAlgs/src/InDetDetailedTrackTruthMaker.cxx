/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "InDetTruthAlgs/InDetDetailedTrackTruthMaker.h"

#include <iterator>

namespace InDet {

//================================================================
InDetDetailedTrackTruthMaker::InDetDetailedTrackTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  AthReentrantAlgorithm(name,pSvcLocator),
  m_PRDTruthNamePixel("PRD_MultiTruthPixel"),
  m_PRDTruthNameSCT("PRD_MultiTruthSCT"),
  m_PRDTruthNameTRT("PRD_MultiTruthTRT"),
  m_trackCollectionName("Tracks"),
  m_detailedTrackTruthName("DetailedTrackTruth"),
  m_truthTool("Trk::DetailedTrackTruthBuilder")
{
  declareProperty("TruthNamePixel",          m_PRDTruthNamePixel);
  declareProperty("TruthNameSCT",            m_PRDTruthNameSCT);
  declareProperty("TruthNameTRT",            m_PRDTruthNameTRT);
  declareProperty("TrackCollectionName",     m_trackCollectionName);
  declareProperty("DetailedTrackTruthName",  m_detailedTrackTruthName);
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
    ATH_MSG_DEBUG ("Retrieved tool " << m_truthTool);
  }

  // Read Handle Key
  ATH_CHECK(m_trackCollectionName.initialize());

  ATH_CHECK(m_PRDTruthNamePixel.initialize(not m_PRDTruthNamePixel.key().empty()));
  ATH_CHECK(m_PRDTruthNameSCT.initialize(not m_PRDTruthNameSCT.key().empty()));
  ATH_CHECK(m_PRDTruthNameTRT.initialize(not m_PRDTruthNameTRT.key().empty()));

  ATH_CHECK(m_detailedTrackTruthName.initialize());

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
StatusCode InDetDetailedTrackTruthMaker::execute_r(const EventContext &ctx) const
{
  ATH_MSG_DEBUG ("InDetDetailedTrackTruthMaker::execute_r(...)");

  //----------------------------------------------------------------
  // Retrieve track collection
  SG::ReadHandle<TrackCollection> tracks(m_trackCollectionName,ctx);
  if (!tracks.isValid()) {
    ATH_MSG_ERROR ("TrackCollection "<<m_trackCollectionName.key()<<" NOT found");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ("Got TrackCollection "<<m_trackCollectionName.key());
  }

  //----------------------------------------------------------------
  // Retrieve prep raw data truth
  std::vector<SG::ReadHandle<PRD_MultiTruthCollection> > read_handle;
  read_handle.reserve(3);

  std::vector<const PRD_MultiTruthCollection*> prdCollectionVector;
  prdCollectionVector.reserve(3);

  if(!m_PRDTruthNamePixel.key().empty()) {
    read_handle.push_back( SG::ReadHandle<PRD_MultiTruthCollection>(m_PRDTruthNamePixel,ctx) );
    if (!read_handle.back().isValid())  {
      ATH_MSG_WARNING ("Pixel PRD_MultiTruthCollection "<<m_PRDTruthNamePixel.key()<<" NOT found");
    }
    else {
      ATH_MSG_DEBUG ("Got Pixel PRD_MultiTruthCollection "<<m_PRDTruthNamePixel.key());
      prdCollectionVector.push_back( &(*read_handle.back()) );
    }
  }

  if(!m_PRDTruthNameSCT.key().empty()) {
    read_handle.push_back( SG::ReadHandle<PRD_MultiTruthCollection>(m_PRDTruthNameSCT,ctx) );
    if (!read_handle.back().isValid())  {
      ATH_MSG_WARNING ("SCT PRD_MultiTruthCollection "<<m_PRDTruthNameSCT.key()<<" NOT found");
    } else {
      ATH_MSG_DEBUG ("Got SCT PRD_MultiTruthCollection "<<m_PRDTruthNameSCT.key());
      prdCollectionVector.push_back( &(*read_handle.back()) );
    }
  }

  if(!m_PRDTruthNameTRT.key().empty()) {
    read_handle.push_back( SG::ReadHandle<PRD_MultiTruthCollection>(m_PRDTruthNameTRT,ctx) );
    if (!read_handle.back().isValid())  {
      ATH_MSG_WARNING ("TRT PRD_MultiTruthCollection "<<m_PRDTruthNameTRT.key()<<" NOT found");
    } else {
      ATH_MSG_DEBUG ("Got TRT PRD_MultiTruthCollection "<<m_PRDTruthNameTRT.key());
      prdCollectionVector.push_back( &(*read_handle.back()) );
    }
  }

  //----------------------------------------------------------------
  // Produce and store the output.
  SG::WriteHandle<DetailedTrackTruthCollection> dttc(m_detailedTrackTruthName,ctx);
  if (dttc.record(std::make_unique<DetailedTrackTruthCollection>(&(*tracks))).isFailure()) {
    ATH_MSG_ERROR ("DetailedTrackTruthCollection '" << m_detailedTrackTruthName.key() << "' could not be recorded !");
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ("DetailedTrackTruthCollection '" << m_detailedTrackTruthName.key()	<< "' is registered in StoreGate, size="<<dttc->size());
    m_truthTool->buildDetailedTrackTruth(&(*dttc), *tracks, prdCollectionVector);
    return StatusCode::SUCCESS;
  }

}

} // namespace InDet
