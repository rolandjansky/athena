/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CommissionUtils/CosmicTriggerTimeTool.h"
#include "CommissionEvent/ComTime.h"
#include "GaudiKernel/IIncidentSvc.h"

// #include "TrackRecord/TimedTrackRecordCollection.h" 
// #include "TrackRecord/TimedTrackRecord.h" 
#include "TrackRecord/TrackRecordCollection.h" 
#include "TrackRecord/TrackRecord.h" 

#include "LArSimEvent/LArHitContainer.h" 

#include "CLHEP/Vector/ThreeVector.h"

CosmicTriggerTimeTool::CosmicTriggerTimeTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent) :
  AthAlgTool(type,name,parent), m_time(0), m_newEvent(true),
  m_comTime(0) , m_useLArTime(false)
{

  declareInterface<ITriggerTime>(this);
  declareProperty("UseLArTime", m_useLArTime);

}


StatusCode CosmicTriggerTimeTool::initialize()
{

  IIncidentSvc* incsvc;
  CHECK( service("IncidentSvc",incsvc) );

  long int pri=100;
  incsvc->addListener(this,"BeginEvent",pri);

  ATH_MSG_DEBUG( "CosmicTriggerTimeTool initialized" );

  return StatusCode::SUCCESS;  

} 


void CosmicTriggerTimeTool::handle(const Incident& /* incident */ )
{

  ATH_MSG_DEBUG( "handle called" );

  m_newEvent = true ; 
  m_comTime  = 0    ;  
  return ; 
} 


double CosmicTriggerTimeTool::trackRecordTime()
{

  // const TimedTrackRecordCollection* coll;
  const TrackRecordCollection* coll; 
  StatusCode sc = evtStore()->retrieve(coll,"CaloMuonRecorder"); 
  if (sc!=StatusCode::SUCCESS) { 
    ATH_MSG_ERROR( "can not retrieve CaloMuonRecorder" ); 
    return 0; 
  } 

  int n = coll->size() ; 

  if ( 0 == n ) { 
    ATH_MSG_WARNING( "size of CaloMuonRecorder == 0" ); 
    return 0; 
  } 

  double t = 0; 
  for (auto it : *coll) {
    // TimedTrackRecord * r = const_cast<TimedTrackRecord*>( *it ); 
    CLHEP::Hep3Vector pos = it.GetPosition(); 
    CLHEP::Hep3Vector p = it.GetMomentum(); 
    // ATH_MSG_DEBUG( "TimedTrackRecord xyz position " <<pos.x()<<" "
    ATH_MSG_DEBUG( "TrackRecord xyz position " <<pos.x()<<" " 
                   << pos.y() << " " << pos.z() ); 
    ATH_MSG_DEBUG( "                      momentum " <<p.x() << " " 
                   << p.y() << " " << p.z() );
    ATH_MSG_DEBUG( "                      time     " << it.GetTime() ); 
    t +=  it.GetTime() ; 
  } 

  t = t/n ; 

  ATH_MSG_DEBUG( "Average time from track record = " << t ); 

  return t ; 

} 


double CosmicTriggerTimeTool::time()
{
	
  if (!m_newEvent) return m_time;         

  // new event, try to get it from LArTime
  if (m_useLArTime) {	
    // double t1 = trackRecordTime(); 
    m_time = larTime() ; 
  }
  else if ( m_comTime ) {    
    m_time = m_comTime->getTime() ; 
    ATH_MSG_DEBUG( "got time from ComTime "<< m_time );
  } 
  else if ( StatusCode::SUCCESS == evtStore()->retrieve(m_comTime) ) {
    m_time = m_comTime->getTime() ; 
    ATH_MSG_DEBUG( "retrieved ComTime, t= "<< m_time );
  }
  else { 
    // double t1 = trackRecordTime(); 
    m_time = larTime() ; 
  } 

  m_newEvent= false; 
  return m_time; 
} 


double CosmicTriggerTimeTool::larTime()
{

  std::vector<std::string> keys ; 
  keys.push_back("LArHitEMB") ; 
  keys.push_back("LArHitEMEC") ; 
  keys.push_back("LArHitHEC") ; 
  keys.push_back("LArHitFCAL") ; 

  std::vector<std::string>::const_iterator it = keys.begin()  ; 
  std::vector<std::string>::const_iterator it_e = keys.end()  ; 
  double te = 0; 
  double e = 0; 
  int n=0; 

  for (;it!=it_e;++it) {
    const LArHitContainer* cont; 

    CHECK( evtStore()->retrieve(cont,(*it)), 0 );

    LArHitContainer::const_iterator hit_it = cont->begin(); 
    LArHitContainer::const_iterator hit_it_e = cont->end(); 
    for (;hit_it!=hit_it_e;++hit_it) { 
      const LArHit * hit = (*hit_it);
      e += hit->energy(); 
      te += hit->energy() * hit->time() ; 
      ++n; 
    } 
  } 

  if (n==0) {
    ATH_MSG_INFO( "no LArHit in this event" ); 
    return 0;
  } 
  if (e==0) { 
    ATH_MSG_INFO( "no LArHit energy in this event" ); 
    return 0; 
  }

  double t = te/e; 
  ATH_MSG_DEBUG( "average time from LArHit =  " <<t ); 

  return t;
}
