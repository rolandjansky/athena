/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_SensorsTestAlg.h Header file for SCT_SensorsTestAlg class.
 * @author  Shaun Roe
 **/

// Multiple inclusion protection
#ifndef SCT_SensorsTestAlg_H
#define SCT_SensorsTestAlg_H

// Include Athena stuff
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/Identifier.h"
#include "SCT_ConditionsTools/ISCT_SensorsTool.h"

// Include STL stuff
#include <string>

/** This class acts as a test/sample client the ConditionsParameter class. 
 */
class SCT_SensorsTestAlg : public AthAlgorithm {
 public:
  // Structors
  SCT_SensorsTestAlg (const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~SCT_SensorsTestAlg() = default;
    
  // Standard Gaudi functions
  StatusCode initialize() override; //!< Gaudi initialiser
  StatusCode execute() override;    //!< Gaudi executer
  StatusCode finalize() override;   //!< Gaudi finaliser
    
 private:
  ToolHandle<ISCT_SensorsTool> m_SensorsTool{this, "SCT_SensorsTool", "SCT_SensorsTool", "Tool to retrieve SCT sensor information"};
};

#endif // SCT_SensorsTestAlg_H
