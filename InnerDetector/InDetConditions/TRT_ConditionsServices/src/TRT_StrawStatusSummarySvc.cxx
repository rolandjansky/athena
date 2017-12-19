/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_StrawStatusSummarySvc.cxx
 *  @brief AthService to manage TRT StrawStatus, LT/HT occupancy, PID and track data
 *  @author Esben Klinkby <klinkby@nbi.dk>
 **/

#include "TRT_StrawStatusSummarySvc.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdio.h>

#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "Identifier/Identifier.h"

#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"
#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include "PathResolver/PathResolver.h"


#include "InDetIdentifier/TRT_ID.h"

#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

class TRT_ID;



TRT_StrawStatusSummarySvc::TRT_StrawStatusSummarySvc( const std::string& name,
						      ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator),
    m_detStore("DetectorStore",name),
    m_toolsvc("ToolSvc",name),
    m_par_strawstatuscontainerkey("/TRT/Cond/Status"),
    m_par_strawstatuspermanentcontainerkey("/TRT/Cond/StatusPermanent"),
    m_par_strawstatusHTcontainerkey("/TRT/Cond/StatusHT"),
    m_par_stattextfile(""), // ("dummyNonExisting_TRT_StrawStatus_InputFile_TRT_StrawStatusSummarySvc"), 
    m_par_stattextfilepermanent(""), 
    m_par_stattextfileHT(""),
    m_par_statstream("StatStream1"),
    m_trtid(0),
    m_trtDetMgr(0),
    m_strawstatuscontainerexists(false),
    m_strawstatuspermanentcontainerexists(false),
    m_strawstatusHTcontainerexists(false)
{
  declareProperty("statusTextFile",m_par_stattextfile);
  declareProperty("statusTextFilePermanent",m_par_stattextfilepermanent);
  declareProperty("statusTextFileHT",m_par_stattextfileHT);
  declareProperty("ToolSvc",m_toolsvc);

    // create arrays for alive straws
  m_stw_total = new int[7];
  m_stw_local = new int*[6];
  m_stw_wheel = new int*[34];
  for (int i=0; i<6 ; ++i) m_stw_local[i] = new int[32];
  for (int i=0; i<34; ++i) m_stw_wheel[i] = new int[32];
                          
  resetArrays();//RM moved to c'tor from initialize() to avoid coverity defects
}
//  declareProperty("statusTextFile",m_par_stattextfile);


TRT_StrawStatusSummarySvc::~TRT_StrawStatusSummarySvc()
{
}

