/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_StrawStatusSummarySvc.cxx
 *  @AthService to manage TRT StrawStatus, LT/HT occupancy, PID and track data
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
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"
#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include "PathResolver/PathResolver.h"

TRT_StrawStatusSummarySvc::TRT_StrawStatusSummarySvc( const std::string& name,
						      ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator),
    m_detStore("DetectorStore",name),
    m_toolsvc("ToolSvc",name),
    par_strawstatuscontainerkey("/TRT/Cond/Status"),
    par_strawstatuspermanentcontainerkey("/TRT/Cond/StatusPermanent"),
    par_strawstatusHTcontainerkey("/TRT/Cond/StatusHT"),
    //par_stattextfile("/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/Status/AprilRepro/listHotStraws.collisions2010.athenaFormat.txt"),
    par_stattextfile(""), // ("dummyNonExisting_TRT_StrawStatus_InputFile_TRT_StrawStatusSummarySvc"), 
// par_stattextfile("/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/Status/2010_09_10_sasa/listNoisyStraws.0162690.athenaFormat.txt"),
  //  par_stattextfile("/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/Status/TrtStrawStatusTemporaryEmpty-BLK-UPD4-00-00/listNoisyStraws.0155678.athenaFormat.txt"),
    par_stattextfilepermanent(""), 
    par_stattextfileHT(""),
    par_statstream("StatStream1"),
    m_trtid(0),
    strawstatuscontainerexists(false),
    strawstatuspermanentcontainerexists(false),
    strawstatusHTcontainerexists(false)
{
  declareProperty("statusTextFile",par_stattextfile);
  declareProperty("statusTextFilePermanent",par_stattextfilepermanent);
  declareProperty("statusTextFileHT",par_stattextfileHT);
  declareProperty("ToolSvc",m_toolsvc);
}
//  declareProperty("statusTextFile",par_stattextfile);


TRT_StrawStatusSummarySvc::~TRT_StrawStatusSummarySvc()
{
}


