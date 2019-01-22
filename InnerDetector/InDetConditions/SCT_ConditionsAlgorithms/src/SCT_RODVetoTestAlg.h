/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoTestAlg.h
 *
 * @brief Header file for the SCT_RODVetoTestAlg class 
 *  in package SCT_ConditionsAlgorithms
 *
 * @author Daiki Hayakawa
 **/

#ifndef SCT_RODVetoTestAlg_H
#define SCT_RODVetoTestAlg_H 

//Athena
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

//Gaudi
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

//STL
#include <string>

///Example algorithm to show calling the SCT_RODVeto to exclude bad components
class SCT_RODVetoTestAlg : public AthReentrantAlgorithm {
 public:
  SCT_RODVetoTestAlg(const std::string &name,ISvcLocator *pSvcLocator);
  virtual ~SCT_RODVetoTestAlg() = default;

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;
   
 private:
  ToolHandle<ISCT_ConditionsTool> m_pRODVetoTool{this, "SCT_RODVetoTool", "SCT_RODVetoTool", "Tool to retrieve bad modules due to vetoed RODs"};
}; //end of class

#endif // SCT_RODVetoTestAlg_H
