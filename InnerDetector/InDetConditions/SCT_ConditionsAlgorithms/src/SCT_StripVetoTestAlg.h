// -*- C++ -*-

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

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  ToolHandle<ISCT_ConditionsTool> m_stripVetoTool{this, "StripVetoTool", "SCT_StripVetoTool", "Tool to retrieve vetoed strips"};
};

#endif // SCT_StripVetoTestAlg_H
