/*
  Filter algorithms for muon trigger
  
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonFilterAlg.h"

MuonFilterAlg::MuonFilterAlg(const std::string& name, ISvcLocator* pSvcLocator )
:AthReentrantAlgorithm(name, pSvcLocator)
{


}

StatusCode MuonFilterAlg::initialize(){

  ATH_CHECK(m_muonContainerKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode MuonFilterAlg::finalize()
{
  ATH_MSG_DEBUG("Finalization");

  return StatusCode::SUCCESS;
}

StatusCode MuonFilterAlg::execute(const EventContext& ctx) const
{

  bool pass = false;
  //Get muon container
  SG::ReadHandle<xAOD::MuonContainer> rh_muons(m_muonContainerKey, ctx);
  if(!rh_muons.isValid()){
    ATH_MSG_ERROR("Could not find muons with name: "<<m_muonContainerKey.key());
    return StatusCode::FAILURE;
  }

  //if we find no muons, pass
  const xAOD::MuonContainer *muons = rh_muons.ptr();
  int nCBmuons=0;
  for(auto mu : *muons){
    if(mu->author()==1) nCBmuons++; //count only combined muons
  }
  if(nCBmuons==0) pass = true;

  ATH_MSG_DEBUG("Found: "<<nCBmuons<<" muons; pass="<<pass);
  setFilterPassed(pass, ctx);

  return StatusCode::SUCCESS;
}
