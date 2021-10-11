/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBphysMonitorAlgorithm.h"

#include "GaudiKernel/SystemOfUnits.h"

using namespace Gaudi::Units;

TrigBphysMonitorAlgorithm::TrigBphysMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
  , m_TrigBphysContainerKeys()
  , m_MonitoredChains()
  , m_MonitoredContainers()
{
  declareProperty("MonitoredChains", m_MonitoredChains);
  declareProperty("MonitoredContainers", m_MonitoredContainers);
}


TrigBphysMonitorAlgorithm::~TrigBphysMonitorAlgorithm() {}


StatusCode TrigBphysMonitorAlgorithm::initialize() {

  for(auto MonitoredContainerName : m_MonitoredContainers) {
    SG::ReadHandleKey<xAOD::TrigBphysContainer> BphysContainerKey(MonitoredContainerName);
    ATH_CHECK( BphysContainerKey.initialize() );
    m_TrigBphysContainerKeys.push_back(BphysContainerKey);
  }
  ATH_CHECK( AthMonitorAlgorithm::initialize() );
  m_mainGMT = getGroup("TrigBphysMonitor");
  return StatusCode::SUCCESS;
}


StatusCode TrigBphysMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    
  // fill the container hists
  ATH_MSG_DEBUG ("fill... Containers");
  if (fillContainerHists(ctx).isFailure()) {ATH_MSG_ERROR("Problems filling Container histograms");}
  
  // fill the chain hists
  ATH_MSG_DEBUG ("fill... Chains");
  if (fillChainHists(ctx).isFailure()) {ATH_MSG_ERROR("Problems filling Chain histograms");}

  return StatusCode::SUCCESS;
}

StatusCode TrigBphysMonitorAlgorithm::fillContainerHists(const EventContext& ctx) const {
  
  for(const SG::ReadHandleKey<xAOD::TrigBphysContainer>& monitoredContainerKey : m_TrigBphysContainerKeys) {
    ATH_MSG_DEBUG("Process container " << monitoredContainerKey.key());
    SG::ReadHandle<xAOD::TrigBphysContainer> trigBphysContainer(monitoredContainerKey, ctx);
    if( !trigBphysContainer.isValid() ) {
      ATH_MSG_ERROR("No valid TrigBphysContainer with tag: " << monitoredContainerKey);
      continue;
    }

    auto ncands = Monitored::Scalar<int>(std::string("ncandidates_Container_"+monitoredContainerKey.key()).c_str(),-999);
    ncands = trigBphysContainer->size();
    fill(m_mainGMT,ncands);
    
    for (auto bphys : (*trigBphysContainer)) {
      fillContainer(bphys, monitoredContainerKey);
    }
  }
  
  return StatusCode::SUCCESS;
}


void TrigBphysMonitorAlgorithm::fillContainer(const xAOD::TrigBphys* bphysObject,
                                              const SG::ReadHandleKey<xAOD::TrigBphysContainer>& trigBphysContainerKey) const
{
  auto mass = Monitored::Scalar<float>(std::string("dimu_mass_Container_"+trigBphysContainerKey.key()).c_str(),-999.);
  
  mass = bphysObject->fitmass() / GeV;
  
  fill(m_mainGMT,mass);
}


StatusCode TrigBphysMonitorAlgorithm::fillChainHists(const EventContext& /*ctx*/) const {
  
  for(const auto& monitoredChain : m_MonitoredChains) {
    ATH_MSG_DEBUG("Process chain " << monitoredChain);
    if( !getTrigDecisionTool()->isPassed(monitoredChain) ) {
      ATH_MSG_DEBUG("Chain " << monitoredChain << " is not passed");
      continue;
    }
    auto ncands = Monitored::Scalar<int>(std::string("ncandidates_Chain_"+monitoredChain).c_str(),-999);
    std::vector< TrigCompositeUtils::LinkInfo<xAOD::TrigBphysContainer> > chainFeatureContainer = getTrigDecisionTool()->features<xAOD::TrigBphysContainer>(monitoredChain, TrigDefs::Physics);
    ncands = chainFeatureContainer.size();
    fill(m_mainGMT,ncands);
    
  }
  
  return StatusCode::SUCCESS;
}
