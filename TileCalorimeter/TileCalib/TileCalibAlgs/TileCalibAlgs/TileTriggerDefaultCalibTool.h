/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBALG_TILETRIGGERDEFAULTCALIBTOOL_H
#define TILECALIBALG_TILETRIGGERDEFAULTCALIBTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "TileCalibAlgs/ITileCalibTool.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h" 
//#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "StoreGate/ReadHandleKey.h"

#include <string> 

class TileCablingService;
class TileCondToolEmscale;
class CaloLVL1_ID;
class TileHWID;
class TileID;
class TFile;
class TileRawChannelContainer;
class TileBeamElemContainer;
class Identifier;
class HWIdentifier;


class TileTriggerDefaultCalibTool : public AthAlgTool, virtual public ITileCalibTool
{

 public:
  TileTriggerDefaultCalibTool(const std::string& type, const std::string& name,const IInterface* pParent);
  virtual ~TileTriggerDefaultCalibTool();

  virtual StatusCode initialize() override;
  virtual StatusCode initNtuple(int runNumber, int runType, TFile * rootfile) override;
  virtual StatusCode execute() override;
  virtual StatusCode finalizeCalculations() override;
  virtual StatusCode writeNtuple(int runNumber, int runType, TFile * rootfile) override;
  virtual StatusCode finalize() override;

 private:

  // jobOptions
  std::string m_rawChannelContainerName;
  std::string m_ntupleID;
  std::string m_triggerTowerLocation;     //trigger tower container name
  int m_maxNTT;
  unsigned int m_nevpmt;

  // Tools / storegate info
  const CaloLVL1_ID* m_TT_ID;
  const TileHWID* m_tileHWID;
  const TileID*   m_tileID;
  const TileCablingService* m_tileCablingService;
  ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool
  SG::ReadHandleKey<TileDQstatus> m_dqStatusKey;
  SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
      "TileRawChannelContainer", "TileRawChannelFit", "Tile raw channel container"};
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_triggerTowerContainerKey{this,
      "TriggerTowerContainer", "xAODTriggerTowers", "Trigger Tower container"};
 
  using Tile = TileCalibUtils;

  // Results Tile
  float (*m_meanTile)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  float (*m_rmsTile)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  float (*m_meanTileDAC)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  float (*m_rmsTileDAC)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  int   (*m_ietaTile)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  int   (*m_iphiTile)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  int   (*m_ipmtTile)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  int   (*m_nEvtTile)[Tile::MAX_DRAWER][Tile::MAX_CHAN];

  // Results L1Calo
  float (*m_meanL1Calo)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  float (*m_rmsL1Calo)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  float (*m_meanL1CaloDAC)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  float (*m_rmsL1CaloDAC)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  int   (*m_ietaL1Calo)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  int   (*m_iphiL1Calo)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  int   (*m_ipmtL1Calo)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  int   (*m_nEvtL1Calo)[Tile::MAX_DRAWER][Tile::MAX_CHAN];

  float (*m_meanTileL1Calo)[Tile::MAX_DRAWER][Tile::MAX_CHAN];
  float (*m_rmsTileL1Calo)[Tile::MAX_DRAWER][Tile::MAX_CHAN];

  float m_DACvalue;

  // CISpar parameters
  float 	m_charge;
  unsigned int	m_ipmt;
  unsigned int	m_ipmtCount;
  unsigned int	m_ipmtOld;
  
  // Events
  int   m_nEvtGlobal[1];

  // for extended CISpar
  protected:

  std::string m_TileBeamContainerID;    //!< Name of the TileBeamElemContainer
  std::string m_TileTriggerContainerID; //!< Name of the TileTriggerContainer
  ToolHandle<LVL1::IL1CaloTTIdTools > m_l1CaloTTIdTools;
  //  ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool; 

  uint32_t m_BCID;         //!< BCID in LASTROD
  int      m_trigType;     //!< Trigger type
  

  const TileBeamElemContainer * m_beamElemCnt;

};

#endif // #ifndef TILECALIBALG_TILETRIGGERDEFAULTCALIBTOOL_H
