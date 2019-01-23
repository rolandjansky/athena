/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file Test class for SCT_ConfigurationConditionsSvc
 * @author gwilliam@mail.cern.ch
 */

#ifndef SCT_TestConfigConditions_H
#define SCT_TestConfigConditions_H

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// C++ includes
#include <string>

// Forward declarations
class SCT_ID;

/** 
 * This class acts as a test/sample client for the SCT_ConfigurationSConditionsSvc class. 
 */

class SCT_ConfigurationConditionsTestAlg : public AthReentrantAlgorithm {
 public:
  // Structors
  SCT_ConfigurationConditionsTestAlg (const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~SCT_ConfigurationConditionsTestAlg() = default;
  
  // Standard Gaudi functions
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;
  
 private:
  ToolHandle<ISCT_ConfigurationConditionsTool> m_configConditions{this, "SCT_ConfigurationConditionsTool", "SCT_ConfigurationConditionsTool", "Tool to retrieve SCT DAQ configuration"};
  const SCT_ID* m_sctId;
};

#endif // SCT_TestConfigConditions_H