StatusCode TRT_StrawStatusSummarySvc::initialize() 
{
  msg(MSG::INFO) << "TRT_StrawStatusSummarySvc initialize method called" << endmsg;

  //  if (m_par_stattextfilepermanent=="") m_par_stattextfilepermanent=PathResolver::find_file ("strawstatuspermanent-01.txt", "DATAPATH");

  // Retrieve the DetectorStore
  if (StatusCode::SUCCESS!=m_detStore.retrieve()) {
    ATH_MSG_FATAL("Could not retrieve " << m_detStore);
    return StatusCode::FAILURE;
  }

  // Find ToolService
  if (StatusCode::SUCCESS!=m_toolsvc.retrieve()) {
    msg(MSG::FATAL) << "ToolSvc not found" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get the TRT ID helper
  if (StatusCode::SUCCESS!=m_detStore->retrieve(m_trtid,"TRT_ID")) {
    msg(MSG::FATAL) << "Problem retrieving TRTID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "TRT_StrawStatusSummarySvc::initialize " << m_par_stattextfile  << endmsg;
  
  if(msgLvl(MSG::DEBUG)) msg() <<"trying to read "<<m_par_strawstatuscontainerkey<< " from StoreGateSvc"<<endmsg;
  m_strawstatuscontainerexists = m_detStore->StoreGateSvc::contains<TRTCond::StrawStatusMultChanContainer>(m_par_strawstatuscontainerkey) ;

  if( m_strawstatuscontainerexists ) {
     if(msgLvl(MSG::DEBUG)) msg() << " strawstatuscontainerexists " <<endmsg; 
     if( (m_detStore->regFcn(&TRT_StrawStatusSummarySvc::IOVCallBack,this,m_strawstatuscontainer,m_par_strawstatuscontainerkey)).isFailure()) 
        msg(MSG::ERROR) << "Could not register IOV callback for key: " << m_par_strawstatuscontainerkey << endmsg ;
  } else if (!m_par_stattextfile.empty()) {
     // create, record and update data handle
     if(msgLvl(MSG::DEBUG)) msg() << "Creating new strawstatus container" << endmsg ;
     const TRTCond::StrawStatusMultChanContainer* strawstatuscontainer = new TRTCond::StrawStatusMultChanContainer() ; 

     if( (m_detStore->record(strawstatuscontainer,m_par_strawstatuscontainerkey))!=StatusCode::SUCCESS ) {
        msg(MSG::ERROR) << "Could not record StrawStatusContainer for key " << m_par_strawstatuscontainerkey << endmsg;
     }
     else{ 
        msg( MSG::VERBOSE) << "Got record StrawStatusContainer for key " << m_par_strawstatuscontainerkey << endmsg; 
        m_strawstatuscontainerexists = true;
     }
     if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatuscontainer,m_par_strawstatuscontainerkey)) {
        msg(MSG::FATAL) << "Could not retrieve data handle for StrawStatusContainer " << endmsg;
        return StatusCode::FAILURE ;
     }else{if(msgLvl(MSG::VERBOSE)) msg() << "Retrieved data handle for StrawStatusContainer " << endmsg;}
    if(!m_par_stattextfile.empty()) return readFromTextFile( m_par_stattextfile ) ;
  }

  if(msgLvl(MSG::DEBUG)) msg() <<"trying to read "<<m_par_strawstatuspermanentcontainerkey<< " from StoreGateSvc"<<endmsg;
  m_strawstatuspermanentcontainerexists = m_detStore->StoreGateSvc::contains<TRTCond::StrawStatusMultChanContainer>(m_par_strawstatuspermanentcontainerkey) ;

  if( m_strawstatuspermanentcontainerexists ) {
     if(msgLvl(MSG::DEBUG)) msg() << " strawstatuspermanentcontainerexists " <<endmsg; 
     if( (m_detStore->regFcn(&TRT_StrawStatusSummarySvc::IOVCallBack,this,m_strawstatuspermanentcontainer,m_par_strawstatuspermanentcontainerkey)).isFailure()) 
        msg(MSG::ERROR) << "Could not register IOV callback for key: " << m_par_strawstatuspermanentcontainerkey << endmsg ;
  } else if (!m_par_stattextfilepermanent.empty()) {
     // create, record and update data handle
     if(msgLvl(MSG::DEBUG)) msg() << "Creating new strawstatuspermanent container" << endmsg ;

     const TRTCond::StrawStatusMultChanContainer* strawstatuspermanentcontainer = new TRTCond::StrawStatusMultChanContainer() ; 
     if( (m_detStore->record(strawstatuspermanentcontainer,m_par_strawstatuspermanentcontainerkey))!=StatusCode::SUCCESS ) {
        msg(MSG::ERROR) << "Could not record StrawStatuspermanentContainer for key " << m_par_strawstatuspermanentcontainerkey << endmsg;
     }
     else{ 
        if(msgLvl(MSG::VERBOSE)) msg() << "Got record StrawStatuspermanentContainer for key " << m_par_strawstatuspermanentcontainerkey << endmsg; 
        m_strawstatuspermanentcontainerexists = true;
     }
     if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatuspermanentcontainer,m_par_strawstatuspermanentcontainerkey)) {
        msg(MSG::FATAL) << "Could not retrieve data handle for StrawStatuspermanentContainer " << endmsg;
        return StatusCode::FAILURE ;
     }else{if(msgLvl(MSG::VERBOSE)) msg() << "Retrieved data handle for StrawStatuspermanentContainer " << endmsg;}
     if(!m_par_stattextfilepermanent.empty()) return readFromTextFile( m_par_stattextfilepermanent ) ;
  }


  
  if(msgLvl(MSG::DEBUG)) msg() <<"trying to read "<<m_par_strawstatusHTcontainerkey<< " from StoreGateSvc"<<endmsg;
  m_strawstatusHTcontainerexists = m_detStore->StoreGateSvc::contains<TRTCond::StrawStatusMultChanContainer>(m_par_strawstatusHTcontainerkey) ;
  
  if( m_strawstatusHTcontainerexists ) {
     if(msgLvl(MSG::DEBUG)) msg() << " strawstatusHTcontainerexists " <<endmsg; 
     if( (m_detStore->regFcn(&TRT_StrawStatusSummarySvc::IOVCallBack,this,m_strawstatusHTcontainer,m_par_strawstatusHTcontainerkey)).isFailure()) 
        msg(MSG::ERROR) << "Could not register IOV callback for key: " << m_par_strawstatusHTcontainerkey << endmsg ;
  } else if (!m_par_stattextfileHT.empty()) {
     // create, record and update data handle
     if(msgLvl(MSG::DEBUG)) msg() << "Creating new strawstatusHT container" << endmsg ;
     const TRTCond::StrawStatusMultChanContainer* strawstatusHTcontainer = new TRTCond::StrawStatusMultChanContainer() ; 
     if( (m_detStore->record(strawstatusHTcontainer,m_par_strawstatusHTcontainerkey))!=StatusCode::SUCCESS ) {
        msg(MSG::ERROR) << "Could not record StrawStatusHTContainer for key " << m_par_strawstatusHTcontainerkey << endmsg;
     }
     else{ 
        if(msgLvl(MSG::VERBOSE)) msg() << "Got record StrawStatusHTContainer for key " << m_par_strawstatusHTcontainerkey << endmsg; 
        m_strawstatusHTcontainerexists = true;
     }
     if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatusHTcontainer,m_par_strawstatusHTcontainerkey)) {
        msg(MSG::FATAL) << "Could not retrieve data handle for StrawStatusHTContainer " << endmsg;
        return StatusCode::FAILURE ;
     }else{if(msgLvl(MSG::VERBOSE)) msg() << "Retrieved data handle for StrawStatusHTContainer " << endmsg;}
     if(!m_par_stattextfileHT.empty()) return readFromTextFile( m_par_stattextfileHT ) ;
  }


  double lt_interval_small = 0.05/64;
  double lt_interval_large = 0.95/64;
  double ht_interval = 1./64;

  for (unsigned int i = 0; i<128; i++ ) {
    if (i<64) {
      m_ltvaluevector.push_back(lt_interval_small*i+lt_interval_small/2.);
      if(msgLvl(MSG::VERBOSE)) msg() <<i<<" "<<lt_interval_small*i+lt_interval_small/2.<<endmsg;
    }
    else if (i<128) {
      m_ltvaluevector.push_back(0.05+lt_interval_large*(i-64)+lt_interval_large/2.);
      if(msgLvl(MSG::VERBOSE)) msg() <<i<<" "<<0.05+lt_interval_large*(i-64)+lt_interval_large/2.<<endmsg;
    }
  }
  for (unsigned int i = 0; i<64; i++ ) {
    m_htvaluevector.push_back(ht_interval*(i+0.5));
    if(msgLvl(MSG::VERBOSE)) msg() <<i<<" "<<ht_interval*(i+0.5)<<endmsg;
  }


  msg(MSG::INFO) << "TRT_StrawStatusSummarySvc initialized successfully  " << endmsg;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////


