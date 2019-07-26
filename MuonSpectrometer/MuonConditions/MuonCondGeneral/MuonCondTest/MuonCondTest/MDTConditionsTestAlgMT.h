/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTConditionsTestAlgMT_H
#define MDTConditionsTestAlgMT_H 

//STL
#include <string>
#include <sstream>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"

//Athena
#include "Identifier/Identifier.h"
#include "MuonCondInterface/IMDTConditionsSvc.h"
#include "MuonCondData/MdtCondDbData.h"
#include "MuonCondSvc/MuonHierarchy.h"


//Forward declarations
class ISvcLocator;
class StatusCode;
class MdtCondDbData;



///Example class to show calling the SCT_ConditionsSummarySvc
class MDTConditionsTestAlgMT : public AthAlgorithm {

 public:
   MDTConditionsTestAlgMT(const std::string &name,ISvcLocator *pSvcLocator) ;
   virtual ~MDTConditionsTestAlgMT();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();
   
 private:
   SG::ReadCondHandleKey<MdtCondDbData> m_readKey{this, "ReadKey", "MdtCondDbData", "Key of MdtCondDbData"};

}; //end of class

#endif
