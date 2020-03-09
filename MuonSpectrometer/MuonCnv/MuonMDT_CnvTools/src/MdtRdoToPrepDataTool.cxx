/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtRdoToPrepDataTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MdtRdoToPrepDataTool.h"

Muon::MdtRdoToPrepDataTool::MdtRdoToPrepDataTool(const std::string& t, const std::string& n, const IInterface*  p)
  :
  AthAlgTool(t,n,p),
  MdtRdoToPrepDataToolCore(t,n,p)
{
}

Muon::MdtRdoToPrepDataTool::~MdtRdoToPrepDataTool()
{
}

StatusCode Muon::MdtRdoToPrepDataTool::initialize()
{    
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( MdtRdoToPrepDataToolCore::initialize() );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::MdtRdoToPrepDataTool::finalize()
{
  return MdtRdoToPrepDataToolCore::finalize();
}

Muon::MdtRdoToPrepDataTool::SetupMdtPrepDataContainerStatus Muon::MdtRdoToPrepDataTool::setupMdtPrepDataContainer()
{
  if(!evtStore()->contains<Muon::MdtPrepDataContainer>(m_mdtPrepDataContainerKey.key())){	 
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
  else {
    const Muon::MdtPrepDataContainer* outputCollection_c = 0;
    if (evtStore()->retrieve (outputCollection_c, m_mdtPrepDataContainerKey.key()).isFailure())
    {
      return FAILED;
    }
    m_mdtPrepDataContainer = const_cast<Muon::MdtPrepDataContainer*> (outputCollection_c);
  }
  return ALREADYCONTAINED;
}
