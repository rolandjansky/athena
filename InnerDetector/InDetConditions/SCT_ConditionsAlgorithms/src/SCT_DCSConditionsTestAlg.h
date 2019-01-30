/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file TestDCSConditions.h  Header file for TestDCSConditions class.
 * @author Andree Robichaud-Veronneau, based on code from Shaun Roe
 */

// Multiple inclusion protection
#ifndef SCT_TestDCSConditions_H
#define SCT_TestDCSConditions_H

// Include Athena stuff
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "SCT_ConditionsTools/ISCT_DCSConditionsTool.h"

// Include Gaudi stuff
#include "GaudiKernel/ToolHandle.h"

// Include STL stuff
#include <string>

/** This class acts as a test/sample client the DCSConditions class. 
 */
class SCT_DCSConditionsTestAlg : public AthReentrantAlgorithm {
 public:
  // Structors
  SCT_DCSConditionsTestAlg(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~SCT_DCSConditionsTestAlg() = default;
    
  // Standard Gaudi functions
  StatusCode initialize() override; //!< Gaudi initialiser
  StatusCode execute(const EventContext& ctx) const override;    //!< Gaudi executer
  StatusCode finalize() override;   //!< Gaudi finaliser
    
 private:
  ToolHandle<ISCT_DCSConditionsTool> m_DCSConditionsTool{this, "SCT_DCSConditionsTool", "SCT_DCSConditionsTool", "Tool to retrieve SCT DCS information"};
};

#endif // SCT_TestDCSConditions_H
