/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileMuonReceiverDecision.h
// Author   : Joao Gentil Mendes Saraiva (jmendes@cern.ch)
// Created  : December 2013
//
// DESCRIPTION
// 
// Tile Muon Receiver decision
//
// Properties (JobOption Parameters):
//
//       MuonReceiverEneThreshCellD6Low        "(RUN2) Setting the lowest trigger threshold for cell D6 in MeV (Def=500 MeV)"
//       MuonReceiverEneThreshCellD6andD5Low   "(RUN2) Setting the lowest trigger threshold for cell D5+D6 in MeV (Def=500 MeV)"
//       MuonReceiverEneThreshCellD6High       "(RUN2) Setting the highest trigger threshold for cell D6 in MeV (Def=600 MeV)"
//       MuonReceiverEneThreshCellD6andD5High  "(RUN2) Setting the highest trigger threshold for cell D5+D6 in MeV (Def=600 MeV)"
//       MuonReceiverEneThreshCellD5           "(RUN3) Setting the single trigger threshold for cell D5 in MeV (Def=500 MeV)"
//       MuonReceiverEneThreshCellD6           "(RUN3) Setting the single trigger threshold for cell D6 in MeV (Def=500 MeV)"
//       MuonReceiverEneThreshCellD5andD6      "(RUN3) Setting the single trigger threshold for cell d5+D6 in MeV (Def=500 MeV)"
//       SelectionCutForMatchedFilterQf        "Selection cut for the quality factor of the matched filters (NOT implemented)"
//       TileMuonReceiverContainer             "Tile Calorimeter decision to TGC Sector Logic"
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILESIMALGS_TILEMUONRECEIVERDECISION_H
#define TILESIMALGS_TILEMUONRECEIVERDECISION_H

// Tile includes
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileContainer.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCablingSvc.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class HWIdentifier;

class TileID;
class TileHWID;
class TileInfo;
class TileCablingService;


class TileMuonReceiverDecision: public AthAlgorithm {

 public:
  // constructor
  TileMuonReceiverDecision(const std::string& name, ISvcLocator* pSvcLocator);
  // destructor
  virtual ~TileMuonReceiverDecision();
  // Gaudi hooks
  virtual StatusCode initialize() override; //!< initialize method
  virtual StatusCode execute() override;    //!< execute method
  virtual StatusCode finalize() override;   //!< finalize method

 private:

  SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,"TileRawChannelContainer",
                                                                      "MuRcvRawChCnt",
                                                                      "Input Tile raw channel container key"};


  SG::WriteHandleKey<TileMuonReceiverContainer> m_muonReceiverContainerKey{this,"TileMuonReceiverContainer",
                                                                           "TileMuRcvCnt",
                                                                           "Output Tile muon receiver container key"};


  std::string m_infoName;             //!< Name of tile info object in TES

  const TileID*             m_tileID;
  const TileHWID*           m_tileHWID;
  const TileCablingService* m_cablingService; //!< TileCabling instance
  const TileInfo*           m_tileInfo;

  float m_threshold_d6_lo;
  float m_threshold_d6_hi;
  float m_threshold_d5d6_lo;
  float m_threshold_d5d6_hi;
  float m_threshold_d5;
  float m_threshold_d6;
  float m_threshold_d5d6;
  float m_selCutQf;

  ToolHandle<TileCondToolEmscale> m_tileToolEmscale{this,
      "TileCondToolEmscale", "TileCondToolEmscale", "Tile EM scale calibration tool"};

  /**
   * @brief Name of Tile cabling service
   */
  ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
      "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};
  int m_runPeriod;

};

#endif
