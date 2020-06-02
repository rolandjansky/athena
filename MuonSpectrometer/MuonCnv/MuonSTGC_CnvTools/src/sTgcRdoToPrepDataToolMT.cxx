/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// sTgcRdoToPrepDataToolMT.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "sTgcRdoToPrepDataToolMT.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"


Muon::sTgcRdoToPrepDataToolMT::sTgcRdoToPrepDataToolMT(const std::string& t,
						   const std::string& n,
						   const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  sTgcRdoToPrepDataToolCore(t,n,p)
{
}

StatusCode Muon::sTgcRdoToPrepDataToolMT::initialize()
{  
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( sTgcRdoToPrepDataToolCore::initialize() );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

Muon::sTgcRdoToPrepDataToolCore::SetupSTGC_PrepDataContainerStatus Muon::sTgcRdoToPrepDataToolMT::setupSTGC_PrepDataContainer() 
{

  if(!evtStore()->contains<Muon::sTgcPrepDataContainer>(m_stgcPrepDataContainerKey.key())){    
    m_fullEventDone=false;
    
    SG::WriteHandle< Muon::sTgcPrepDataContainer > handle(m_stgcPrepDataContainerKey);
    StatusCode status = handle.record(std::make_unique<Muon::sTgcPrepDataContainer>(m_idHelperSvc->stgcIdHelper().module_hash_max()));
    
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of STGC PrepData Container at " << m_stgcPrepDataContainerKey.key()); 
      return FAILED;
    }
    m_stgcPrepDataContainer = handle.ptr();
    return ADDED;
    
  }
  return ALREADYCONTAINED;
}
