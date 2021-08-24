/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "METRoIsUnpackingTool.h"
#include "TrigT1Result/JetEnergyResult.h"
#include "TrigT1Result/RoIBResult.h"


METRoIsUnpackingTool::METRoIsUnpackingTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
  : RoIsUnpackingToolBase(type, name, parent) {}


StatusCode METRoIsUnpackingTool::initialize() {
  ATH_CHECK(RoIsUnpackingToolBase::initialize());
  return StatusCode::SUCCESS;
}


StatusCode METRoIsUnpackingTool::start() {
  ATH_CHECK(decodeMapping([](const std::string& name){
    return name.find("TE") == 0 or name.find("XE") == 0 or name.find("XS") == 0;
  }));

  for ( auto th2chains: m_thresholdToChainMapping ) {
    m_allMETChains.insert( th2chains.second.begin(), th2chains.second.end() );
  }

  return StatusCode::SUCCESS;
}


StatusCode METRoIsUnpackingTool::unpack(const EventContext& ctx,
                                        const ROIB::RoIBResult& roib,
                                        const HLT::IDSet& activeChains) const {
  using namespace TrigCompositeUtils;
  SG::WriteHandle<DecisionContainer> decisionOutput = createAndStore(m_decisionsKey, ctx );

  // Retrieve the L1 menu configuration
  SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_l1MenuKey, ctx);
  ATH_CHECK(l1Menu.isValid());
  ThrVec thresholds;
  std::optional<ThrVecRef> xeThresholds;
  std::optional<ThrVecRef> teThresholds;
  std::optional<ThrVecRef> xsThresholds;
  ATH_CHECK(getL1Thresholds(*l1Menu, "XE", xeThresholds));
  ATH_CHECK(getL1Thresholds(*l1Menu, "TE", teThresholds));
  ATH_CHECK(getL1Thresholds(*l1Menu, "XS", xsThresholds));
  thresholds.insert(thresholds.end(), xeThresholds.value().get().cbegin(), xeThresholds.value().get().cend());
  thresholds.insert(thresholds.end(), teThresholds.value().get().cbegin(), teThresholds.value().get().cend());
  thresholds.insert(thresholds.end(), xsThresholds.value().get().cbegin(), xsThresholds.value().get().cend());

  HLT::IDSet activeMETchains;
  // see if any chain we care of is active
  std::set_intersection(activeChains.begin(), activeChains.end(),
                        m_allMETChains.begin(), m_allMETChains.end(),
                        std::inserter(activeMETchains, activeMETchains.end()));

  ATH_MSG_DEBUG("Unpacking MET RoI for " << activeMETchains.size() << " chains");

  // This hltSeedingNodeName() denotes an initial node with no parents
  auto *decision  = TrigCompositeUtils::newDecisionIn( decisionOutput.ptr(), hltSeedingNodeName() );
  for (const auto& th: thresholds) {
    addChainsToDecision(  HLT::Identifier( th->name() ), decision, activeChains );
  }

  ATH_MSG_DEBUG("Linking to FS RoI descriptor");
  decision->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_fsRoIKey, 0 ) );

  // check the MET RoI, TODO unpack and create L1 MET object (only if turns out to be needed)
  bool foundMETRoI = false;
  const std::vector<ROIB::JetEnergyResult>& jetEnergyResult = roib.jetEnergyResult();
  for ( const ROIB::JetEnergyResult& jeResult: jetEnergyResult ) {
    for ( const ROIB::JetEnergyRoI& roi: jeResult.roIVec() ) {
      const LVL1::TrigT1CaloDefs::RoIType roiType = m_jepDecoder.roiType( roi.roIWord() );
      if ( roiType == LVL1::TrigT1CaloDefs::EnergyRoIWordType0 ) {
        foundMETRoI = true;
        break;
      }
    }
  }

  // inconsistency, active MET chains, yet missing MET RoI
  if ( (not activeMETchains.empty()) and not foundMETRoI) {
    ATH_MSG_WARNING( "" << activeMETchains.size() << " active MET chains while missing  MET RoI" );
  }
  return StatusCode::SUCCESS;
}
