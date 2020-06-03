// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// OverlayTTL1 class description
// ================================================
//
// THIS TEXT TO BE REPLACED BY ATLAS STANDARD FORMAT
//
//
// class: OverlayTTL1
//
// Description:
//
//The OverlayTTL1 class takes calorimeter cells from the TES and
// forms Trigger Towers, which it then places back into the TES  The calorimeter
// cells can be produced either by GEANT or other fast simulation packages
// - this is defined by setting a parameter
// CellType to 1 for CaloCells, 2 to reprocess TriggerTowers and 3 for LAr/Tile TTL1 input (a simulation of analogue towers);
//
// ................................................................
//

#ifndef TRIGT1CALOSIM_OVERLAYTTL1_H
#define TRIGT1CALOSIM_OVERLAYTTL1_H

// STL
#include <array>
#include <map>
#include <string>
#include <utility> // for std::pair, std::make_pair
#include <vector>

// Athena/Gaudi
#include "OverlayAlgBase/OverlayAlgBase.h"
//#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "DataModel/DataVector.h"
#include "StoreGate/ReadHandleKey.h"
#include "Identifier/Identifier.h"


//Calorimeter tower includes
#include "LArRawEvent/LArTTL1Container.h"
#include "TileEvent/TileTTL1Container.h"

namespace LVL1
{

class IL1CaloMappingTool;


class OverlayTTL1 : public OverlayAlgBase
{
public:
  //-------------------------
  // Constructors/Destructors
  //-------------------------
  OverlayTTL1(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~OverlayTTL1();

  // These are disallowed
  OverlayTTL1(const OverlayTTL1&) = delete;
  OverlayTTL1& operator=(const OverlayTTL1&) = delete;

  //------------------------------------------------------
  // Methods used by Athena to run the algorithm
  //------------------------------------------------------
  StatusCode overlayInitialize();
  StatusCode overlayExecute();
  StatusCode overlayFinalize();

private:

  // locations within the TES to store collections of Trigger Towers
  std::string m_outputLocation;
  //SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_outputKey;

  // location of TTL1 data
  SG::ReadHandleKey<LArTTL1Container> m_EmTTL1MainKey;
  SG::ReadHandleKey<LArTTL1Container> m_HadTTL1MainKey;
  SG::ReadHandleKey<TileTTL1Container> m_TileTTL1MainKey;
  SG::ReadHandleKey<TileTTL1Container> m_TileMBTSTTL1MainKey;

  // locations of overlay TTL1 data
  SG::ReadHandleKey<LArTTL1Container> m_EmTTL1OverlayKey;
  SG::ReadHandleKey<LArTTL1Container> m_HadTTL1OverlayKey;
  SG::ReadHandleKey<TileTTL1Container> m_TileTTL1OverlayKey;
  SG::ReadHandleKey<TileTTL1Container> m_TileMBTSTTL1OverlayKey;

  // locations of output TTL1 data
  SG::WriteHandleKey<LArTTL1Container> m_EmTTL1OutputKey;
  SG::WriteHandleKey<LArTTL1Container> m_HadTTL1OutputKey;
  SG::WriteHandleKey<TileTTL1Container> m_TileTTL1OutputKey;
  SG::WriteHandleKey<TileTTL1Container> m_TileMBTSTTL1OutputKey;

  const CaloLVL1_ID* m_caloId; //non-owning ptr

  ToolHandle<IL1CaloMappingTool> m_mappingTool;


  std::vector<std::vector<double>> m_towersAmps; // stores the Amps (vector<double>) for each xaodTower

  std::size_t m_curIndex = 0u;
  std::map<unsigned int, std::size_t> m_towersIndex; 

  std::map<std::string, std::vector<LArTTL1*>> m_emTowerMap; 
  std::map<std::string, std::vector<LArTTL1*>> m_hecTowerMap; 
  std::map<std::string, std::vector<TileTTL1*>> m_tileTowerMap; 
  std::map<std::string, std::vector<TileTTL1*>> m_tileMBTSTowerMap; 

  /** fetch Calorimeter Towers */
  StatusCode getTowers();

  /** overlay amplitudes from other TTL1 */
  void groupLArTowers(SG::ReadHandle<LArTTL1Container>& towers, std::map<std::string, std::vector<LArTTL1*>> &towerMap);
  void groupTileTowers(SG::ReadHandle<TileTTL1Container>& towers, std::map<std::string, std::vector<TileTTL1*>> &towerMap);

  /** Merge tower amplitudes */
  StatusCode mergeTowers();

};

} // namespace LVL1
#endif
