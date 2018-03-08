/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */

 /*
 *  NAME:     HLTCaloCellSumMaker.cxx
 *  PACKAGE:  Trigger/TrigDataAccess/TrigCaloDataAccessSvc
 *
 *  AUTHOR:   Denis Oliveira Damazio
 *
 *  PURPOSE:  New Algorithm to produce CaloCellContainer Vector
 *
 **/

#include "HLTCaloCellSumMaker.h"

HLTCaloCellSumMaker::HLTCaloCellSumMaker(const std::string & name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

HLTCaloCellSumMaker::~HLTCaloCellSumMaker()
{
}

StatusCode HLTCaloCellSumMaker::initialize() {
        return StatusCode::SUCCESS;
}

StatusCode HLTCaloCellSumMaker::execute_r( const EventContext& /*context*/ ) const {


  return StatusCode::SUCCESS;
}
