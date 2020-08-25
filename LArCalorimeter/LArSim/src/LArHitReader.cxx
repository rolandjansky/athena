/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSim/LArHitReader.h"

#include "Gaudi/Property.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "LArSimEvent/LArHit.h"
#include "StoreGate/ReadHandle.h"

//Initialize method
StatusCode LArHitReader::initialize(){
  ATH_CHECK( m_hitKey.initialize() );
  return StatusCode::SUCCESS ;   
}


//Execute method
StatusCode LArHitReader::execute (const EventContext& ctx) const
{
  SG::ReadHandle<LArHitContainer> cont (m_hitKey, ctx);

  ATH_MSG_DEBUG(" LArHitContainer Dump ----------- " );
  ATH_MSG_DEBUG( (std::string)(*cont)   );

  return StatusCode::SUCCESS ;   
}

