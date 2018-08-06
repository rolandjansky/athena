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
#include "StoreGate/ReadCondHandle.h"



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
    m_condSvc("CondSvc",name),
    m_statReadKey("/TRT/Cond/Status"),
    m_permReadKey("/TRT/Cond/StatusPermanent"),
    m_statHTReadKey("/TRT/Cond/StatusHT"),
    m_isGEANT4(true),
    m_strawstatusG4(nullptr),
    m_strawstatuspermanentG4(nullptr),
    m_strawstatusHTG4(nullptr),
    m_setup(false)

{
  declareProperty("statusTextFile",m_par_stattextfile);
  declareProperty("statusTextFilePermanent",m_par_stattextfilepermanent);
  declareProperty("statusTextFileHT",m_par_stattextfileHT);
  declareProperty("ToolSvc",m_toolsvc);
  declareProperty("isGEANT4",m_isGEANT4);

    // create arrays for alive straws
  m_stw_total = new int[7];
  m_stw_local = new int*[6];
  m_stw_wheel = new int*[34];
  for (int i=0; i<6 ; ++i) m_stw_local[i] = new int[32];
  for (int i=0; i<34; ++i) m_stw_wheel[i] = new int[32];

  // initialise event number cache
  m_evtstat.push_back(-1);
  m_evtperm.push_back(-1);
  m_evtstatHT.push_back(-1);
  m_strawstatuscontainer.push_back(nullptr);
  m_strawstatuspermanentcontainer.push_back(nullptr);
  m_strawstatusHTcontainer.push_back(nullptr);


}



TRT_StrawStatusSummarySvc::~TRT_StrawStatusSummarySvc()
{
}

StatusCode TRT_StrawStatusSummarySvc::initialize() 
{
  msg(MSG::INFO) << "TRT_StrawStatusSummarySvc initialize method called" << endmsg;


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


  if(!m_isGEANT4) {
  
    // Read keys in case of normal reconstruction/digitization
    ATH_CHECK( m_statReadKey.initialize() );
    ATH_CHECK( m_permReadKey.initialize() );
    ATH_CHECK( m_statHTReadKey.initialize() );
  }

  if(m_isGEANT4) {
    // Here we are either doing GEANT4 simulation or reading from text file. In either case revert to old non-MT style
    if (m_par_stattextfile.empty()) {
      msg(MSG::INFO) << "TRT_StrawStatusSummarySvc::initialize for simulation or streaming to/from text file"  << endmsg;
      if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatusG4,m_par_strawstatuscontainerkey)) {
        ATH_MSG_FATAL("Could not retrieve folder " << m_par_strawstatuscontainerkey);
        return StatusCode::FAILURE;
      }
    }
    if (m_par_stattextfilepermanent.empty()) {
      if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatuspermanentG4,m_par_strawstatuspermanentcontainerkey)) {
        ATH_MSG_FATAL("Could not retrieve folder " << m_par_strawstatuspermanentcontainerkey);
        return StatusCode::FAILURE;
      }
    }
    if (m_par_stattextfileHT.empty()) {
      if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatusHTG4,m_par_strawstatusHTcontainerkey)) {
        ATH_MSG_FATAL("Could not retrieve folder " << m_par_strawstatusHTcontainerkey);
        return StatusCode::FAILURE;
      }
    }
    if (!m_par_stattextfile.empty()) {
      // Assume implicitely that the folder was blocked. Create, record and update data handle
      if(msgLvl(MSG::INFO)) msg() << "Creating new strawstatus container" << endmsg ;
      const TRTCond::StrawStatusMultChanContainer*  strawstatuscontainer = new TRTCond::StrawStatusMultChanContainer() ; 

      if( (m_detStore->record(strawstatuscontainer,m_par_strawstatuscontainerkey))!=StatusCode::SUCCESS ) {
           msg(MSG::ERROR) << "Could not record StrawStatusContainer for key " << m_par_strawstatuscontainerkey << endmsg;
           return StatusCode::FAILURE;
      }
      if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatusG4,m_par_strawstatuscontainerkey)) {
	msg(MSG::FATAL) << "Could not retrieve data handle for StrawStatusContainer " << endmsg;
	return StatusCode::FAILURE ;
      }

      return readFromTextFile( m_par_stattextfile ) ;
    }
    if (!m_par_stattextfilepermanent.empty()) {
      // Assume implicitely that the folder was blocked. Create, record and update data handle
      if(msgLvl(MSG::INFO)) msg() << "Creating new strawstatuspermanent container" << endmsg ;
      const TRTCond::StrawStatusMultChanContainer* strawstatuscontainer = new TRTCond::StrawStatusMultChanContainer() ; 

      if( (m_detStore->record(strawstatuscontainer,m_par_strawstatuspermanentcontainerkey))!=StatusCode::SUCCESS ) {
           msg(MSG::ERROR) << "Could not record StrawStatusContainer for key " << m_par_strawstatuspermanentcontainerkey << endmsg;
           return StatusCode::FAILURE;
      }

      if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatuspermanentG4,m_par_strawstatuspermanentcontainerkey)) {
	  msg(MSG::FATAL) << "Could not retrieve data handle for StrawStatusPermanentContainer " << endmsg;
	  return StatusCode::FAILURE ;
      }


      return readFromTextFile( m_par_stattextfilepermanent ) ;
    }
    if (!m_par_stattextfileHT.empty()) {
      // Assume implicitely that the folder was blocked. Create, record and update data handle
      if(msgLvl(MSG::INFO)) msg() << "Creating new strawstatusHT container" << endmsg ;
      const TRTCond::StrawStatusMultChanContainer* strawstatuscontainer = new TRTCond::StrawStatusMultChanContainer() ; 

      if( (m_detStore->record(strawstatuscontainer,m_par_strawstatusHTcontainerkey))!=StatusCode::SUCCESS ) {
           msg(MSG::ERROR) << "Could not record StrawStatusContainer for key " << m_par_strawstatusHTcontainerkey << endmsg;
           return StatusCode::FAILURE;
      }

      if(StatusCode::SUCCESS!=m_detStore->retrieve(m_strawstatusHTG4,m_par_strawstatusHTcontainerkey)) {
	msg(MSG::FATAL) << "Could not retrieve data handle for StrawStatusHTContainer " << endmsg;
	return StatusCode::FAILURE ;
      }

      return readFromTextFile( m_par_stattextfileHT ) ;
    }

  } else {
    msg(MSG::INFO) << "TRT_StrawStatusSummarySvc::initialize for reconstruction or digitization "  << endmsg;
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
  const StrawStatusContainer* strawstatuscontainer = getStrawStatusContainer();
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
  const StrawStatusContainer* strawstatuspermanentcontainer = getStrawStatusPermanentContainer();
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
  const StrawStatusContainer* strawstatusHTcontainer = getStrawStatusHTContainer();

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


    StrawStatusContainer* strawstatuscontainer = const_cast<StrawStatusContainer*>(getStrawStatusContainer());
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
  
  StrawStatusContainer* strawstatuscontainer = const_cast<StrawStatusContainer*>(getStrawStatusContainer());
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

  StrawStatusContainer* strawstatuscontainer = const_cast<StrawStatusContainer*>(getStrawStatusContainer());

  if (set) {   (*strawstatuscontainer).set(id, (org_stat|statusbitmask)); }else{    (*strawstatuscontainer).set(id, (org_stat & (~statusbitmask) )); }
}



