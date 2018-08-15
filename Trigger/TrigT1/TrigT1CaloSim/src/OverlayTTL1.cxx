/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// OverlayTTL1 class Implementation
// ================================================

#include "TrigT1CaloSim/OverlayTTL1.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

namespace LVL1 {

OverlayTTL1::OverlayTTL1(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)

{
  declareProperty("EnableTileTTL1Overlay", m_enableTileTTL1Overlay=false);
}

OverlayTTL1::~OverlayTTL1() {}

StatusCode OverlayTTL1::initialize()
{
  ATH_MSG_DEBUG("Initialising");

  // StoreGate keys
  ATH_CHECK( m_bkgEmTTL1Key.initialize() );
  ATH_CHECK( m_bkgHadTTL1Key.initialize() );
  ATH_CHECK( m_signalEmTTL1Key.initialize() );
  ATH_CHECK( m_signalHadTTL1Key.initialize() );
  ATH_CHECK( m_outputEmTTL1Key.initialize() );
  ATH_CHECK( m_outputHadTTL1Key.initialize() );

  if (m_enableTileTTL1Overlay) {
    ATH_CHECK( m_bkgTileTTL1Key.initialize() );
    ATH_CHECK( m_signalTileTTL1Key.initialize() );
    ATH_CHECK( m_outputTileTTL1Key.initialize() );
  }

  return StatusCode::SUCCESS;
}

/// Main algorithm execute
StatusCode OverlayTTL1::execute()
{
  ATH_CHECK( overlayLArTTL1(m_bkgEmTTL1Key, m_signalEmTTL1Key, m_outputEmTTL1Key, "EM") );
  ATH_CHECK( overlayLArTTL1(m_bkgHadTTL1Key, m_signalHadTTL1Key, m_outputHadTTL1Key, "Hadronic") );

  if (m_enableTileTTL1Overlay) {
    ATH_CHECK( overlayTileTTL1(m_bkgTileTTL1Key, m_signalTileTTL1Key, m_outputTileTTL1Key) );
  }

  return StatusCode::SUCCESS;
}

StatusCode OverlayTTL1::finalize()
{
  ATH_MSG_DEBUG("Finalizing");
  return StatusCode::SUCCESS;
}

/** steps over LAr tower collection and overlays on existing collection  */
void OverlayTTL1::groupLArTowers(SG::ReadHandle<LArTTL1Container>& towers, std::map<Identifier, std::vector<const LArTTL1*>> &towerMap) const
{
  for(const auto& tower : *towers){

    // Obtain identifier
    Identifier id = tower->ttOfflineID();

    // Does this tower already exist?
    // Get channel ID = key to map
    //uint32_t channel = coolId.id();
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
      towerMap.insert(std::pair<Identifier, std::vector<const LArTTL1*>>(id, towers));
    }

  } // end for loop
}


/// Real Tile overlay towers and overlay on existing event
void OverlayTTL1::groupTileTowers(SG::ReadHandle<TileTTL1Container>& towers, std::map<Identifier, std::vector<const TileTTL1*>> &towerMap) const
{
  // Step over all towers
  for(const auto& tower : *towers) {

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
      towerMap.insert(std::pair<Identifier, std::vector<const TileTTL1*>>(id, towers));
    }

  } // end for loop

  return;
}