StatusCode TRT_StrawStatusSummarySvc::finalize()
{
  for (int i=0; i<6 ; ++i) delete[] m_stw_local[i]; 
  for (int i=0; i<34; ++i) delete[] m_stw_wheel[i];
  delete [] m_stw_total;
  delete [] m_stw_local;
  delete [] m_stw_wheel;

  msg(MSG::INFO) << " in finalize() " << endmsg;
  return StatusCode::SUCCESS;
}


int TRT_StrawStatusSummarySvc::getStatus(Identifier offlineID){
  int stat = 0;
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								       m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								       m_trtid->straw(offlineID),level );
  StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();
  static int countStrawStatusContainerError(0);
  if (!strawstatuscontainer) {
    if (countStrawStatusContainerError<5) { 
      msg(MSG::WARNING) << "TRT_StrawStatusSummarySvc::getStatus, strawstatuscontainer == 0, dead straws not set" << endmsg; 
      countStrawStatusContainerError++; 
    }
    return 0;
  }
  stat=int((*strawstatuscontainer).get(id).getstatus());
  return stat;
}



int TRT_StrawStatusSummarySvc::getStatusPermanent(Identifier offlineID){
  int stat = 0;
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								       m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								       m_trtid->straw(offlineID),level );
  StrawStatusContainer* strawstatuspermanentcontainer = getStrawStatusPermanentContainer();
  static int countStrawStatusContainerError(0);
  if (!strawstatuspermanentcontainer) {
    if (countStrawStatusContainerError<5) {       msg(MSG::WARNING) << "TRT_StrawStatusSummarySvc::getStatusPermanent, strawstatuspermanentcontainer == 0, dead straws not set" << endmsg; 
      countStrawStatusContainerError++; 
    }
    return 0;
  }
  stat=int((*strawstatuspermanentcontainer).get(id).getstatus());
  return stat;
}



int TRT_StrawStatusSummarySvc::getStatusHT(Identifier offlineID){
  int stat = 0;
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  StrawStatusContainer* strawstatusHTcontainer = getStrawStatusHTContainer();

 static int countStrawStatusContainerError(0);
 if (!strawstatusHTcontainer) {
   if (countStrawStatusContainerError<5) { 
     msg(MSG::WARNING) << "TRT_StrawStatusSummarySvc::getStatusHT, strawstatusHTcontainer == 0, dead straws not set" << endmsg; 
     countStrawStatusContainerError++; 
   }
    return 0;
 }
 stat=int((*strawstatusHTcontainer).get(id).getstatus());
 return stat;
}


///////////////////////////////////////////////////

void  TRT_StrawStatusSummarySvc::set_LT_occ(Identifier offlineID, double lt_occ){
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  
  unsigned int org_stat = getStatus(offlineID);
  unsigned int status = 0;

  if (lt_occ<=0.05) {
    status= static_cast<unsigned int>(64*(lt_occ/0.05));
  }
  else if (lt_occ>0.05) {
    status = static_cast<unsigned int>(64+64*(lt_occ/0.95) ) ;
  }


  status = status << 9;

  org_stat = org_stat << 7;  //removed lt if present
  org_stat = org_stat >> 7; 

  //carefull not to change information which is already set 
    status = status | org_stat ; 


  StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();
  ((*strawstatuscontainer).set(id,status));
}

///////////////////////////////////////////////////

double TRT_StrawStatusSummarySvc::get_LT_occ(Identifier offlineID){
  const unsigned int ltbitmask = (0xFE << 8) ;

  return ((m_ltvaluevector[( (ltbitmask & getStatus(offlineID)) >> 9 )-4] <1.0 && m_ltvaluevector[(  (ltbitmask & getStatus(offlineID)) >> 9)-4] >0.0000001) ?
	  m_ltvaluevector[( (ltbitmask & getStatus(offlineID)) >> 9) -4 ] :
	  0.0);
}

///////////////////////////////////////////////////

void  TRT_StrawStatusSummarySvc::set_HT_occ(Identifier offlineID, double ht_occ){
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  
  unsigned int org_stat = getStatus(offlineID);
  unsigned int status = 0;

  //ht:[0:64]
  status= static_cast<unsigned int>(64*ht_occ);

  //shift to first byte 
  // status = status << 8 ;

  const unsigned int htbitmask = 0  ;  

  //carefull not to change information which is already set 
  status = status | (org_stat & (~htbitmask&(~(0x31) ))) ;
  
  StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();
  ((*strawstatuscontainer).set(id,status));
}

///////////////////////////////////////////////////

double TRT_StrawStatusSummarySvc::get_HT_occ(Identifier offlineID){
  const unsigned int htbitmask = 0x3F;
  return m_htvaluevector[(htbitmask & ( getStatus(offlineID) ) )];
}

///////////////////////////////////////////////////

////////////////////////////////
//set special bits
void  TRT_StrawStatusSummarySvc::set_status_temp(Identifier offlineID, bool set){
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  unsigned int org_stat = getStatus(offlineID);
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  /*
  unsigned int status = 0;
  if (set) { 
    status= (org_stat | statusbitmask); 
  } 
  else if ((!set)&&(get_status(offlineID))) {
    status = org_stat - 256;
  }
  m_data.set(id,status);
  */
  StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();

  if (set) {   (*strawstatuscontainer).set(id, (org_stat|statusbitmask)); }else{    (*strawstatuscontainer).set(id, (org_stat & (~statusbitmask) )); }
}



void  TRT_StrawStatusSummarySvc::set_status_permanent(Identifier offlineID, bool set){
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  unsigned int org_stat = getStatusPermanent(offlineID);
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  StrawStatusContainer* strawstatuspermanentcontainer = getStrawStatusPermanentContainer();

  if (set) {   (*strawstatuspermanentcontainer).set(id, (org_stat|statusbitmask)); }else{    (*strawstatuspermanentcontainer).set(id, (org_stat & (~statusbitmask) )); }
}




