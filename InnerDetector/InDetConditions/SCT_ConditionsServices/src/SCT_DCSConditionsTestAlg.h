/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TestDCSConditions.h  Header file for TestDCSConditions class.
* @author Andree Robichaud-Veronneau, based on code from Shaun Roe
*/

// Multiple inclusion protection
#ifndef SCT_TestDCSConditions_H
#define SCT_TestDCSConditions_H

// Include Athena stuff
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Identifier/Identifier.h"

// Include STL stuff
#include <string>

// Forward declarations
class ISCT_DCSConditionsSvc;
class StatusCode;
class ISvcLocator;
namespace InDetDD{class SCT_DetectorManager;}

  /** This class acts as a test/sample client the DCSConditions class. 
  */
  class SCT_DCSConditionsTestAlg : public AthAlgorithm {
public:
    // Structors
    SCT_DCSConditionsTestAlg (const std::string& name, ISvcLocator* pSvcLocator); 
    ~SCT_DCSConditionsTestAlg();                                                  
    
    // Standard Gaudi functions
    StatusCode initialize(); //!< Gaudi initialiser
    StatusCode execute();    //!< Gaudi executer
    StatusCode finalize();   //!< Gaudi finaliser
    
private:
    ServiceHandle<ISCT_DCSConditionsSvc> m_DCSConditionsSvc;
   };

#endif //

