// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_FlaggedConditionTestAlg.h
 *
 * @brief Class to test SCT_FlaggedConditionTool (header)
 *
 * @author Susumu.Oda@cern.ch
 **/

#ifndef SCT_FlaggedConditionTestAlg_H
#define SCT_FlaggedConditionTestAlg_H 

//Athena
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "SCT_ConditionsTools//ISCT_FlaggedConditionTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

class SCT_FlaggedConditionTestAlg : public AthReentrantAlgorithm {
 public:
  SCT_FlaggedConditionTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_FlaggedConditionTestAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  ToolHandle<ISCT_FlaggedConditionTool> m_flaggedTool{this, "FlaggedConditionTool", "InDetSCT_FlaggedConditionTool", "Tool to retrieve SCT flagged condition"};
};

#endif // SCT_FlaggedConditionTestAlg_H