bool TRT_StrawStatusSummarySvc::get_status(Identifier offlineID){
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  if(msgLvl(MSG::VERBOSE)) msg() <<"offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & statusbitmask) >> 8)<<endmsg;
  bool st = false, stperm=false;
  if (getStatus(offlineID)==1) st = true;
  else if (getStatus(offlineID)==0) st = false;
  else {st = bool( (getStatus(offlineID) & statusbitmask) >> 8);};

  // Safety check: Won't do any good unless a somewhere a StatusPermanent tag is defined.


  if (m_strawstatuspermanentcontainerexists) {
    if (getStatusPermanent(offlineID)==1) stperm = true;
    else if (getStatusPermanent(offlineID)==0) stperm = false;
    else {stperm = bool( (getStatusPermanent(offlineID) & statusbitmask) >> 8);};
  }

  return ( (st||stperm) );
  
}



bool TRT_StrawStatusSummarySvc::get_statusHT(Identifier offlineID){
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  if(msgLvl(MSG::VERBOSE)) msg() <<"offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & statusbitmask) >> 8)<<endmsg;
  bool stHT=false;

  // Safety check: Won't do any good unless a somewhere a StatusHT

  
  if (m_strawstatusHTcontainerexists) {
    if (getStatusHT(offlineID)==1) stHT = true;
    else if (getStatusHT(offlineID)==0) stHT = false;
    else {stHT = bool( (getStatusHT(offlineID) & statusbitmask) >> 8);};
  }
  else { if(msgLvl(MSG::WARNING)) msg() <<"Attempt to use get_statusHT flag, but CondDB folder not available. Returning StrawStatus::Good "<<endmsg; }

  return ( stHT );

}





////////////////////////////////////
void  TRT_StrawStatusSummarySvc::set_pid_status(Identifier offlineID, bool set){
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  unsigned int org_stat = getStatus(offlineID);
  const unsigned int pidstatusbitmask = 0x80; //0000000 10000000
   StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();


  if (set) {   (*strawstatuscontainer).set(id, (org_stat|pidstatusbitmask));  }else{    (*strawstatuscontainer).set(id, (org_stat & (~pidstatusbitmask) )); } 
}

bool TRT_StrawStatusSummarySvc::get_pid_status(Identifier offlineID){
  const unsigned int pidstatusbitmask = 0x80; //0000000 10000000
  return  bool( (getStatus(offlineID) & pidstatusbitmask) >> 7);
}

///////////////////////////////////////////////
void  TRT_StrawStatusSummarySvc::set_track_status(Identifier offlineID, bool set){
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  unsigned int org_stat = getStatus(offlineID);
  const unsigned int trackstatusbitmask = 0x40; //0000000 01000000
  StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();


  if (set) {   (*strawstatuscontainer).set(id, (org_stat|trackstatusbitmask));  }else{   (*strawstatuscontainer).set(id, (org_stat & (~trackstatusbitmask) )); } 
}

bool TRT_StrawStatusSummarySvc::get_track_status(Identifier offlineID){
  const unsigned int trackstatusbitmask = 0x40; //0000000 01000000
  return  bool( (getStatus(offlineID) & trackstatusbitmask) >> 6);
}










///////////////////////////////////////////////////


