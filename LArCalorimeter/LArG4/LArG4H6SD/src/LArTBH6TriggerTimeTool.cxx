/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArTBH6TriggerTimeTool.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "LArSimEvent/LArHitContainer.h"

LArTBH6TriggerTimeTool::LArTBH6TriggerTimeTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent) :
  AthAlgTool(type,name,parent), m_time(0), m_newEvent(true),
  m_fixed(true)
{
  declareInterface< ITriggerTime >(this) ;
  declareProperty("isFixed",m_fixed);
  declareProperty("FixedTime",m_time);
}


StatusCode LArTBH6TriggerTimeTool::initialize()
{

  IIncidentSvc* incsvc;
  if (StatusCode::SUCCESS!=service("IncidentSvc",incsvc))
    {
      ATH_MSG_FATAL ( "Incident service not found" );
      return StatusCode::FAILURE ;
    }

  long int pri=100;
  incsvc->addListener(this,"BeginEvent",pri);

  ATH_MSG_DEBUG ( " LArTBH6TriggerTimeTool initialized " );

  return StatusCode::SUCCESS;

}


void LArTBH6TriggerTimeTool::handle(const Incident& incident){

  ATH_MSG_DEBUG ( " handle called  " << "for incident "<< incident. type() );

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

    if(StatusCode::SUCCESS!=evtStore()->retrieve(cont,(*it) ) )
      {
        ATH_MSG_ERROR ( " Failed to retrieve LArHitContainer  " <<*it );
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
    ATH_MSG_INFO ( " no LArHit in this event" );
    return 0 ;
  }
  if(e==0){
    ATH_MSG_INFO ( " no LArHit energy in this event" );
    return 0;
  }

  double t = te/e;
  ATH_MSG_DEBUG ( " average time from LArHit =  " <<t );

  return t;


}