/// Main algorithm execute
StatusCode OverlayTTL1::overlayLArTTL1(const SG::ReadHandleKey<LArTTL1Container> &bkgKey, const SG::ReadHandleKey<LArTTL1Container> &signalKey, const SG::WriteHandleKey<LArTTL1Container> &outputKey, const std::string &label)
{
  // setup maps
  std::map<Identifier, std::vector<const LArTTL1*>> towerMap;

  // Find LAr towers in TES
  SG::ReadHandle<LArTTL1Container> bkgTowers(bkgKey);
  if (!bkgTowers.isValid()) {
    ATH_MSG_ERROR("Could not get background " << label << " LArTTL1Container container " << bkgTowers.name() << " from store " << bkgTowers.store());
    return StatusCode::FAILURE;
  }

  /// If we are doing MC-MC overlay there should be a second set of TTL1
  /// So here we retrieve those, match them up and sum their amplitudes
  SG::ReadHandle<LArTTL1Container> signalTowers(signalKey);
  if (!signalTowers.isValid()) {
    ATH_MSG_ERROR("Could not get signal " << label << " LArTTL1Container container " << signalTowers.name() << " from store " << signalTowers.store());
    return StatusCode::FAILURE;
  }

  // Group towers by ID
  groupLArTowers(bkgTowers, towerMap);
  groupLArTowers(signalTowers, towerMap);

  // Set up output containers  
  SG::WriteHandle<LArTTL1Container> outputContainer(outputKey);
  // Register the TTL1 container in the TES
  ATH_CHECK( outputContainer.record(std::make_unique<LArTTL1Container>()) );
  ATH_MSG_DEBUG( "Output " << label << " LArTTL1Container registered successfully (" << outputKey.key() << ")" );

  // Make the output collection
  for (std::map<Identifier, std::vector<const LArTTL1*>>::iterator itMap = towerMap.begin(); itMap != towerMap.end(); ++itMap) {
     std::vector<const LArTTL1*>::iterator it = (itMap->second).begin();
     HWIdentifier hwId = (*it)->ttOnlineID();
     Identifier Id = (*it)->ttOfflineID();

     std::vector<float> mergedAmps;
     for (; it != (itMap->second).end(); ++it) {
        if (mergedAmps.size() == 0) mergedAmps = (*it)->samples();
        else {
          std::vector<float> amps = (*it)->samples();
          if (amps.size() != mergedAmps.size()) ATH_MSG_WARNING("LAr " << label << " vectors have different lengths: " << amps.size() << ", " << mergedAmps.size()); 
          else for (unsigned int i = 0; i < amps.size(); ++i) mergedAmps[i] += amps[i];     
        }
     }

     // Create new tower and add to output vector
     std::unique_ptr<LArTTL1> ttL1 = std::make_unique<LArTTL1>(hwId, Id, mergedAmps);
     outputContainer->push_back(ttL1.release());
  }

  return StatusCode::SUCCESS;
}

StatusCode OverlayTTL1::overlayTileTTL1(const SG::ReadHandleKey<TileTTL1Container> &bkgKey, const SG::ReadHandleKey<TileTTL1Container> &signalKey, const SG::WriteHandleKey<TileTTL1Container> &outputKey)
{
  // setup map
  std::map<Identifier, std::vector<const TileTTL1*>> towerMap;

  // Find Tile towers in TES
  SG::ReadHandle<TileTTL1Container> bkgTowers(bkgKey);
  if (!bkgTowers.isValid()) {
    ATH_MSG_ERROR("Could not get background TileTTL1Container container " << bkgTowers.name() << " from store " << bkgTowers.store());
    return StatusCode::FAILURE;
  }

  /// If we are doing MC-MC overlay there should be a second set of TTL1
  /// So here we retrieve those, match them up and sum their amplitudes
  SG::ReadHandle<TileTTL1Container> signalTowers(signalKey);
  if (!signalTowers.isValid()) {
    ATH_MSG_ERROR("Could not get signal TileTTL1Container container " << signalTowers.name() << " from store " << signalTowers.store());
    return StatusCode::FAILURE;
  }

  // Group towers by ID
  groupTileTowers(bkgTowers, towerMap);
  groupTileTowers(signalTowers, towerMap);

  SG::WriteHandle<TileTTL1Container> outputContainer(outputKey);
  // Register the TTL1 container in the TES
  ATH_CHECK( outputContainer.record(std::make_unique<TileTTL1Container>()) );
  ATH_MSG_DEBUG( "Output TileTTL1Container registered successfully (" << outputKey.key() << ")" );

  // Then the process Tile TTL1 collection
  for (std::map<Identifier, std::vector<const TileTTL1*>>::iterator itMap = towerMap.begin(); itMap != towerMap.end(); ++itMap) {
    std::vector<const TileTTL1*>::iterator it = (itMap->second).begin();
    Identifier Id = (*it)->TTL1_ID();

    std::vector<float> mergedAmps;
    for (; it != (itMap->second).end(); ++it) {
      if (mergedAmps.size() == 0) mergedAmps = (*it)->fsamples();
      else {
        std::vector<float> amps = (*it)->fsamples();
        if (amps.size() != mergedAmps.size()) ATH_MSG_WARNING("Tile vectors have different lengths: " << amps.size() << ", " << mergedAmps.size()); 
        else for (unsigned int i = 0; i < amps.size(); ++i) mergedAmps[i] += amps[i];     
      }
    }

    // Create new tower and add to output vector
    std::unique_ptr<TileTTL1> ttL1 = std::make_unique<TileTTL1>(Id, mergedAmps);
    outputContainer->push_back(ttL1.release());
  }

  return StatusCode::SUCCESS;
}

} // close namespace bracket
