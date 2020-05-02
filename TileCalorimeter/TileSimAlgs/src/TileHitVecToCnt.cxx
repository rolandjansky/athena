/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    AthAlgorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode TileHitVecToCnt::initialize() {
  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_DEBUG ( "Retrieved ITileHitVecToCntTool." );
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------
StatusCode TileHitVecToCnt::execute() {
  ATH_MSG_DEBUG ( "execute()" );
  return m_digTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