void  TRT_StrawStatusSummarySvc::set_status_permanent(Identifier offlineID, bool set){
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(offlineID),m_trtid->layer_or_wheel(offlineID),
								m_trtid->phi_module(offlineID),m_trtid->straw_layer(offlineID),
								m_trtid->straw(offlineID),level );
  unsigned int org_stat = getStatusPermanent(offlineID);
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  StrawStatusContainer* strawstatuspermanentcontainer = const_cast<StrawStatusContainer*>(getStrawStatusPermanentContainer());

  if (set) {   (*strawstatuspermanentcontainer).set(id, (org_stat|statusbitmask)); }else{    (*strawstatuspermanentcontainer).set(id, (org_stat & (~statusbitmask) )); }
}




bool TRT_StrawStatusSummarySvc::get_status(Identifier offlineID){
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  if(msgLvl(MSG::VERBOSE)) msg() <<"offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & statusbitmask) >> 8)<<endmsg;
  bool st = false, stperm=false;
  if (getStatus(offlineID)==1) st = true;
  else if (getStatus(offlineID)==0) st = false;
  else {st = bool( (getStatus(offlineID) & statusbitmask) >> 8);};


    if (getStatusPermanent(offlineID)==1) stperm = true;
    else if (getStatusPermanent(offlineID)==0) stperm = false;
    else {stperm = bool( (getStatusPermanent(offlineID) & statusbitmask) >> 8);};

  return ( (st||stperm) );
  
}



bool TRT_StrawStatusSummarySvc::get_statusHT(Identifier offlineID){
  const unsigned int statusbitmask = 1 << 8;// 0000001 00000000
  if(msgLvl(MSG::VERBOSE)) msg() <<"offlineID "<<offlineID<<" "<<getStatus(offlineID)<<" "<<((getStatus(offlineID) & statusbitmask) >> 8)<<endmsg;
  bool stHT=false;


    if (getStatusHT(offlineID)==1) stHT = true;
    else if (getStatusHT(offlineID)==0) stHT = false;
    else {stHT = bool( (getStatusHT(offlineID) & statusbitmask) >> 8);};

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
  StrawStatusContainer* strawstatuscontainer = const_cast<StrawStatusContainer*>(getStrawStatusContainer());


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
  StrawStatusContainer* strawstatuscontainer = const_cast<StrawStatusContainer*>(getStrawStatusContainer());


  if (set) {   (*strawstatuscontainer).set(id, (org_stat|trackstatusbitmask));  }else{   (*strawstatuscontainer).set(id, (org_stat & (~trackstatusbitmask) )); } 
}

bool TRT_StrawStatusSummarySvc::get_track_status(Identifier offlineID){
  const unsigned int trackstatusbitmask = 0x40; //0000000 01000000
  return  bool( (getStatus(offlineID) & trackstatusbitmask) >> 6);
}










