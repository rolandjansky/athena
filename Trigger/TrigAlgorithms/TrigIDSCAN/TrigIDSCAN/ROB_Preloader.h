/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROB_PRELOADER_H
#define ROB_PRELOADER_H

#include <string> 
#include <vector>                               
#include "GaudiKernel/Algorithm.h"

class IROBDataProviderSvc;
class StoreGateSvc;

class ROB_Preloader : public Algorithm 
{
 public:
  
  ROB_Preloader(const std::string& name, ISvcLocator* pSvcLocator);
  ~ROB_Preloader();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  StoreGateSvc*        m_storeGate;
  std::string m_ROBDataProviderSvcName;
  IROBDataProviderSvc* m_robDataProvider;
  std::vector<unsigned int> m_uIntListOfPixelRobs;
  std::vector<unsigned int> m_uIntListOfSCT_Robs;

};


#endif
