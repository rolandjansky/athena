/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYPILEUPPARTICLETRUTHINFO_H
#define OVERLAYCOPYALGS_COPYPILEUPPARTICLETRUTHINFO_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTruth/TruthParticleContainer.h"

class CopyPileupParticleTruthInfo : public AthReentrantAlgorithm
{
public:

  CopyPileupParticleTruthInfo(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_bkgInputKey{ this, "BkgInputKey", "", "ReadHandleKey for Background Truth Particle Containers" };
  SG::WriteHandleKey<xAOD::TruthParticleContainer> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output Truth Particle Containers" };

};

#endif // OVERLAYCOPYALGS_COPYPILEUPPARTICLETRUTHINFO_H