StatusCode TRT_StrawStatusSummarySvc::readFromTextFile(const std::string& filename)
{

/*

TRT_StrawStatusSummarySvc                  INFO  IOVCALLBACK for key /TRT/Cond/Status number 2
TRT_StrawStatusSummarySvc                  INFO  IOVCALLBACK for key /TRT/Cond/StatusPermanent number 2
TRT_StrawStatusSummarySvc                  INFO TRT_StrawStatusSummarySvc::readFromTextFile dummyNonExisting_TRT_StrawStatus_InputFile_TRT_StrawStatusSummarySvc
TRT_StrawStatusSummarySvc                 DEBUG Permanently dead straws are removed based on the txt file: 
TRT_StrawStatusSummarySvc                 ERROR Cannot initialize TRT_StrawStatusSummarySvc: Read from text file chosen, but no text file is found. To resolve: copy strawstatus.txt from TRT_ConditionsAlgs/share to your run dir
IOVSvcTool                                ERROR Problems calling TRT_StrawStatusSummarySvc[0xf2f0b80]+d1
ServiceManager                            DEBUG Stopping service TRT_StrawStatusSummarySvc
myOCA                                      INFO 0: TRTCond::StrawStatusMultChanContainer#/TRT/Cond/Status#/TRT/Cond/Status
ToolSvc.myOCATool                         DEBUG Found object for type TRTCond::StrawStatusMultChanContainer key /TRT/Cond/Status
AthenaSealSvc                             DEBUG Checking members of type NestedContainerBase<2,TRTCond::NestedContainer<3,TRTCond::StrawStatus,TRTCond::DeadStrawTrait>,TRTCond::StrawStatus,TRTCond::DeadStrawTrait> for 2 members: ok  
*/
  msg(MSG::INFO) << "TRT_StrawStatusSummarySvc::readFromTextFile " << filename << endmsg;
//  m_par_stattextfile = filename;
  if(!m_par_stattextfile.empty()) {
    StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();
    strawstatuscontainer->clear();
  
    const InDetDD::TRT_DetectorManager* TRTDetectorManager ;
    if ((m_detStore->retrieve(TRTDetectorManager)).isFailure()) {
      msg(MSG::FATAL) << "Problem retrieving TRT_DetectorManager" << endmsg;
    }

    // initialize strawlayers with status 'good'
    for( InDetDD::TRT_DetElementCollection::const_iterator it =  TRTDetectorManager->getDetectorElementBegin() ;
	 it != TRTDetectorManager->getDetectorElementEnd() ; ++it) {
      Identifier ident = (*it)->identify() ;
      TRTCond::ExpandedIdentifier id( m_trtid->barrel_ec(ident),
				      m_trtid->layer_or_wheel(ident),
				      0,
				      m_trtid->straw_layer(ident),
				      m_trtid->straw(ident),
				      TRTCond::ExpandedIdentifier::LAYERWHEEL ) ;
      strawstatuscontainer->setStatus(id,TRTCond::StrawStatus::Good );
    }
    
    
   // std::ifstream ifs(filename.c_str()) ;
    std::ifstream ifs(m_par_stattextfile.c_str()) ;
    if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Permanently dead straws are removed based on the txt file: "<<m_par_stattextfilepermanent<< endmsg;
    

    if (!ifs)  { 
    msg(MSG::ERROR) << "Cannot initialize TRT_StrawStatusSummarySvc: Read from text file chosen, but no text file is found. To resolve: copy strawstatus.txt from TRT_ConditionsAlgs/share to your run dir" << endmsg;
    return StatusCode::FAILURE;
    }




    if (ifs) {

      int bec, layer, sector, strawlayer, straw, status ;
      //txt file format : bec sector straw strawlayer layer dead/alive
      int line =0 ;
      while( ifs >> bec >> sector>> straw >> strawlayer >> layer >> status ) {
	if ( (status == 3) || (status == 4)  || (status == 5) ) status = 1;
	if (status!=1){
		msg(MSG::FATAL) << " The Status Temp : " << status << " IS NOT ACCEPTED, Use 1 for dead " << endmsg;
		return StatusCode::FAILURE;	
	}	
	int level = TRTCond::ExpandedIdentifier::STRAW ;
	line +=1;
	//    std::cout<<"read "<< bec<<  " "<<layer<<" "<<sector<<" "<<strawlayer<<" "<<straw<<" "<<level<<" "<<std::endl;
	if( straw<0      ) { level = TRTCond::ExpandedIdentifier::STRAWLAYER ; straw = 0 ; }
	if( strawlayer<0 ) { level = TRTCond::ExpandedIdentifier::MODULE ;     strawlayer = 0 ; }
	if( sector<0     ) { level = TRTCond::ExpandedIdentifier::LAYERWHEEL ; sector = 0 ; }
	if( layer<0      ) { level = TRTCond::ExpandedIdentifier::BARRELEC ;   layer = 0 ; }
	if( bec<-2       ) { level = TRTCond::ExpandedIdentifier::DETECTOR ;   bec = -2 ; }
              msg(MSG::INFO) << "StrawRead FromText File: bec: "  << bec << " layer: " << layer << " phi: " << sector << " stl:  " << strawlayer << " stw: " << straw   << endmsg;
	TRTCond::ExpandedIdentifier id( bec,layer,sector,strawlayer,straw,level);
	strawstatuscontainer->set(id,TRTCond::StrawStatus::Dead) ;
	strawstatuscontainer->setStatus(id,TRTCond::StrawStatus::Dead );


	
	Identifier ID = m_trtid->straw_id(bec,sector,layer,strawlayer,straw);
	
	set_status_temp(ID,status==1?true:false);

      }
     msg(MSG::INFO) << "Straw Status read from: "  <<  filename << " We read: " << line << " straws on it"  << endmsg;

    } else {

      msg(MSG::ERROR) << "TRT_StrawStatusSummarySvc::readFromTextFile: can not read the file, file missing: " << filename << endmsg;
    }
    
  }
  //HT

  std::ifstream ifsHT(m_par_stattextfileHT.c_str()) ;
  if(!m_par_stattextfileHT.empty()) {
    StrawStatusContainer* strawstatuscontainer = getStrawStatusHTContainer();
    strawstatuscontainer->clear();
    const InDetDD::TRT_DetectorManager* TRTDetectorManager ;
    if ((m_detStore->retrieve(TRTDetectorManager)).isFailure()) {
      msg(MSG::FATAL) << "Problem retrieving TRT_DetectorManager" << endmsg;
    }
    // initialize strawlayers with status 'good'
    for( InDetDD::TRT_DetElementCollection::const_iterator it =  TRTDetectorManager->getDetectorElementBegin() ;
         it != TRTDetectorManager->getDetectorElementEnd() ; ++it) {
      Identifier ident = (*it)->identify() ;
      TRTCond::ExpandedIdentifier id( m_trtid->barrel_ec(ident),
                                      m_trtid->layer_or_wheel(ident),
                                      0,
                                      m_trtid->straw_layer(ident),
                                      m_trtid->straw(ident),
                                      TRTCond::ExpandedIdentifier::LAYERWHEEL ) ;
      strawstatuscontainer->setStatus(id,TRTCond::StrawStatus::Good );
    }
    std::ifstream ifsHT(m_par_stattextfileHT.c_str()) ;
    if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "HT dead straws are removed based on the txt file: "<<m_par_stattextfileHT<< endmsg;

    if (!ifsHT) {
      msg(MSG::ERROR) << "Cannot initialize TRT_StrawStatusSummarySvc: Read from text file chosen, but no text file is found. To resolve: copy strawstatus.txt from TRT_ConditionsAlgs/share to your run dir" << endmsg;
      return StatusCode::FAILURE;
    }
    

    
    
    if (ifsHT) {

      int bec, layer, sector, strawlayer, straw, status ;
      //txt file format : bec sector straw strawlayer layer dead/alive
      int line   = 0; 
      int lineAr = 0; 
      int lineKr = 0; 
      int lineEAr = 0; 
      int lineEKr = 0; 

      while( ifsHT >> bec >> sector>> straw >> strawlayer >> layer >> status ) {
        int level = TRTCond::ExpandedIdentifier::STRAW ;
        //    std::cout<<"read "<< bec<<  " "<<layer<<" "<<sector<<" "<<strawlayer<<" "<<straw<<" "<<level<<" "<<std::endl;
	line +=1;
        if( straw<0      ) { level = TRTCond::ExpandedIdentifier::STRAWLAYER ; straw = 0 ; }
        if( strawlayer<0 ) { level = TRTCond::ExpandedIdentifier::MODULE ;     strawlayer = 0 ; }
        if( sector<0     ) { level = TRTCond::ExpandedIdentifier::LAYERWHEEL ; sector = 0 ; }
        if( layer<0      ) { level = TRTCond::ExpandedIdentifier::BARRELEC ;   layer = 0 ; }
        if( bec<-2       ) { level = TRTCond::ExpandedIdentifier::DETECTOR ;   bec = -2 ; }

	msg(MSG::DEBUG) << "HT StrawRead FromText File: bec: "  << bec << " layer: " << layer << " phi: " << sector << " stl:  " << strawlayer << " stw: " << straw  << " status: "<< status << endmsg;
	TRTCond::ExpandedIdentifier id( bec,layer,sector,strawlayer,straw,level);
	if (status == 4){ // Argon
                strawstatuscontainer->set(id,TRTCond::StrawStatus::Argon) ;
                strawstatuscontainer->setStatus(id,TRTCond::StrawStatus::Argon);
		lineAr+=1;
       	}
	else if (status == 5){ // Krypton 
                strawstatuscontainer->set(id,TRTCond::StrawStatus::Krypton) ;
                strawstatuscontainer->setStatus(id,TRTCond::StrawStatus::Krypton);
		lineKr+=1;
       	}
	else if (status == 6){ // EmulateArgon 
                strawstatuscontainer->set(id,TRTCond::StrawStatus::EmulateArgon) ;
                strawstatuscontainer->setStatus(id,TRTCond::StrawStatus::EmulateArgon);
		lineEAr+=1;
       	}
	else if (status == 7){ // EmulateKrypton 
                strawstatuscontainer->set(id,TRTCond::StrawStatus::EmulateKrypton) ;
                strawstatuscontainer->setStatus(id,TRTCond::StrawStatus::EmulateKrypton);
		lineEKr+=1;
       	}
        else{
		msg(MSG::FATAL) << " The HT Status: " << status << " IS NOT ACCEPTED, 4 for Argon, 5 for Krypton!! " 
				<< " Or 6 or 7 for emulated types!" << endmsg;
		return StatusCode::FAILURE;	
	}
	
      }
      msg(MSG::INFO) << "HT Straw Status read from: "  << filename << " We read: " << line << " straws on it. Argon straws:  "<< lineAr << "  Krypton straws " << lineKr  << " EmulateArgon " << lineEAr << " EmulateKrypton " << lineEKr << endmsg;
    }
    

  }
  





  //Permanent


  std::ifstream ifspermanent(m_par_stattextfilepermanent.c_str()) ;
  
  if(!m_par_stattextfilepermanent.empty()) {
    
    if (ifspermanent) {


      StrawStatusContainer* strawstatuspermanentcontainer = getStrawStatusPermanentContainer();
      strawstatuspermanentcontainer->clear();


      int bec, layer, sector, strawlayer, straw, status;
      //int  pid_status,  track_status;
      //double lt_occ, ht_occ;
      //txt file format : bec sector straw strawlayer layer dead/alive
	int line = 0;
      while( ifspermanent >> bec >> sector>> straw >> strawlayer >> layer >> status ) {
        if ( (status == 3) || (status == 4)  || (status == 5) ) status = 1;
	if (status!=1){
		msg(MSG::FATAL) << " The Status Permanent: " << status << " IS NOT ACCEPTED, Use 1 for dead " << endmsg;
		return StatusCode::FAILURE;	
	}	
	int level = TRTCond::ExpandedIdentifier::STRAW ;
	//    std::cout<<"read "<< bec<<  " "<<layer<<" "<<sector<<" "<<strawlayer<<" "<<straw<<" "<<level<<" "<<std::endl;
	line +=1;
	if( straw<0      ) { level = TRTCond::ExpandedIdentifier::STRAWLAYER ; straw = 0 ; }
	if( strawlayer<0 ) { level = TRTCond::ExpandedIdentifier::MODULE ;     strawlayer = 0 ; }
	if( sector<0     ) { level = TRTCond::ExpandedIdentifier::LAYERWHEEL ; sector = 0 ; }
	if( layer<0      ) { level = TRTCond::ExpandedIdentifier::BARRELEC ;   layer = 0 ; }
	if( bec<-2       ) { level = TRTCond::ExpandedIdentifier::DETECTOR ;   bec = -2 ; }
        msg(MSG::DEBUG) << "Permanent StrawRead FromText File: bec: "  << bec << " layer: " << layer << " phi: " << sector << " stl:  " << strawlayer << " stw: " << straw   << endmsg;

	TRTCond::ExpandedIdentifier id( bec,layer,sector,strawlayer,straw,level);
	strawstatuspermanentcontainer->set(id,TRTCond::StrawStatus::Dead) ;
	strawstatuspermanentcontainer->setStatus(id,TRTCond::StrawStatus::Dead );
	
	Identifier ID = m_trtid->straw_id(bec,sector,layer,strawlayer,straw);
	
	set_status_permanent(ID,status==1?true:false);

      }
     msg(MSG::INFO) << "Permanent Straw Status read from: "  << filename << " We read: " << line << " straws on it"  << endmsg;

    }
    
  }
    return StatusCode::SUCCESS ;
  }
  
