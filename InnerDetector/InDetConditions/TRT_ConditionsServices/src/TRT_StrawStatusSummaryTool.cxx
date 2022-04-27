/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_StrawStatusSummaryTool.cxx
 *  @brief AthTool to manage TRT StrawStatus, LT/HT occupancy, PID and track data
 *  @author Peter Hansen <hansenph@nbi.dk>
 **/

#include "Identifier/Identifier.h"
#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include "TRT_StrawStatusSummaryTool.h"
#include "InDetIdentifier/TRT_ID.h"
#include "GaudiKernel/IIncidentSvc.h"

TRT_StrawStatusSummaryTool::TRT_StrawStatusSummaryTool( const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent),
    m_condSvc("CondSvc",name) {}

void TRT_StrawStatusSummaryTool::handle( const Incident& inc ){
  if (m_optimizeCondAccess){
    if (inc.type() == "TRTStrawCalib") {
      const EventContext& ctx = Gaudi::Hive::currentContext();
      SG::ReadHandle<TRTStrawStatusData> statData(m_statReadDataKey, ctx);
      SG::ReadHandle<TRTStrawStatusData> statPermData(m_permReadDataKey, ctx);
      p_statData = statData.get();
      p_statPermData = statPermData.get();
    }
  }
}

StatusCode TRT_StrawStatusSummaryTool::initialize() 
{
  ATH_MSG_DEBUG("TRT_StrawStatusSummaryTool initialize method called");

  // Get the TRT ID helper
  if (StatusCode::SUCCESS!=detStore()->retrieve(m_trtId,"TRT_ID")) {
    ATH_MSG_FATAL("Problem retrieving TRTID helper");
    return StatusCode::FAILURE;
  }

  // reconstruction/digitization
  // Read keys in case of direct conditions access 
  ATH_CHECK( m_statReadKey.initialize(!m_optimizeCondAccess) );
  ATH_CHECK( m_permReadKey.initialize(!m_optimizeCondAccess) );

  // Read keys in case of precomputed conditions
  ATH_CHECK( m_statReadDataKey.initialize(m_optimizeCondAccess) );
  ATH_CHECK( m_permReadDataKey.initialize(m_optimizeCondAccess) );

  // Only require this input if not using G4 sim
  ATH_CHECK( m_statHTReadKey.initialize( !m_isGEANT4 ) );

  // G4sim
  if ( m_isGEANT4 ) {
    // processing GEANT4 simulation - revert to old non-MT style cond access
    if ( StatusCode::SUCCESS!=detStore()->retrieve( m_strawstatusHTG4, m_par_strawstatusHTcontainerkey ) ) {
      ATH_MSG_FATAL( "Could not retrieve folder " << m_par_strawstatusHTcontainerkey );
      return StatusCode::FAILURE;
    }
  }

  ServiceHandle<IIncidentSvc> p_incSvc("IncidentSvc", this->name());
  p_incSvc->addListener( this, "TRTStrawCalib" );

  ATH_MSG_DEBUG("TRT_StrawStatusSummaryTool initialized successfully  ");
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////


StatusCode TRT_StrawStatusSummaryTool::finalize()
{
  return StatusCode::SUCCESS;
}


int TRT_StrawStatusSummaryTool::getStatus(Identifier offlineID, const EventContext& ctx) const{
  int strawStatus = -1;

  if (!m_optimizeCondAccess) {
    constexpr int level = TRTCond::ExpandedIdentifier::STRAW ;
    TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtId->barrel_ec(offlineID),m_trtId->layer_or_wheel(offlineID),
								  m_trtId->phi_module(offlineID),m_trtId->straw_layer(offlineID),
								  m_trtId->straw(offlineID),level );
    
    SG::ReadCondHandle<StrawStatusContainer> rst(m_statReadKey,ctx);
    const StrawStatusContainer* strawstatuscontainer(*rst);
    
    strawStatus = int((*strawstatuscontainer).get(id).getstatus());
  }
  else {
    IdentifierHash hashID = m_trtId->straw_hash(offlineID);
    if (p_statData){
      strawStatus = p_statData->findStatus(hashID);
    } 
    else {
      ATH_MSG_ERROR("Cannot access StrawStat calibration data");
    }
  }
  
  return strawStatus;
}



int TRT_StrawStatusSummaryTool::getStatusPermanent(Identifier offlineID, const EventContext& ctx) const{

  int strawStatus = -1;  

  if (!m_optimizeCondAccess) {

    constexpr int level = TRTCond::ExpandedIdentifier::STRAW ;
    TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtId->barrel_ec(offlineID),m_trtId->layer_or_wheel(offlineID),
								  m_trtId->phi_module(offlineID),m_trtId->straw_layer(offlineID),
								  m_trtId->straw(offlineID),level );
    
    SG::ReadCondHandle<StrawStatusContainer> rp(m_permReadKey,ctx);
    const StrawStatusContainer* strawstatuspermanentcontainer=(*rp);
    
    strawStatus = int((*strawstatuspermanentcontainer).get(id).getstatus());
  }
  
  else {
    IdentifierHash hashID = m_trtId->straw_hash(offlineID);
    if (p_statPermData){
      strawStatus = p_statPermData->findStatus(hashID);
    }
    else {
      ATH_MSG_ERROR("Cannot access StrawStatPerm calibration data");
    }
  }
  return strawStatus;
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
  const int status=getStatus(offlineID,ctx);
  const int statusperm=getStatusPermanent(offlineID,ctx);
  ATH_MSG_VERBOSE("offlineID "<<offlineID<<" "<<status <<" "<< statusperm);
  
  bool st = false, stperm=false;
  
  if (status==1) st = true;
  else if (status==0) st = false;
  else {st = bool( (status & statusbitmask) >> 8);};

  if (statusperm==1) stperm = true;
  else if (statusperm==0) stperm = false;
  else {stperm = bool( (statusperm & statusbitmask) >> 8);};

  return (st||stperm);
  
}



bool TRT_StrawStatusSummaryTool::get_statusHT(Identifier offlineID, const EventContext& ctx) const{
  constexpr unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  bool stHT=false;
  const int statusHT=getStatusHT(offlineID,ctx);

  if (statusHT==1) stHT = true;
  else if (statusHT==0) stHT = false;
  else {stHT = bool( (statusHT & statusbitmask) >> 8);};

  return ( stHT );

}

