/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBphysMonitorAlgorithm.h"

#include "GaudiKernel/SystemOfUnits.h"

using namespace Gaudi::Units;

TrigBphysMonitorAlgorithm::TrigBphysMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{}


TrigBphysMonitorAlgorithm::~TrigBphysMonitorAlgorithm() {}


StatusCode TrigBphysMonitorAlgorithm::initialize() {
  
  for(auto MonitoredContainerName : m_ContainerNames) {
    SG::ReadHandleKey<xAOD::TrigBphysContainer> BphysContainerKey(MonitoredContainerName);
    ATH_CHECK( BphysContainerKey.initialize() );
    m_TrigBphysContainerKeys.push_back(BphysContainerKey);
  }
  
  ATH_CHECK( AthMonitorAlgorithm::initialize() );
  
  return StatusCode::SUCCESS;
  
}


StatusCode TrigBphysMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    
  // fill the container hists
  ATH_MSG_DEBUG ("fill... Containers");
  if (fillContainers(ctx).isFailure()) {ATH_MSG_ERROR("Problems filling Container histograms");}
  
  // fill the chain hists
  ATH_MSG_DEBUG ("fill... Chains");
  if (fillChains(ctx).isFailure()) {ATH_MSG_ERROR("Problems filling Chain histograms");}

  return StatusCode::SUCCESS;
}

