/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBALG_TILETRIGGERDEFAULTCALIBTOOL_H
#define TILECALIBALG_TILETRIGGERDEFAULTCALIBTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "TileCalibAlgs/ITileCalibTool.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h" 
//#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"

#include <string> 

class TileBeamInfoProvider;
class TileCablingService;
class CaloLVL1_ID;
class TileHWID;
class TileID;
class TFile;
class TileRawChannelContainer;
class TileBeamElemContainer;
class Identifier;
class HWIdentifier;


class TileTriggerDefaultCalibTool : public AlgTool, virtual public ITileCalibTool
{

 public:
  TileTriggerDefaultCalibTool(const std::string& type, const std::string& name,const IInterface* pParent);
  virtual ~TileTriggerDefaultCalibTool();

  virtual StatusCode initialize();
  virtual StatusCode initNtuple(int runNumber, int runType, TFile * rootfile);
  virtual StatusCode execute();
  virtual StatusCode finalizeCalculations();
  virtual StatusCode writeNtuple(int runNumber, int runType, TFile * rootfile);
  virtual StatusCode finalize();

 private:

  // jobOptions
  std::string m_rawChannelContainerName;
  std::string m_ntupleID;
  std::string m_triggerTowerLocation;     //trigger tower container name
  int m_maxNTT;
  int m_nevpmt;

  // Tools / storegate info
  StoreGateSvc* m_evtStore;
  StoreGateSvc* m_detStore;
  TileBeamInfoProvider *m_beamPrv;
  const CaloLVL1_ID* m_TT_ID;
  const TileHWID* m_tileHWID;
  const TileID*   m_tileID;
  const TileCablingService* m_tileCablingService;
 
  // Results Tile
  float meanTile[5][64][48];
  float rmsTile[5][64][48];
  float meanTileDAC[5][64][48];
  float rmsTileDAC[5][64][48];
  int   ietaTile[5][64][48];
  int   iphiTile[5][64][48];
  int   ipmtTile[5][64][48];
  int   nEvtTile[5][64][48];

  // Results L1Calo
  float meanL1Calo[5][64][48];
  float rmsL1Calo[5][64][48];
  float meanL1CaloDAC[5][64][48];
  float rmsL1CaloDAC[5][64][48];
  int   ietaL1Calo[5][64][48];
  int   iphiL1Calo[5][64][48];
  int   ipmtL1Calo[5][64][48];
  int   nEvtL1Calo[5][64][48];

  float meanTileL1Calo[5][64][48];
  float rmsTileL1Calo[5][64][48];

  float DACvalue;

  // CISpar parameters
  float 	charge;
  unsigned int	ipmt;
  unsigned int	idrawer;
  unsigned int	itower;
  unsigned int	ipmtCount;
  unsigned int	ipmtOld;
  
  // Events
  int   nEvtGlobal[1];

  // for extended CISpar
  protected:

  std::string m_TileBeamContainerID;    //!< Name of the TileBeamElemContainer
  std::string m_TileTriggerContainerID; //!< Name of the TileTriggerContainer
  ServiceHandle<StoreGateSvc> m_storeGate; //!< StoreGate pointer
  ToolHandle<LVL1::IL1CaloTTIdTools > m_l1CaloTTIdTools;
  //  ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool; 

  uint32_t m_BCID;         //!< BCID in LASTROD
  int      m_trigType;     //!< Trigger type
  

  const TileBeamElemContainer * m_beamElemCnt;

};

#endif // #ifndef TILECALIBALG_TILETRIGGERDEFAULTCALIBTOOL_H
