/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_RODVetoTestWriteAlg.h
*
* @brief Header file for the SCT_RODVetoTestWriteAlg class 
*  in package SCT_ConditionsServices
*
* @author Daiki Hayakawa
**/

#ifndef SCT_RODVetoTestWriteAlg_H
#define SCT_RODVetoTestWriteAlg_H 
// STL
#include <string>
#include <vector>

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandle.h"

// Local
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

//Forward declarations
class ISvcLocator;
class StatusCode;

/// Algorithm needs to show calling the SCT_RODVeto to exclude bad components
class SCT_RODVetoTestWriteAlg : public AthAlgorithm {
 public:
  SCT_RODVetoTestWriteAlg(const std::string &name, ISvcLocator *pSvcLocator) ;
 ~SCT_RODVetoTestWriteAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
  SG::WriteHandle<std::vector<unsigned int>> m_badRODElements;
  std::vector<unsigned int> m_badRODElementsInput;
}; //end of class

#endif // SCT_RODVetoTestWriteAlg_H
