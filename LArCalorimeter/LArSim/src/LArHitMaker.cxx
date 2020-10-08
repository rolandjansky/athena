/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSim/LArHitMaker.h"
#include "LArSim/LArHitProducer.h"

#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"

#include "StoreGate/StoreGateSvc.h"


//********************************************************************************

// Constructor
LArHitMaker::LArHitMaker(const std::string& name, ISvcLocator* pSvcLocator ) :
 AthLegacySequence(name , pSvcLocator),
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
    // Get sub-algo Type and instance name
    std::string type = findType(m_HitRetrieverNameAndType);
    std::string name = findName(m_HitRetrieverNameAndType);
    ATH_MSG_INFO("Sub-Algorithm type="<<type<<" and instance name="<<name);
    ATH_MSG_INFO("location of stored hits="<<m_HitContainerLocation);
    
    // Instanciate LArHitProducer
    ATH_CHECK(  createSubAlgorithm(type, name, m_HitRetrieverAlgorithm) );
    return StatusCode::SUCCESS;
}

//*****************************************************************************************

//Execute method
StatusCode LArHitMaker::execute(){
  ATH_MSG_DEBUG(" Start executing execute() method of "<< m_HitRetrieverNameAndType );
    
    ATH_CHECK( m_HitRetrieverAlgorithm -> execute(Gaudi::Hive::currentContext()) );
    
    LArHitProducer * producer = dynamic_cast<LArHitProducer*> ( m_HitRetrieverAlgorithm ) ;
    if(producer == NULL ) {
      ATH_MSG_FATAL("LAr FAILURE : Impossible to down cast "<<m_HitRetrieverNameAndType<<" to LArHitProducer ");
      return StatusCode::FAILURE ;
    }
    
    LArHitContainer * hit_container =  producer->hitContainer() ;
    if( hit_container == 0 ) {
    
      ATH_MSG_FATAL("LAr FAILURE : Null pointer to LArHit Container ");
      return StatusCode::FAILURE ;
      
     }
    
    ATH_MSG_DEBUG(" Storing LArHitContainer in TDS");
    ATH_CHECK( evtStore()->record(hit_container , m_HitContainerLocation ) );
    
    //print contents of new LArHitContainer 
         
    if( m_DebugFlag != 0 ) { 
      ATH_MSG_DEBUG ( ( (std::string) *hit_container ) );
    }
    
    return StatusCode::SUCCESS;
}

//******************************************************************************************

//Finalize method
StatusCode LArHitMaker::finalize(){
  ATH_MSG_INFO("finalize() completed correctly");
  return StatusCode::SUCCESS;
}
