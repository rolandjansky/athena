/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTRUTHDECORATION_H
#define PILEUPTRUTHDECORATION_H

// Base class include
#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <GeneratorObjects/McEventCollection.h>
#include <xAODBase/IParticleContainer.h>
#include <StoreGate/WriteDecorHandleKey.h>

class PileUpTruthDecoration : public AthReentrantAlgorithm
{
public:
  PileUpTruthDecoration(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

private:
  virtual float getPVtxZ(SG::ReadHandle<McEventCollection>& inputCollection) const;

  SG::ReadHandleKey<McEventCollection> m_inputTruthCollectionKey {this, "InputTruthCollection", "TruthEventOLD", "Input truth collection name"};
  SG::ReadHandleKey<xAOD::IParticleContainer> m_particleKey{this, "InputParticleContainer", "", "Input particle collection name"};
  SG::WriteDecorHandleKey<xAOD::IParticleContainer> m_decKey{this, "OutputDecoration", "", "Output decoration name"};
};

#endif
