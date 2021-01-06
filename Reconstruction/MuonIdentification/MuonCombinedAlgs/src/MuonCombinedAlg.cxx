/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedAlg.h"

MuonCombinedAlg::MuonCombinedAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator) {
}

StatusCode MuonCombinedAlg::initialize()
{
  ATH_CHECK(m_muonCombinedTool.retrieve());
  ATH_CHECK(m_indetCandidateCollectionName.initialize());
  ATH_CHECK(m_muonCandidateCollectionName.initialize());
  ATH_CHECK(m_combTagMaps.initialize());
  ATH_CHECK(m_muidCombinedTracks.initialize());
  ATH_CHECK(m_muidMETracks.initialize());

  return StatusCode::SUCCESS; 
}

StatusCode MuonCombinedAlg::execute()
{

  SG::ReadHandle<InDetCandidateCollection> inDetCandidateCollection(m_indetCandidateCollectionName);
  if(!inDetCandidateCollection.isValid()){
    ATH_MSG_ERROR("Could not read "<< m_indetCandidateCollectionName);
    return StatusCode::FAILURE;
  }
  if(!inDetCandidateCollection.isPresent()){
    ATH_MSG_WARNING(m_indetCandidateCollectionName<<" not present");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<MuonCandidateCollection> muonCandidateCollection(m_muonCandidateCollectionName);
  if(!muonCandidateCollection.isValid()){
    ATH_MSG_ERROR("Could not read "<< m_muonCandidateCollectionName);
    return StatusCode::FAILURE;
  }
  if(!muonCandidateCollection.isPresent()){
    ATH_MSG_WARNING(m_muonCandidateCollectionName<<" not present");
    return StatusCode::SUCCESS;
  }

  std::vector<SG::WriteHandle<MuonCombined::InDetCandidateToTagMap> > tagMaps=m_combTagMaps.makeHandles();
  std::vector<MuonCombined::InDetCandidateToTagMap*> maps;
  for(auto& h : tagMaps){
    ATH_CHECK( h.record (std::make_unique<MuonCombined::InDetCandidateToTagMap>()) );
    maps.push_back(h.ptr());
  }

  SG::WriteHandle<TrackCollection> muidCombTracks(m_muidCombinedTracks);
  ATH_CHECK(muidCombTracks.record(std::make_unique<TrackCollection>()));

  SG::WriteHandle<TrackCollection> muidMETracks(m_muidMETracks);
  ATH_CHECK(muidMETracks.record(std::make_unique<TrackCollection>()));

  if(inDetCandidateCollection->empty() || muonCandidateCollection->empty()) return StatusCode::SUCCESS;

  //note that STACO does not create new Trk::Tracks so it doesn't need collections here
  m_muonCombinedTool->combine(*muonCandidateCollection,*inDetCandidateCollection,maps,muidCombTracks.ptr(),muidMETracks.ptr());

  m_muonCombinedTool->cleanUp();
  
  return StatusCode::SUCCESS;
}

