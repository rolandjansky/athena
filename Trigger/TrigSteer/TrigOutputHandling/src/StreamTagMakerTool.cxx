/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/HLTIdentifier.h"
#include "StreamTagMakerTool.h"

StreamTagMakerTool::StreamTagMakerTool( const std::string& type, const std::string& name, const IInterface* parent ):
  base_class( type, name, parent ) {}

StreamTagMakerTool::~StreamTagMakerTool() {}

StatusCode StreamTagMakerTool::initialize() {
  ATH_CHECK( m_finalChainDecisions.initialize() );
  // decode mapping - temporary solution for testing
  for ( auto& chainAndStream: m_chainToStreamProperty ) {
    struct { std::string chain, stream; } conf { chainAndStream.first, chainAndStream.second };
    ATH_MSG_DEBUG( "Chain " << conf.chain << " accepts events to stream " << conf.stream );
    // find subdets
    std::set<eformat::SubDetector> dets;
    const auto itSubDetMap = m_streamSubDets.value().find(conf.stream);
    if (itSubDetMap != m_streamSubDets.value().cend()) {
      for (const uint32_t detid : itSubDetMap->second)
        dets.insert(static_cast<eformat::SubDetector>(detid & 0xFF)); // cast from uint32_t
    }
    // find robs
    std::set<uint32_t> robs;
    const auto itRobsMap = m_streamRobs.value().find(conf.stream);
    if (itRobsMap != m_streamRobs.value().cend()) {
      for (const uint32_t robid : itRobsMap->second)
        robs.insert(robid);
    }
    // create the stream tag
    m_mapping[ HLT::Identifier( conf.chain ) ] = eformat::helper::StreamTag( conf.stream, "physics", true, robs, dets );
  }

  return StatusCode::SUCCESS;
}

StatusCode StreamTagMakerTool::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode StreamTagMakerTool::fill( HLT::HLTResultMT& resultToFill ) const {
  // obtain chain decisions,
  using namespace TrigCompositeUtils;
  auto chainsHandle = SG::makeHandle( m_finalChainDecisions );

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
    resultToFill.addStreamTag( mappingIter->second ); // TODO assume multiple streams per chain
  }

  ATH_MSG_DEBUG("Number of streams for event " <<  resultToFill.getStreamTags().size() );
  return StatusCode::SUCCESS;
}