StatusCode TRT_StrawStatusSummarySvc::initialize() 
{
  msg(MSG::INFO) << "TRT_StrawStatusSummarySvc initialize method called" << endreq;

  //  if (par_stattextfilepermanent=="") par_stattextfilepermanent=PathResolver::find_file ("strawstatuspermanent-01.txt", "DATAPATH");
  

  // Retrieve the DetectorStore
  if (StatusCode::SUCCESS!=m_detStore.retrieve()) {
    ATH_MSG_FATAL("Could not retrieve " << m_detStore);
    return StatusCode::FAILURE;
  }

  // Find ToolService
  if (StatusCode::SUCCESS!=m_toolsvc.retrieve()) {
    msg(MSG::FATAL) << "ToolSvc not found" << endreq;
    return StatusCode::FAILURE;
  }

  // Get the TRT ID helper
  if (StatusCode::SUCCESS!=m_detStore->retrieve(m_trtid,"TRT_ID")) {
    msg(MSG::FATAL) << "Problem retrieving TRTID helper" << endreq;
    return StatusCode::FAILURE;
  }


//  msg(MSG::INFO) <<  "/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/Status/TrtStrawStatusTemporaryEmpty-BLK-UPD4-00-00/listNoisyStraws.0155678.athenaFormat.txt"  << endreq;
	  
  //  par_stattextfile("/afs/cern.ch/user/a/attrtcal/TRT_Calibration/uploadedDB/Status/2010_06_30/listNoisyStraws.0158269.athenaFormat.txt");
  
  msg(MSG::INFO) << "TRT_StrawStatusSummarySvc::initialize " << par_stattextfile  << endreq;

  
  if(msgLvl(MSG::DEBUG)) msg() <<"trying to read "<<par_strawstatuscontainerkey<< " from StoreGateSvc"<<endreq;
  strawstatuscontainerexists = m_detStore->StoreGateSvc::contains<TRTCond::StrawStatusMultChanContainer>(par_strawstatuscontainerkey) ;

  if( strawstatuscontainerexists ) {
     if(msgLvl(MSG::DEBUG)) msg() << " strawstatuscontainerexists " <<endreq; 
     if( (m_detStore->regFcn(&TRT_StrawStatusSummarySvc::IOVCallBack,this,m_strawstatuscontainer,par_strawstatuscontainerkey)).isFailure()) 
        msg(MSG::ERROR) << "Could not register IOV callback for key: " << par_strawstatuscontainerkey << endreq ;
  } else if (!par_stattextfile.empty()) {
     // create, record and update data handle
     if(msgLvl(MSG::DEBUG)) msg() << "Creating new strawstatus container" << endreq ;
     const TRTCond::StrawStatusMultChanContainer* strawstatuscontainer = new TRTCond::StrawStatusMultChanContainer() ; 

     if( (m_detStore->record(strawstatuscontainer,par_strawstatuscontainerkey))!=StatusCode::SUCCESS ) {
        msg(MSG::ERROR) << "Could not record StrawStatusContainer for key " << par_strawstatuscontainerkey << endreq;
     }
     else{ 
        msg( MSG::VERBOSE) << "Got record StrawStatusContainer for key " << par_strawstatuscontainerkey << endreq; 
        strawstatuscontainerexists = true;
     }
     if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatuscontainer,par_strawstatuscontainerkey)) {
        msg(MSG::FATAL) << "Could not retrieve data handle for StrawStatusContainer " << endreq;
        return StatusCode::FAILURE ;
     }else{if(msgLvl(MSG::VERBOSE)) msg() << "Retrieved data handle for StrawStatusContainer " << endreq;}
    if(!par_stattextfile.empty()) return readFromTextFile( par_stattextfile ) ;
  }

  if(msgLvl(MSG::DEBUG)) msg() <<"trying to read "<<par_strawstatuspermanentcontainerkey<< " from StoreGateSvc"<<endreq;
  strawstatuspermanentcontainerexists = m_detStore->StoreGateSvc::contains<TRTCond::StrawStatusMultChanContainer>(par_strawstatuspermanentcontainerkey) ;

  if( strawstatuspermanentcontainerexists ) {
     if(msgLvl(MSG::DEBUG)) msg() << " strawstatuspermanentcontainerexists " <<endreq; 
     if( (m_detStore->regFcn(&TRT_StrawStatusSummarySvc::IOVCallBack,this,m_strawstatuspermanentcontainer,par_strawstatuspermanentcontainerkey)).isFailure()) 
        msg(MSG::ERROR) << "Could not register IOV callback for key: " << par_strawstatuspermanentcontainerkey << endreq ;
  } else if (!par_stattextfilepermanent.empty()) {
     // create, record and update data handle
     if(msgLvl(MSG::DEBUG)) msg() << "Creating new strawstatuspermanent container" << endreq ;

     const TRTCond::StrawStatusMultChanContainer* strawstatuspermanentcontainer = new TRTCond::StrawStatusMultChanContainer() ; 
     if( (m_detStore->record(strawstatuspermanentcontainer,par_strawstatuspermanentcontainerkey))!=StatusCode::SUCCESS ) {
        msg(MSG::ERROR) << "Could not record StrawStatuspermanentContainer for key " << par_strawstatuspermanentcontainerkey << endreq;
     }
     else{ 
        if(msgLvl(MSG::VERBOSE)) msg() << "Got record StrawStatuspermanentContainer for key " << par_strawstatuspermanentcontainerkey << endreq; 
        strawstatuspermanentcontainerexists = true;
     }
     if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatuspermanentcontainer,par_strawstatuspermanentcontainerkey)) {
        msg(MSG::FATAL) << "Could not retrieve data handle for StrawStatuspermanentContainer " << endreq;
        return StatusCode::FAILURE ;
     }else{if(msgLvl(MSG::VERBOSE)) msg() << "Retrieved data handle for StrawStatuspermanentContainer " << endreq;}
     if(!par_stattextfilepermanent.empty()) return readFromTextFile( par_stattextfilepermanent ) ;
  }


  
  if(msgLvl(MSG::DEBUG)) msg() <<"trying to read "<<par_strawstatusHTcontainerkey<< " from StoreGateSvc"<<endreq;
  strawstatusHTcontainerexists = m_detStore->StoreGateSvc::contains<TRTCond::StrawStatusMultChanContainer>(par_strawstatusHTcontainerkey) ;
  
  if( strawstatusHTcontainerexists ) {
     if(msgLvl(MSG::DEBUG)) msg() << " strawstatusHTcontainerexists " <<endreq; 
     if( (m_detStore->regFcn(&TRT_StrawStatusSummarySvc::IOVCallBack,this,m_strawstatusHTcontainer,par_strawstatusHTcontainerkey)).isFailure()) 
        msg(MSG::ERROR) << "Could not register IOV callback for key: " << par_strawstatusHTcontainerkey << endreq ;
  } else if (!par_stattextfileHT.empty()) {
     // create, record and update data handle
     if(msgLvl(MSG::DEBUG)) msg() << "Creating new strawstatusHT container" << endreq ;
     const TRTCond::StrawStatusMultChanContainer* strawstatusHTcontainer = new TRTCond::StrawStatusMultChanContainer() ; 
     if( (m_detStore->record(strawstatusHTcontainer,par_strawstatusHTcontainerkey))!=StatusCode::SUCCESS ) {
        msg(MSG::ERROR) << "Could not record StrawStatusHTContainer for key " << par_strawstatusHTcontainerkey << endreq;
     }
     else{ 
        if(msgLvl(MSG::VERBOSE)) msg() << "Got record StrawStatusHTContainer for key " << par_strawstatusHTcontainerkey << endreq; 
        strawstatusHTcontainerexists = true;
     }
     if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatusHTcontainer,par_strawstatusHTcontainerkey)) {
        msg(MSG::FATAL) << "Could not retrieve data handle for StrawStatusHTContainer " << endreq;
        return StatusCode::FAILURE ;
     }else{if(msgLvl(MSG::VERBOSE)) msg() << "Retrieved data handle for StrawStatusHTContainer " << endreq;}
     if(!par_stattextfileHT.empty()) return readFromTextFile( par_stattextfileHT ) ;
  }


  double lt_interval_small = 0.05/64;
  double lt_interval_large = 0.95/64;
  double ht_interval = 1./64;

  for (unsigned int i = 0; i<128; i++ ) {
    if (i<64) {
      m_ltvaluevector.push_back(lt_interval_small*i+lt_interval_small/2.);
      if(msgLvl(MSG::VERBOSE)) msg() <<i<<" "<<lt_interval_small*i+lt_interval_small/2.<<endreq;
    }
    else if (i<128) {
      m_ltvaluevector.push_back(0.05+lt_interval_large*(i-64)+lt_interval_large/2.);
      if(msgLvl(MSG::VERBOSE)) msg() <<i<<" "<<0.05+lt_interval_large*(i-64)+lt_interval_large/2.<<endreq;
    }
  }
  for (unsigned int i = 0; i<64; i++ ) {
    m_htvaluevector.push_back(ht_interval*(i+0.5));
    if(msgLvl(MSG::VERBOSE)) msg() <<i<<" "<<ht_interval*(i+0.5)<<endreq;
  }

  msg(MSG::INFO) << "TRT_StrawStatusSummarySvc initialized successfully  " << endreq;




  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////


