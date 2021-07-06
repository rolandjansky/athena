/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGETRUTHPARTICLES_H
#define MCTRUTHSIMALGS_MERGETRUTHPARTICLES_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

/** @class MergeTruthParticles
 *  @brief an algorithm to merge truth jet collections in the overlay store
 */
class MergeTruthParticles : public AthAlgorithm {
public:
  MergeTruthParticles(const std::string& name, ISvcLocator* svcLoc);
  virtual ~MergeTruthParticles() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }
private:
  ToolHandle<IPileUpTool> m_mergeTool{this, "MergeTruthParticlesTool", "MergeTruthParticlesTool", ""};
};
#endif
