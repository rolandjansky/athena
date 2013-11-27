/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH62004Algs/LArTBH6TriggerTimeTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "LArSimEvent/LArHitContainer.h" 

LArTBH6TriggerTimeTool::LArTBH6TriggerTimeTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent) :
  AlgTool(type,name,parent), m_time(0), m_sg(0),m_newEvent(true),
  m_fixed(true) 
{
 declareProperty("isFixed",m_fixed);
 declareProperty("FixedTime",m_time);
}


StatusCode LArTBH6TriggerTimeTool::initialize()
{

  MsgStream log( msgSvc(), name() );

  // Pointer to StoreGate (cached)
  StatusCode sc = service("StoreGateSvc", m_sg);

  if (sc.isFailure())
  {
    log << MSG::ERROR
        << "Unable to retrieve pointer to StoreGateSvc "
        << endreq;
    return sc;
  }

  IIncidentSvc* incsvc;
  if (StatusCode::SUCCESS!=service("IncidentSvc",incsvc))
  { 
     log << MSG::FATAL << "Incident service not found" << endreq;
     return StatusCode::FAILURE ; 
  }  

  long int pri=100;
  incsvc->addListener(this,"BeginEvent",pri);

  log << MSG::DEBUG
        << " LArTBH6TriggerTimeTool initialized "
        << endreq;

  return StatusCode::SUCCESS;  

} 


void LArTBH6TriggerTimeTool::handle(const Incident& incident){

  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG
        << " handle called  "
        << endreq;

  m_newEvent = true ; 
  return ; 
} 

double LArTBH6TriggerTimeTool::time(){
	
        if(m_fixed || !m_newEvent) return m_time;         

        // new event, try to get it from FrontHit 
        if( false )
        {
        }
	else
	{ 
	  // double t1 = trackRecordTime(); 
          m_time = larTime() ; 
        } 

	m_newEvent= false; 
	return m_time; 
} 


double LArTBH6TriggerTimeTool::larTime(){

    MsgStream log( msgSvc(), name() );

    std::vector<std::string> keys ; 
//    keys.push_back("LArHitEMB") ; 
    keys.push_back("LArHitEMEC") ; 
    keys.push_back("LArHitHEC") ; 
    keys.push_back("LArHitFCAL") ; 

    std::vector<std::string>::const_iterator it = keys.begin()  ; 
    std::vector<std::string>::const_iterator it_e = keys.end()  ; 
    double te = 0; 
    double e = 0; 
    int n=0; 

    for(;it!=it_e;++it){
     const LArHitContainer* cont; 

     if(StatusCode::SUCCESS!=m_sg->retrieve(cont,(*it) ) )
     { 
      log<<MSG::ERROR<<" Failed to retrieve LArHitContainer  " <<*it<<endreq; 
      return StatusCode::FAILURE; 
     } 
     LArHitContainer::const_iterator hit_it = cont->begin(); 
     LArHitContainer::const_iterator hit_it_e = cont->end(); 
     for(;hit_it!=hit_it_e;++hit_it){ 
	const LArHit * hit = (*hit_it);
	e += hit->energy(); 
	te += hit->energy() * hit->time() ; 
	++n; 
     } 
    } 

    if(n==0){ 
	log<<MSG::INFO<<" no LArHit in this event" <<endreq; 
        return 0 ;
    } 
    if(e==0){ 
	 log<<MSG::INFO<<" no LArHit energy in this event" <<endreq; 
	 return 0; 
    }

    double t = te/e; 
    log<<MSG::DEBUG << " average time from LArHit =  " <<t <<endreq; 

    return t; 


}
