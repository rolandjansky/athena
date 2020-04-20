/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "BCM_Digitization.h"
#include "PileUpTools/IPileUpTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
BCM_Digitization::BCM_Digitization(const std::string &name, ISvcLocator *pSvcLocator) :
    AthAlgorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode BCM_Digitization::initialize()
{
  ATH_MSG_DEBUG ( "initialize()" );
  // intitialize store gate active store
  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_DEBUG ( "Retrieved BCM Digitization Tool." );

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------
StatusCode BCM_Digitization::execute()
{
  ATH_MSG_DEBUG ( "execute()" );
  return m_digTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
