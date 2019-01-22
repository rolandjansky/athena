/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_StripVetoTestAlg.h
 *
 * @brief Class to test SCT_StripVetoTool (header)
 *
 * @author Susumu.Oda@cern.ch
 **/

#ifndef SCT_StripVetoTestAlg_H
#define SCT_StripVetoTestAlg_H 

//Athena
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

class SCT_StripVetoTestAlg : public AthReentrantAlgorithm {
 public:
  SCT_StripVetoTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_StripVetoTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;
   
 private:
  ToolHandle<ISCT_ConditionsTool> m_stripVetoTool{this, "StripVetoTool", "SCT_StripVetoTool", "Tool to retrieve vetoed strips"};
};

#endif // SCT_StripVetoTestAlg_H
