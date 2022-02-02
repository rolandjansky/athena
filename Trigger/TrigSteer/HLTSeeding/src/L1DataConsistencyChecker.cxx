/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// Local includes
#include "L1DataConsistencyChecker.h"
#include "HLTSeedingRoIToolDefs.h"
// Trigger includes
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigConfData/L1Item.h"
#include "TrigConfData/LogicParser.h"
// Athena includes
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
// System includes
#include <unordered_map>
#include <sstream>

namespace{
  // this can be constexpr in C++20
  bool vectorContains(const std::vector<std::string>& vec, const std::string& str) {
    return (std::find(vec.cbegin(),vec.cend(),str)!=vec.cend());
  }
}

// =============================================================================
StatusCode L1DataConsistencyChecker::initialize() {
  ATH_CHECK(m_hltMenuKey.initialize());
  ATH_CHECK(m_l1MenuKey.initialize());
  if (!m_monTool.empty()) {
    ATH_CHECK(m_monTool.retrieve());
  }
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode L1DataConsistencyChecker::start() {
  SG::ReadHandle<TrigConf::L1Menu> l1Menu{m_l1MenuKey};
  ATH_CHECK(l1Menu.isValid());
  SG::ReadHandle<TrigConf::HLTMenu> hltMenu{m_hltMenuKey};
  ATH_CHECK(hltMenu.isValid());

  TrigConf::LogicParser logicParser;

  m_thresholdMap.clear();
  for (const TrigConf::Chain& chain : *hltMenu) {
    try {
      // For chains with multiple seeds, we cannot determine in this tool which seed passed and which didn't,
      // so we need to skip these kinds of chains
      if (chain.l1item().empty() || chain.l1item().find(',')!=std::string::npos) {continue;}

      TrigConf::L1Item item = l1Menu->item(chain.l1item());
      std::map<std::string,unsigned int> thresholds = logicParser.parse(item.definition())->elementsCount();
      for (const auto& [thrName, multiplicity] : thresholds) {
        const TrigConf::L1Threshold& thr = l1Menu->threshold(thrName);
        // Skip thresholds for which there are no TOBs, e.g. randoms
        if (vectorContains(m_excludedThresholdTypes.value(), thr.type())) {
          continue;
        }
        TrigCompositeUtils::DecisionID thrNameHash = HLT::Identifier(thr.name());
        m_thresholdMap[chain.namehash()].insert({thrNameHash, multiplicity});
        m_monitoredThresholds.insert(thrNameHash);
      }
    }
    catch (const std::exception& ex) {
      ATH_MSG_ERROR("Exception caught while parsing L1 thresholds for chain " << chain.name() << ": " << ex.what());
      return StatusCode::FAILURE;
    }
  }

  for (const std::shared_ptr<TrigConf::L1Threshold>& thr : l1Menu->thresholds()) {
    m_thresholdNames[HLT::Identifier(thr->name()).numeric()] = thr->name();
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode L1DataConsistencyChecker::consistencyCheck(const HLT::IDVec& l1SeededChains, const EventContext& ctx) const {
  auto monTimer = Monitored::Timer<std::chrono::duration<float, std::milli>>("TIME_consistencyCheck");

  // ---------------------------------------------------------------------------
  // Get information from TOBs
  // ---------------------------------------------------------------------------
  std::ostringstream ssPassedThresholds; // ss for debug printouts, allocate before loop
  std::unordered_map<TrigCompositeUtils::DecisionID, size_t> tobThresholdCounts; // {thrName hash, nTOBs passing thr}
  // Loop over threshold types
  for (const auto& [thrType, decisionsKey] : m_thresholdToDecisionMap.value()) {
    SG::ReadHandle<TrigCompositeUtils::DecisionContainer> decisions{decisionsKey, ctx.getExtension<Atlas::ExtendedEventContext>().proxy()->name()};
    if (!decisions.isValid()) {
      ATH_MSG_DEBUG("No DecisionContainer " << decisionsKey << " for threshold type " << thrType << " found in this event");
      continue;
    }
    size_t idec{0}; /// Counter for debug printouts
    // Loop over TOBs of the given threshold type (one decision = one TOB)
    for (const TrigCompositeUtils::Decision* d : *decisions) {
      std::vector<TrigCompositeUtils::DecisionID> passedThresholdIDs;
      const bool hasDetail = d->getDetail("thresholds", passedThresholdIDs);
      if (!hasDetail) {
        ATH_MSG_ERROR("Detail \"thresholds\" missing from Decision in the container " << decisionsKey);
        return StatusCode::FAILURE;
      }
      if (doDebug()) {ssPassedThresholds.str("");}
      for (const TrigCompositeUtils::DecisionID thrNameHash : passedThresholdIDs) {
        if (tobThresholdCounts.find(thrNameHash)==tobThresholdCounts.end()) {
          tobThresholdCounts[thrNameHash] = 1;
        }
        else {
          tobThresholdCounts[thrNameHash] += 1;
        }
        if (doDebug()) {
          if (!ssPassedThresholds.str().empty()) ssPassedThresholds << ", ";
          ssPassedThresholds << m_thresholdNames.at(thrNameHash);
        }
      }
      if (doDebug()) {
        namespace RoIDefs = HLTSeedingRoIToolDefs;
        std::optional<RoIDefs::AnyRoIPointer> optTob = RoIDefs::roiFromLink(*d, TrigCompositeUtils::initialRecRoIString());
        if (optTob.has_value()) {
          const RoIDefs::AnyRoIPointer& tob = optTob.value();
          ATH_MSG_DEBUG("Decision " << decisionsKey << "[" << idec << "] corresponds to TOB with word / et / eta / phi = 0x"
                        << MSG::hex << RoIDefs::roiWord(tob) << MSG::dec << " / " << RoIDefs::roiTobEt(tob)
                        << " / " << RoIDefs::roiEta(tob) << " / " << RoIDefs::roiPhi(tob));
        } else {
          ATH_MSG_DEBUG("Decision " << decisionsKey << "[" << idec << "] TOB type not supported by template code for "
                        << "debug printout of word / et / eta / phi");
        }
        ATH_MSG_DEBUG("Decision " << decisionsKey << "[" << idec << "] TOB passes " << passedThresholdIDs.size()
                      << " thresholds: " << ssPassedThresholds.str());
        ++idec;
      }
    }
  }

  // ---------------------------------------------------------------------------
  // Get information from CTP
  // ---------------------------------------------------------------------------
  std::unordered_map<TrigCompositeUtils::DecisionID, size_t> ctpThresholdCounts; // {thrName hash, max CTP multiplicity}
  // Loop over HLT chains activated by CTP
  for (const TrigCompositeUtils::DecisionID chainID : l1SeededChains) {
    const auto it = m_thresholdMap.find(chainID);
    if (it==m_thresholdMap.end()) {
      ATH_MSG_DEBUG("Cannot monitor chain " << chainID << " -> " << HLT::Identifier(chainID).name() << ", skipping");
      continue;
    }
    const MultiplicityInfo& thresholds = it->second;
    // Loop over all thresholds required to activate this chain
    for (const auto& [thrNameHash, multiplicity] : thresholds) {
      if (ctpThresholdCounts.find(thrNameHash)==ctpThresholdCounts.end()) {
        ctpThresholdCounts[thrNameHash] = multiplicity;
      }
      else if (multiplicity > ctpThresholdCounts[thrNameHash]) {
        ctpThresholdCounts[thrNameHash] = multiplicity;
      }
    }
  }

  // ---------------------------------------------------------------------------
  // Perform the consistency check
  // ---------------------------------------------------------------------------
  for (const auto& [thrNameHash, ctpCount] : ctpThresholdCounts) {
    size_t tobCount{0};
    bool tooFew{false};
    const auto it = m_thresholdNames.find(thrNameHash);
    if (it==m_thresholdNames.end()) {
      ATH_MSG_ERROR("Threshold with hash " << thrNameHash << " not found in the hash->name map");
      return StatusCode::FAILURE;
    }
    const std::string& thrName = it->second;

    // True if no TOBs found for this threshold but CTP accepted
    bool missing = (tobThresholdCounts.find(thrNameHash)==tobThresholdCounts.end());

    if (missing) {
      ATH_MSG_WARNING("No TOBs found passing the threshold " << thrName
                      << " accepted by the CTP with multiplicity " << ctpCount);
    }
    else {
      tobCount = tobThresholdCounts.at(thrNameHash);
      // True if fewer TOBs found for this threshold than the multiplicity accepted by CTP
      tooFew = (tobCount < ctpCount);
    }

    if (tooFew) {
      ATH_MSG_WARNING("Too few (" << tobCount << ") TOBs found passing the threshold " << thrName
                      << " accepted by the CTP with multiplicity " << ctpCount);
    }

    if (missing || tooFew) {
      // Fill the histogram
      Monitored::Scalar monMissingTOBs{"MissingTOBs", thrName};
      Monitored::Group{m_monTool, monMissingTOBs};
    }
    else {
      ATH_MSG_DEBUG("Check passed. Found " << tobCount << " TOBs passing the threshold " << thrName
                    << " accepted by the CTP with multiplicity " << ctpCount);
    }
  }

  // Fill extra histograms for debugging
  for (const TrigCompositeUtils::DecisionID thrNameHash : m_monitoredThresholds) {
    const auto ctpIt = ctpThresholdCounts.find(thrNameHash);
    size_t ctpCount = (ctpIt==ctpThresholdCounts.cend()) ? 0 : ctpIt->second;
    const auto tobIt = tobThresholdCounts.find(thrNameHash);
    size_t tobCount = (tobIt==tobThresholdCounts.cend()) ? 0 : tobIt->second;
    const std::string& thrName = m_thresholdNames.at(thrNameHash);

    Monitored::Scalar monThrName{"ThrName", thrName};
    Monitored::Scalar monCtpCount{"CtpCount", ctpCount};
    Monitored::Scalar monTobCount{"TobCount", tobCount};
    Monitored::Scalar monTobMinusCtpCount{"TobMinusCtpCount", tobCount-ctpCount};
    Monitored::Group{m_monTool, monThrName, monCtpCount, monTobCount, monTobMinusCtpCount};
  }

  Monitored::Group(m_monTool, monTimer);
  return StatusCode::SUCCESS;
}
