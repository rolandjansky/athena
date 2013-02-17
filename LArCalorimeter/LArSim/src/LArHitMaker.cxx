/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSim/LArHitMaker.h"
#include "LArSim/LArHitProducer.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "StoreGate/StoreGateSvc.h"


//********************************************************************************

// Constructor
LArHitMaker::LArHitMaker(const std::string& name, ISvcLocator* pSvcLocator ) :
 Algorithm(name , pSvcLocator),
 m_HitRetrieverNameAndType("LArHitProducerTest/producer1"),
 m_HitContainerLocation("LArHits"),
 m_HitRetrieverAlgorithm(0) ,
 m_DebugFlag(0)
{
   // assignment of data members from property values 
  declareProperty("HitRetrieverNameAndType", m_HitRetrieverNameAndType); // Sub-Algo name 
  declareProperty("HitContainerLocation",m_HitContainerLocation);        // Place where the hit collection will be stored in TDS 
  declareProperty("DebugFlag",m_DebugFlag);        // to switch on( !=0 ) or off ( 0 ) the Debug Flag 
                                                  // has an action on the container content printout
}

//********************************************************************************

//Find type method 

std::string LArHitMaker::findType(std::string l_property){
  int slash_pos = l_property.find_first_of("/");
  std::string type = l_property.substr( 0, slash_pos );
  return type ;
}

//**********************************************************************************

//Find instance name

std::string LArHitMaker::findName(std::string l_property){
  int slash_pos = l_property.find_first_of("/");
  std::string type = l_property.substr( 0, slash_pos );
  std::string name = (slash_pos > 0) ? l_property.substr( slash_pos + 1) : type ;
  return name ;
}

//***************************************************************************************

//Initialize method
StatusCode LArHitMaker::initialize(){
    MsgStream log( messageService(), name() ); 
    StatusCode sc = StatusCode::SUCCESS ;
    
    // Get sub-algo Type and instance name
    std::string type = findType(m_HitRetrieverNameAndType);
    std::string name = findName(m_HitRetrieverNameAndType);
    log<<MSG::INFO<<"Sub-Algorithm type="<<type<<" and instance name="<<name<<endreq; 
    log<<MSG::INFO<<"location of stored hits="<<m_HitContainerLocation<<endreq;
    
    // Instanciate LArHitProducer
    sc = createSubAlgorithm(type, name, m_HitRetrieverAlgorithm);
    if ( sc.isFailure() ){
      log << MSG::FATAL << "LAr FAILURE : Unable to create " << m_HitRetrieverNameAndType << endreq;
    }
    return sc;
}

//*****************************************************************************************

//Execute method
StatusCode LArHitMaker::execute(){
    MsgStream log( messageService(), name() );
    StatusCode sc = StatusCode::SUCCESS ; 
    
    log<<MSG::DEBUG<<" Start executing execute() method of "<< m_HitRetrieverNameAndType << endreq ;
    
    StoreGateSvc * l_storeGateSvc ;
    
    sc = service ( "StoreGateSvc" , l_storeGateSvc ) ;
    if( sc.isFailure() ) {
      log<<MSG::FATAL<<"LAr FAILURE : Cannot locate StoreGate " << endreq ;
      sc = StatusCode::FAILURE ;
      return sc ;
    }
    
    sc = m_HitRetrieverAlgorithm -> execute() ;
    
    
    LArHitProducer * producer = dynamic_cast<LArHitProducer*> ( m_HitRetrieverAlgorithm ) ;
    if(producer == NULL ) {
     log<<MSG::FATAL<<"LAr FAILURE : Impossible to down cast "<<m_HitRetrieverNameAndType<<" to LArHitProducer "<<endreq;
     sc = StatusCode::FAILURE ;
     return sc ;
    }
    
    LArHitContainer * hit_container =  producer->hitContainer() ;
    if( hit_container == 0 ) {
    
      log<<MSG::FATAL<<"LAr FAILURE : Null pointer to LArHit Container "<<endreq;
      sc = StatusCode::FAILURE ;
      return sc ;
      
     }
    
    log<<MSG::DEBUG<<" Storing LArHitContainer in TDS"<<endreq; 
    sc = l_storeGateSvc->record(hit_container , m_HitContainerLocation ) ;
    if( sc.isFailure() ){
      log << MSG::FATAL<< "LAr FAILURE : Could not store new LArHitContainer in TDS at location :" << m_HitContainerLocation << endreq;
      sc = StatusCode::FAILURE ;
      return sc;
    }
    
    //print contents of new LArHitContainer 
         
    if( m_DebugFlag != 0 ) { 
      log << MSG::DEBUG << ( (std::string) *hit_container ) << endreq ;
    }
    
    return sc;
}

//******************************************************************************************

//Finalize method
StatusCode LArHitMaker::finalize(){
     MsgStream log( messageService(), name() );
     StatusCode sc = StatusCode::SUCCESS ;
      
     
     log<<MSG::INFO<<"finalize() completed correctly"<<endreq;
     return sc;
}
