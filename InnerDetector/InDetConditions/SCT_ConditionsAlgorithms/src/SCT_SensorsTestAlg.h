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
#include "GaudiKernel/ServiceHandle.h"
#include "Identifier/Identifier.h"

// Include STL stuff
#include <string>

// Forward declarations
class ISCT_SensorsSvc;

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
  ServiceHandle<ISCT_SensorsSvc> m_SensorsSvc;
};

#endif // SCT_SensorsTestAlg_H