StatusCode TrigBphysMonitorAlgorithm::fillContainers(const EventContext& ctx) const {
  
  for(const SG::ReadHandleKey<xAOD::TrigBphysContainer>& monitoredContainerKey : m_TrigBphysContainerKeys) {
    ATH_MSG_DEBUG("Process container " << monitoredContainerKey.key().c_str());
    if(fillContainerHists(ctx, monitoredContainerKey).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling %s container histograms", monitoredContainerKey.key().c_str()));
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBphysMonitorAlgorithm::fillContainerHists(const EventContext& ctx, const SG::ReadHandleKey<xAOD::TrigBphysContainer>& trigBphysContainerKey) const {
  SG::ReadHandle<xAOD::TrigBphysContainer> trigBphysContainer(trigBphysContainerKey, ctx);
  if( !trigBphysContainer.isValid() ) {
    ATH_MSG_ERROR("No valid TrigBphysContainer with tag: " << trigBphysContainerKey);
  }
  else {
    // Fill container-wise histograms
    std::string monGroupName = std::string("Container_")+trigBphysContainerKey.key();
    auto monGroup = getGroup(monGroupName);
    
    auto ncandidates = Monitored::Scalar<int>("ncandidates",-999);
    ncandidates = trigBphysContainer->size();
    
    auto bphys_mass = Monitored::Collection("bphys_mass", (*trigBphysContainer),  [](const xAOD::TrigBphys* bphys){ return bphys->fitmass() / GeV;});
    
    fill(monGroup, ncandidates, bphys_mass);
    
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigBphysMonitorAlgorithm::fillChains(const EventContext& ctx) const {
  
  for(const auto& monitoredChain : m_ChainNames_MuMu) {
    ATH_MSG_DEBUG("Process chain " << monitoredChain);
    if( !getTrigDecisionTool()->isPassed(monitoredChain) ) {
      ATH_MSG_DEBUG("Chain " << monitoredChain << " is not passed");
      continue;
    }
    if(fillDimuonChainHists(ctx, monitoredChain).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling %s chain histograms", monitoredChain.c_str()));
    }
  }
  
  for(const auto& monitoredChain : m_ChainNames_MuMuX) {
    ATH_MSG_DEBUG("Process chain " << monitoredChain);
    if( !getTrigDecisionTool()->isPassed(monitoredChain) ) {
      ATH_MSG_DEBUG("Chain " << monitoredChain << " is not passed");
      continue;
    }
    if(fillBmumuxChainHists(ctx, monitoredChain).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling %s chain histograms", monitoredChain.c_str()));
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBphysMonitorAlgorithm::fillDimuonChainHists(const EventContext& ctx, const std::string& chainName) const {
  ATH_MSG_DEBUG("Filling  " << chainName << " chain histograms");
  
  std::string monGroupName = std::string("Chain_")+chainName;
  auto monGroup = getGroup(monGroupName);
    
  if (fillChainGenericHists(ctx, monGroup, chainName).isFailure()) {
    ATH_MSG_ERROR(Form("Problems filling generic histograms for %s chain",chainName.c_str()));
  }
  
  std::vector< TrigCompositeUtils::LinkInfo<xAOD::TrigBphysContainer> > chainFeatureContainer = getTrigDecisionTool()->features<xAOD::TrigBphysContainer>(chainName, TrigDefs::Physics);
  for (const auto& featLinkInfo: chainFeatureContainer){
    ATH_CHECK(featLinkInfo.isValid());
    const auto featLink = featLinkInfo.link;
    if (fillBphysObjectHists(monGroup, featLink, "dimu").isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling bphys object histograms for %s chain",chainName.c_str()));
    }
    if (fillMuonHists(monGroup, featLink).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling muon histograms for %s chain",chainName.c_str()));
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigBphysMonitorAlgorithm::fillBmumuxChainHists(const EventContext& ctx, const std::string& chainName) const {
  ATH_MSG_DEBUG("Filling  " << chainName << " chain histograms");
  
  std::string monGroupName = std::string("Chain_")+chainName;
  auto monGroup = getGroup(monGroupName);
    
  if (fillChainGenericHists(ctx, monGroup, chainName).isFailure()) {
    ATH_MSG_ERROR(Form("Problems filling generic histograms for %s chain",chainName.c_str()));
  }
  
  std::vector< TrigCompositeUtils::LinkInfo<xAOD::TrigBphysContainer> > chainFeatureContainer = getTrigDecisionTool()->features<xAOD::TrigBphysContainer>(chainName, TrigDefs::Physics);
  for (const auto& featLinkInfo: chainFeatureContainer){
    ATH_CHECK(featLinkInfo.isValid());
    const auto featLink = featLinkInfo.link;
    if (fillBphysObjectHists(monGroup, featLink, "B").isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling bphys object histograms for %s chain",chainName.c_str()));
    }
    
    auto dimuonLink = (*featLink)->lowerChainLink();
    ATH_CHECK(dimuonLink.isValid());
    if (fillBphysObjectHists(monGroup, dimuonLink, "dimu").isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling dimuon object histograms for %s chain",chainName.c_str()));
    }
    if (fillMuonHists(monGroup, dimuonLink).isFailure()) {
      ATH_MSG_ERROR(Form("Problems filling muon histograms for %s chain",chainName.c_str()));
    }
  }
  
  return StatusCode::SUCCESS;
}


// Function to fill general per-chain hists, independent on the type of chains
StatusCode TrigBphysMonitorAlgorithm::fillChainGenericHists(const EventContext& /*ctx*/, const ToolHandle<GenericMonitoringTool>& currentMonGroup, const std::string& chainName) const {
  
  std::vector< TrigCompositeUtils::LinkInfo<xAOD::TrigBphysContainer> > chainFeatureContainer = getTrigDecisionTool()->features<xAOD::TrigBphysContainer>(chainName, TrigDefs::Physics);
  auto ncandidates = Monitored::Scalar<int>("ncandidates",-999);
  ncandidates = chainFeatureContainer.size();
  
  fill(currentMonGroup, ncandidates);
  
  return StatusCode::SUCCESS;
}


// Function to fill per-object hists (e.g. for dimuon, B from Bmumux, or X from Bmumux)
StatusCode TrigBphysMonitorAlgorithm::fillBphysObjectHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const ElementLink<xAOD::TrigBphysContainer> bphysLink, const std::string& objStr) const {
  
  auto dimu_mass = Monitored::Scalar<float>(objStr+"_mass",-999.);
  auto dimu_pt = Monitored::Scalar<float>(objStr+"_pt",-999.);
  auto dimu_y = Monitored::Scalar<float>(objStr+"_y",-999.);
  auto dimu_chi2 = Monitored::Scalar<float>(objStr+"_chi2",-999.);
  
  dimu_mass = (*bphysLink)->mass() / GeV;
  dimu_pt   = (*bphysLink)->pt() / GeV;
  dimu_chi2 = (*bphysLink)->fitchi2();
  
  fill(currentMonGroup, dimu_mass, dimu_pt, dimu_y, dimu_chi2);
  
  return StatusCode::SUCCESS;
}


// Function to fill per-muon hists, assuming that the passed object is a dimuon
StatusCode TrigBphysMonitorAlgorithm::fillMuonHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const ElementLink<xAOD::TrigBphysContainer> bphysLink) const {
  
  const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = (*bphysLink)->trackParticleLinks();
  if (trackVector.size() <2) {
      ATH_MSG_ERROR("Unexpected number of tracks in a dimuon: " << trackVector.size());
      return StatusCode::FAILURE;
  }
  
  const xAOD::TrackParticle *trk1(nullptr),*trk2(nullptr);
  ATH_CHECK(trackVector.at(0).isValid());
  ATH_CHECK(trackVector.at(1).isValid());
  trk1 = *trackVector.at(0);
  trk2 = *trackVector.at(1);
  if (!trk1 || !trk2) {
      ATH_MSG_ERROR("Null pointer for track in a dimuon!");
      return StatusCode::FAILURE;
  }
  
  auto mu1_pt  = Monitored::Scalar<float>("mu1_pt",-999.);
  auto mu1_eta = Monitored::Scalar<float>("mu1_eta",-999.);
  auto mu1_d0  = Monitored::Scalar<float>("mu1_d0",-999.);
  auto mu2_pt  = Monitored::Scalar<float>("mu2_pt",-999.);
  auto mu2_eta = Monitored::Scalar<float>("mu2_eta",-999.);
  auto mu2_d0  = Monitored::Scalar<float>("mu2_d0",-999.);
  
  mu1_pt  = trk1->pt() / GeV;
  mu1_eta = trk1->eta();
  mu1_d0  = trk1->d0();
  mu2_pt  = trk2->pt() / GeV;
  mu2_eta = trk2->eta();
  mu2_d0  = trk2->d0();
  
  fill(currentMonGroup, mu1_pt, mu1_eta, mu1_d0, mu2_pt, mu2_eta, mu2_d0);
  
  return StatusCode::SUCCESS;
}
