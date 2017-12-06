/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedMuonCandidateAlg.h"

MuonCombinedMuonCandidateAlg::MuonCombinedMuonCandidateAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator)
{}

MuonCombinedMuonCandidateAlg::~MuonCombinedMuonCandidateAlg()
{}

StatusCode MuonCombinedMuonCandidateAlg::initialize()
{
  ATH_CHECK(m_muonCandidateTool.retrieve());
  ATH_CHECK(m_muonTrackParticleLocation.initialize());
  ATH_CHECK(m_candidateCollectionName.initialize());
  return StatusCode::SUCCESS; 
}

StatusCode MuonCombinedMuonCandidateAlg::execute()
{

  // retrieve MuonSpectrometer tracks
  SG::ReadHandle<xAOD::TrackParticleContainer> muonTrackParticles(m_muonTrackParticleLocation);
  if(!muonTrackParticles.isValid()){
    ATH_MSG_ERROR("Could not read "<<m_muonTrackParticleLocation);
    return StatusCode::FAILURE;
  }
  if(!muonTrackParticles.isPresent()){
    ATH_MSG_WARNING(m_muonTrackParticleLocation<<" not present");
    return StatusCode::SUCCESS;
  }

  SG::WriteHandle<MuonCandidateCollection> muonCandidates(m_candidateCollectionName);
  ATH_CHECK(muonCandidates.record(std::make_unique<MuonCandidateCollection>()));
  m_muonCandidateTool->create(*muonTrackParticles,*muonCandidates);

  return StatusCode::SUCCESS;
}


StatusCode MuonCombinedMuonCandidateAlg::finalize()
{
  return StatusCode::SUCCESS;
}
