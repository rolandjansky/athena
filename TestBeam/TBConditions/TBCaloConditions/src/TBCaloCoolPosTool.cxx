/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBCaloCoolPosTool.h"

#include "GaudiKernel/ToolFactory.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 
#include "GaudiKernel/IIncidentSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


TBCaloCoolPosTool::TBCaloCoolPosTool(const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent)
    : 
    AlgTool(type, name, parent),m_init(0)
{

    // Declare additional interface
    declareInterface<ITBCaloPosTool>(this);

}

//--------------------------------------------------------------------------
TBCaloCoolPosTool::~TBCaloCoolPosTool()
{}

//--------------------------------------------------------------------------
StatusCode TBCaloCoolPosTool::finalize()
{
 return StatusCode::SUCCESS ; 
}

//--------------------------------------------------------------------------
StatusCode TBCaloCoolPosTool::initialize()
{
    MsgStream log(msgSvc(), name());
    log <<MSG::DEBUG <<"in initialize()" <<endreq;


    // Get the DetectorStore 
    StatusCode sc = serviceLocator()->service( "DetectorStore", m_detStore );
    if ( sc.isFailure() )
	{
	    log << MSG::ERROR << "Unable to get the DetectorStore" << endreq;
	    return StatusCode::FAILURE;
    }


    // Incident Service:
    IIncidentSvc* incSvc;
    sc = serviceLocator()->service("IncidentSvc", incSvc); 

    if (sc.isFailure())
    {
     log << MSG::ERROR
        << "Unable to retrieve pointer to IncidentSvc "
        << endreq;
     return sc;
    }


    if( initHandles() ) { 
      m_init = true; 
    } else 
    { // wait for the begin run 
      //start listening to "BeginRun"
      int PRIORITY = 100;
      incSvc->addListener(this, "BeginRun", PRIORITY);
    } 

    return StatusCode::SUCCESS;

}


void TBCaloCoolPosTool::handle(const Incident&) 
{
  // This should be the beginning of Run.  EventInfo is available now. 
  
    MsgStream log(msgSvc(), name());
    log <<MSG::DEBUG <<"in handle()" <<endreq;
    if(! m_init) { 
      // not yet initialized. 
      if( initHandles() ) { 
        m_init = true; 
      } else  
      {
	log << MSG::ERROR << " unable initialize DataHandle in BeginRun Incident "
	<< endreq;
      }

    } 

    return; 

} 

bool TBCaloCoolPosTool::initHandles() 
{ 

      MsgStream log(msgSvc(), name());
      log <<MSG::DEBUG <<"in initHandles()" <<endreq;

      StoreGateSvc* evtStore; 
      StatusCode sc = serviceLocator()->service( "StoreGateSvc", evtStore);
      if ( sc.isFailure() )
	{
	    log << MSG::ERROR << "Unable to get the StoreGateSvc" << endreq;
	    return false ; 
        }

      const EventInfo* evtInfo;
      sc = evtStore->retrieve(evtInfo);
      if ( sc.isFailure() )
	{
	    log << MSG::INFO << "Unable to get EventInfo, run probably not begun yet " << endreq;
	    return false ; 
        }

      int run = evtInfo->event_ID()->run_number(); 

      std::string etaKey,thetaKey,zKey,deltaKey; 

      if(run<1000454){ 
	// this is the period before July 12, folder name is under Tile 
	thetaKey = "/TILE/DCS/SYSTEM1/TABLE/THETA"    ;
	etaKey = "/TILE/DCS/SYSTEM1/TABLE/ETA" 	    ;
	zKey = "/TILE/DCS/SYSTEM1/TABLE/Z" 	    ;
	deltaKey = "/TILE/DCS/SYSTEM1/TABLE/DELTA"    ;	
	log << MSG::DEBUG << " runs before 1000454, using Folders with SYSTEM1..." << endreq;

      } else
      { // Folder moved under System: 
	thetaKey = "/TILE/DCS/TILE_LV_62/TABLE/THETA" ;	
	etaKey = "/TILE/DCS/TILE_LV_62/TABLE/ETA" ;	
	zKey = "/TILE/DCS/TILE_LV_62/TABLE/Z" ;	
	deltaKey = "/TILE/DCS/TILE_LV_62/TABLE/DELTA" ;	
	log << MSG::DEBUG << " runs after 1000454, using Folders with TILE_LV_62..." << endreq;
      } 

      m_detStore->regHandle(m_deltaTable,deltaKey); 
      m_detStore->regHandle(m_thetaTable,thetaKey); 
      m_detStore->regHandle(m_zTable,zKey); 
      m_detStore->regHandle(m_etaTable,etaKey); 

      log << MSG::DEBUG << " eta =    " <<   eta() << endreq;
      log << MSG::DEBUG << " theta =  " << theta() << endreq;
      log << MSG::DEBUG << " z =      " <<     z() << endreq;
      log << MSG::DEBUG << " delta =  " << delta() << endreq;

      return true; 
}

//--------------------------------------------------------------------------
double TBCaloCoolPosTool::eta() 
{
  float e=0; 
  try {
    e=(* m_etaTable)["eta"].data<float>();
  }
  catch (std::exception ex) {
     MsgStream  log(msgSvc(),name());
     log << MSG::ERROR<<"eta AttributeList access failed"<<endreq;
     return 0 ; 
  }
  return e; 
}

double TBCaloCoolPosTool::theta() 
{
  float t=0; 
  try {
    t=(* m_thetaTable)["theta"].data<float>();
  }
  catch (std::exception ex) {
     MsgStream  log(msgSvc(),name());
     log << MSG::ERROR<<"theta AttributeList access failed"<<endreq;
     return 0 ; 
  }
  return t; 
}

double TBCaloCoolPosTool::z() 
{
  float z=0; 
  try {
    z=(* m_zTable)["z"].data<float>();
  }
  catch (std::exception ex) {
     MsgStream  log(msgSvc(),name());
     log << MSG::ERROR<<"z AttributeList access failed"<<endreq;
     return 0 ; 
  }
  return z; 
}

double TBCaloCoolPosTool::delta() 
{
  float d=0; 
  try {
    d=(* m_deltaTable)["delta"].data<float>();
  }
  catch (std::exception ex) {
     MsgStream  log(msgSvc(),name());
     log << MSG::ERROR<<"delta AttributeList access failed"<<endreq;
     return 0 ; 
  }
  return d; 
}

