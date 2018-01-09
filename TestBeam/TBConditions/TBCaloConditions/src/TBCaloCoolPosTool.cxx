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
    AthAlgTool(type, name, parent),m_init(0)
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
    ATH_MSG_DEBUG ("in initialize()" );

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


void TBCaloCoolPosTool::handle(const Incident&) 
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

bool TBCaloCoolPosTool::initHandles() 
{ 
      ATH_MSG_DEBUG ("in initHandles()" );

      const EventInfo* evtInfo = nullptr;
      ATH_CHECK( evtStore()->retrieve(evtInfo), false );

      int run = evtInfo->event_ID()->run_number(); 

      std::string etaKey,thetaKey,zKey,deltaKey; 

      if(run<1000454){ 
	// this is the period before July 12, folder name is under Tile 
	thetaKey = "/TILE/DCS/SYSTEM1/TABLE/THETA"    ;
	etaKey = "/TILE/DCS/SYSTEM1/TABLE/ETA" 	    ;
	zKey = "/TILE/DCS/SYSTEM1/TABLE/Z" 	    ;
	deltaKey = "/TILE/DCS/SYSTEM1/TABLE/DELTA"    ;	
	ATH_MSG_DEBUG ( " runs before 1000454, using Folders with SYSTEM1..." );

      } else
      { // Folder moved under System: 
	thetaKey = "/TILE/DCS/TILE_LV_62/TABLE/THETA" ;	
	etaKey = "/TILE/DCS/TILE_LV_62/TABLE/ETA" ;	
	zKey = "/TILE/DCS/TILE_LV_62/TABLE/Z" ;	
	deltaKey = "/TILE/DCS/TILE_LV_62/TABLE/DELTA" ;	
	ATH_MSG_DEBUG ( " runs after 1000454, using Folders with TILE_LV_62..." );
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
double TBCaloCoolPosTool::eta() 
{
  float e=0; 
  try {
    e=(* m_etaTable)["eta"].data<float>();
  }
  catch (const std::exception& ex) {
     ATH_MSG_ERROR("eta AttributeList access failed");
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
  catch (const std::exception& ex) {
     ATH_MSG_ERROR("theta AttributeList access failed");
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
  catch (const std::exception& ex) {
     ATH_MSG_ERROR("z AttributeList access failed");
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
  catch (const std::exception& ex) {
     ATH_MSG_ERROR("delta AttributeList access failed");
     return 0 ; 
  }
  return d; 
}

