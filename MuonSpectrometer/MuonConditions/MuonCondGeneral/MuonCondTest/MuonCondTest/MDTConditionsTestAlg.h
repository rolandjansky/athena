/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTConditionsTestAlg_H
#define MDTConditionsTestAlg_H 
//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

//Athena

//#include "MuonCondInterface/IMuonConditionsSvc.h"
#include "MuonCondInterface/IMDTConditionsSvc.h"

//Forward declarations
class ISvcLocator;
class StatusCode;

///Example class to show calling the SCT_ConditionsSummarySvc
class MDTConditionsTestAlg : public AthAlgorithm {
 public:
  MDTConditionsTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
 ~MDTConditionsTestAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
  ServiceHandle<IMDTConditionsSvc> m_pSummarySvc;
}; //end of class

#endif