StatusCode TRT_StrawStatusSummarySvc::finalize()
{

  msg(MSG::INFO) << " in finalize() " << endreq;
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
      msg(MSG::WARNING) << "TRT_StrawStatusSummarySvc::getStatus, strawstatuscontainer == 0, dead straws not set" << endreq; 
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
    if (countStrawStatusContainerError<5) {       msg(MSG::WARNING) << "TRT_StrawStatusSummarySvc::getStatusPermanent, strawstatuspermanentcontainer == 0, dead straws not set" << endreq; 
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
     msg(MSG::WARNING) << "TRT_StrawStatusSummarySvc::getStatusHT, strawstatusHTcontainer == 0, dead straws not set" << endreq; 
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
  const unsigned int m_ltbitmask = (0xFE << 8) ;

  return ((m_ltvaluevector[( (m_ltbitmask & getStatus(offlineID)) >> 9 )-4] <1.0 && m_ltvaluevector[(  (m_ltbitmask & getStatus(offlineID)) >> 9)-4] >0.0000001) ?
	  m_ltvaluevector[( (m_ltbitmask & getStatus(offlineID)) >> 9) -4 ] :
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

  const unsigned int m_htbitmask = 0  ;  

  //carefull not to change information which is already set 
  status = status | (org_stat & (~m_htbitmask&(~(0x31) ))) ;
  
  StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();
  ((*strawstatuscontainer).set(id,status));
}

///////////////////////////////////////////////////

double TRT_StrawStatusSummarySvc::get_HT_occ(Identifier offlineID){
  const unsigned int m_htbitmask = 0x3F;
  return m_htvaluevector[(m_htbitmask & ( getStatus(offlineID) ) )];
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
  const unsigned int m_statusbitmask = 1 << 8;// 0000001 00000000
  /*
  unsigned int status = 0;
  if (set) { 
    status= (org_stat | m_statusbitmask); 
  } 
  else if ((!set)&&(get_status(offlineID))) {
    status = org_stat - 256;
  }
  m_data.set(id,status);
  */
  StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();

  if (set) {   (*strawstatuscontainer).set(id, (org_stat|m_statusbitmask)); }else{    (*strawstatuscontainer).set(id, (org_stat & (~m_statusbitmask) )); }
}



void  TRT_StrawStatusSummarySvc::set_status_permanent(Identifier offlineID, bool set){
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  unsigned int org_stat = getStatusPermanent(offlineID);
  const unsigned int m_statusbitmask = 1 << 8;// 0000001 00000000
  StrawStatusContainer* strawstatuspermanentcontainer = getStrawStatusPermanentContainer();

  if (set) {   (*strawstatuspermanentcontainer).set(id, (org_stat|m_statusbitmask)); }else{    (*strawstatuspermanentcontainer).set(id, (org_stat & (~m_statusbitmask) )); }
}




bool TRT_StrawStatusSummarySvc::get_status(Identifier offlineID){
  const unsigned int m_statusbitmask = 1 << 8;// 0000001 00000000
  if(msgLvl(MSG::VERBOSE)) msg() <<"offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & m_statusbitmask) >> 8)<<endreq;
  bool st = false, stperm=false;
  if (getStatus(offlineID)==1) st = true;
  else if (getStatus(offlineID)==0) st = false;
  else {st = bool( (getStatus(offlineID) & m_statusbitmask) >> 8);};

  // Safety check: Won't do any good unless a somewhere a StatusPermanent tag is defined.


  if (strawstatuspermanentcontainerexists) {
    if (getStatusPermanent(offlineID)==1) stperm = true;
    else if (getStatusPermanent(offlineID)==0) stperm = false;
    else {stperm = bool( (getStatusPermanent(offlineID) & m_statusbitmask) >> 8);};
  }

  return ( (st||stperm) );
  
}



bool TRT_StrawStatusSummarySvc::get_statusHT(Identifier offlineID){
  const unsigned int m_statusbitmask = 1 << 8;// 0000001 00000000
  if(msgLvl(MSG::VERBOSE)) msg() <<"offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & m_statusbitmask) >> 8)<<endreq;
  bool stHT=false;

  // Safety check: Won't do any good unless a somewhere a StatusHT

  
  if (strawstatusHTcontainerexists) {
    if (getStatusHT(offlineID)==1) stHT = true;
    else if (getStatusHT(offlineID)==0) stHT = false;
    else {stHT = bool( (getStatusHT(offlineID) & m_statusbitmask) >> 8);};
  }
  else { if(msgLvl(MSG::WARNING)) msg() <<"Attempt to use get_statusHT flag, but CondDB folder not available. Returning StrawStatus::Good "<<endreq; }

  return ( stHT );

}





////////////////////////////////////
void  TRT_StrawStatusSummarySvc::set_pid_status(Identifier offlineID, bool set){
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  unsigned int org_stat = getStatus(offlineID);
  const unsigned int m_pidstatusbitmask = 0x80; //0000000 10000000
   StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();


  if (set) {   (*strawstatuscontainer).set(id, (org_stat|m_pidstatusbitmask));  }else{    (*strawstatuscontainer).set(id, (org_stat & (~m_pidstatusbitmask) )); } 
}

bool TRT_StrawStatusSummarySvc::get_pid_status(Identifier offlineID){
  const unsigned int m_pidstatusbitmask = 0x80; //0000000 10000000
  return  bool( (getStatus(offlineID) & m_pidstatusbitmask) >> 7);
}

///////////////////////////////////////////////
void  TRT_StrawStatusSummarySvc::set_track_status(Identifier offlineID, bool set){
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  unsigned int org_stat = getStatus(offlineID);
  const unsigned int m_trackstatusbitmask = 0x40; //0000000 01000000
  StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();


  if (set) {   (*strawstatuscontainer).set(id, (org_stat|m_trackstatusbitmask));  }else{   (*strawstatuscontainer).set(id, (org_stat & (~m_trackstatusbitmask) )); } 
}

bool TRT_StrawStatusSummarySvc::get_track_status(Identifier offlineID){
  const unsigned int m_trackstatusbitmask = 0x40; //0000000 01000000
  return  bool( (getStatus(offlineID) & m_trackstatusbitmask) >> 6);
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
  msg(MSG::INFO) << "TRT_StrawStatusSummarySvc::readFromTextFile " << filename << endreq;
//  par_stattextfile = filename;
  if(!par_stattextfile.empty()) {
    StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();
    strawstatuscontainer->clear();
    
    const InDetDD::TRT_DetectorManager* m_TRTDetectorManager ;
    if ((m_detStore->retrieve(m_TRTDetectorManager)).isFailure()) {
      msg(MSG::FATAL) << "Problem retrieving TRT_DetectorManager" << endreq;
    }
    

    // initialize strawlayers with status 'good'
    for( InDetDD::TRT_DetElementCollection::const_iterator it =  m_TRTDetectorManager->getDetectorElementBegin() ;
	 it != m_TRTDetectorManager->getDetectorElementEnd() ; ++it) {
      Identifier ident = (*it)->identify() ;
      TRTCond::ExpandedIdentifier id( m_trtid->barrel_ec(ident),
				      m_trtid->layer_or_wheel(ident),
				      0,
				      m_trtid->straw_layer(ident),
				      m_trtid->straw(ident),
				      TRTCond::ExpandedIdentifier::LAYERWHEEL ) ;
      strawstatuscontainer->setStatus(id,TRTCond::StrawStatus::Good );
    }
    
    
    std::ifstream ifs(filename.c_str()) ;
    if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Permanently dead straws are removed based on the txt file: "<<par_stattextfilepermanent<< endreq;
    

    if (!ifs)  { 
    msg(MSG::ERROR) << "Cannot initialize TRT_StrawStatusSummarySvc: Read from text file chosen, but no text file is found. To resolve: copy strawstatus.txt from TRT_ConditionsAlgs/share to your run dir" << endreq;
    return StatusCode::FAILURE;
    }




    if (ifs) {

      int bec, layer, sector, strawlayer, straw, status ;
      //txt file format : bec sector straw strawlayer layer dead/alive
      int line =0 ;
      while( ifs >> bec >> sector>> straw >> strawlayer >> layer >> status ) {
	int level = TRTCond::ExpandedIdentifier::STRAW ;
	line +=1;
	//    std::cout<<"read "<< bec<<  " "<<layer<<" "<<sector<<" "<<strawlayer<<" "<<straw<<" "<<level<<" "<<std::endl;
	if( straw<0      ) { level = TRTCond::ExpandedIdentifier::STRAWLAYER ; straw = 0 ; }
	if( strawlayer<0 ) { level = TRTCond::ExpandedIdentifier::MODULE ;     strawlayer = 0 ; }
	if( sector<0     ) { level = TRTCond::ExpandedIdentifier::LAYERWHEEL ; sector = 0 ; }
	if( layer<0      ) { level = TRTCond::ExpandedIdentifier::BARRELEC ;   layer = 0 ; }
	if( bec<-2       ) { level = TRTCond::ExpandedIdentifier::DETECTOR ;   bec = -2 ; }
              msg(MSG::INFO) << "StrawRead FromText File: bec: "  << bec << " layer: " << layer << " phi: " << sector << " stl:  " << strawlayer << " stw: " << straw   << endreq;
	TRTCond::ExpandedIdentifier id( bec,layer,sector,strawlayer,straw,level);
	strawstatuscontainer->set(id,TRTCond::StrawStatus::Dead) ;
	strawstatuscontainer->setStatus(id,TRTCond::StrawStatus::Dead );


	
	Identifier ID = m_trtid->straw_id(bec,sector,layer,strawlayer,straw);
	
	set_status_temp(ID,status==1?true:false);

      }
     msg(MSG::INFO) << "Straw Status read from: "  <<  filename << " We read: " << line << " straws on it"  << endreq;

    } else {

      msg(MSG::ERROR) << "TRT_StrawStatusSummarySvc::readFromTextFile: can not read the file, file missing: " << filename << endreq;
    }
    
  }
  //HT

  std::ifstream ifsHT(par_stattextfileHT.c_str()) ;
  if(!par_stattextfileHT.empty()) {
    StrawStatusContainer* strawstatuscontainer = getStrawStatusHTContainer();
    strawstatuscontainer->clear();
    const InDetDD::TRT_DetectorManager* m_TRTDetectorManager ;
    if ((m_detStore->retrieve(m_TRTDetectorManager)).isFailure()) {
      msg(MSG::FATAL) << "Problem retrieving TRT_DetectorManager" << endreq;
    }
    // initialize strawlayers with status 'good'
    for( InDetDD::TRT_DetElementCollection::const_iterator it =  m_TRTDetectorManager->getDetectorElementBegin() ;
         it != m_TRTDetectorManager->getDetectorElementEnd() ; ++it) {
      Identifier ident = (*it)->identify() ;
      TRTCond::ExpandedIdentifier id( m_trtid->barrel_ec(ident),
                                      m_trtid->layer_or_wheel(ident),
                                      0,
                                      m_trtid->straw_layer(ident),
                                      m_trtid->straw(ident),
                                      TRTCond::ExpandedIdentifier::LAYERWHEEL ) ;
      strawstatuscontainer->setStatus(id,TRTCond::StrawStatus::Good );
    }
    std::ifstream ifsHT(par_stattextfileHT.c_str()) ;
    if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "HT dead straws are removed based on the txt file: "<<par_stattextfileHT<< endreq;

    if (!ifsHT) {
      msg(MSG::ERROR) << "Cannot initialize TRT_StrawStatusSummarySvc: Read from text file chosen, but no text file is found. To resolve: copy strawstatus.txt from TRT_ConditionsAlgs/share to your run dir" << endreq;
      return StatusCode::FAILURE;
    }
    

    
    
    if (ifsHT) {

      int bec, layer, sector, strawlayer, straw, status ;
      //txt file format : bec sector straw strawlayer layer dead/alive
	int line = 0; 

      while( ifsHT >> bec >> sector>> straw >> strawlayer >> layer >> status ) {
        int level = TRTCond::ExpandedIdentifier::STRAW ;
        //    std::cout<<"read "<< bec<<  " "<<layer<<" "<<sector<<" "<<strawlayer<<" "<<straw<<" "<<level<<" "<<std::endl;
	line +=1;
        if( straw<0      ) { level = TRTCond::ExpandedIdentifier::STRAWLAYER ; straw = 0 ; }
        if( strawlayer<0 ) { level = TRTCond::ExpandedIdentifier::MODULE ;     strawlayer = 0 ; }
        if( sector<0     ) { level = TRTCond::ExpandedIdentifier::LAYERWHEEL ; sector = 0 ; }
        if( layer<0      ) { level = TRTCond::ExpandedIdentifier::BARRELEC ;   layer = 0 ; }
        if( bec<-2       ) { level = TRTCond::ExpandedIdentifier::DETECTOR ;   bec = -2 ; }

	      msg(MSG::WARNING) << "HT StrawRead FromText File: bec: "  << bec << " layer: " << layer << " phi: " << sector << " stl:  " << strawlayer << " stw: " << straw   << endreq;
	TRTCond::ExpandedIdentifier id( bec,layer,sector,strawlayer,straw,level);
        strawstatuscontainer->set(id,TRTCond::StrawStatus::Dead) ;
        strawstatuscontainer->setStatus(id,TRTCond::StrawStatus::Dead );
	
      }

     msg(MSG::WARNING) << "HT Straw Status read from: "  << filename << " We read: " << line << " straws on it"  << endreq;

    }
    

  }
  





  //Permanent


  std::ifstream ifspermanent(par_stattextfilepermanent.c_str()) ;
  
  if(!par_stattextfilepermanent.empty()) {
    
    if (ifspermanent) {


      StrawStatusContainer* strawstatuspermanentcontainer = getStrawStatusPermanentContainer();
      strawstatuspermanentcontainer->clear();


      int bec, layer, sector, strawlayer, straw, status;
      //int  pid_status,  track_status;
      //double lt_occ, ht_occ;
      //txt file format : bec sector straw strawlayer layer dead/alive
	int line = 0;
      while( ifspermanent >> bec >> sector>> straw >> strawlayer >> layer >> status ) {
	
	int level = TRTCond::ExpandedIdentifier::STRAW ;
	//    std::cout<<"read "<< bec<<  " "<<layer<<" "<<sector<<" "<<strawlayer<<" "<<straw<<" "<<level<<" "<<std::endl;
	line +=1;
	if( straw<0      ) { level = TRTCond::ExpandedIdentifier::STRAWLAYER ; straw = 0 ; }
	if( strawlayer<0 ) { level = TRTCond::ExpandedIdentifier::MODULE ;     strawlayer = 0 ; }
	if( sector<0     ) { level = TRTCond::ExpandedIdentifier::LAYERWHEEL ; sector = 0 ; }
	if( layer<0      ) { level = TRTCond::ExpandedIdentifier::BARRELEC ;   layer = 0 ; }
	if( bec<-2       ) { level = TRTCond::ExpandedIdentifier::DETECTOR ;   bec = -2 ; }
              msg(MSG::WARNING) << "Permanent StrawRead FromText File: bec: "  << bec << " layer: " << layer << " phi: " << sector << " stl:  " << strawlayer << " stw: " << straw   << endreq;

	TRTCond::ExpandedIdentifier id( bec,layer,sector,strawlayer,straw,level);
	strawstatuspermanentcontainer->set(id,TRTCond::StrawStatus::Dead) ;
	strawstatuspermanentcontainer->setStatus(id,TRTCond::StrawStatus::Dead );
	
	Identifier ID = m_trtid->straw_id(bec,sector,layer,strawlayer,straw);
	
	set_status_permanent(ID,status==1?true:false);

      }
     msg(MSG::WARNING) << "Permanent Straw Status read from: "  << filename << " We read: " << line << " straws on it"  << endreq;

    }
    
  }
    return StatusCode::SUCCESS ;
  }
  
///////////////////////////////////////////////////








///////////////////////////////////////////////////


StatusCode TRT_StrawStatusSummarySvc::writeToTextFile(const std::string& filename ) const
{

  std::ofstream outfile(filename.c_str());
  TRTCond::StrawStatusLayerContainer::FlatContainer flatcontainer;
  m_strawstatuscontainer->getall(flatcontainer) ;

  
  for( TRTCond::StrawStatusContainer::FlatContainer::const_iterator
         it = flatcontainer.begin() ; it != flatcontainer.end() ; ++it) {
    TRTCond::ExpandedIdentifier id = it->first ;
    const TRTCond::StrawStatus* status = it->second ;
    outfile << id << " " << int(status->getstatus()) << std::endl ;
  }
                                                                                                                                                             
  return StatusCode::SUCCESS ;
}

///////////////////////////////////////////////////


StatusCode TRT_StrawStatusSummarySvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  for (std::list<std::string>::const_iterator 
	 itr=keys.begin(); itr!=keys.end(); ++itr) 
    msg(MSG::INFO) << " IOVCALLBACK for key " << *itr << " number " << I << endreq;
  
  // if constants need to be read from textfile, we sue the call back routine to refill the IOV objects
   if(!par_stattextfile.empty()) return readFromTextFile( par_stattextfile) ;
   return StatusCode::SUCCESS;
}
////////////////////////////////


StatusCode TRT_StrawStatusSummarySvc::streamOutStrawStatObjects() const
{
  msg(MSG::INFO) << "entering streamOutStatObjects "  << endreq;
  StatusCode sc;
  
  // Get Output Stream tool for writing
  IAthenaOutputStreamTool*  streamer;
  sc = m_toolsvc->retrieveTool("AthenaOutputStreamTool",par_statstream,streamer);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to find AthenaOutputStreamTool" << endreq;
    return StatusCode::FAILURE;
  }
 
  sc = streamer->connectOutput();

  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not connect stream to output" <<endreq;
    return( StatusCode::FAILURE);
  }
 
  IAthenaOutputStreamTool::TypeKeyPairs typeKeys;
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(StrawStatusContainer::classname(),par_strawstatuscontainerkey)) ;

  sc = streamer->streamObjects(typeKeys);
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not stream out " <<endreq;
    return( StatusCode::FAILURE);
  }
    
  sc = streamer->commitOutput();
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not commit output stream" <<endreq;
    return( StatusCode::FAILURE);
  }
  
  msg(MSG::INFO) << "Streamed out and committed "  << typeKeys.size() << " objects " << endreq;
  return StatusCode::SUCCESS;
}