///////////////////////////////////////////////////








///////////////////////////////////////////////////


StatusCode TRT_StrawStatusSummarySvc::writeToTextFile(const std::string& filename ) const
{
  std::ofstream outfile(filename.c_str());


   msg(MSG::INFO) << " Dump To File: StrawStatus "  << endmsg;
if(m_strawstatuscontainer->numObjects()>0){
    outfile << " Status: " << std::endl;
  TRTCond::StrawStatusLayerContainer::FlatContainer flatcontainer;
  m_strawstatuscontainer                ->getall(flatcontainer) ;
  for( TRTCond::StrawStatusContainer::FlatContainer::const_iterator
         it = flatcontainer.begin() ; it != flatcontainer.end() ; ++it) {
    TRTCond::ExpandedIdentifier id = it->first ;
    const TRTCond::StrawStatus* status = it->second ;
    outfile << id << " " << int(status->getstatus()) << std::endl ;
  }
}


    msg(MSG::INFO) << " Dump To File: StrawStatus HT "  << endmsg;
if( m_strawstatusHTcontainer->numObjects()>0){
  TRTCond::StrawStatusLayerContainer::FlatContainer flatcontainerHT;
  m_strawstatusHTcontainer              ->getall(flatcontainerHT) ;


    outfile << " HT Status: " << std::endl;
  for( TRTCond::StrawStatusContainer::FlatContainer::const_iterator
         it = flatcontainerHT.begin() ; it != flatcontainerHT.end() ; ++it) {
    TRTCond::ExpandedIdentifier id = it->first ;
    const TRTCond::StrawStatus* status = it->second ;
    outfile << id << " " << int(status->getstatus()) << std::endl ;
  }
}


if(m_strawstatuspermanentcontainer->numObjects()>0){
    msg(MSG::INFO) << " Dump To File: StrawStatus permanent "  << endmsg;
  TRTCond::StrawStatusLayerContainer::FlatContainer flatcontainerpermanent;
  m_strawstatuspermanentcontainer               ->getall(flatcontainerpermanent) ;


    outfile << " permanent Status: " << std::endl;
  for( TRTCond::StrawStatusContainer::FlatContainer::const_iterator
         it = flatcontainerpermanent.begin() ; it != flatcontainerpermanent.end() ; ++it) {
    TRTCond::ExpandedIdentifier id = it->first ;
    const TRTCond::StrawStatus* status = it->second ;
    outfile << id << " " << int(status->getstatus()) << std::endl ;
  }
}
  return StatusCode::SUCCESS ;



}







