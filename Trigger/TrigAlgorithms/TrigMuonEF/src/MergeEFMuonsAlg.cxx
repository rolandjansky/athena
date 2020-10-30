/*
  Merge muon containers from inside-out and outside-in reconstruction
  
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeEFMuonsAlg.h"
#include "xAODMuon/MuonAuxContainer.h"

MergeEFMuonsAlg::MergeEFMuonsAlg(const std::string& name, ISvcLocator* pSvcLocator )
:AthReentrantAlgorithm(name, pSvcLocator)
{


}

StatusCode MergeEFMuonsAlg::initialize(){

  ATH_CHECK(m_muonCBContainerKey.initialize());
  renounce(m_muonInsideOutContainerKey);
  ATH_CHECK(m_muonInsideOutContainerKey.initialize());
  ATH_CHECK(m_muonOutputKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode MergeEFMuonsAlg::finalize()
{
  ATH_MSG_DEBUG("Finalization");

  return StatusCode::SUCCESS;
}

StatusCode MergeEFMuonsAlg::execute(const EventContext& ctx) const
{
  
  SG::WriteHandle<xAOD::MuonContainer> wh_outmuons(m_muonOutputKey, ctx);
  ATH_CHECK(wh_outmuons.record(std::make_unique<xAOD::MuonContainer>(), std::make_unique<xAOD::MuonAuxContainer>()));
  xAOD::MuonContainer *muonsout = wh_outmuons.ptr();

  SG::ReadHandle<xAOD::MuonContainer> rh_cbmuons(m_muonCBContainerKey, ctx);
  if(rh_cbmuons.isValid()){
    const xAOD::MuonContainer *cbMuons = rh_cbmuons.ptr();
    ATH_MSG_DEBUG("adding outside-in muons with size: "<<cbMuons->size());
    for(auto muon : *cbMuons){
      muonsout->push_back(new xAOD::Muon(*muon));
    }
  }

  SG::ReadHandle<xAOD::MuonContainer> rh_insideoutmuons(m_muonInsideOutContainerKey, ctx);
  if(rh_insideoutmuons.isValid()){
    const xAOD::MuonContainer *insideOutMuons = rh_insideoutmuons.ptr();
    ATH_MSG_DEBUG("adding inside-out muons with size: "<<insideOutMuons->size());
    for(auto muon : *insideOutMuons){
      muonsout->push_back(new xAOD::Muon(*muon));
    }
  }

  return StatusCode::SUCCESS;

}