StatusCode TRT_StrawStatusSummarySvc::registerObjects(std::string tag, int run1, int event1, int run2, int event2) const 
{
  msg(MSG::INFO) << "registerObjects with IOV " << endreq;
  msg(MSG::INFO) << "Run/evt1 [" << run1 << "," << event1 << "]" << endreq;
  msg(MSG::INFO) << "Run/evt2 [" << run2 << "," << event2 << "]" << endreq;

  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Registered StrawStatusContainer object with key " << par_strawstatuscontainerkey << endreq;
  
  // get pointer to registration svc
  IIOVRegistrationSvc* regsvc;
  if (StatusCode::SUCCESS!=service("IOVRegistrationSvc",regsvc)) {
    msg(MSG::FATAL) << "IOVRegistrationSvc not found" << endreq;
    return( StatusCode::FAILURE);
  }
  
  if (StatusCode::SUCCESS==regsvc->registerIOV(StrawStatusContainer::classname(),
					       par_strawstatuscontainerkey,tag,run1,run2,event1,event2))
    msg(MSG::INFO) << "Registered StrawStatusContainer object with key " << par_strawstatuscontainerkey << endreq ;
  else 
    msg(MSG::ERROR) << "Could not register StrawStatusContainer object with key " << par_strawstatuscontainerkey << endreq ;
  
  return( StatusCode::SUCCESS);
}


