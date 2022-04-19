/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////

#include "FastCaloSim/FSStoregateClean.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

namespace FastCaloSim {

  FSStoregateClean:: FSStoregateClean(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator)
  {    
    declareProperty("StoreGateKeys",      m_SG_keys);
  }
  
  //__________________________________________________________________________
  FSStoregateClean::~FSStoregateClean()= default;

  //__________________________________________________________________________
  StatusCode FSStoregateClean::initialize()
  {    
    ATH_MSG_DEBUG("initialize()");
    return StatusCode::SUCCESS; 
  }
  
  StatusCode FSStoregateClean::finalize()
  {
    return StatusCode::SUCCESS; 
  }
  
  //_________________________________________________________________________
  StatusCode FSStoregateClean::execute() {
  
    ATH_MSG_DEBUG("execute()");
    
    StatusCode sc;
    
    for(unsigned int i=0;i<m_SG_keys.size();++i) {
      msg(MSG::INFO) << "deleting : "<<m_SG_keys[i]<<" ..."<<endmsg;
      
      const INavigable4MomentumCollection* p = nullptr;
      sc = evtStore()->retrieve(p,m_SG_keys[i]);

      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Unable to retrieve pointer to Object "<<m_SG_keys[i]<< endmsg;
      } else {
        msg(MSG::INFO) <<m_SG_keys[i]<<" at "<<p<<endmsg;
        sc = evtStore()->remove(p);
        if (sc.isFailure()) {
	  msg(MSG::ERROR) << "Unable to delete pointer to Object "<<m_SG_keys[i]<< endmsg;
        } else {
          p=nullptr;
          sc = evtStore()->retrieve(p,m_SG_keys[i]);
	  sc.ignore();
          msg(MSG::INFO) << "deleting "<<m_SG_keys[i]<<" done, test p*="<<p<<endmsg;
        }
      }
    }

//    sc = m_storeGate->retrieve(cellContainer, cellName);

    return StatusCode::SUCCESS;
  }

} // end of namespace bracket



