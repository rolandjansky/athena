/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ConditionsParameterTestAlg.h  Header file for TestConditionsParameter class.
 * @author  Shaun Roe
 **/

// Multiple inclusion protection
#ifndef SCT_ConditionsParameterTestAlg_H
#define SCT_ConditionsParameterTestAlg_H

// Include Athena stuff
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "SCT_ConditionsTools/ISCT_ConditionsParameterTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

// Include Gaudi stuff
#include "GaudiKernel/ToolHandle.h"

/** This class acts as a test/sample client the ConditionsParameter class.
 */
class SCT_ConditionsParameterTestAlg : public AthReentrantAlgorithm {
 public:
  // Structors
  SCT_ConditionsParameterTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_ConditionsParameterTestAlg() = default;
    
  // Standard Gaudi functions
  StatusCode initialize() override; //!< Gaudi initialiser
  StatusCode execute(const EventContext& ctx) const override;    //!< Gaudi executer
  StatusCode finalize() override;   //!< Gaudi finaliser
    
 private:
  SG::ReadHandleKey<xAOD::EventInfo> m_currentEventKey{this, "EventInfoKey", "EventInfo", "Key of EventInfo"};
  ToolHandle<ISCT_ConditionsParameterTool> m_conditionsParameterTool{this, "SCT_ConditionsParameterTool", "SCT_ConditionsParameterTool", "Tool to retrieve SCT conditions parameters"};
};

#endif // SCT_ConditionsParameterTestAlg_H
