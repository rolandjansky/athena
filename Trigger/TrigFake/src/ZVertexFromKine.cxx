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

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigVertex.h"

#include "ZVertexFromKine.h"                               



      
////////////////////////////////////////////////////////////////                               
//  CONSTRUCTOR:                               
                                   
ZVertexFromKine::ZVertexFromKine(const std::string& name, ISvcLocator* pSvcLocator):                                
  AthAlgorithm(name, pSvcLocator)
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
  return StatusCode::SUCCESS;                               
}                               
                               
/////////////////////////////////////////////////////////////////                               
// FINALIZE METHOD:                               
                                 
StatusCode ZVertexFromKine::finalize() { return StatusCode::SUCCESS; }                               
                               
/////////////////////////////////////////////////////////////////                               
// EXECUTE METHOD:                               
                                 
StatusCode ZVertexFromKine::execute()                               
{                                
                               
   ATH_MSG_DEBUG("Executing ZVertexFromKine");

  //  Get the collection generator events (physics and pile-up) making up this event 
  
  const McEventCollection* mcEvents;
  StatusCode sc = evtStore()->retrieve( mcEvents, m_mcEventCollectionName );
  if( sc.isFailure() ) {
     ATH_MSG_INFO( "Could not retrieve mcEventCollection with key " << m_mcEventCollectionName); 
     return StatusCode::SUCCESS; 
  };

  McEventCollection::const_iterator  event = mcEvents->begin();
  HepMC::GenVertex* vertex = *((*event)->vertices_begin());

  TrigVertexCollection* zinfo = new TrigVertexCollection();
  TrigVertex* zvert = new TrigVertex( vertex->position().z() );
  zinfo->push_back(zvert);

  if( evtStore()->record( zinfo, "TrueTrigVertexColl" ) ) {
     ATH_MSG_DEBUG("Stored TrueTrigVertexColl");
  }
  else {
     ATH_MSG_WARNING("Failed to Store TrueTrigVertexColl");
  }

  return StatusCode::SUCCESS;
}

