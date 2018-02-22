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
// STL
#include <string>
#include <vector>
#include <set>

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/Identifier.h"
#include "StoreGate/WriteHandle.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_ConditionsData/IdentifierSet.h"

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
  ServiceHandle<ISCT_CablingSvc> m_cabling;
  const SCT_ID* m_pHelper;
  SG::WriteHandle<std::vector<unsigned int>> m_badRODElements;
  SG::WriteHandle<IdentifierSet> m_badIds;
  std::vector<unsigned int> m_badRODElementsInput;
}; //end of class

#endif // SCT_RODVetoCondAlg_H
