/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



// ================================================
// OverlayTTL1 class Implementation
// ================================================

#include "TrigT1CaloSim/OverlayTTL1.h"

// trigger include(s)

// calorimeter include(s)
//#include "CaloIdentifier/CaloLVL1_ID.h"
//#include "TileConditions/TileInfo.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

// L1Calo includes
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <limits> // for std::numeric_limits<std::streamsize>
#include <utility> // for std::move
#include <stdexcept>
#include <sys/types.h>

namespace LVL1 {

namespace {

}

OverlayTTL1::OverlayTTL1(const std::string& name, ISvcLocator* pSvcLocator)
  : OverlayAlgBase(name, pSvcLocator),
    m_mappingTool("LVL1::PpmMappingTool/PpmMappingTool")

{
  declareProperty("PpmMappingTool", m_mappingTool);

  declareProperty("EmTTL1MainKey", m_EmTTL1MainKey);
  declareProperty("HadTTL1MainKey", m_HadTTL1MainKey);
  declareProperty("TileTTL1MainKey", m_TileTTL1MainKey);
  declareProperty("TileMBTSTTL1MainKey", m_TileMBTSTTL1MainKey);

  declareProperty("EmTTL1OverlayKey", m_EmTTL1OverlayKey);
  declareProperty("HadTTL1OverlayKey", m_HadTTL1OverlayKey);
  declareProperty("TileTTL1OverlayKey", m_TileTTL1OverlayKey);
  declareProperty("TileMBTSTTL1OverlayKey", m_TileMBTSTTL1OverlayKey);

  declareProperty("EmTTL1OutputKey", m_EmTTL1OutputKey);
  declareProperty("HadTTL1OutputKey", m_HadTTL1OutputKey);
  declareProperty("TileTTL1OutputKey", m_TileTTL1OutputKey);
  declareProperty("TileMBTSTTL1OutputKey", m_TileMBTSTTL1OutputKey);
}

OverlayTTL1::~OverlayTTL1() {}

StatusCode OverlayTTL1::overlayInitialize()
{
  ATH_MSG_INFO("Initialising");

  CHECK(m_mappingTool.retrieve());

  // StoreGate keys
  ATH_CHECK( m_EmTTL1MainKey.initialize() );
  ATH_CHECK( m_HadTTL1MainKey.initialize() );
  ATH_CHECK( m_EmTTL1OverlayKey.initialize() );
  ATH_CHECK( m_HadTTL1OverlayKey.initialize() );
  ATH_CHECK( m_EmTTL1OutputKey.initialize() );
  ATH_CHECK( m_HadTTL1OutputKey.initialize() );

  ATH_CHECK( m_TileTTL1MainKey.initialize() );
  ATH_CHECK( m_TileMBTSTTL1MainKey.initialize() );
  ATH_CHECK( m_TileTTL1OverlayKey.initialize() );
  ATH_CHECK( m_TileMBTSTTL1OverlayKey.initialize() );
  ATH_CHECK( m_TileTTL1OutputKey.initialize() );
  ATH_CHECK( m_TileMBTSTTL1OutputKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode OverlayTTL1::overlayFinalize()
{
  ATH_MSG_INFO("Finalizing");
  return StatusCode::SUCCESS;
}

/** Checks that the Cell Type is supported (terminates with errors if not)
    and calls relevant routine to look for the cells.
*/
StatusCode OverlayTTL1::overlayExecute()
{
  ATH_MSG_VERBOSE("Executing");

  // Retrieve towers
  CHECK(getTowers());

  // Merge towers
  CHECK(mergeTowers());

  return StatusCode::SUCCESS;
}


/** fetches LAr & Tile calorimeter towers */
StatusCode LVL1::OverlayTTL1::getTowers()
{
  // clear maps
  m_emTowerMap.clear();
  m_hecTowerMap.clear();
  m_tileTowerMap.clear();
  m_tileMBTSTowerMap.clear();

  // Find LAr towers in TES
  SG::ReadHandle<LArTTL1Container> EMTowers(m_EmTTL1MainKey);
  if(!EMTowers.isValid()){
    ATH_MSG_WARNING("EM LArTTL1Container not found");
  }
  else groupLArTowers(EMTowers, m_emTowerMap);

  SG::ReadHandle<LArTTL1Container> HECTowers(m_HadTTL1MainKey);
  if(!HECTowers.isValid()){
    ATH_MSG_WARNING("Had LArTTL1Container not found");
  }
  else groupLArTowers(HECTowers, m_hecTowerMap);

  /// If we are doing MC-MC overlay there should be a second set of TTL1
  /// So here we retrieve those, match them up and sum their amplitudes
  SG::ReadHandle<LArTTL1Container> EMOverlay(m_EmTTL1OverlayKey);
  if(!EMOverlay.isValid()){
    ATH_MSG_WARNING("EM Overlay LArTTL1Container not found");
  }
  else groupLArTowers(EMOverlay, m_emTowerMap);

  SG::ReadHandle<LArTTL1Container> HadOverlay(m_HadTTL1OverlayKey);
  if(!HadOverlay.isValid()){
    ATH_MSG_WARNING("Had LArTTL1 Overlay Container not found");
  } 
  else groupLArTowers(HadOverlay, m_hecTowerMap);

  // Find Tile towers in TES
  SG::ReadHandle<TileTTL1Container> TileOverlay(m_TileTTL1OverlayKey);
  if(!TileOverlay.isValid()){
  ATH_MSG_WARNING("TileTTL1 Overlay Container not found");
  } 
  else groupTileTowers(TileOverlay, m_tileTowerMap);

  SG::ReadHandle<TileTTL1Container> TileMBTSOverlay(m_TileMBTSTTL1OverlayKey);
  if(!TileMBTSOverlay.isValid()){
  ATH_MSG_WARNING("MBTS TileTTL1 Overlay Container not found");
  } 
  else groupTileTowers(TileMBTSOverlay, m_tileMBTSTowerMap);

  /// If we are doing MC-MC overlay there should be a second set of TTL1
  /// So here we retrieve those, match them up and sum their amplitudes
  SG::ReadHandle<TileTTL1Container> TileTowers(m_TileTTL1MainKey);
  if(!TileTowers.isValid()){
  ATH_MSG_WARNING("Tile TTL1Container not found");
  }
  else groupTileTowers(TileTowers, m_tileTowerMap);

  SG::ReadHandle<TileTTL1Container> TileMBTSTowers(m_TileMBTSTTL1MainKey);
  if(!TileMBTSTowers.isValid()){
  ATH_MSG_WARNING("MBTS Tile TTL1Container not found");
  }
  else groupTileTowers(TileMBTSTowers, m_tileMBTSTowerMap);

  return StatusCode::SUCCESS;
}

/** steps over LAr tower collection and overlays on existing collection  */
void LVL1::OverlayTTL1::groupLArTowers(SG::ReadHandle<LArTTL1Container>& towers, std::map<std::string, std::vector<LArTTL1*>> &towerMap)
{
  for(const auto& tower : *towers){

    // Obtain identifier
    Identifier id = tower->ttOfflineID();
    std::string idString = id.getString();

    // Does this tower already exist?
    // Get channel ID = key to map
    //uint32_t channel = coolId.id();
    std::map<std::string, std::vector<LArTTL1*>>::iterator test = towerMap.find( idString );
    // If already exists, add tower to location 
    if (test != towerMap.end()) {
      // Add this pointer to the vector
      test->second.push_back(tower);
    }
    // Otherwise create new entry in the map
    else {
      std::vector<LArTTL1*> towers;
      towers.push_back(tower);
      towerMap.insert(std::pair<std::string, std::vector<LArTTL1*>>(idString,towers));
    }

  } // end for loop
}


/// Real Tile overlay towers and overlay on existing event
void LVL1::OverlayTTL1::groupTileTowers(SG::ReadHandle<TileTTL1Container>& towers, std::map<std::string, std::vector<TileTTL1*>> &towerMap)
{
  // Step over all towers
  for(const auto& tower : *towers) {

    // Obtain identifier
    Identifier id = tower->TTL1_ID();
    std::string idString = id.getString();

    // Is this one already in the map
    std::map<std::string, std::vector<TileTTL1*>>::iterator test = towerMap.find( idString );
    // If already exists, add tower to location 
    if (test != towerMap.end()) {
      // Add this pointer to the vector
      test->second.push_back(tower);
    }
    // Otherwise create new entry in the map
    else {
      std::vector<TileTTL1*> towers;
      towers.push_back(tower);
      towerMap.insert(std::pair<std::string, std::vector<TileTTL1*>>(idString,towers));
    }

  } // end for loop

  return;
}


/** Combine towers  */
StatusCode LVL1::OverlayTTL1::mergeTowers()
{

  // Set up output containers
  
  SG::WriteHandle<LArTTL1Container> outputEmTTL1Container(m_EmTTL1OutputKey);
  // Register the TTL1 container in the TES
  ATH_CHECK( outputEmTTL1Container.record(std::make_unique<LArTTL1Container>()) );
  ATH_MSG_DEBUG( "Output EM LArTTL1Container registered successfully (" << m_EmTTL1OutputKey.key() << ")" );
  
  SG::WriteHandle<LArTTL1Container> outputHadTTL1Container(m_HadTTL1OutputKey);
  // Register the TTL1 container in the TES
  ATH_CHECK( outputHadTTL1Container.record(std::make_unique<LArTTL1Container>()) );
  ATH_MSG_DEBUG( "Output Hadronic LArTTL1Container registered successfully (" << m_HadTTL1OutputKey.key() << ")" );
  
  SG::WriteHandle<TileTTL1Container> outputTileTTL1Container(m_TileTTL1OutputKey);
  // Register the TTL1 container in the TES
  ATH_CHECK( outputTileTTL1Container.record(std::make_unique<TileTTL1Container>()) );
  ATH_MSG_DEBUG( "Output TileTTL1Container registered successfully (" << m_TileTTL1OutputKey.key() << ")" );
  
  SG::WriteHandle<TileTTL1Container> outputTileMBTSTTL1Container(m_TileMBTSTTL1OutputKey);
  // Register the TTL1 container in the TES
  ATH_CHECK( outputTileMBTSTTL1Container.record(std::make_unique<TileTTL1Container>()) );
  ATH_MSG_DEBUG( "Output MBTS TileTTL1Container registered successfully (" << m_TileMBTSTTL1OutputKey.key() << ")" );


  // Start with EM collection
  for (std::map<std::string, std::vector<LArTTL1*>>::iterator emIt = m_emTowerMap.begin(); emIt != m_emTowerMap.end(); ++emIt) {
     std::vector<LArTTL1*>::iterator it = (emIt->second).begin();
     HWIdentifier hwId = (*it)->ttOnlineID();
     Identifier Id = (*it)->ttOfflineID();

     std::vector<float> mergedAmps;
     for (; it != (emIt->second).end(); ++it) {
        if (mergedAmps.size() == 0) mergedAmps = (*it)->samples();
        else {
          std::vector<float> amps = (*it)->samples();
          if (amps.size() != mergedAmps.size()) ATH_MSG_WARNING("LAr EM vectors have different lengths: " << amps.size() << ", " << mergedAmps.size()); 
          else for (unsigned int i = 0; i < amps.size(); ++i) mergedAmps[i] += amps[i];     
        }
     }

     // Create new tower and add to output vector
     //LArTTL1 * ttL1 = new LArTTL1(hwId,Id,mergedAmps);
     //outputEmTTL1Container->push_back(ttL1);
     std::unique_ptr<LArTTL1> ttL1 = std::make_unique<LArTTL1>(hwId,Id,mergedAmps);
     outputEmTTL1Container->push_back(ttL1.release());
  }


  // Then the Had TTL1 collection
  for (std::map<std::string, std::vector<LArTTL1*>>::iterator hadIt = m_hecTowerMap.begin(); hadIt != m_hecTowerMap.end(); ++hadIt) {
     std::vector<LArTTL1*>::iterator it = (hadIt->second).begin();
     HWIdentifier hwId = (*it)->ttOnlineID();
     Identifier Id = (*it)->ttOfflineID();

     std::vector<float> mergedAmps;
     for (; it != (hadIt->second).end(); ++it) {
        if (mergedAmps.size() == 0) mergedAmps = (*it)->samples();
        else {
          std::vector<float> amps = (*it)->samples();
          if (amps.size() != mergedAmps.size()) ATH_MSG_WARNING("LAr Had vectors have different lengths: " << amps.size() << ", " << mergedAmps.size()); 
          else for (unsigned int i = 0; i < amps.size(); ++i) mergedAmps[i] += amps[i];     
        }
     }

     // Create new tower and add to output vector
     //LArTTL1 * ttL1 = new LArTTL1(hwId,Id,mergedAmps);
     //outputHadTTL1Container->push_back(ttL1);
     std::unique_ptr<LArTTL1> ttL1 = std::make_unique<LArTTL1>(hwId,Id,mergedAmps);
     outputHadTTL1Container->push_back(ttL1.release());
  }

  // Then the process Tile TTL1 collection
  for (std::map<std::string, std::vector<TileTTL1*>>::iterator tileIt = m_tileTowerMap.begin(); tileIt != m_tileTowerMap.end(); ++tileIt) {
    std::vector<TileTTL1*>::iterator it = (tileIt->second).begin();
    Identifier Id = (*it)->TTL1_ID();

    std::vector<float> mergedAmps;
    for (; it != (tileIt->second).end(); ++it) {
      if (mergedAmps.size() == 0) mergedAmps = (*it)->fsamples();
      else {
        std::vector<float> amps = (*it)->fsamples();
        if (amps.size() != mergedAmps.size()) ATH_MSG_WARNING("Tile vectors have different lengths: " << amps.size() << ", " << mergedAmps.size()); 
        else for (unsigned int i = 0; i < amps.size(); ++i) mergedAmps[i] += amps[i];     
      }
    }

    // Create new tower and add to output vector
    //TileTTL1 * ttL1 = new TileTTL1(Id,mergedAmps);
    //outputTileTTL1Container->push_back(ttL1;
    std::unique_ptr<TileTTL1> ttL1 = std::make_unique<TileTTL1>(Id,mergedAmps);
    outputTileTTL1Container->push_back(ttL1.release());
  }
  
  // ... and MBTS Tile TTL1 collection
  for (std::map<std::string, std::vector<TileTTL1*>>::iterator tileIt = m_tileMBTSTowerMap.begin(); tileIt != m_tileMBTSTowerMap.end(); ++tileIt) {
    std::vector<TileTTL1*>::iterator it = (tileIt->second).begin();
    Identifier Id = (*it)->TTL1_ID();

    std::vector<float> mergedAmps;
    for (; it != (tileIt->second).end(); ++it) {
      if (mergedAmps.size() == 0) mergedAmps = (*it)->fsamples();
      else {
        std::vector<float> amps = (*it)->fsamples();
        if (amps.size() != mergedAmps.size()) ATH_MSG_WARNING("MBTS Tile vectors have different lengths: " << amps.size() << ", " << mergedAmps.size()); 
        else for (unsigned int i = 0; i < amps.size(); ++i) mergedAmps[i] += amps[i];     
      }
    }

    // Create new tower and add to output vector
    //TileTTL1 * ttL1 = new TileTTL1(Id,mergedAmps);
    //outputTileTTL1Container->push_back(ttL1;
    std::unique_ptr<TileTTL1> ttL1 = std::make_unique<TileTTL1>(Id,mergedAmps);
    outputTileMBTSTTL1Container->push_back(ttL1.release());
  }

  return StatusCode::SUCCESS;

}

} // close namespace bracket
