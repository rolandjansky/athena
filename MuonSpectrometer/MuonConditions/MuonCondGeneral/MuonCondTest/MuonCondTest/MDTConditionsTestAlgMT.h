/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTConditionsTestAlgMT_H
#define MDTConditionsTestAlgMT_H 

//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

//Athena
#include "MuonCondInterface/IMDTConditionsSvc.h"
#include "MuonCondData/MdtCondDbData.h"


//Forward declarations
class ISvcLocator;
class StatusCode;
class MdtCondDbData;



///Example class to show calling the SCT_ConditionsSummarySvc
class MDTConditionsTestAlgMT : public AthAlgorithm {

 public:
  MDTConditionsTestAlgMT(const std::string &name,ISvcLocator *pSvcLocator) ;
 ~MDTConditionsTestAlgMT();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
  SG::ReadCondHandleKey<MdtCondDbData> m_readKey{this, "ReadKey", "MdtCondDbData", "Key of MdtCondDbData"};

}; //end of class

#endif
