/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ConditionsParameterTestAlg.h  Header file for TestConditionsParameter class.
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
class EventInfo;
class ISCT_SensorsSvc;
class StatusCode;
class ISvcLocator;
namespace InDetDD{class SCT_DetectorManager;}

  /** This class acts as a test/sample client the ConditionsParameter class. 
  */
  class SCT_SensorsTestAlg : public AthAlgorithm {
public:
    // Structors
    SCT_SensorsTestAlg (const std::string& name, ISvcLocator* pSvcLocator); 
    ~SCT_SensorsTestAlg();                                                  
    
    // Standard Gaudi functions
    StatusCode initialize(); //!< Gaudi initialiser
    StatusCode execute();    //!< Gaudi executer
    StatusCode finalize();   //!< Gaudi finaliser
    
private:
    ServiceHandle<ISCT_SensorsSvc> m_SensorsSvc;
   };

#endif //

