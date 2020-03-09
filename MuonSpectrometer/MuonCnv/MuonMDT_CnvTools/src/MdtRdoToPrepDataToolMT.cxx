/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtRdoToPrepDataToolMT.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MdtRdoToPrepDataToolMT.h"


Muon::MdtRdoToPrepDataToolMT::MdtRdoToPrepDataToolMT(const std::string& t, const std::string& n, const IInterface* p)
  :
  AthAlgTool(t,n,p),
  MdtRdoToPrepDataToolCore(t,n,p)
{
}

Muon::MdtRdoToPrepDataToolMT::~MdtRdoToPrepDataToolMT()
{
}

StatusCode Muon::MdtRdoToPrepDataToolMT::initialize()
{    
    ATH_MSG_VERBOSE("Starting init");
    ATH_CHECK( MdtRdoToPrepDataToolCore::initialize() );
    ATH_MSG_DEBUG("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

StatusCode Muon::MdtRdoToPrepDataToolMT::finalize()
{
  return MdtRdoToPrepDataToolCore::finalize();
}

Muon::MdtRdoToPrepDataToolMT::SetupMdtPrepDataContainerStatus Muon::MdtRdoToPrepDataToolMT::setupMdtPrepDataContainer()
{
  // MT version of this method always adds container. Caching will be added later.
  m_fullEventDone=false;

  SG::WriteHandle< Muon::MdtPrepDataContainer >handle(m_mdtPrepDataContainerKey);
  StatusCode status = handle.record(std::make_unique<Muon::MdtPrepDataContainer>(m_muonIdHelperTool->mdtIdHelper().module_hash_max()));

  if (status.isFailure() || !handle.isValid() ) 	{
    ATH_MSG_FATAL("Could not record container of MDT PrepData Container at " << m_mdtPrepDataContainerKey.key());	
    return FAILED;
  }
  m_mdtPrepDataContainer = handle.ptr();
  return ADDED;
}