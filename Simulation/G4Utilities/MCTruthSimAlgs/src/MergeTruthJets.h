/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGETRUTHJETS_H
#define MCTRUTHSIMALGS_MERGETRUTHJETS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

/** @class MergeTruthJets
 *  @brief an algorithm to merge truth jet collections in the overlay store
 */
class MergeTruthJets : public AthAlgorithm {
public:
  MergeTruthJets(const std::string& name, ISvcLocator* svcLoc);
  virtual ~MergeTruthJets() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }
private:
  ToolHandle<IPileUpTool> m_mergeTool{this, "MergeTruthJetsTool", "MergeTruthJetsTool", ""};
};
#endif
