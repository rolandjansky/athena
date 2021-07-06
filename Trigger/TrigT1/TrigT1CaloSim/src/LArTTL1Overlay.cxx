/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTTL1Overlay.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

namespace LVL1
{

LArTTL1Overlay::LArTTL1Overlay(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { }

StatusCode LArTTL1Overlay::initialize()
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

  return StatusCode::SUCCESS;
}

/// Main algorithm execute
StatusCode LArTTL1Overlay::execute(const EventContext& ctx) const
{
  ATH_CHECK( overlay(ctx, m_bkgEmTTL1Key, m_signalEmTTL1Key, m_outputEmTTL1Key, "EM") );
  ATH_CHECK( overlay(ctx, m_bkgHadTTL1Key, m_signalHadTTL1Key, m_outputHadTTL1Key, "Hadronic") );

  return StatusCode::SUCCESS;
}

/// Steps over LAr tower collection and overlays on existing collection
void LArTTL1Overlay::groupTowers(const LArTTL1Container *towers,
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

/// Main algorithm execute
StatusCode LArTTL1Overlay::overlay(const EventContext& ctx,
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
  groupTowers(bkgTowers.cptr(), towerMap);
  groupTowers(signalTowers.cptr(), towerMap);

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

} // namespace LVL1
