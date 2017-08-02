/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedInDetCandidateAlg.h"
#include "MuonCombinedToolInterfaces/IInDetCandidateTool.h"


MuonCombinedInDetCandidateAlg::MuonCombinedInDetCandidateAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_indetCandidateTool("MuonCombined::InDetCandidateTool/InDetCandidateTool")
{  
  declareProperty("InDetCandidateTool",m_indetCandidateTool);
  declareProperty("InDetForwardCandidateTool",m_indetForwardCandidateTool);
  declareProperty("TrackParticleLocation",m_indetTrackParticleLocation = "InDetTrackParticles");
  declareProperty("ForwardParticleLocation",m_indetForwardTrackParticleLocation = "InDetForwardTrackParticles");
  declareProperty("InDetCandidateLocation", m_candidateCollectionName = "InDetCandidates");
  // declareProperty("ReadInDetCandidateLocation", m_indetCandidateCollectionName = "InDetCandidates");
  declareProperty("DoSiliconAssocForwardMuons",m_doSiliconForwardMuons = false);
}

MuonCombinedInDetCandidateAlg::~MuonCombinedInDetCandidateAlg(){}

StatusCode MuonCombinedInDetCandidateAlg::initialize()
{
  ATH_CHECK(m_indetCandidateTool.retrieve());
  if (m_doSiliconForwardMuons) {
    ATH_CHECK(m_indetForwardCandidateTool.retrieve());
  }
  ATH_CHECK(m_indetTrackParticleLocation.initialize());
  ATH_CHECK(m_indetForwardTrackParticleLocation.initialize());
  ATH_CHECK(m_candidateCollectionName.initialize());
  // ATH_CHECK(m_indetCandidateCollectionName.initialize());

  return StatusCode::SUCCESS; 
}

StatusCode MuonCombinedInDetCandidateAlg::execute()
{
  ATH_MSG_INFO("MARCUS: execute InDetAlg");
  // retrieve MuonSpectrometer tracks
  std::vector<ToolHandle<MuonCombined::IInDetCandidateTool> > canTool;
  std::vector<SG::ReadHandleKey<xAOD::TrackParticleContainer> > trackParticleLoc;

  canTool.push_back(m_indetCandidateTool);
  trackParticleLoc.push_back(m_indetTrackParticleLocation);

  if (m_doSiliconForwardMuons){
    canTool.push_back(m_indetForwardCandidateTool);
    trackParticleLoc.push_back(m_indetForwardTrackParticleLocation);
  }


  //output collection
  auto collection = std::make_unique<InDetCandidateCollection>(SG::OWN_ELEMENTS);

  for (unsigned int i(0);i<canTool.size();++i){

    SG::ReadHandle<xAOD::TrackParticleContainer> indetTrackParticles(trackParticleLoc[i]);
    if(!indetTrackParticles.isValid()){
      ATH_MSG_ERROR("Could not read "<< trackParticleLoc[i]);
      return StatusCode::FAILURE;
    }
  
    InDetCandidateCollection* tempCandidates = new InDetCandidateCollection(SG::VIEW_ELEMENTS);
    canTool[i]->create(*indetTrackParticles,*tempCandidates);

    collection->insert(collection->end(), tempCandidates->begin(), tempCandidates->end());

    delete tempCandidates;
  }
  SG::WriteHandle<InDetCandidateCollection> indetCandidateCollection(m_candidateCollectionName);
  ATH_CHECK(indetCandidateCollection.record(std::move(collection)));
  ATH_MSG_DEBUG("MARCUS: INDET SIZE: " << indetCandidateCollection->size());
  ATH_MSG_INFO("MARCUS: INDET SIZE: " << indetCandidateCollection->size());

  // SG::ReadHandle<InDetCandidateCollection> indetCandidateCollection2(m_indetCandidateCollectionName);
  // if(!indetCandidateCollection2.isValid()){
  //   ATH_MSG_ERROR("Could not read "<< m_indetCandidateCollectionName);
  //   return StatusCode::FAILURE;
  // }
  // ATH_MSG_INFO("MARCUS: INDET 2 SIZE: " << indetCandidateCollection2->size());

  return StatusCode::SUCCESS;
}


StatusCode MuonCombinedInDetCandidateAlg::finalize()
{
  return StatusCode::SUCCESS;
}
