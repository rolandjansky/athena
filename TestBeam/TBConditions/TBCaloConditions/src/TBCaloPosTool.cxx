/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBCaloPosTool.h"

#include "GaudiKernel/ToolFactory.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 
#include "GaudiKernel/IIncidentSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


TBCaloPosTool::TBCaloPosTool(const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent)
    : 
    AlgTool(type, name, parent),m_init(0)
{

    // Declare additional interface
    declareInterface<ITBCaloPosTool>(this);

}

//--------------------------------------------------------------------------
TBCaloPosTool::~TBCaloPosTool()
{}

//--------------------------------------------------------------------------
StatusCode TBCaloPosTool::finalize()
{
 return StatusCode::SUCCESS ; 
}

//--------------------------------------------------------------------------
StatusCode TBCaloPosTool::initialize()
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


void TBCaloPosTool::handle(const Incident&) 
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

bool TBCaloPosTool::initHandles() 
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
	thetaKey = "/tile/dcs/System1:table.theta:online..value"    ;
	etaKey = "/tile/dcs/System1:table.eta:online..value" 	    ;
	zKey = "/tile/dcs/System1:table.z:online..value" 	    ;
	deltaKey = "/tile/dcs/System1:table.delta:online..value"    ;	
	log << MSG::DEBUG << " runs before 1000454, using Folders with System1..." << endreq;

      } else
      { // Folder moved under System: 
	thetaKey = "/tile/dcs/Tile_LV_62:table.theta:online..value" ;	
	etaKey = "/tile/dcs/Tile_LV_62:table.eta:online..value"     ;
	zKey = "/tile/dcs/Tile_LV_62:table.z:online..value" 	    ;
	deltaKey = "/tile/dcs/Tile_LV_62:table.delta:online..value" ;
	log << MSG::DEBUG << " runs after 1000454, using Folders with Tile_LV_62..." << endreq;
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
double TBCaloPosTool::eta() 
{
  double e=0; 

  if(m_etaTable->getNumRows() !=1 || m_etaTable->getNumColumns()!=1)
        {
              MsgStream  log(msgSvc(),name());
              log << MSG::ERROR<<" eta GenericDbTable has wrong dimension"<<endreq;
             return 0 ; 
        }

  m_etaTable->getCell(0,0,e);

  return e; 
}



double TBCaloPosTool::theta() 
{
  double t=0; 

  if(m_thetaTable->getNumRows() !=1 || m_thetaTable->getNumColumns()!=1)
        {
              MsgStream  log(msgSvc(),name());
              log << MSG::ERROR<<" theta GenericDbTable has wrong dimension"<<endreq;
              return 0 ; 
        }

  m_thetaTable->getCell(0,0,t);

  return t;


}



double TBCaloPosTool::z() 
{

  double zzz=0;

  if(m_zTable->getNumRows() !=1 || m_zTable->getNumColumns()!=1)
        {
              MsgStream  log(msgSvc(),name());
              log << MSG::ERROR<<" z GenericDbTable has wrong dimension"<<endreq;
             return 0 ; 
        }

  m_zTable->getCell(0,0,zzz);

 
  return zzz;
	

}



double TBCaloPosTool::delta() 
{
  double d=0;

  if(m_deltaTable->getNumRows() !=1 || m_deltaTable->getNumColumns()!=1)
        {
              MsgStream  log(msgSvc(),name());
              log << MSG::ERROR<<" delta GenericDbTable has wrong dimension"<<endreq;
             return 0 ; 
        }

  m_deltaTable->getCell(0,0,d);

  return d;
	

}

