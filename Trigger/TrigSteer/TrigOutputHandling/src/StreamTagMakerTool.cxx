/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "StreamTagMakerTool.h"
#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfData/DataStructure.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/HLTChain.h"
#include "eformat/StreamTag.h"

using namespace TrigCompositeUtils;

// =============================================================================

StreamTagMakerTool::StreamTagMakerTool( const std::string& type, const std::string& name, const IInterface* parent ):
  base_class( type, name, parent ) {}

// =============================================================================

StreamTagMakerTool::~StreamTagMakerTool() {}

// =============================================================================

StatusCode StreamTagMakerTool::initialize() {
  renounceArray( m_pebDecisionKeys );
  ATH_CHECK( m_pebDecisionKeys.initialize() );
  ATH_CHECK( m_finalChainDecisions.initialize() );

  TrigConf::JsonFileLoader fileLoader;
  TrigConf::HLTMenu hltMenu;
  ATH_CHECK( fileLoader.loadFile(m_menuJSON, hltMenu) );

  ATH_MSG_INFO("Configuring from " << m_menuJSON << " with " << hltMenu.size() << " chains");

  for (TrigConf::HLTChain chain : hltMenu) {
    std::vector<TrigConf::DataStructure> streams = chain.streams();
    if (streams.empty()) {
      ATH_MSG_ERROR("Chain " << chain.name() << " has no streams assigned");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Chain " << chain.name() << " is assigned to " << streams.size() << " streams");
    m_mapping[ HLT::Identifier( chain.name() ) ] = {};
    for (const TrigConf::DataStructure& stream : streams) {
      try {
        std::string stream_name         = stream.getAttribute("name");
        std::string stream_type         = stream.getAttribute("type");
        std::string_view obeyLB         = stream.getAttribute("obeyLB");
        std::string_view fullEventBuild = stream.getAttribute("forceFullEventBuilding");
        StreamTagInfo streamTag = {
          stream_name,
          stream_type,
          obeyLB == "true",
          fullEventBuild == "true"
        };
        m_mapping[ HLT::Identifier(chain.name()).numeric() ].push_back(streamTag);
        ATH_MSG_DEBUG("-- " << stream_type << "_" << stream_name
                      << " (obeyLB=" << obeyLB << ", forceFullEventBuilding=" << fullEventBuild << ")");
      } catch (const std::exception& ex) {
        ATH_MSG_ERROR("Failure reading stream tag configuration from JSON: " << ex.what());
        return StatusCode::FAILURE;
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode StreamTagMakerTool::finalize() {
  return StatusCode::SUCCESS;
}

// =============================================================================

StatusCode StreamTagMakerTool::fill( HLT::HLTResultMT& resultToFill ) const {
  // obtain chain decisions,
  using namespace TrigCompositeUtils;
  auto chainsHandle = SG::makeHandle( m_finalChainDecisions );
  if (!chainsHandle.isValid()) {
    ATH_MSG_ERROR("Unable to read in the HLTNav_Summary from the DecisionSummaryMakerAlg");
    return StatusCode::FAILURE;
  }

  const Decision* passRawChains = nullptr;
  const Decision* rerunChains = nullptr;
  for (const Decision* d : *chainsHandle) {
    if (d->name() == "HLTPassRaw") {
      passRawChains = d;
    } else if (d->name() == "HLTRerun") {
      rerunChains = d;
    }
    if (passRawChains && rerunChains) {
      break;
    }
  }

  if (passRawChains == nullptr || rerunChains == nullptr) {
    ATH_MSG_ERROR("Unable to read in the HLTNav_Summary from the DecisionSummaryMakerAlg");
    return StatusCode::FAILURE;
  }

  DecisionIDContainer passRawIDs;
  decisionIDs(passRawChains, passRawIDs);
  DecisionIDContainer rerunIDs;
  decisionIDs(rerunChains, rerunIDs);

  std::unordered_map<unsigned int, PEBInfoWriterToolBase::PEBInfo> chainToPEBInfo;
  ATH_CHECK(fillPEBInfoMap(chainToPEBInfo));

  // for each accepted chain lookup the map of chainID -> ST
  for ( DecisionID chain: passRawIDs ) {

    // Note: The default is to NOT allow rerun chains to add a stream tag
    if (!m_allowRerunChains) {
      if ( rerunIDs.find(chain) != rerunIDs.end() ) {
        // This chain has entries in both the passedRaw and rerun sets. As we are not allowing rerun chains, we skip this one.
        continue;
      }
    }

    auto mappingIter = m_mapping.find( chain );
    const std::vector<StreamTagInfo>& streams = mappingIter->second;
    for (const StreamTagInfo& streamTagInfo : streams) {
      auto [st_name, st_type, obeysLB, forceFullEvent] = streamTagInfo;
      std::set<uint32_t> robs;
      std::set<eformat::SubDetector> subdets;
      if (!forceFullEvent) {
        auto it = chainToPEBInfo.find(chain);
        if (it != chainToPEBInfo.end()) {
          ATH_MSG_DEBUG("Chain " << HLT::Identifier( chain ) << " adds PEBInfo " << it->second << " to stream " << st_type << "_" << st_name);
          // Copy ROB IDs directly
          robs = it->second.robs;
          // Convert SubDet IDs from uint32_t to eformat::SubDetector aka uint8_t
          for (const uint32_t subdetid : it->second.subdets) {
            subdets.insert(static_cast<eformat::SubDetector>( subdetid & 0xFF ));
          }
        }
      }

      eformat::helper::StreamTag streamTag(st_name, st_type, obeysLB, robs, subdets);
      resultToFill.addStreamTag(streamTag);
    }
  }

  ATH_MSG_DEBUG("Number of streams for event " <<  resultToFill.getStreamTags().size() );
  return StatusCode::SUCCESS;
}

// =============================================================================

StatusCode StreamTagMakerTool::fillPEBInfoMap(std::unordered_map<DecisionID, PEBInfoWriterToolBase::PEBInfo>& map) const {
  for (const auto& key: m_pebDecisionKeys) {
    // Retrieve the decision container
    auto handle = SG::makeHandle(key); //TODO: pass EventContext& explicitly here
    if (not handle.isValid())  {
      ATH_MSG_DEBUG("Missing input " <<  key.key() << " likely rejected");
      continue;
    }
    ATH_MSG_DEBUG("Processing decision container " << key.key());
    // Loop over decisions
    for (const Decision* d : *handle) {
      ATH_MSG_DEBUG("Processing decision " << *d);
      std::vector<uint32_t> robs;
      std::vector<uint32_t> subdets;
      if (d->getDetail(PEBInfoWriterToolBase::robListKey(), robs)) {
        ATH_MSG_DEBUG("Retrieved a list of " << robs.size() << " ROBs for this decision");
      }
      else {
        ATH_MSG_DEBUG("Failed to retrieve " << PEBInfoWriterToolBase::robListKey() << " for this decision");
      }
      if (d->getDetail(PEBInfoWriterToolBase::subDetListKey(), subdets)) {
        ATH_MSG_DEBUG("Retrieved a list of " << subdets.size() << " SubDets for this decision");
      }
      else {
        ATH_MSG_DEBUG("Failed to retrieve " << PEBInfoWriterToolBase::subDetListKey() << " for this decision");
      }
      DecisionIDContainer ids;
      decisionIDs(d,ids);
      /// Assign PEBInfo to all passed chains for this decision
      for (unsigned int id : ids) {
        ATH_MSG_DEBUG("Mapping PEBInfo to passed chain " << HLT::Identifier(id).name());
        PEBInfoWriterToolBase::PEBInfo& pebi = map[id];
        pebi.robs.insert(robs.begin(),robs.end());
        pebi.subdets.insert(subdets.begin(),subdets.end());
      }
    } // Loop over decisions
  } // Loop over decision containers
  return StatusCode::SUCCESS;
}
