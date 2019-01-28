/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_ConditionsData/IdentifierSet.h"
#include "StoreGate/WriteHandleKey.h"

// STL
#include <string>
#include <vector>

// Forward declarations
class SCT_ID;

/// Algorithm needs to show calling the SCT_RODVeto to exclude bad components
class SCT_RODVetoCondAlg : public AthReentrantAlgorithm {
 public:
  SCT_RODVetoCondAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~SCT_RODVetoCondAlg() = default;

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;
   
 private:
  ToolHandle<ISCT_CablingTool> m_cabling{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};
  const SCT_ID* m_pHelper;
  SG::WriteHandleKey<IdentifierSet> m_badIds{this, "BadModuleIds", "BadSCTModuleIds_RODVeto", "Write key for bad module identifiers"};
  UnsignedIntegerArrayProperty m_badRODElementsInput{this, "BadRODIds", {}, "Input list of RODs to be vetoed"};
}; //end of class

#endif // SCT_RODVetoCondAlg_H
