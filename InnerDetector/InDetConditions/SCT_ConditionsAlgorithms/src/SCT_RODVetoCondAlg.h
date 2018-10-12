/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoCondAlg.h
 *
 * @brief Header file for the SCT_RODVetoCondAlg class 
 *  in package SCT_ConditionsAlgorithms
 *
 * @author Susumu Oda
 **/

#ifndef SCT_RODVetoCondAlg_H
#define SCT_RODVetoCondAlg_H 

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"

#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_ConditionsData/IdentifierSet.h"
#include "StoreGate/WriteHandle.h"

// STL
#include <string>
#include <vector>

// Forward declarations
class SCT_ID;

/// Algorithm needs to show calling the SCT_RODVeto to exclude bad components
class SCT_RODVetoCondAlg : public AthAlgorithm {
 public:
  SCT_RODVetoCondAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~SCT_RODVetoCondAlg() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;
   
 private:
  ToolHandle<ISCT_CablingTool> m_cabling{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};
  const SCT_ID* m_pHelper;
  SG::WriteHandle<IdentifierSet> m_badIds;
  std::vector<unsigned int> m_badRODElementsInput;
}; //end of class

#endif // SCT_RODVetoCondAlg_H
