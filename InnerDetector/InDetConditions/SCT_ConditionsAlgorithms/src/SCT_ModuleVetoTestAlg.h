// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_ModuleVetoTestAlg.h
*
* @brief Header file for the SCT_ModuleVetoTestAlg class 
*  in package SCT_ConditionsAlgorithms
*
* @author Shaun Roe
**/

#ifndef SCT_ModuleVetoTestAlg_H
#define SCT_ModuleVetoTestAlg_H 

//Athena
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//STL
#include <string>

///Example algorithm to show calling the SCT_ModuleVetoSvc to exclude bad components
class SCT_ModuleVetoTestAlg : public AthReentrantAlgorithm {
 public:
  SCT_ModuleVetoTestAlg(const std::string &name,ISvcLocator *pSvcLocator) ;
  virtual ~SCT_ModuleVetoTestAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  ToolHandle<ISCT_ConditionsTool> m_pModuleVetoTool{this, "ModuleVetoTool", "SCT_ModuleVetoTool", "Tool to retrieve vetoed modules"};
}; //end of class

#endif // SCT_ModuleVetoTestAlg_H
