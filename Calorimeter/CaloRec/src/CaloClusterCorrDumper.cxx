/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
//
// Description: see CaloClusterCorrDumper.h
//-----------------------------------------------------------------------


#include "CaloClusterCorrDumper.h"
#include "StoreGate/ReadCondHandle.h"



StatusCode CaloClusterCorrDumper::initialize()
{
  ATH_CHECK( m_constants.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode CaloClusterCorrDumper::execute (const EventContext& ctx) const
{
  for (const SG::ReadCondHandleKey<CaloRec::ToolConstants>& k : m_constants) {
    SG::ReadCondHandle<CaloRec::ToolConstants> constant (k, ctx);
    ATH_MSG_INFO( constant->toString ("dumper") );
  }
  return StatusCode::SUCCESS;
}


