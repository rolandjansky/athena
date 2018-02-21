/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoTestWriteAlg.h
 *
 * @brief Header file for the SCT_RODVetoTestWriteAlg class 
 *  in package SCT_ConditionsAlgorithms
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

/// Algorithm needs to show calling the SCT_RODVeto to exclude bad components
class SCT_RODVetoTestWriteAlg : public AthAlgorithm {
 public:
  SCT_RODVetoTestWriteAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~SCT_RODVetoTestWriteAlg() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;
   
 private:
  SG::WriteHandle<std::vector<unsigned int>> m_badRODElements;
  std::vector<unsigned int> m_badRODElementsInput;
}; //end of class

#endif // SCT_RODVetoTestWriteAlg_H