///////////////////////////////////////////////////


StatusCode TRT_StrawStatusSummarySvc::readFromTextFile(const std::string& filename)
{

  msg(MSG::INFO) << "TRT_StrawStatusSummarySvc::readFromTextFile " << filename << endmsg;

  // Input text file
  std::ifstream ifs(m_par_stattextfile.c_str()) ;
  if(!m_par_stattextfile.empty()) {
    StrawStatusContainer* strawstatuscontainer = const_cast<StrawStatusContainer*>(getStrawStatusContainer());
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

    if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Run dependent dead straws are removed based on the txt file: "<<m_par_stattextfile<< endmsg;    
    

    if (!ifs)  { 
    msg(MSG::ERROR) << "No text file is found. To resolve: copy strawstatus.txt from TRT_ConditionsAlgs/share to your run dir" << endmsg;
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
    StrawStatusContainer* strawstatuscontainer = const_cast<StrawStatusContainer*>(getStrawStatusHTContainer());
    strawstatuscontainer->clear();

    const InDetDD::TRT_DetectorManager* TRTDetectorManager ;
    if ((m_detStore->retrieve(TRTDetectorManager)).isFailure()) {
      msg(MSG::FATAL) << "Problem retrieving TRT_DetectorManager" << endmsg;
    }

    if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "HT dead straws are removed based on the txt file: "<<m_par_stattextfileHT<< endmsg;

    if (!ifsHT) {
      msg(MSG::ERROR) << "No text file is found. To resolve: copy strawstatus.txt from TRT_ConditionsAlgs/share to your run dir" << endmsg;
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


      StrawStatusContainer* strawstatuspermanentcontainer = const_cast<StrawStatusContainer*>(getStrawStatusPermanentContainer());
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


StatusCode TRT_StrawStatusSummarySvc::writeToTextFile(const std::string& filename )
{
  std::ofstream outfile(filename.c_str());



  if(getStrawStatusContainer()->numObjects()>0){
    msg(MSG::INFO) << " Dump To File: StrawStatus "  << endmsg;
    outfile << " Status: " << std::endl;
    TRTCond::StrawStatusLayerContainer::FlatContainer flatcontainer;

    getStrawStatusContainer()->getall(flatcontainer) ;

    for( TRTCond::StrawStatusContainer::FlatContainer::const_iterator
         it = flatcontainer.begin() ; it != flatcontainer.end() ; ++it) {
      TRTCond::ExpandedIdentifier id = it->first ;
      const TRTCond::StrawStatus* status = it->second ;
      outfile << id << " " << int(status->getstatus()) << std::endl ;
    }
  }



 if( getStrawStatusHTContainer()->numObjects()>0){
    msg(MSG::INFO) << " Dump To File: StrawStatus HT "  << endmsg;
    outfile << " HT Status: " << std::endl;
    TRTCond::StrawStatusLayerContainer::FlatContainer flatcontainerHT;

    getStrawStatusHTContainer()->getall(flatcontainerHT) ;


    for( TRTCond::StrawStatusContainer::FlatContainer::const_iterator
         it = flatcontainerHT.begin() ; it != flatcontainerHT.end() ; ++it) {
      TRTCond::ExpandedIdentifier id = it->first ;
      const TRTCond::StrawStatus* status = it->second ;
      outfile << id << " " << int(status->getstatus()) << std::endl ;
    }
  }


 if(getStrawStatusPermanentContainer()->numObjects()>0){
   msg(MSG::INFO) << " Dump To File: StrawStatus permanent "  << endmsg;
   outfile << " permanent Status: " << std::endl;
   TRTCond::StrawStatusLayerContainer::FlatContainer flatcontainerpermanent;

   getStrawStatusPermanentContainer() ->getall(flatcontainerpermanent) ;


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
void TRT_StrawStatusSummarySvc::ComputeAliveStraws(){
   // The TRT helper:
  bool detectorManager = true;
  if (StatusCode::SUCCESS!=m_detStore->retrieve(m_trtDetMgr,"TRT")) {
    msg(MSG::WARNING) << "Problem retrieving TRT Detector Manager. Computation will use IDhelper. CPU slower " << endmsg;
    detectorManager = false;
  }

  resetArrays();
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

 m_setup=true;
 ATH_MSG_DEBUG("Active Straws: " << m_stw_total[0] << " \t" << m_stw_total[1]<< "\t" << m_stw_total[2]<< "\t" << m_stw_total[3] );	

}

int*  TRT_StrawStatusSummarySvc::getStwTotal()
{
  if(!m_setup) {
    resetArrays();
    ComputeAliveStraws();
  }
  return m_stw_total;
}

int**  TRT_StrawStatusSummarySvc::getStwLocal()
{
  if(!m_setup) {
    resetArrays();
    ComputeAliveStraws();
  }

  return m_stw_local;
}

int**  TRT_StrawStatusSummarySvc::getStwWheel()
{
  if(!m_setup) {
    resetArrays();
    ComputeAliveStraws();
  }
  return m_stw_wheel;
}


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


