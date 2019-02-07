/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileTBHitToBeamElem.cxx
//  Author   : Alexander Solodkov
//  Created  : July 2004
//
//  DESCRIPTION:
//     Implement the TileTBHitToBeamElem class
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileSimAlgs/TileTBHitToBeamElem.h"
#include "TileEvent/TileMutableBeamElemContainer.h"
#include "TileConditions/TileInfo.h"

// Calo includes
#include "CaloIdentifier/TileTBID.h"
#include "TileIdentifier/TileHWID.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"


//
// Constructor
//
TileTBHitToBeamElem::TileTBHitToBeamElem(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_infoName("TileInfo")
  , m_tileTBID(0)
  , m_tileHWID(0)
  , m_tileInfo(0)
{
  declareProperty("TileInfoName", m_infoName);
}

TileTBHitToBeamElem::~TileTBHitToBeamElem() {
}

//
// Alg standard initialize function
//
StatusCode TileTBHitToBeamElem::initialize() {

  // retrieve TileID helper and TileIfno from det store

  CHECK( detStore()->retrieve(m_tileTBID) );

  CHECK( detStore()->retrieve(m_tileHWID) );

  CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );

  ATH_CHECK( m_hitVectorKey.initialize() );
  ATH_CHECK( m_beamElemContainerKey.initialize() );

  ATH_MSG_INFO( "TileTBHitToBeamElem initialization completed" );

  return StatusCode::SUCCESS;
}

//
// Alg standard execute function
//
StatusCode TileTBHitToBeamElem::execute() {

  ATH_MSG_DEBUG( "Executing TileTBHitToBeamElem" );

  // create new container
  auto beamElemContainer = std::make_unique<TileMutableBeamElemContainer>(true);
  ATH_CHECK( beamElemContainer->status() );

  //**
  //* Get TileHits from TileHitVector
  //**
  SG::ReadHandle<TileHitVector> hitVector(m_hitVectorKey);
  ATH_CHECK( hitVector.isValid() );

  //**
  //* Iterate over hits
  //**
  for (const TileHit& tile_hit : *hitVector) {

    // Get hit Identifier (= identifier of pmt)
    Identifier pmt_id = tile_hit.pmt_ID();
    // adc_id and channel_id for beam elem are the same
    // because there is only one gain, i.e. gain bit is always 0
    HWIdentifier adc_id = tile_hit.pmt_HWID();

    /* Get hit amplitude and convert to energy (cell-dependent) */
    double hit_calib = m_tileInfo->BeamElemHitCalib(pmt_id);
    double e_hit = tile_hit.energy();
    double e_ch = e_hit * hit_calib;

    /* Convert to amplitude of channel */
    /* need to divide here, because "calib" converts amp to energy */
    double calib = m_tileInfo->BeamElemChannelCalib(adc_id);
    // amplitude is an integer number, this is the signal we get from ADC
    int amp_ch = (int) (e_ch / calib);

    ATH_MSG_VERBOSE( " pmt=" << m_tileTBID->to_string(pmt_id)
                    << " adc=" << m_tileHWID->to_string(adc_id)
                    << " amp=" << amp_ch);

    std::unique_ptr<TileBeamElem> beamElem = std::make_unique<TileBeamElem>(adc_id, amp_ch);
    ATH_CHECK( beamElemContainer->push_back(std::move(beamElem)) );
  }

  SG::WriteHandle<TileBeamElemContainer> beamElemCnt(m_beamElemContainerKey);
  ATH_CHECK( beamElemCnt.record(std::move(beamElemContainer)) );

  // Execution completed.
  ATH_MSG_DEBUG( "TileTBHitToBeamElem execution completed." );

  return StatusCode::SUCCESS;
}

StatusCode TileTBHitToBeamElem::finalize() {

  ATH_MSG_INFO( "TileTBHitToBeamElem::finalize() end." );

  return StatusCode::SUCCESS;
}

