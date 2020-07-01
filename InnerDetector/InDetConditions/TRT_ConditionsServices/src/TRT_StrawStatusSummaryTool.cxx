/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_StrawStatusSummaryTool.cxx
 *  @brief AthTool to manage TRT StrawStatus, LT/HT occupancy, PID and track data
 *  @author Peter Hansen <hansenph@nbi.dk>
 **/

#include "Identifier/Identifier.h"
#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include "TRT_StrawStatusSummaryTool.h"
#include "InDetIdentifier/TRT_ID.h"

TRT_StrawStatusSummaryTool::TRT_StrawStatusSummaryTool( const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent),
    m_condSvc("CondSvc",name) {}


StatusCode TRT_StrawStatusSummaryTool::initialize() 
{
  ATH_MSG_INFO("TRT_StrawStatusSummaryTool initialize method called");

  // Get the TRT ID helper
  if (StatusCode::SUCCESS!=detStore()->retrieve(m_trtId,"TRT_ID")) {
    ATH_MSG_FATAL("Problem retrieving TRTID helper");
    return StatusCode::FAILURE;
  }

  // Read keys in case of normal reconstruction/digitization
  ATH_CHECK( m_statReadKey.initialize() );
  ATH_CHECK( m_permReadKey.initialize() );

  // Only require this input if not using G4 sim
  ATH_CHECK( m_statHTReadKey.initialize( !m_isGEANT4 ) );

  if ( m_isGEANT4 ) {
    // processing GEANT4 simulation - revert to old non-MT style cond access
    if ( StatusCode::SUCCESS!=detStore()->retrieve( m_strawstatusHTG4, m_par_strawstatusHTcontainerkey ) ) {
      ATH_MSG_FATAL( "Could not retrieve folder " << m_par_strawstatusHTcontainerkey );
      return StatusCode::FAILURE;
    }
  }

  ATH_MSG_INFO("TRT_StrawStatusSummaryTool initialized successfully  ");
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////


StatusCode TRT_StrawStatusSummaryTool::finalize()
{
  ATH_MSG_INFO(" in finalize() ");
  return StatusCode::SUCCESS;
}


int TRT_StrawStatusSummaryTool::getStatus(Identifier offlineID, const EventContext& ctx) const{
  constexpr int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtId->barrel_ec(offlineID),m_trtId->layer_or_wheel(offlineID),
								       m_trtId->phi_module(offlineID),m_trtId->straw_layer(offlineID),
								       m_trtId->straw(offlineID),level );

  SG::ReadCondHandle<StrawStatusContainer> rst(m_statReadKey,ctx);
  const StrawStatusContainer* strawstatuscontainer(*rst);
  
  return int((*strawstatuscontainer).get(id).getstatus());
}



int TRT_StrawStatusSummaryTool::getStatusPermanent(Identifier offlineID, const EventContext& ctx) const{
  constexpr int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtId->barrel_ec(offlineID),m_trtId->layer_or_wheel(offlineID),
								       m_trtId->phi_module(offlineID),m_trtId->straw_layer(offlineID),
								       m_trtId->straw(offlineID),level );

  SG::ReadCondHandle<StrawStatusContainer> rp(m_permReadKey,ctx);
  const StrawStatusContainer* strawstatuspermanentcontainer=(*rp);

  return int((*strawstatuspermanentcontainer).get(id).getstatus());
}



int TRT_StrawStatusSummaryTool::getStatusHT(Identifier offlineID, const EventContext& ctx) const{

  constexpr int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtId->barrel_ec(offlineID),m_trtId->layer_or_wheel(offlineID),
								m_trtId->phi_module(offlineID),m_trtId->straw_layer(offlineID),
								m_trtId->straw(offlineID),level );

  const StrawStatusContainer* strawstatusHTcontainer;
  if(m_isGEANT4) {
    strawstatusHTcontainer=m_strawstatusHTG4.cptr();
  }
  else {
    SG::ReadCondHandle<StrawStatusContainer> rht(m_statHTReadKey,ctx);
    strawstatusHTcontainer=(*rht);
  }

 return int((*strawstatusHTcontainer).get(id).getstatus());
}

const TRTCond::StrawStatusMultChanContainer* TRT_StrawStatusSummaryTool::getStrawStatusHTContainer() const{

  const StrawStatusContainer* strawstatusHTcontainer;
  if(m_isGEANT4) {
    strawstatusHTcontainer=m_strawstatusHTG4.cptr();
  }
  else {
    SG::ReadCondHandle<StrawStatusContainer> rht(m_statHTReadKey,Gaudi::Hive::currentContext());
    strawstatusHTcontainer=(*rht);
  }

 return strawstatusHTcontainer;
}


///////////////////////////////////////////////////


bool TRT_StrawStatusSummaryTool::get_status(Identifier offlineID, const EventContext& ctx) const{
  constexpr unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  ATH_MSG_VERBOSE("offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & statusbitmask) >> 8));
  bool st = false, stperm=false;
  const int status=getStatus(offlineID,ctx);
  
  if (status==1) st = true;
  else if (status==0) st = false;
  else {st = bool( (status & statusbitmask) >> 8);};


  const int statusperm=getStatusPermanent(offlineID,ctx);
  if (statusperm==1) stperm = true;
  else if (statusperm==0) stperm = false;
  else {stperm = bool( (statusperm & statusbitmask) >> 8);};

  return (st||stperm);
  
}



bool TRT_StrawStatusSummaryTool::get_statusHT(Identifier offlineID, const EventContext& ctx) const{
  constexpr unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  ATH_MSG_VERBOSE("offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & statusbitmask) >> 8));
  bool stHT=false;
  const int statusHT=getStatusHT(offlineID,ctx);

  if (statusHT==1) stHT = true;
  else if (statusHT==0) stHT = false;
  else {stHT = bool( (statusHT & statusbitmask) >> 8);};

  return ( stHT );

}

