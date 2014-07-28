/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////

#include "FastCaloSim/FSStoregateClean.h"

// Gaudi includes
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ListItem.h"

#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

namespace FastCaloSim {

  FSStoregateClean:: FSStoregateClean(const std::string& name, ISvcLocator* pSvcLocator):
    Algorithm(name,pSvcLocator)
  {
    
    declareProperty("StoreGateKeys",      m_SG_keys);
  }
  
  //__________________________________________________________________________
  FSStoregateClean::~FSStoregateClean(){
    MsgStream m_log( messageService(), name() );
  }

  //__________________________________________________________________________
  StatusCode FSStoregateClean::initialize()
  {
    
    MsgStream m_log( messageService(), name() );

    m_log << MSG::DEBUG << "initialize()" << endreq;

    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
       m_log << MSG::ERROR
            << "Unable to retrieve pointer to StoreGateSvc"
            << endreq;
       return sc;
    }

    return StatusCode::SUCCESS; 
  }
  
  StatusCode FSStoregateClean::finalize()
  {
    
    MsgStream m_log( messageService(), name() );
    return StatusCode::SUCCESS; 
  }
  
  //_________________________________________________________________________
  StatusCode FSStoregateClean::execute()
  {
    MsgStream m_log( messageService(), name() );

    m_log << MSG::DEBUG << "execute()" << endreq;
    
    StatusCode sc;
    
    for(unsigned int i=0;i<m_SG_keys.size();++i) {
      m_log << MSG::INFO << "deleting : "<<m_SG_keys[i]<<" ..."<<endreq;
      
      const INavigable4MomentumCollection* p = 0;
      sc = m_storeGate->retrieve(p,m_SG_keys[i]);

      if (sc.isFailure()) {
         m_log << MSG::ERROR << "Unable to retrieve pointer to Object "<<m_SG_keys[i]<< endreq;
      } else {
        m_log << MSG::INFO <<m_SG_keys[i]<<" at "<<p<<endreq;
        sc = m_storeGate->remove(p);
        if (sc.isFailure()) {
           m_log << MSG::ERROR << "Unable to delete pointer to Object "<<m_SG_keys[i]<< endreq;
        } else {
          p=0;
          sc = m_storeGate->retrieve(p,m_SG_keys[i]);
          m_log << MSG::INFO << "deleting "<<m_SG_keys[i]<<" done, test p*="<<p<<endreq;
        }
      }
    }

//    sc = m_storeGate->retrieve(cellContainer, cellName);

    return StatusCode::SUCCESS;
  }

} // end of namespace bracket



