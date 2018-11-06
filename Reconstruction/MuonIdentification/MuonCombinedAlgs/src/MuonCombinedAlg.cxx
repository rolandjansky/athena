/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedAlg.h"

MuonCombinedAlg::MuonCombinedAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator)
{}

MuonCombinedAlg::~MuonCombinedAlg(){}

StatusCode MuonCombinedAlg::initialize()
{
  ATH_CHECK(m_muonCombinedTool.retrieve());
  ATH_CHECK(m_indetCandidateCollectionName.initialize());
  ATH_CHECK(m_muonCandidateCollectionName.initialize());
  ATH_CHECK(m_combTagMaps.initialize());

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

  if(inDetCandidateCollection->empty() || muonCandidateCollection->empty()) return StatusCode::SUCCESS;

  m_muonCombinedTool->combine(*muonCandidateCollection,*inDetCandidateCollection,maps);

  return StatusCode::SUCCESS;
}

StatusCode MuonCombinedAlg::finalize()
{
  return StatusCode::SUCCESS;
}
