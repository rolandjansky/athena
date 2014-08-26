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
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILESIMALGS_TILEMUONRECEIVERDECISION_H
#define TILESIMALGS_TILEMUONRECEIVERDECISION_H

#include "AthenaBaseComps/AthAlgorithm.h"
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
  TileMuonReceiverDecision(std::string name, ISvcLocator* pSvcLocator);
  // destructor
  virtual ~TileMuonReceiverDecision();
  // Gaudi hooks
  StatusCode initialize(); //!< initialize method
  StatusCode execute();    //!< execute method
  StatusCode finalize();   //!< finalize method

 private:
  std::string m_MuRcvRawChContainer;  //!< Name of the algorithm raw channel container 
  std::string m_TileMuRcvContainer;
  std::string m_infoName;             //!< Name of tile info object in TES

  const TileID*             m_tileID;
  const TileHWID*           m_tileHWID;
  const TileCablingService* m_cablingService; //!< TileCabling instance
  
  float m_threshold_d6_lo;
  float m_threshold_d6_hi;
  float m_threshold_d5d6_lo;
  float m_threshold_d5d6_hi;
  float m_selCutQf;

};

#endif
