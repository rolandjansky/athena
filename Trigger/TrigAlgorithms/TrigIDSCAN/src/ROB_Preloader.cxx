/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************                               
                               
NAME:     ROB_Preloader.cxx                               
PACKAGE:                                 
                               
AUTHORS: J. Baines        john.baines@rl.ac.uk based on TrigSteer/Lvl1Converstion by
         G. Comune        gianluca.comune@cern.ch (http://cern.ch/gcomune)

CREATED:  Nov, 2002                               
                               
PURPOSE:  Fake Lvl1 RoI for seeding Lvl2
                               
Modified :
         
                               
********************************************************************/                               
                                  
#include <vector>                               
#include <utility>

// INCLUDE GAUDI HEADER FILES:                                     
#include "GaudiKernel/MsgStream.h"                               
#include "GaudiKernel/Property.h"                               
#include "GaudiKernel/ISvcLocator.h"                               

#include "StoreGate/StoreGateSvc.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigIDSCAN/ROB_Preloader.h"
                    
      
////////////////////////////////////////////////////////////////                               
//  CONSTRUCTOR:                               
                                   
ROB_Preloader::ROB_Preloader(const std::string& name, ISvcLocator* pSvcLocator):                                
  Algorithm(name, pSvcLocator)  
{                               
  declareProperty( "ROBDataProviderSvcName", m_ROBDataProviderSvcName = "ROBDataProviderSvc" );
  declareProperty( "PixelRoBlist", m_uIntListOfPixelRobs);
  declareProperty( "SCT_RoBlist", m_uIntListOfSCT_Robs);

}                               
                               
// DESTRUCTOR:                               
                                  
ROB_Preloader::~ROB_Preloader()                               
{ }                               
                               
/////////////////////////////////////////////////////////////////                               
// INITIALIZE METHOD:                               
                                      
StatusCode ROB_Preloader::initialize()                               
{                               
  MsgStream athenaLog(msgSvc(), name());

//   Get StoreGate service
//
  StatusCode sc = service( "StoreGateSvc", m_storeGate );
  if (sc.isFailure()) {
    athenaLog << MSG::FATAL 
	      << "Unable to retrieve StoreGate service" << endreq;
    return sc;
  }
    
  //  Get ROBDataProvider
  sc = service( m_ROBDataProviderSvcName, m_robDataProvider );
  if( sc.isFailure() ) {
    athenaLog << MSG::FATAL << " Can't get ROBDataProviderSvc " << endreq;
    return sc;
  }
  if (m_uIntListOfPixelRobs.size() > 0) {
    athenaLog << MSG::INFO<< " Pre-loading the following pixel RoBs: " << endreq;
    for ( std::vector<unsigned int>::iterator iPixRob=m_uIntListOfPixelRobs.begin() ; 
	  iPixRob!=m_uIntListOfPixelRobs.end() ;  iPixRob++) {
      athenaLog << MSG::INFO<< *iPixRob << " == 0x" << std::hex << *iPixRob << std::dec << endreq;
    }
  } else {
    athenaLog << MSG::INFO<< " No Pixel RoBs to pre-load " << endreq;
  }
  if (m_uIntListOfSCT_Robs.size() > 0) {
    athenaLog << MSG::INFO<< " Pre-loading the  following SCT RoBs: " << endreq;
    for ( std::vector<unsigned int>::iterator iSctRob=m_uIntListOfSCT_Robs.begin() ; 
	  iSctRob!=m_uIntListOfSCT_Robs.end() ;  iSctRob++) {
      athenaLog << MSG::INFO << *iSctRob << " == 0x" << std::hex << *iSctRob << std::dec << endreq;
    }
  } else {
    athenaLog << MSG::INFO<< " No SCT RoBs to pre-load " << endreq;
  }
     
  return StatusCode::SUCCESS;                               
}                               
                               
/////////////////////////////////////////////////////////////////                               
// FINALIZE METHOD:                               
                                 
StatusCode ROB_Preloader::finalize() { return StatusCode::SUCCESS; }                               
                               
/////////////////////////////////////////////////////////////////                               
// EXECUTE METHOD:                               
                                 
StatusCode ROB_Preloader::execute()                               
{                                
  MsgStream athenaLog(msgSvc(), name());

  if (m_uIntListOfPixelRobs.size() > 0) {
    athenaLog << MSG::DEBUG<< " Pre-loading the following pixel RoBs: " << endreq;
    for ( std::vector<unsigned int>::iterator iPixRob=m_uIntListOfPixelRobs.begin() ; 
	  iPixRob!=m_uIntListOfPixelRobs.end() ;  iPixRob++) {
      athenaLog << MSG::DEBUG<< *iPixRob << endreq;
    }
    m_robDataProvider->addROBData( m_uIntListOfPixelRobs );
  } else {
    athenaLog << MSG::DEBUG<< " No Pixel RoBs to pre-load " << endreq;
  }
  if (m_uIntListOfSCT_Robs.size() > 0) {
    athenaLog << MSG::DEBUG<< " Pre-loading the  following SCT RoBs: " << endreq;
    for ( std::vector<unsigned int>::iterator iSctRob=m_uIntListOfSCT_Robs.begin() ; 
	  iSctRob!=m_uIntListOfSCT_Robs.end() ;  iSctRob++) {
      athenaLog << MSG::DEBUG<< *iSctRob << endreq;
    }
    m_robDataProvider->addROBData( m_uIntListOfSCT_Robs );
  } else {
    athenaLog << MSG::DEBUG<< " No SCT RoBs to pre-load " << endreq;
  }

  return StatusCode::SUCCESS;
}



       

      
      


