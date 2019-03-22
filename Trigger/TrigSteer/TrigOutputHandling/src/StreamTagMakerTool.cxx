/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "StreamTagMakerTool.h"
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
  // Transform string->vector<string> map from the ChainToStream property into id->StreamTagInfo map
  for (const auto& [chainName, streamTagInfoAsStringVec] : m_chainToStreamProperty) {
    if (streamTagInfoAsStringVec.size()!=4) {
      ATH_MSG_ERROR("Invalid StreamTagInfo object, expected 4 elements {name,type,obeysLumiBlock,forceFullEventBuilding}, but received"
                    << streamTagInfoAsStringVec.size() << " elements for chain " << chainName);
      return StatusCode::FAILURE;
    }
    StreamTagInfo streamTagInfo = {
      streamTagInfoAsStringVec[0],
      streamTagInfoAsStringVec[1],
      (streamTagInfoAsStringVec[2] == "True" || streamTagInfoAsStringVec[2] == "true"),
      (streamTagInfoAsStringVec[3] == "True" || streamTagInfoAsStringVec[3] == "true")
    };
    ATH_MSG_DEBUG( "Chain " << chainName << " accepts events to stream " << std::get<1>(streamTagInfo) << "_" << std::get<0>(streamTagInfo) );
    m_mapping[ HLT::Identifier( chainName ) ] = streamTagInfo;
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
    ATH_MSG_ERROR("Unable to read in the HLTSummary from the DecisionSummaryMakerAlg");
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
  }

  if (passRawChains == nullptr || rerunChains == nullptr) {
    ATH_MSG_ERROR("Unable to read in the HLTSummary from the DecisionSummaryMakerAlg");
    return StatusCode::FAILURE;
  }

  std::unordered_map<unsigned int, PEBInfoWriterToolBase::PEBInfo> chainToPEBInfo;
  ATH_CHECK(fillPEBInfoMap(chainToPEBInfo));

  // for each accepted chain lookup the map of chainID -> ST
  for ( DecisionID chain: decisionIDs( passRawChains ) ) {

    // Note: The default is to NOT allow rerun chains to add a stream tag
    if (!m_allowRerunChains) {
      const auto iterator = std::find(decisionIDs(rerunChains).begin(), decisionIDs(rerunChains).end(), chain);
      if ( iterator != decisionIDs(rerunChains).end() ) {
        // This chain has entries in both the passedRaw and rerun sets. As we are not allowing rerun chains, we skip this one.
        continue;
      }
    }

    auto mappingIter = m_mapping.find( chain );
    // each chain has to have stream
    if( mappingIter == m_mapping.end() ) {
      ATH_MSG_ERROR("Each chain has to have stream associated whereas the " << HLT::Identifier( chain ) << " does not" );
      return StatusCode::FAILURE;
    }

    auto [st_name, st_type, obeysLB, forceFullEvent] = mappingIter->second;
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
    resultToFill.addStreamTag(streamTag); // TODO assume multiple streams per chain?
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