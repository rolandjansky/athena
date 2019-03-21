/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_StrawStatusSummaryTool.cxx
 *  @brief AthTool to manage TRT StrawStatus, LT/HT occupancy, PID and track data
 *  @author Peter Hansen <hansenph@nbi.dk>
 **/



#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdio.h>

#include "GaudiKernel/IToolSvc.h"
#include "Identifier/Identifier.h"

#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"
#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include "PathResolver/PathResolver.h"

#include "TRT_StrawStatusSummaryTool.h"
#include "InDetIdentifier/TRT_ID.h"
#include "StoreGate/ReadCondHandle.h"



TRT_StrawStatusSummaryTool::TRT_StrawStatusSummaryTool( const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent),
    m_detStore("DetectorStore",name),
    m_toolsvc("ToolSvc",name),
    m_par_strawstatuscontainerkey("/TRT/Cond/Status"),
    m_par_strawstatuspermanentcontainerkey("/TRT/Cond/StatusPermanent"),
    m_par_strawstatusHTcontainerkey("/TRT/Cond/StatusHT"),
    m_trtId(0),
    m_condSvc("CondSvc",name),
    m_statReadKey("/TRT/Cond/Status"),
    m_permReadKey("/TRT/Cond/StatusPermanent"),
    m_statHTReadKey("/TRT/Cond/StatusHT"),
    m_isGEANT4(true),
    m_strawstatusG4(nullptr),
    m_strawstatuspermanentG4(nullptr),
    m_strawstatusHTG4(nullptr)

{
  declareProperty("ToolSvc",m_toolsvc);
  declareProperty("isGEANT4",m_isGEANT4);

  // initialise event id cache
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  m_evtstat.push_back(invalidValue);
  m_evtperm.push_back(invalidValue);
  m_evtstatHT.push_back(invalidValue);

  m_strawstatuscontainer.push_back(nullptr);
  m_strawstatuspermanentcontainer.push_back(nullptr);
  m_strawstatusHTcontainer.push_back(nullptr);


}


StatusCode TRT_StrawStatusSummaryTool::initialize() 
{
  ATH_MSG_INFO("TRT_StrawStatusSummaryTool initialize method called");


  // Retrieve the DetectorStore
  if (StatusCode::SUCCESS!=m_detStore.retrieve()) {
    ATH_MSG_FATAL("Could not retrieve " << m_detStore);
    return StatusCode::FAILURE;
  }

  // Find ToolService
  if (StatusCode::SUCCESS!=m_toolsvc.retrieve()) {
    ATH_MSG_FATAL("ToolSvc not found");
    return StatusCode::FAILURE;
  }

  // Get the TRT ID helper
  if (StatusCode::SUCCESS!=m_detStore->retrieve(m_trtId,"TRT_ID")) {
    ATH_MSG_FATAL("Problem retrieving TRTID helper");
    return StatusCode::FAILURE;
  }

    // Read keys in case of normal reconstruction/digitization
  ATH_CHECK( m_statReadKey.initialize() );
  ATH_CHECK( m_permReadKey.initialize() );

  if(!m_isGEANT4) {
  
    ATH_CHECK( m_statHTReadKey.initialize() );
  }

  if(m_isGEANT4) {
    // processing GEANT4 simulation - revert to old non-MT style cond access

      if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatusHTG4,m_par_strawstatusHTcontainerkey)) {
        ATH_MSG_FATAL("Could not retrieve folder " << m_par_strawstatusHTcontainerkey);
        return StatusCode::FAILURE;
      }

  }

  ATH_MSG_INFO("TRT_StrawStatusSummaryTool initialized successfully  ");
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////


StatusCode TRT_StrawStatusSummaryTool::finalize()
{
  msg(MSG::INFO) << " in finalize() " << endmsg;
  return StatusCode::SUCCESS;
}


int TRT_StrawStatusSummaryTool::getStatus(Identifier offlineID) const{
  int stat = 0;
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtId->barrel_ec(offlineID),m_trtId->layer_or_wheel(offlineID),
								       m_trtId->phi_module(offlineID),m_trtId->straw_layer(offlineID),
								       m_trtId->straw(offlineID),level );
  const StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();
  static int countStrawStatusContainerError(0);
  if (!strawstatuscontainer) {
    if (countStrawStatusContainerError<5) { 
      ATH_MSG_WARNING(" getStatus, strawstatuscontainer == 0, dead straws not set"); 
      countStrawStatusContainerError++; 
    }
    return 0;
  }
  stat=int((*strawstatuscontainer).get(id).getstatus());
  return stat;
}



int TRT_StrawStatusSummaryTool::getStatusPermanent(Identifier offlineID) const{
  int stat = 0;
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtId->barrel_ec(offlineID),m_trtId->layer_or_wheel(offlineID),
								       m_trtId->phi_module(offlineID),m_trtId->straw_layer(offlineID),
								       m_trtId->straw(offlineID),level );
  const StrawStatusContainer* strawstatuspermanentcontainer = getStrawStatusPermanentContainer();
  static int countStrawStatusContainerError(0);
  if (!strawstatuspermanentcontainer) {
    if (countStrawStatusContainerError<5) { ATH_MSG_WARNING(" getStatusPermanent, strawstatuspermanentcontainer == 0, dead straws not set"); 
      countStrawStatusContainerError++; 
    }
    return 0;
  }
  stat=int((*strawstatuspermanentcontainer).get(id).getstatus());
  return stat;
}



int TRT_StrawStatusSummaryTool::getStatusHT(Identifier offlineID) const{
  int stat = 0;
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtId->barrel_ec(offlineID),m_trtId->layer_or_wheel(offlineID),
								m_trtId->phi_module(offlineID),m_trtId->straw_layer(offlineID),
								m_trtId->straw(offlineID),level );
  const StrawStatusContainer* strawstatusHTcontainer = getStrawStatusHTContainer();

 static int countStrawStatusContainerError(0);
 if (!strawstatusHTcontainer) {
   if (countStrawStatusContainerError<5) { 
     ATH_MSG_WARNING(" getStatusHT, strawstatusHTcontainer == 0, dead straws not set"); 
     countStrawStatusContainerError++; 
   }
    return 0;
 }
 stat=int((*strawstatusHTcontainer).get(id).getstatus());
 return stat;
}


///////////////////////////////////////////////////


bool TRT_StrawStatusSummaryTool::get_status(Identifier offlineID) const{
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  ATH_MSG_VERBOSE("offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & statusbitmask) >> 8));
  bool st = false, stperm=false;
  if (getStatus(offlineID)==1) st = true;
  else if (getStatus(offlineID)==0) st = false;
  else {st = bool( (getStatus(offlineID) & statusbitmask) >> 8);};


    if (getStatusPermanent(offlineID)==1) stperm = true;
    else if (getStatusPermanent(offlineID)==0) stperm = false;
    else {stperm = bool( (getStatusPermanent(offlineID) & statusbitmask) >> 8);};

  return ( (st||stperm) );
  
}



bool TRT_StrawStatusSummaryTool::get_statusHT(Identifier offlineID) const{
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  ATH_MSG_VERBOSE("offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & statusbitmask) >> 8));
  bool stHT=false;


    if (getStatusHT(offlineID)==1) stHT = true;
    else if (getStatusHT(offlineID)==0) stHT = false;
    else {stHT = bool( (getStatusHT(offlineID) & statusbitmask) >> 8);};

  return ( stHT );

}





