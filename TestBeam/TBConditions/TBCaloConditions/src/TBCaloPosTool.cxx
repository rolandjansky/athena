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
  AthAlgTool(type, name, parent),m_init(0)
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
    ATH_MSG_DEBUG ("in initialize()" );

    // Incident Service:
    IIncidentSvc* incSvc = nullptr;
    ATH_CHECK( serviceLocator()->service("IncidentSvc", incSvc) );

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
  
    ATH_MSG_DEBUG ("in handle()" );
    if(! m_init) { 
      // not yet initialized. 
      if( initHandles() ) { 
        m_init = true; 
      } else  
      {
	ATH_MSG_ERROR ( " unable initialize DataHandle in BeginRun Incident " );
      }
    } 

    return; 
} 

bool TBCaloPosTool::initHandles() 
{ 
      ATH_MSG_DEBUG ("in initHandles()" );

      const EventInfo* evtInfo = nullptr;
      ATH_CHECK( evtStore()->retrieve(evtInfo), false );

      int run = evtInfo->event_ID()->run_number(); 

      std::string etaKey,thetaKey,zKey,deltaKey; 

      if(run<1000454){ 
	// this is the period before July 12, folder name is under Tile 
	thetaKey = "/tile/dcs/System1:table.theta:online..value"    ;
	etaKey = "/tile/dcs/System1:table.eta:online..value" 	    ;
	zKey = "/tile/dcs/System1:table.z:online..value" 	    ;
	deltaKey = "/tile/dcs/System1:table.delta:online..value"    ;	
	ATH_MSG_DEBUG ( " runs before 1000454, using Folders with System1..." );

      } else
      { // Folder moved under System: 
	thetaKey = "/tile/dcs/Tile_LV_62:table.theta:online..value" ;	
	etaKey = "/tile/dcs/Tile_LV_62:table.eta:online..value"     ;
	zKey = "/tile/dcs/Tile_LV_62:table.z:online..value" 	    ;
	deltaKey = "/tile/dcs/Tile_LV_62:table.delta:online..value" ;
	ATH_MSG_DEBUG ( " runs after 1000454, using Folders with Tile_LV_62..." );
      } 

      detStore()->regHandle(m_deltaTable,deltaKey); 
      detStore()->regHandle(m_thetaTable,thetaKey); 
      detStore()->regHandle(m_zTable,zKey); 
      detStore()->regHandle(m_etaTable,etaKey); 

      ATH_MSG_DEBUG ( " eta =    " <<   eta() );
      ATH_MSG_DEBUG ( " theta =  " << theta() );
      ATH_MSG_DEBUG ( " z =      " <<     z() );
      ATH_MSG_DEBUG ( " delta =  " << delta() );

      return true; 
}

//--------------------------------------------------------------------------
double TBCaloPosTool::eta() 
{
  double e=0; 
  if(m_etaTable->getNumRows() !=1 || m_etaTable->getNumColumns()!=1) {
    ATH_MSG_ERROR(" eta GenericDbTable has wrong dimension");
    return 0 ; 
  }

  m_etaTable->getCell(0,0,e);
  return e; 
}



double TBCaloPosTool::theta() 
{
  double t=0; 
  if(m_thetaTable->getNumRows() !=1 || m_thetaTable->getNumColumns()!=1) {
    ATH_MSG_ERROR(" theta GenericDbTable has wrong dimension");
    return 0 ; 
  }

  m_thetaTable->getCell(0,0,t);
  return t;
}



double TBCaloPosTool::z() 
{
  double zzz=0;
  if(m_zTable->getNumRows() !=1 || m_zTable->getNumColumns()!=1) {
    ATH_MSG_ERROR(" z GenericDbTable has wrong dimension");
    return 0 ; 
  }

  m_zTable->getCell(0,0,zzz);
  return zzz;
}



double TBCaloPosTool::delta() 
{
  double d=0;
  if(m_deltaTable->getNumRows() !=1 || m_deltaTable->getNumColumns()!=1) {
    ATH_MSG_ERROR(" delta GenericDbTable has wrong dimension");
    return 0 ; 
  }

  m_deltaTable->getCell(0,0,d);
  return d;
}

