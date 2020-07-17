/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// OverlayTTL1 class Implementation
// ================================================

#include "OverlayTTL1.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

namespace LVL1 {

OverlayTTL1::OverlayTTL1(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { }

StatusCode OverlayTTL1::initialize()
{
  ATH_MSG_DEBUG("Initialising");

  // StoreGate keys for LAr
  ATH_CHECK( m_bkgEmTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgEmTTL1Key);
  ATH_CHECK( m_bkgHadTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgHadTTL1Key);
  ATH_CHECK( m_signalEmTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalEmTTL1Key);
  ATH_CHECK( m_signalHadTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalHadTTL1Key);
  ATH_CHECK( m_outputEmTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_outputEmTTL1Key);
  ATH_CHECK( m_outputHadTTL1Key.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_outputHadTTL1Key);

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
StatusCode OverlayTTL1::execute(const EventContext& ctx) const
{
  ATH_CHECK( overlayLArTTL1(ctx, m_bkgEmTTL1Key, m_signalEmTTL1Key, m_outputEmTTL1Key, "EM") );
  ATH_CHECK( overlayLArTTL1(ctx, m_bkgHadTTL1Key, m_signalHadTTL1Key, m_outputHadTTL1Key, "Hadronic") );

  ATH_CHECK( overlayTileTTL1(ctx, m_bkgTileTTL1Key, m_signalTileTTL1Key, m_outputTileTTL1Key, "") );
  ATH_CHECK( overlayTileTTL1(ctx, m_bkgTileMBTSTTL1Key, m_signalTileMBTSTTL1Key, m_outputTileMBTSTTL1Key, "MBTS") );

  return StatusCode::SUCCESS;
}

/// Steps over LAr tower collection and overlays on existing collection
void OverlayTTL1::groupLArTowers(const LArTTL1Container *towers,
                                 std::map<Identifier, std::vector<const LArTTL1*>> &towerMap) const
{
  for (const LArTTL1 *tower : *towers) {

    // Obtain identifier
    Identifier id = tower->ttOfflineID();

    // Does this tower already exist?
    std::map<Identifier, std::vector<const LArTTL1*>>::iterator test = towerMap.find( id );
    // If already exists, add tower to location
    if (test != towerMap.end()) {
      // Add this pointer to the vector
      test->second.push_back(tower);
    }
    // Otherwise create new entry in the map
    else {
      std::vector<const LArTTL1*> towers;
      towers.push_back(tower);
      towerMap.emplace(id, towers);
    }

  } // end for loop
}


/// Real Tile overlay towers and overlay on existing event
void OverlayTTL1::groupTileTowers(const TileTTL1Container *towers,
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
StatusCode OverlayTTL1::overlayLArTTL1(const EventContext& ctx,
                                       const SG::ReadHandleKey<LArTTL1Container> &bkgKey,
                                       const SG::ReadHandleKey<LArTTL1Container> &signalKey,
                                       const SG::WriteHandleKey<LArTTL1Container> &outputKey,
                                       const std::string &label) const
{
  // setup maps
  std::map<Identifier, std::vector<const LArTTL1*>> towerMap;

  // Find LAr towers in TES
  SG::ReadHandle<LArTTL1Container> bkgTowers(bkgKey, ctx);
  if (!bkgTowers.isValid()) {
    ATH_MSG_ERROR("Could not get background " << label << " LArTTL1Container container " << bkgTowers.name() << " from store " << bkgTowers.store());
    return StatusCode::FAILURE;
  }

  /// If we are doing MC-MC overlay there should be a second set of TTL1
  /// So here we retrieve those, match them up and sum their amplitudes
  SG::ReadHandle<LArTTL1Container> signalTowers(signalKey, ctx);
  if (!signalTowers.isValid()) {
    ATH_MSG_ERROR("Could not get signal " << label << " LArTTL1Container container " << signalTowers.name() << " from store " << signalTowers.store());
    return StatusCode::FAILURE;
  }

  // Group towers by ID
  groupLArTowers(bkgTowers.cptr(), towerMap);
  groupLArTowers(signalTowers.cptr(), towerMap);

  // Set up output containers
  SG::WriteHandle<LArTTL1Container> outputContainer(outputKey, ctx);
  // Register the TTL1 container in the TES
  ATH_CHECK( outputContainer.record(std::make_unique<LArTTL1Container>()) );
  ATH_MSG_DEBUG( "Output " << label << " LArTTL1Container registered successfully (" << outputKey.key() << ")" );

  // Make the output collection
  for (const auto &[dummy, towers] : towerMap) {
     const LArTTL1 *firstTower = towers.front();
     HWIdentifier hwId = firstTower->ttOnlineID();
     Identifier Id = firstTower->ttOfflineID();

     std::vector<float> mergedAmps;
     for (const LArTTL1 *tower : towers) {
        if (mergedAmps.empty()) {
          mergedAmps = tower->samples();
        } else {
          std::vector<float> amps = tower->samples();
          if (amps.size() != mergedAmps.size()) {
            ATH_MSG_WARNING("LAr " << label << " vectors have different lengths: " << amps.size() << ", " << mergedAmps.size());
          } else {
            for (size_t i = 0; i < amps.size(); ++i) {
              mergedAmps[i] += amps[i];
            }
          }
        }
     }

     // Create new tower and add to output vector
     std::unique_ptr<LArTTL1> ttL1 = std::make_unique<LArTTL1>(hwId, Id, mergedAmps);
     outputContainer->push_back(ttL1.release());
  }

  return StatusCode::SUCCESS;
}

StatusCode OverlayTTL1::overlayTileTTL1(const EventContext& ctx,
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
  groupTileTowers(bkgTowers.cptr(), towerMap);
  groupTileTowers(signalTowers.cptr(), towerMap);

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

} // close namespace bracket
