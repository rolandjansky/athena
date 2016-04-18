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

// Gaudi Includes
#include "GaudiKernel/ISvcLocator.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloIdentifier/TileTBID.h"
#include "TileIdentifier/TileHWID.h"

// Tile includes
#include "TileConditions/TileInfo.h"
#include "TileSimEvent/TileHitVector.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileSimAlgs/TileTBHitToBeamElem.h"

//
// Constructor
//
TileTBHitToBeamElem::TileTBHitToBeamElem(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_hitVector("TileTBHits")
  , m_beamElemContainer("TileBeamElemCnt")
  , m_infoName("TileInfo")
  , m_tileTBID(0)
  , m_tileHWID(0)
  , m_tileInfo(0)
{
  declareProperty("TileTBHitVector", m_hitVector);    
  declareProperty("TileBeamElemContainer", m_beamElemContainer);
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

  ATH_MSG_INFO( "TileTBHitToBeamElem initialization completed" );

  return StatusCode::SUCCESS;
}

//
// Alg standard execute function
//
StatusCode TileTBHitToBeamElem::execute() {

  ATH_MSG_DEBUG( "Executing TileTBHitToBeamElem" );

  // create new container
  TileBeamElemContainer * pBeamElemContainer = new TileBeamElemContainer(true);

  //**
  //* Get TileHits from TileHitVector
  //**
  const TileHitVector * inputHits;
  CHECK( evtStore()->retrieve(inputHits, m_hitVector) );

  TileHitVecConstIterator hitItr = inputHits->begin();
  TileHitVecConstIterator end = inputHits->end();

  //**
  //* Iterate over hits
  //**

  for (; hitItr != end; ++hitItr) {

    // Get hit Identifier (= identifier of pmt)
    Identifier pmt_id = hitItr->pmt_ID();
    // adc_id and channel_id for beam elem are the same
    // because there is only one gain, i.e. gain bit is always 0
    HWIdentifier adc_id = hitItr->pmt_HWID();

    /* Get hit amplitude and convert to energy (cell-dependent) */
    double hit_calib = m_tileInfo->BeamElemHitCalib(pmt_id);
    double e_hit = hitItr->energy();
    double e_ch = e_hit * hit_calib;

    /* Convert to amplitude of channel */
    /* need to divide here, because "calib" converts amp to energy */
    double calib = m_tileInfo->BeamElemChannelCalib(adc_id);
    // amplitude is an integer number, this is the signal we get from ADC
    int amp_ch = (int) (e_ch / calib);

    ATH_MSG_VERBOSE( " pmt=" << m_tileTBID->to_string(pmt_id)
                    << " adc=" << m_tileHWID->to_string(adc_id)
                    << " amp=" << amp_ch);

    TileBeamElem * pBeamElem = new TileBeamElem(adc_id, amp_ch);
    pBeamElemContainer->push_back(pBeamElem);

  }

  // step3: register the cell container in the TES
  CHECK( evtStore()->record(pBeamElemContainer, m_beamElemContainer, false) );

  // Execution completed.
  ATH_MSG_DEBUG( "TileTBHitToBeamElem execution completed." );

  return StatusCode::SUCCESS;
}

StatusCode TileTBHitToBeamElem::finalize() {

  ATH_MSG_INFO( "TileTBHitToBeamElem::finalize() end." );

  return StatusCode::SUCCESS;
}

