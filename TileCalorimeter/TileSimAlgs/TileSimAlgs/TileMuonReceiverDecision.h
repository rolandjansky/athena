/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
//       MuonReceiverEneThreshCellD6Low        "Setting the lowest trigger threshold for cell D6 in MeV (Def=500 MeV)"
//       MuonReceiverEneThreshCellD6andD5Low   "Setting the lowest trigger threshold for cell D5+D6 in MeV (Def=500 MeV)"
//       MuonReceiverEneThreshCellD6High       "Setting the highest trigger threshold for cell D6 in MeV (Def=600 MeV)"
//       MuonReceiverEneThreshCellD6andD5High  "Setting the highest trigger threshold for cell D5+D6 in MeV (Def=600 MeV)"
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
class TileCondToolEmscale;

class TileMuonReceiverDecision: public AthAlgorithm {

 public:
  // constructor
  TileMuonReceiverDecision(std::string name, ISvcLocator* pSvcLocator);
  // destructor
  virtual ~TileMuonReceiverDecision();
  // Gaudi hooks
  StatusCode initialize(); //!< initialize method
  StatusCode execute();    //!< execute method
  StatusCode finalize();   //!< finalize method

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
  float m_selCutQf;

  ToolHandle<TileCondToolEmscale> m_tileToolEmscale;         //!< main Tile Calibration tool

  bool m_run2;
};

#endif
