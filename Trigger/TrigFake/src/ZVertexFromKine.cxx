/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************                               
                               
NAME:     ZVertexFromKine.cxx                               
PACKAGE:                                 
                               
AUTHOR:   Nikos Konstantinidis
         
CREATED:  May 2005                               
                               
PURPOSE:  Make z of first MC particle available to algorithms
                               
Modified :
         
                               
********************************************************************/                               

#include "GaudiKernel/MsgStream.h"                               
#include "StoreGate/StoreGateSvc.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigVertex.h"

#include "TrigFake/ZVertexFromKine.h"                               

using namespace HepMC;

      
////////////////////////////////////////////////////////////////                               
//  CONSTRUCTOR:                               
                                   
ZVertexFromKine::ZVertexFromKine(const std::string& name, ISvcLocator* pSvcLocator):                                
  Algorithm(name, pSvcLocator)
{

  declareProperty("McEventCollectionName", m_mcEventCollectionName="GEN_EVENT");

}                               
                               
// DESTRUCTOR:                               
                                  
ZVertexFromKine::~ZVertexFromKine()                               
{ }                               
                               
/////////////////////////////////////////////////////////////////                               
// INITIALIZE METHOD:                               
                                      
StatusCode ZVertexFromKine::initialize()                               
{                               
  MsgStream athenaLog(msgSvc(), name());

                               
  StatusCode sc = service("StoreGateSvc", m_StoreGate);                               
  if (sc.isFailure())  {                               
    athenaLog << MSG::ERROR                               
	      << "Unable to get pointer to StoreGate Service" << endreq;                               
    return sc;                               
  } 

  return StatusCode::SUCCESS;                               
}                               
                               
/////////////////////////////////////////////////////////////////                               
// FINALIZE METHOD:                               
                                 
StatusCode ZVertexFromKine::finalize() { return StatusCode::SUCCESS; }                               
                               
/////////////////////////////////////////////////////////////////                               
// EXECUTE METHOD:                               
                                 
StatusCode ZVertexFromKine::execute()                               
{                                
                               
  MsgStream athenaLog(msgSvc(), name());
  athenaLog << MSG::DEBUG << "Executing ZVertexFromKine" << endreq;     

  //  Get the collection generator events (physics and pile-up) making up this event 
  
  const McEventCollection* mcEvents;
  StatusCode sc = m_StoreGate->retrieve( mcEvents, m_mcEventCollectionName );
  if( sc.isFailure() ) {
    athenaLog << MSG::INFO <<"Could not retrieve mcEventCollection with key " << m_mcEventCollectionName << endreq; 
    return StatusCode::SUCCESS; 
  };

  McEventCollection::const_iterator  event = mcEvents->begin();
  HepMC::GenVertex* vertex = *((*event)->vertices_begin());

  TrigVertexCollection* zinfo = new TrigVertexCollection();
  TrigVertex* zvert = new TrigVertex( vertex->position().z() );
  zinfo->push_back(zvert);

  if( m_StoreGate->record( zinfo, "TrueTrigVertexColl" ) ) {
    athenaLog << MSG::DEBUG <<"Stored TrueTrigVertexColl" << endreq;
  }
  else {
    athenaLog << MSG::WARNING << "Failed to Store TrueTrigVertexColl" << endreq;
  }

  return StatusCode::SUCCESS;
}

