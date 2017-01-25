/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// BCIDFilterTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PrimaryDPDMaker/BCIDFilterTool.h"

#include "xAODEventInfo/EventInfo.h"

#include <string>

// Constructor
BCIDFilterTool::BCIDFilterTool( const std::string& t,
				const std::string& n,
				const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("AcceptBCIDs", m_acceptBCIDs);
    declareProperty("RejectBCIDs", m_rejectBCIDs);
  }
  
// Destructor
BCIDFilterTool::~BCIDFilterTool() {}

// Athena initialize and finalize
StatusCode BCIDFilterTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     if (m_acceptBCIDs.size() && m_rejectBCIDs.size()) {
       ATH_MSG_ERROR("Failed to initialize - both accept and reject BCIDs specified, please only use one");
       return StatusCode::FAILURE;
     }
     if (!(m_acceptBCIDs.size() || m_rejectBCIDs.size())) {
       ATH_MSG_ERROR("Neither AcceptBCIDs nor RejectBCIDs specified, can't use filter!");
     }
     if (m_acceptBCIDs.size()) {
       ATH_MSG_INFO("Events with the following " << m_acceptBCIDs.size() << " BCIDs will be accepted into the stream");
       for (int bcid : m_acceptBCIDs) ATH_MSG_INFO("   " << bcid);
     }
     if (m_rejectBCIDs.size()) {
       ATH_MSG_INFO("Events with the following " << m_rejectBCIDs.size() << " BCIDs will be rejected from the stream");
       for (int bcid : m_rejectBCIDs) ATH_MSG_INFO("   " << bcid);
     }

     return StatusCode::SUCCESS;
}
StatusCode BCIDFilterTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     return StatusCode::SUCCESS;
}

// The filter itself
bool BCIDFilterTool::eventPassesFilter() const
{
     ++m_ntot;

     const xAOD::EventInfo* ei(0);
     ATH_CHECK( evtStore()->retrieve(ei, "EventInfo") );

     int bcid = ei->bcid();
     
     if (m_acceptBCIDs.size()) {
       for (auto acceptBCID : m_acceptBCIDs) {
	 if (bcid == acceptBCID) return true;
       }
       return false;
     }

     else {
       for (auto rejectBCID : m_rejectBCIDs) {
	 if (bcid == rejectBCID) return false;
       }
       return true;
     }
}  
  
