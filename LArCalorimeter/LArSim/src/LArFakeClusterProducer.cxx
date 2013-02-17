/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>

#include "LArSim/LArFakeClusterProducer.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/DataHandle.h"

//********************************************************************************

// Constructor
LArFakeClusterProducer::LArFakeClusterProducer(const std::string& name, ISvcLocator* pSvcLocator ) :  LArHitProducer(name, pSvcLocator) ,
 m_Etot(100.),
 m_NumberOfClusters(12),
 m_ClusterSpacing(4),
 m_EtaStart(3),
 m_SampFracEMBPS(0.2),
 m_SampFracEMB1(0.2),
 m_SampFracEMB2(0.2),
 m_SampFracEMB3(0.2)
{
   // assignment of data members from property values 
  declareProperty("Etot", m_Etot); // total energy
  declareProperty("ClusterSpacing",m_ClusterSpacing); // number of cells between 2 clusters in eta
  declareProperty("EtaStart",m_EtaStart); // eta cell of first cluster    
  declareProperty("SampFracEMBPS", m_SampFracEMBPS ) ; // sampling fraction of EMB presampler
  declareProperty("SampFracEMB1", m_SampFracEMB1 ) ; // sampling fraction of EMB first compartment
  declareProperty("SampFracEMB2", m_SampFracEMB2 ) ; // sampling fraction of EMB middle compartment
  declareProperty("SampFracEMB3", m_SampFracEMB3 ) ; // sampling fraction of EMB back compartment
  m_NumberOfClusters = ( ( 54 - m_EtaStart ) / m_ClusterSpacing )  + 1  ; // Number of Clusters in eta                         
}

//************************************************************************************

StatusCode LArFakeClusterProducer::hitConstruction(){
  
  MsgStream log( messageService(), name() );
   
  LArHit * hit ; // Use this Abstract Base Class as much as possible for handling hits 

  StoreGateSvc* detStore;

  StatusCode sc = service("DetectorStore", detStore);
  if (sc.isFailure()){
    log << MSG::ERROR << "cannot access DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

  const DataHandle<CaloIdManager> caloIdMgr;
  sc = detStore->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to retrieve CaloIdManager from DetectoreStore"
        << endreq; 
    return StatusCode::FAILURE;
  }   

  const LArEM_ID* id_generator=caloIdMgr->getEM_ID();
  
  LArHitContainer * hit_container = this -> hitContainer() ; // get pointer to hit container 
  
  Identifier ps_id ; // presampler ID
  Identifier strip_id ; // EM strip ID
  Identifier middle_id ; // EM middle ID
  Identifier back_id ; // EM back ID
  
  double frontE  = 0.05 * m_Etot * m_SampFracEMBPS;
  double stripE  = 0.20 * m_Etot * m_SampFracEMB1 ;
  double middleE = 0.60 * m_Etot * m_SampFracEMB2 ;
  double backE   = 0.15 * m_Etot * m_SampFracEMB3 ; 
  
  for( int i=0 ; i < m_NumberOfClusters ; i++ ) {
  
   int ieta = m_EtaStart + i * m_ClusterSpacing ; 
   
    try{
    
     for(int k=-1 ; k< 2 ; k++) { // EMB PS hits in 3 cells in eta
      double profile = ( k == 0 ? 0.75 : 0.125 ) ; 
      ps_id = id_generator->channel_id( 1, 0 , 0 , ieta+k , 0 ) ; 
      hit = this->newLArHit(ps_id , frontE * profile , 0. ) ; 
      if( hit == 0 ){   
       log<<MSG::FATAL<<"Unable to create a new LArHit for cell : " << id_generator->show_to_string(ps_id) <<endreq ;
       return StatusCode::FAILURE;
      }  
      hit_container->push_back( hit ) ; 
     }
     
     for(int k=-3 ; k < 4 ; k++) { // EMB PS hits in 3 cells in eta
       double profile = 0 ;
       switch ( k*k ) {
          case 9 : 
	   profile = 0.05 ;
	   break ;
	  case 4 :
	   profile = 0.075 ;
	   break ;
	  case 1 :
	   profile = .15 ;
	   break ;
	  case 0 :
	   profile = .45 ;
	   break ;      
       }
       strip_id = id_generator->channel_id( 1, 1 , 0 , (ieta * 8 + 4)+k , 0 ) ;    
       hit = this->newLArHit(strip_id , stripE * profile , 0. ) ; 
       if( hit == 0 ){   // EMB front strips in 7 cells in eta 
         log<<MSG::FATAL<<"Unable to create a new LArHit for cell : " << id_generator->show_to_string(strip_id) <<endreq ;
         return StatusCode::FAILURE;
       }
       hit_container->push_back( hit ) ; 
     }
     
     for(int k=-1 ; k< 2 ; k++) { // EMB middle layer 3 X 3 cluster
      double profile = (k == 0 ? 0.8 : 0.1 ) ;
      for(int p=0 ; p<3 ; p++) {
       double profilep = ((p-1) == 0 ? 0.8 * profile : 0.1 * profile ) ;
       middle_id = id_generator->channel_id( 1, 2 , 0 , ieta + k  , p ) ;
       hit = this->newLArHit(middle_id , middleE * profilep , 0. ) ; 
       if( hit == 0 ){   
         log<<MSG::FATAL<<"Unable to create a new LArHit for cell : " << id_generator->show_to_string(middle_id) <<endreq ;
         return StatusCode::FAILURE;
       }        
       hit_container->push_back( hit ) ;
      }           
     }
     
     
     for(int k=0 ; k < 2 ; k++) { // EMB back layer 
       double profile = ( k== 0 ? 0.2 : 0.8 ) ; 
       for(int p=0 ; p<3 ; p++) {
        double profilep = ((p-1) == 0 ? 0.8 * profile : 0.1 * profile ) ;
        back_id = id_generator->channel_id( 1, 3 , 0 , (ieta / 2) + k , p ) ;     
        hit = this->newLArHit(back_id , backE * profilep , 0. ) ; 
        if( hit == 0 ){   
          log<<MSG::FATAL<<"Unable to create a new LArHit for cell : " << id_generator->show_to_string(back_id) <<endreq ;
          return StatusCode::FAILURE;
        }  
        hit_container->push_back( hit ) ; 
       }
     }
     
     
    }catch(LArID_Exception & except) {
  
     log<<MSG::FATAL<<"Unable to create a new Identifier " << endreq ;
     log<<MSG::FATAL<< (std::string) except << endreq ;
     return StatusCode::FAILURE ;
  
    } 
  
  }

  return StatusCode::SUCCESS;

}
