/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ConditionsParameterTestAlg.h  Header file for TestConditionsParameter class.
 * @author  Shaun Roe
 **/

// Multiple inclusion protection
#ifndef SCT_ConditionsParameterTestAlg_H
#define SCT_ConditionsParameterTestAlg_H

// Include Athena stuff
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Identifier/Identifier.h"

// Read Handle Key
#include "StoreGate/ReadHandleKey.h"
// Event Info
#include "xAODEventInfo/EventInfo.h"

// Include STL stuff
#include <string>

// Forward declarations
class ISCT_ConditionsParameterSvc;
namespace InDetDD{class SCT_DetectorManager;}

/** This class acts as a test/sample client the ConditionsParameter class.
 */
class SCT_ConditionsParameterTestAlg : public AthAlgorithm {
 public:
  // Structors
  SCT_ConditionsParameterTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_ConditionsParameterTestAlg() = default;
    
  // Standard Gaudi functions
  StatusCode initialize() override; //!< Gaudi initialiser
  StatusCode execute() override;    //!< Gaudi executer
  StatusCode finalize() override;   //!< Gaudi finaliser
    
 private:
  SG::ReadHandleKey<xAOD::EventInfo> m_currentEventKey;  //!< Current event
  ServiceHandle<ISCT_ConditionsParameterSvc> m_conditionsParameterSvc;
  std::string m_histoString;
};

#endif // SCT_ConditionsParameterTestAlg_H
