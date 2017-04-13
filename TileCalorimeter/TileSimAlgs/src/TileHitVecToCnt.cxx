/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileHitVecToCnt.cxx
//  Author   : Alexandre Solodkov
//  Created  : Jan, 2004
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

#include "TileSimAlgs/TileHitVecToCnt.h"
#include "PileUpTools/IPileUpTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
TileHitVecToCnt::TileHitVecToCnt(const std::string &name, ISvcLocator *pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_digTool("TileHitVecToCntTool", this )
{
  declareProperty("DigitizationTool", m_digTool, "AthAlgTool which performs the conversion from TileHitVectors to TileHitContainers");
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode TileHitVecToCnt::initialize() {
// intitialize store gate active store
  if (m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Could not retrieve ITileHitVecToCntTool!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved ITileHitVecToCntTool." );

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------

StatusCode TileHitVecToCnt::execute() {

  ATH_MSG_DEBUG ( "execute()" );

  return m_digTool->processAllSubEvents();
}

//----------------------------------------------------------------------//
// Finalize method:                                                     //
//----------------------------------------------------------------------//
StatusCode TileHitVecToCnt::finalize() {

  ATH_MSG_DEBUG ( "finalize()" );

  return StatusCode::SUCCESS;
}
