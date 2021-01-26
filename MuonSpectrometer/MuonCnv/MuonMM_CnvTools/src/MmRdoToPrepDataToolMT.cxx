/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MmRdoToPrepDataToolMT.h"

Muon::MmRdoToPrepDataToolMT::MmRdoToPrepDataToolMT(const std::string& t,
					       const std::string& n,
					       const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  MmRdoToPrepDataToolCore(t,n,p)
{
}

StatusCode Muon::MmRdoToPrepDataToolMT::initialize()
{  
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( MmRdoToPrepDataToolCore::initialize() );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

Muon::MMPrepDataContainer* Muon::MmRdoToPrepDataToolMT::setupMM_PrepDataContainer() const
{
  // MT version of this method always adds container. Caching will be added later.
  SG::WriteHandle< Muon::MMPrepDataContainer > handle(m_mmPrepDataContainerKey);
  StatusCode status = handle.record(std::make_unique<Muon::MMPrepDataContainer>(m_idHelperSvc->mmIdHelper().module_hash_max()));
  
  if (status.isFailure() || !handle.isValid() )   {
    ATH_MSG_FATAL("Could not record container of MicroMega PrepData Container at " << m_mmPrepDataContainerKey.key()); 
    return nullptr;
  }
  return handle.ptr();
}
