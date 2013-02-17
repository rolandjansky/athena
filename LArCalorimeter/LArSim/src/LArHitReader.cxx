/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSim/LArHitReader.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "LArSimEvent/LArHit.h"
#include "StoreGate/StoreGate.h"

// Constructor
LArHitReader::LArHitReader(const std::string& name, ISvcLocator* pSvcLocator ) :
 Algorithm(name , pSvcLocator)
{
  
}


//Initialize method
StatusCode LArHitReader::initialize(){
    MsgStream log( this->messageService(), this->name() ); 
    StatusCode sc = StatusCode::SUCCESS ;   
    log<<MSG::DEBUG<<"LArHitReader "<< this->name()<<" initialized"<<endreq ;        
    return sc;
}


//Execute method
StatusCode LArHitReader::execute(){
    MsgStream log( this->messageService(), this->name() );
    StatusCode sc = StatusCode::SUCCESS ;   

    const LArHitContainer* cont; 

    if(StatusCode::SUCCESS != StoreGate::instance().retrieve(cont,"LArHitFake") ) 
    { 
      log<<MSG::ERROR<<" Failed to retrieve LArHitContainer  " <<endreq; 
      return StatusCode::FAILURE; 
    } 

    log<<MSG::DEBUG<<" LArHitContainer Dump ----------- " << endreq; 
    log<<MSG::DEBUG<< (std::string)(*cont)   << endreq; 

    return sc;
}

//Finalize method
StatusCode LArHitReader::finalize(){
     MsgStream log( this->messageService(), this->name() );
     StatusCode sc = StatusCode::SUCCESS ;
     
     log<<MSG::DEBUG<<"finalize() completed correctly"<<endreq;
     return sc;
}


