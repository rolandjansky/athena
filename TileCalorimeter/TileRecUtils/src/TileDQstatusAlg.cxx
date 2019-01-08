/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/src/TileDQstatusAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2018
 * @brief Produce a TileDQstatus object.
 *        This replaces the old TileBeamInfoProvider.
 */


#include "TileDQstatusAlg.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileIdentifier/TileHWID.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthenaKernel/RNGWrapper.h"
#include <CLHEP/Random/RandomEngine.h>
#include <CLHEP/Random/RandFlat.h>


/**
 * @brief Constructor.
 * @param name Algorithm name.
 * @param svcloc Gaudi service locator.
 */
TileDQstatusAlg::TileDQstatusAlg (const std::string& name, ISvcLocator* svcloc)
  : AthReentrantAlgorithm (name, svcloc)
{
}


/**
 * @brief Gaudi initialize method.
 */
StatusCode TileDQstatusAlg::initialize()
{
  ATH_CHECK( m_tileDQstatusTool.retrieve() );

  ATH_CHECK( m_DQstatusKey.initialize() );
  if (!m_rawChannelContainerKey.key().empty())
    ATH_CHECK( m_rawChannelContainerKey.initialize() );
  if (!m_digitsContainerKey.key().empty())
    ATH_CHECK( m_digitsContainerKey.initialize() );
  if (!m_beamElemContainerKey.key().empty())
    ATH_CHECK( m_beamElemContainerKey.initialize() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Gaudi execute method.
 * @param ctx Event context.
 */
StatusCode TileDQstatusAlg::execute (const EventContext& ctx) const
{
  auto dqstatus = std::make_unique<TileDQstatus>();

  const TileDigitsContainer* tileDigitsContainer = nullptr;
  if (!m_digitsContainerKey.key().empty()) {
    tileDigitsContainer = SG::makeHandle (m_digitsContainerKey, ctx).get();
  }

  const TileRawChannelContainer* tileRawChannelContainer = nullptr;
  if (!m_rawChannelContainerKey.key().empty()) {
    tileRawChannelContainer = SG::makeHandle (m_rawChannelContainerKey, ctx).get();
  }

  const TileBeamElemContainer* tileBeamElemContainer = nullptr;
  if (!m_beamElemContainerKey.key().empty()) {
    tileBeamElemContainer = SG::makeHandle (m_beamElemContainerKey, ctx).get();
  }

  ATH_CHECK( m_tileDQstatusTool->makeStatus (ctx,
                                             tileRawChannelContainer,
                                             tileDigitsContainer,
                                             tileBeamElemContainer,
                                             *dqstatus) );

  ATH_CHECK( SG::makeHandle (m_DQstatusKey, ctx).record (std::move (dqstatus)) );
  return StatusCode::SUCCESS;
}

