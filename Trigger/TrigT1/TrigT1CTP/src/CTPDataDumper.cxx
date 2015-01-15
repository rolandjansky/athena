/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// this class header file
#include "TrigT1CTP/CTPDataDumper.h"

// STL includes:
#include <vector>
#include <iomanip>

// Athena/Gaudi includes:
#include "StoreGate/DataHandle.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

// Common interface includes:
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Interfaces/CTPSLink.h"

// CTP DAQ output:
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"



namespace LVL1CTP {

  CTPDataDumper::CTPDataDumper( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator )
  {

    // Property setting general behaviour:
    declareProperty( "RawFormat", m_rawFormat = false, "Dump data content in raw, compressed format" );
    declareProperty( "PrettyFormat", m_prettyFormat = true, "Dump data content in human understandable format" );

    // Properties setting up the locations of the objects:
    declareProperty( "RoILocation", m_roiLoc = LVL1CTP::DEFAULT_CTPSLinkLocation,
		     "StoreGate location of CTP RoI");
    declareProperty( "RDOLocation", m_rdoLoc = LVL1CTP::DEFAULT_RDOOutputLocation,
		     "StoreGate location of CTP RDO" );

  }

  CTPDataDumper::~CTPDataDumper() {

  }

  StatusCode CTPDataDumper::initialize() {

    ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);

    return StatusCode::SUCCESS;
  }

  StatusCode CTPDataDumper::finalize() {

    ATH_MSG_INFO("Finalizing " << name() << " - package version " << PACKAGE_VERSION);

    return StatusCode::SUCCESS;
  }

  StatusCode CTPDataDumper::execute() {

    ATH_MSG_DEBUG("Executing CTPDataDumper algorithm");

    // dump content of CTP_RDO
    StatusCode sc = dumpCTP_RDO();
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Couldn't dump CTP_RDO object from StoreGate");
    }
    
    // dump content of CTPSLink
    sc = dumpCTPSLink();
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Couldn't dump CTPSLink object from StoreGate");
    }

    // dump content of EventInfo
    sc = dumpEventInfo();
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Couldn't dump EventInfo object from StoreGate");
    }

    return StatusCode::SUCCESS;
  }


  StatusCode CTPDataDumper::dumpCTPSLink() {

    // get object from StoreGate
    const DataHandle<CTPSLink> ctpSLink;
    StatusCode sc = evtStore()->retrieve(ctpSLink, m_roiLoc);

    if (sc.isFailure()) {
      ATH_MSG_WARNING("No CTP SLink found in TES at: " << m_roiLoc);
    } else {

      if (m_rawFormat) {
	ATH_MSG_DEBUG(std::endl << ctpSLink->dump());
      }
      if (m_prettyFormat) {
	ATH_MSG_DEBUG(std::endl << ctpSLink->print());
      }
      
    }

    return StatusCode::SUCCESS;
  }


  StatusCode CTPDataDumper::dumpCTP_RDO() {

    // get object from StoreGate
    const DataHandle<CTP_RDO> ctpRDO;
    StatusCode sc = evtStore()->retrieve(ctpRDO, m_rdoLoc);

    if (sc.isFailure()) {
      ATH_MSG_WARNING("No CTP RDO found in TES at: " << m_rdoLoc);
    } else {

      if (m_rawFormat) {
	ATH_MSG_DEBUG("CTP_RDO: " << ctpRDO->dump());
      }
      if (m_prettyFormat) {
	ATH_MSG_DEBUG("CTP_RDO: " << ctpRDO->print());
      }
      
    }

    return StatusCode::SUCCESS;
  }


  StatusCode CTPDataDumper::dumpEventInfo() {

    // get object from StoreGate
    const DataHandle< ::EventInfo > eventInfo;
    StatusCode sc = evtStore()->retrieve( eventInfo );

    if( sc.isFailure() ) {
      ATH_MSG_WARNING("No EventInfo found in TES");
    } 
    else {
      if (m_rawFormat) {
      }
      if (m_prettyFormat) {
	ATH_MSG_DEBUG("EventInfo dump:" 
		      << "\nEventID: " << *eventInfo->event_ID() 
		      // << "\nEventInfo: " << eventInfo->event_Type();
		      << "\nTriggerInfo: " << *eventInfo->trigger_info());
      }
    }
    
    return StatusCode::SUCCESS;
  }
  
} // namespace LVL1CTP
