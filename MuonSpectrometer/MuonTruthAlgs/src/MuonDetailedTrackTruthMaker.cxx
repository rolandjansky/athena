/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "MuonTruthAlgs/MuonDetailedTrackTruthMaker.h"

#include <iterator>
#include <vector>

//================================================================
MuonDetailedTrackTruthMaker::MuonDetailedTrackTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{
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
  
  if (m_hasSTgc && m_hasMM) {
    if (m_hasCSC) m_PRD_TruthNames={"STGC_TruthMap","MM_TruthMap","RPC_TruthMap","TGC_TruthMap","MDT_TruthMap","CSC_TruthMap"};
    else m_PRD_TruthNames={"STGC_TruthMap","MM_TruthMap","RPC_TruthMap","TGC_TruthMap","MDT_TruthMap"};
  }

  m_detailedTrackTruthNames.reserve ( m_trackCollectionNames.size());
  for(unsigned int i=0;i<m_trackCollectionNames.size();i++){
    m_detailedTrackTruthNames.emplace_back(m_trackCollectionNames.at(i).key()+"DetailedTruth");
    ATH_MSG_INFO("process "<<m_trackCollectionNames.at(i).key()<<" for detailed truth collection "<<m_detailedTrackTruthNames.at(i).key());
  }

  ATH_CHECK(m_trackCollectionNames.initialize());
  ATH_CHECK(m_PRD_TruthNames.initialize());
  ATH_CHECK(m_detailedTrackTruthNames.initialize());
  
  //----------------
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