///////////////////////////////////////////////////
//
// Helpers for the alive straws tool Need to consistent with TRT_ElectronPIDTool
//
///////////////////////////////////////////////////

void TRT_StrawStatusSummarySvc::resetArrays(){
    for (int i=0; i<7; ++i){
      m_stw_total[i]=0;
    }
    for (int i=0; i<6; ++i){
      for (int j=0; j<32; ++j){
        m_stw_local[i][j]=0;
      }
    }
    for (int i=0; i<34; ++i){
      for (int j=0; j<32; ++j){
        m_stw_wheel[i][j]=0;
      }
    }
  return;
}



  int TRT_StrawStatusSummarySvc::findArrayTotalIndex(const int det, const int lay){
    int arrayindex = 0; // to be reset below
    // NOTE: Below, arrayindex starts at 1
    // because index 0 is filled with TOTAL value.
    if      (det == -1) arrayindex = 1; // barrel side C
    else if (det == -2) {               // endcap side C
      if (lay < 6)      arrayindex = 2; //   wheel A
      else              arrayindex = 3; //   wheel B
    }
    else if (det ==  1) arrayindex = 4; // barrel side A
    else if (det ==  2) {               // endcap side A
      if (lay < 6)      arrayindex = 5; //   wheel A
      else              arrayindex = 6; //   wheel B
    }
    else        ATH_MSG_WARNING(" detector value is: " << det << ", out of range -2, -1, 1, 2, so THIS IS NOT TRT!!!");
    return arrayindex;
  }

  int TRT_StrawStatusSummarySvc::findArrayLocalWheelIndex(const int det, const int lay){
    int arrayindex = 9; // to be reset below
    if      (det == -1) {                // barrel side C
      if      (lay == 0) arrayindex = 0; // layer 0
      else if (lay == 1) arrayindex = 1; // layer 1
      else if (lay == 2) arrayindex = 2; // layer 2
    }
    else if (det == -2) {                // endcap side C
      for (int i=0; i<14; ++i){
        if (lay==i) arrayindex=i+3;
      }
    }
    else if (det ==  1) {                // barrel side A
      if      (lay == 0) arrayindex = 17; // layer 0
      else if (lay == 1) arrayindex = 18; // layer 1
      else if (lay == 2) arrayindex = 19; // layer 2
    }
    else if (det ==  2) {                // endcap side A
      for (int i=0; i<14; ++i){
        if (lay==i) arrayindex=i+20;
      }
    }
    else        ATH_MSG_WARNING(" detector value is: " << det << ", out of range -2, -1, 1, 2, so THIS IS NOT TRT!!!");
    return arrayindex;
  }


  int TRT_StrawStatusSummarySvc::findArrayLocalStrawIndex(const int det, const int lay, const int strawlay){
    int arrayindex = 9; // to be reset below
    if      (det == -1) {                // barrel side C
      if      (lay == 0){                // layer 0
        if (strawlay < 9) arrayindex = 0; // short guys
        else              arrayindex = 1;
      }
      else if (lay == 1)  arrayindex = 2; // layer 1
      else if (lay == 2)  arrayindex = 3; // layer 2
    }
    else if (det == -2) {                // endcap side C
      for (int i=0; i<14; ++i){
        if (lay==i) arrayindex=i+4;
      }
    }
    else if (det ==  1) {                // barrel side A
      if      (lay == 0){                 // layer 0
        if (strawlay < 9) arrayindex = 18;
        else              arrayindex = 19;
      }
      else if (lay == 1)  arrayindex = 20; // layer 1
      else if (lay == 2)  arrayindex = 21; // layer 2
    }
    else if (det ==  2) {                // endcap side A
      for (int i=0; i<14; ++i){
        if (lay==i) arrayindex=i+22;
      }
    }
    else        ATH_MSG_WARNING(" detector value is: " << det << ", out of range -2, -1, 1, 2, so THIS IS NOT TRT!!!");
    return arrayindex;
  }





