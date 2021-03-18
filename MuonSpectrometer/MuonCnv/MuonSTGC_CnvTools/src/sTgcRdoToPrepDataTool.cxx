/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// sTgcRdoToPrepDataTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "sTgcRdoToPrepDataTool.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"


Muon::sTgcRdoToPrepDataTool::sTgcRdoToPrepDataTool(const std::string& t,
               const std::string& n,
               const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  sTgcRdoToPrepDataToolCore(t,n,p)
{
}

StatusCode Muon::sTgcRdoToPrepDataTool::initialize()
{  
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( sTgcRdoToPrepDataToolCore::initialize() );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

Muon::sTgcPrepDataContainer* Muon::sTgcRdoToPrepDataTool::setupSTGC_PrepDataContainer() const
{

  if(!evtStore()->contains<Muon::sTgcPrepDataContainer>(m_stgcPrepDataContainerKey.key())){    
    SG::WriteHandle< Muon::sTgcPrepDataContainer > handle(m_stgcPrepDataContainerKey);
    StatusCode status = handle.record(std::make_unique<Muon::sTgcPrepDataContainer>(m_idHelperSvc->stgcIdHelper().module_hash_max()));
    
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of STGC PrepData Container at " << m_stgcPrepDataContainerKey.key()); 
      return nullptr;
    }
    m_stgcPrepDataContainer = handle.ptr();
    
  }
  return m_stgcPrepDataContainer;
}
