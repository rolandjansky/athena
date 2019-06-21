/*
  Filter algorithms for muon trigger
  
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonFilterAlg.h"

MuonFilterAlg::MuonFilterAlg(const std::string& name, ISvcLocator* pSvcLocator )
:AthAlgorithm(name, pSvcLocator)
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

StatusCode MuonFilterAlg::execute()
{

  bool pass = false;
  //Get muon container
  SG::ReadHandle<xAOD::MuonContainer> rh_muons(m_muonContainerKey);
  if(!rh_muons.isValid()){
    ATH_MSG_ERROR("Could not find muons with name: "<<m_muonContainerKey.key());
    return StatusCode::FAILURE;
  }

  //if we find no muons, pass
  const xAOD::MuonContainer *muons = rh_muons.ptr();
  if(muons->size()==0) pass = true;
  ATH_MSG_DEBUG("Found: "<<muons->size()<<" muons; pass="<<pass);
  setFilterPassed(pass);

  return StatusCode::SUCCESS;
}
