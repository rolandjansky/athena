/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTTL1Overlay.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

namespace LVL1
{

TileTTL1Overlay::TileTTL1Overlay(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { }

StatusCode TileTTL1Overlay::initialize()
{
  ATH_MSG_DEBUG("Initialising");

  // StoreGate keys for Tile
  ATH_CHECK( m_bkgTileTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgTileTTL1Key);
  ATH_CHECK( m_bkgTileMBTSTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgTileMBTSTTL1Key);
  ATH_CHECK( m_signalTileTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalTileTTL1Key);
  ATH_CHECK( m_signalTileMBTSTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalTileMBTSTTL1Key);
  ATH_CHECK( m_outputTileTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_outputTileTTL1Key);
  ATH_CHECK( m_outputTileMBTSTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_outputTileMBTSTTL1Key);

  return StatusCode::SUCCESS;
}

/// Main algorithm execute
StatusCode TileTTL1Overlay::execute(const EventContext& ctx) const
{
  ATH_CHECK( overlay(ctx, m_bkgTileTTL1Key, m_signalTileTTL1Key, m_outputTileTTL1Key, "") );
  ATH_CHECK( overlay(ctx, m_bkgTileMBTSTTL1Key, m_signalTileMBTSTTL1Key, m_outputTileMBTSTTL1Key, "MBTS") );

  return StatusCode::SUCCESS;
}

/// Real Tile overlay towers and overlay on existing event
void TileTTL1Overlay::groupTowers(const TileTTL1Container *towers,
                                  std::map<Identifier, std::vector<const TileTTL1*>> &towerMap) const
{
  // Step over all towers
  for (const TileTTL1 *tower : *towers) {

    // Obtain identifier
    Identifier id = tower->TTL1_ID();

    // Is this one already in the map
    std::map<Identifier, std::vector<const TileTTL1*>>::iterator test = towerMap.find( id );
    // If already exists, add tower to location
    if (test != towerMap.end()) {
      // Add this pointer to the vector
      test->second.push_back(tower);
    }
    // Otherwise create new entry in the map
    else {
      std::vector<const TileTTL1*> towers;
      towers.push_back(tower);
      towerMap.emplace(id, towers);
    }

  } // end for loop

  return;
}

/// Main algorithm execute
StatusCode TileTTL1Overlay::overlay(const EventContext& ctx,
                                    const SG::ReadHandleKey<TileTTL1Container> &bkgKey,
                                    const SG::ReadHandleKey<TileTTL1Container> &signalKey,
                                    const SG::WriteHandleKey<TileTTL1Container> &outputKey,
                                    const std::string &label) const
{
  // setup map
  std::map<Identifier, std::vector<const TileTTL1*>> towerMap;

  // Find Tile towers in TES
  SG::ReadHandle<TileTTL1Container> bkgTowers(bkgKey, ctx);
  if (!bkgTowers.isValid()) {
    ATH_MSG_ERROR("Could not get background " << label << " TileTTL1Container container " << bkgTowers.name() << " from store " << bkgTowers.store());
    return StatusCode::FAILURE;
  }

  /// If we are doing MC-MC overlay there should be a second set of TTL1
  /// So here we retrieve those, match them up and sum their amplitudes
  SG::ReadHandle<TileTTL1Container> signalTowers(signalKey, ctx);
  if (!signalTowers.isValid()) {
    ATH_MSG_ERROR("Could not get signal " << label << " TileTTL1Container container " << signalTowers.name() << " from store " << signalTowers.store());
    return StatusCode::FAILURE;
  }

  // Group towers by ID
  groupTowers(bkgTowers.cptr(), towerMap);
  groupTowers(signalTowers.cptr(), towerMap);

  SG::WriteHandle<TileTTL1Container> outputContainer(outputKey, ctx);
  // Register the TTL1 container in the TES
  ATH_CHECK( outputContainer.record(std::make_unique<TileTTL1Container>()) );
  ATH_MSG_DEBUG( "Output " << label << " TileTTL1Container registered successfully (" << outputKey.key() << ")" );

  // Then the process Tile TTL1 collection
  for (const auto &[dummy, towers] : towerMap) {
    Identifier Id = towers.front()->TTL1_ID();

    std::vector<float> mergedAmps;
    for (const TileTTL1 *tower : towers) {
      if (mergedAmps.empty()) {
        mergedAmps = tower->fsamples();
      } else {
        std::vector<float> amps = tower->fsamples();
        if (amps.size() != mergedAmps.size()) {
          ATH_MSG_WARNING("Tile " << label << " vectors have different lengths: " << amps.size() << ", " << mergedAmps.size());
        } else {
          for (size_t i = 0; i < amps.size(); ++i) {
            mergedAmps[i] += amps[i];
          }
        }
      }
    }

    // Create new tower and add to output vector
    std::unique_ptr<TileTTL1> ttL1 = std::make_unique<TileTTL1>(Id, mergedAmps);
    outputContainer->push_back(ttL1.release());
  }

  return StatusCode::SUCCESS;
}

} // namespace LVL1
