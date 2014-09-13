/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file Test class for SCT_ConfigurationConditionsSvc
 * @author gwilliam@mail.cern.ch
 */

#ifndef SCT_TestConfigConditions_H
#define SCT_TestConfigConditions_H

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Identifier/Identifier.h"

// C++ includes
#include <string>

// Forward declarations
class EventInfo;
class ISCT_ConfigurationConditionsSvc;
class StatusCode;
class ISvcLocator;
class SCT_ID;

namespace InDetDD{class SCT_DetectorManager;}

  /** 
   * This class acts as a test/sample client for the SCT_ConfigurationSConditionsSvc class. 
   */

class SCT_ConfigurationConditionsTestAlg : public AthAlgorithm {
public:
  // Structors
  SCT_ConfigurationConditionsTestAlg (const std::string& name, ISvcLocator* pSvcLocator); 
  ~SCT_ConfigurationConditionsTestAlg();                                                  
  
  // Standard Gaudi functions
  StatusCode initialize(); 
  StatusCode execute();    
  StatusCode finalize();   
  
private:
  ServiceHandle<ISCT_ConfigurationConditionsSvc> m_configConditions;
  const SCT_ID*                                  m_sctId;
  
};

#endif

