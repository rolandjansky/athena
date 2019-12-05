/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "MuonTruthAlgs/MuonDetailedTrackTruthMaker.h"
#include <iterator>
#include <map>

//================================================================
MuonDetailedTrackTruthMaker::MuonDetailedTrackTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_truthTool("Trk::DetailedTrackTruthBuilder")
{  
  declareProperty("TruthTool",               m_truthTool);
  declareProperty("HasCSC",m_hasCSC=true);
  declareProperty("HasSTgc",m_hasSTgc=true);
  declareProperty("HasMM",m_hasMM=true);
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
    if (m_hasCSC) m_PRD_TruthNames={"sTGC_TruthMap","MM_TruthMap","RPC_TruthMap","TGC_TruthMap","MDT_TruthMap","CSC_TruthMap"};
    else m_PRD_TruthNames={"sTGC_TruthMap","MM_TruthMap","RPC_TruthMap","TGC_TruthMap","MDT_TruthMap"};
  }

  if(m_detailedTrackTruthNames.empty()){
    m_detailedTrackTruthNames.reserve ( m_trackCollectionNames.size());
    for(unsigned int i=0;i<m_trackCollectionNames.size();i++){
      m_detailedTrackTruthNames.emplace_back(m_trackCollectionNames.at(i).key()+"DetailedTruth");
      ATH_MSG_INFO("process "<<m_trackCollectionNames.at(i).key()<<" for detailed truth collection "<<m_detailedTrackTruthNames.at(i).key());
    }
  }
  else{
    for(unsigned int i=0;i<m_detailedTrackTruthNames.size();i++){
      m_detailedTrackTruthNames.at(i)=m_detailedTrackTruthNames.at(i).key()+"DetailedTruth";
    }
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

  std::map<std::string,DetailedTrackTruthCollection*> dttcMap;
  for(SG::WriteHandle<DetailedTrackTruthCollection>& h_dttc : m_detailedTrackTruthNames.makeHandles()){
    ATH_CHECK(h_dttc.record(std::make_unique<DetailedTrackTruthCollection>()));
    if(h_dttc.key().find("MSOnlyExtrapolated")!=std::string::npos) dttcMap.insert(std::pair<std::string,DetailedTrackTruthCollection*>("MSOnlyExtrapolated",h_dttc.ptr()));
    else if(h_dttc.key().find("ExtrapolatedMuonTracks")!=std::string::npos) dttcMap.insert(std::pair<std::string,DetailedTrackTruthCollection*>("METracks",h_dttc.ptr()));
    else if(h_dttc.key().find("CombinedMuonTracks")!=std::string::npos) dttcMap.insert(std::pair<std::string,DetailedTrackTruthCollection*>("CombinedTracks",h_dttc.ptr()));
    else{
      std::string cname=h_dttc.key();
      int pos=cname.find("DetailedTruth");
      cname.erase(pos,cname.length()-pos);
      dttcMap.insert(std::pair<std::string,DetailedTrackTruthCollection*>(cname,h_dttc.ptr()));
    }
  }

  int i=0;
  for(SG::ReadHandle<TrackCollection>& tcol : m_trackCollectionNames.makeHandles()){
    if(!tcol.isValid()){
      ATH_MSG_WARNING("invalid TrackCollection "<<tcol.name());
      return StatusCode::FAILURE;
    }
    if(!tcol.isPresent()) continue;

    //----------------------------------------------------------------
    // Produce and store the output.

    DetailedTrackTruthCollection* dttc=0;
    for(auto entry : dttcMap){
      if(tcol.key().find(entry.first)!=std::string::npos){
	dttc=entry.second;
	break;
      }
    }
    if(!dttc){
      ATH_MSG_WARNING("no detailed track collection found for "<<tcol.key()<<"!");
      continue;
    }
    if(!dttc->trackCollectionLink().isValid()) dttc->setTrackCollection(tcol.cptr());
    m_truthTool->buildDetailedTrackTruth(dttc, *(tcol.cptr()), prdCollectionVector);
    i++;
  }
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
