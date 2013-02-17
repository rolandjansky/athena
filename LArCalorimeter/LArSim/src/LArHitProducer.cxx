/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSim/LArHitProducer.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IDataProviderSvc.h"


#include "LArSimEvent/LArHit.h"


// Constructor
LArHitProducer::LArHitProducer(const std::string& name, ISvcLocator* pSvcLocator ) :
 Algorithm(name , pSvcLocator),
 m_hitcontainer(0) 
{
  
}


//Initialize method
StatusCode LArHitProducer::initialize(){
    MsgStream log( this->messageService(), this->name() ); 
    StatusCode sc = StatusCode::SUCCESS ;   
    log<<MSG::DEBUG<<"LArHitProducer "<< this->name()<<" initialized"<<endreq ;        
    return sc;
}


//Execute method
StatusCode LArHitProducer::execute(){
    MsgStream log( this->messageService(), this->name() );
    StatusCode sc = StatusCode::SUCCESS ;   
    // a new hit container for each event
    // this container will be finally
    // stored into the TDS by a LArHitMaker
    // So no delete needed 
    
    log<<MSG::DEBUG<<"Creating a new LArHitContainer"<<endreq;
    m_hitcontainer  = new LArHitContainer() ;
    if( m_hitcontainer == 0 ) {
    
       log<<MSG::FATAL<<"FAILURE : Unable to create a new LArHitContainer "<<endreq ;
       return StatusCode::FAILURE ;
    }
    
    // construction of hits delegated to this method 
    // to be implemented in specialized classes
    log<<MSG::DEBUG<<"Start construction of hits " << endreq ;
    sc = this->hitConstruction() ; 
    return sc;
}

//Finalize method
StatusCode LArHitProducer::finalize(){
     MsgStream log( this->messageService(), this->name() );
     StatusCode sc = StatusCode::SUCCESS ;
     
     log<<MSG::DEBUG<<"finalize() completed correctly"<<endreq;
     return sc;
}


//newLArHit method
LArHit * LArHitProducer::newLArHit(Identifier l_cell , double l_energy , double l_time) {

  LArHit * hit ;
  
  hit = new LArHit( l_cell , l_energy , l_time) ; 
  
  return hit ;

}
