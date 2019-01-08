// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/src/TileDQstatusAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2018
 * @brief Produce a TileDQstatus object.
 *        This replaces the old TileBeamInfoProvider.
 */


#ifndef TILERECUTILS_TILEDQSTATUSALG_H
#define TILERECUTILS_TILEDQSTATUSALG_H


#include "TileRecUtils/ITileDQstatusTool.h"
#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileConditions/ITileBadChanTool.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
class TileHWID;


/**
 * @brief Produce a TileDQstatus object.
 *        This replaces the old TileBeamInfoProvider.
 */
class TileDQstatusAlg
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name Algorithm name.
   * @param svcloc Gaudi service locator.
   */
  TileDQstatusAlg (const std::string& name, ISvcLocator* svcloc);


  /**
   * @brief Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Gaudi execute method.
   * @param ctx Event context.
   */
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  /// ** Properties
  SG::WriteHandleKey<TileDQstatus> m_DQstatusKey
  {this, "TileDQstatus", "TileDQstatus", "Output TileDQstatus key" };

  SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey
  {this, "TileRawChannelContainer", "", "Input TileRawChannelContainer key" };

  SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey
  {this, "TileDigitsContainer", "", "Input TileDigitsContainer key" };

  SG::ReadHandleKey<TileBeamElemContainer> m_beamElemContainerKey
  {this, "TileBeamElemContainer",  "", "Input Tile beam elements container key"};

  ToolHandle<ITileDQstatusTool> m_tileDQstatusTool
  {this, "TileDQstatusTool", "TileDQstatusTool", "TileDQstatusTool"};
};


#endif // not TILERECUTILS_TILEDQSTATUSALG_H
