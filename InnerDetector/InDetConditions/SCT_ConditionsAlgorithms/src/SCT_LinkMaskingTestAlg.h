/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_LinkMaskingTestAlg.h
*
* @brief 
*  
*
* @author gwilliam@mail.cern.ch
**/

#ifndef SCT_LinkMaskingTestAlg_H
#define SCT_LinkMaskingTestAlg_H 
//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

//Athena
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"

//Forward declarations
class ISCT_ConditionsSvc;

///Example class to show calling the SCT_LinkMaskingSvc
class SCT_LinkMaskingTestAlg : public AthAlgorithm {
 public:
  SCT_LinkMaskingTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_LinkMaskingTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;
   
 private:
  ServiceHandle<ISCT_ConditionsSvc> m_linkMaskingSvc;
}; 

#endif // SCT_LinkMaskingTestAlg_H
