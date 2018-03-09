/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBALG_TILETRIGGERDEFAULTCALIBTOOL_H
#define TILECALIBALG_TILETRIGGERDEFAULTCALIBTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TileCalibAlgs/ITileCalibTool.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h" 
//#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"

#include <string> 

class TileBeamInfoProvider;
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
  unsigned int m_nevpmt;

  // Tools / storegate info
  TileBeamInfoProvider *m_beamPrv;
  const CaloLVL1_ID* m_TT_ID;
  const TileHWID* m_tileHWID;
  const TileID*   m_tileID;
  const TileCablingService* m_tileCablingService;
  ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool
 
  // Results Tile
  float m_meanTile[5][64][48];
  float m_rmsTile[5][64][48];
  float m_meanTileDAC[5][64][48];
  float m_rmsTileDAC[5][64][48];
  int   m_ietaTile[5][64][48];
  int   m_iphiTile[5][64][48];
  int   m_ipmtTile[5][64][48];
  int   m_nEvtTile[5][64][48];

  // Results L1Calo
  float m_meanL1Calo[5][64][48];
  float m_rmsL1Calo[5][64][48];
  float m_meanL1CaloDAC[5][64][48];
  float m_rmsL1CaloDAC[5][64][48];
  int   m_ietaL1Calo[5][64][48];
  int   m_iphiL1Calo[5][64][48];
  int   m_ipmtL1Calo[5][64][48];
  int   m_nEvtL1Calo[5][64][48];

  float m_meanTileL1Calo[5][64][48];
  float m_rmsTileL1Calo[5][64][48];

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
