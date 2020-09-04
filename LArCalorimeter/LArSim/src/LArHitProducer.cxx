/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSim/LArHitProducer.h"

#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IDataProviderSvc.h"


#include "LArSimEvent/LArHit.h"


// Constructor
LArHitProducer::LArHitProducer(const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm(name , pSvcLocator),
 m_hitcontainer(0) 
{
  
}


//Initialize method
StatusCode LArHitProducer::initialize(){
  ATH_MSG_DEBUG("LArHitProducer "<< this->name()<<" initialized");
  return StatusCode::SUCCESS;
}


//Execute method
StatusCode LArHitProducer::execute(){
    // a new hit container for each event
    // this container will be finally
    // stored into the TDS by a LArHitMaker
    // So no delete needed 
    
    ATH_MSG_DEBUG("Creating a new LArHitContainer");
    m_hitcontainer  = new LArHitContainer() ;
    if( m_hitcontainer == 0 ) {
      ATH_MSG_FATAL("FAILURE : Unable to create a new LArHitContainer ");
      return StatusCode::FAILURE ;
    }
    
    // construction of hits delegated to this method 
    // to be implemented in specialized classes
    ATH_MSG_DEBUG("Start construction of hits " );
    return this->hitConstruction() ; 
}

//Finalize method
StatusCode LArHitProducer::finalize(){
  ATH_MSG_DEBUG("finalize() completed correctly");
  return StatusCode::SUCCESS;
}


//newLArHit method
LArHit * LArHitProducer::newLArHit(Identifier l_cell , double l_energy , double l_time) {

  LArHit * hit ;
  
  hit = new LArHit( l_cell , l_energy , l_time) ; 
  
  return hit ;

}
