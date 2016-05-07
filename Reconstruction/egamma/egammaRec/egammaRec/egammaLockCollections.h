/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAREC_EGAMMALOCKCOLLECTIONS_H
#define EGAMMAREC_EGAMMALOCKCOLLECTIONS_H
/**
  @class egammaLockCollections 
*/
// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>

class egammaLockCollections : public AthAlgorithm{
 public:
  /** @brief Default constructor*/
  egammaLockCollections(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** @brief Destructor*/
  ~egammaLockCollections();

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute();

 private:
  std::string m_outputElectronKey;
  std::string m_outputPhotonKey;
  std::string m_outputForwardElectronKey;
  
  template<typename T>
    StatusCode impl(const std::string& name) const {
    if (evtStore()->contains<T>(name)){
      T* output=0;   
      ATH_CHECK(evtStore()->retrieve(output,name));
      ATH_CHECK(evtStore()->setConst(output));
    }
    return StatusCode::SUCCESS;
  }
};

#endif