///////////////////////////////////////////////////
StatusCode TRT_StrawStatusSummarySvc::ComputeAliveStraws(){
   // The TRT helper:
  bool detectorManager = true;
  if (StatusCode::SUCCESS!=m_detStore->retrieve(m_trtDetMgr,"TRT")) {
    msg(MSG::WARNING) << "Problem retrieving TRT Detector Manager. Computation will use IDhelper. CPU slower " << endmsg;
    detectorManager = false;
  }

  for (std::vector<Identifier>::const_iterator it = m_trtid->straw_layer_begin(); it != m_trtid->straw_layer_end(); it++  ) {

   unsigned int nstraws = 0;
   if (detectorManager){
     const InDetDD::TRT_BaseElement *el = m_trtDetMgr->getElement(*it);
     if( !el ) continue;
     nstraws = el->nStraws();
   }
   else{
     nstraws = m_trtid->straw_max( *it) + 1; // There is a difference of 1 between both methods....
   }
   for (unsigned int i=0; i<nstraws  ;i++) {
      Identifier id = m_trtid->straw_id( *it, i);
      int det = m_trtid->barrel_ec(         id)     ;
      int lay = m_trtid->layer_or_wheel(    id)     ;
      int phi = m_trtid->phi_module(        id)     ;

      bool status               = get_status( id );
      if ( status ) {
	ATH_MSG_VERBOSE(" The sector " << det << " " << lay << " " << phi << " has status " << status);
	continue;
      }

      int i_total = findArrayTotalIndex(det, lay);
      int i_wheel = findArrayLocalWheelIndex(det, lay);

      m_stw_total[0]                        +=1;
      m_stw_total[i_total]                  +=1;
      m_stw_local[i_total-1][phi]           +=1;
      m_stw_wheel[i_wheel][phi]             +=1;

    }//For
 }//For

 ATH_MSG_DEBUG("Active Straws: " << m_stw_total[0] << " \t" << m_stw_total[1]<< "\t" << m_stw_total[2]<< "\t" << m_stw_total[3] );	
 return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////

StatusCode TRT_StrawStatusSummarySvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  for (std::list<std::string>::const_iterator 
	 itr=keys.begin(); itr!=keys.end(); ++itr) 
    msg(MSG::INFO) << " IOVCALLBACK for key " << *itr << " number " << I << endmsg;
  
  // if constants need to be read from textfile, we sue the call back routine to refill the IOV objects
   if(!m_par_stattextfile.empty()) return readFromTextFile( m_par_stattextfile) ;

   msg(MSG::INFO) << " Compute Alive Straws " << endmsg;
   StatusCode sc = ComputeAliveStraws();
   return sc ;
}
////////////////////////////////


StatusCode TRT_StrawStatusSummarySvc::streamOutStrawStatObjects() const
{
  msg(MSG::INFO) << "entering streamOutStatObjects "  << endmsg;
  StatusCode sc;
  
  // Get Output Stream tool for writing
  IAthenaOutputStreamTool*  streamer;
  sc = m_toolsvc->retrieveTool("AthenaOutputStreamTool",m_par_statstream,streamer);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to find AthenaOutputStreamTool" << endmsg;
    return StatusCode::FAILURE;
  }
 
  sc = streamer->connectOutput();

  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not connect stream to output" <<endmsg;
    return( StatusCode::FAILURE);
  }
 
  IAthenaOutputStreamTool::TypeKeyPairs typeKeys;
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(StrawStatusContainer::classname(),m_par_strawstatuscontainerkey)) ;

  sc = streamer->streamObjects(typeKeys);
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not stream out " <<endmsg;
    return( StatusCode::FAILURE);
  }
    
  sc = streamer->commitOutput();
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not commit output stream" <<endmsg;
    return( StatusCode::FAILURE);
  }
  
  msg(MSG::INFO) << "Streamed out and committed "  << typeKeys.size() << " objects " << endmsg;
  return StatusCode::SUCCESS;
}



StatusCode TRT_StrawStatusSummarySvc::registerObjects(std::string tag, int run1, int event1, int run2, int event2) const 
{
  msg(MSG::INFO) << "registerObjects with IOV " << endmsg;
  msg(MSG::INFO) << "Run/evt1 [" << run1 << "," << event1 << "]" << endmsg;
  msg(MSG::INFO) << "Run/evt2 [" << run2 << "," << event2 << "]" << endmsg;

  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Registered StrawStatusContainer object with key " << m_par_strawstatuscontainerkey << endmsg;
  
  // get pointer to registration svc
  IIOVRegistrationSvc* regsvc;
  if (StatusCode::SUCCESS!=service("IOVRegistrationSvc",regsvc)) {
    msg(MSG::FATAL) << "IOVRegistrationSvc not found" << endmsg;
    return( StatusCode::FAILURE);
  }
  
  if (StatusCode::SUCCESS==regsvc->registerIOV(StrawStatusContainer::classname(),
					       m_par_strawstatuscontainerkey,tag,run1,run2,event1,event2))
    msg(MSG::INFO) << "Registered StrawStatusContainer object with key " << m_par_strawstatuscontainerkey << endmsg ;
  else 
    msg(MSG::ERROR) << "Could not register StrawStatusContainer object with key " << m_par_strawstatuscontainerkey << endmsg ;
  
  return( StatusCode::SUCCESS);
}


