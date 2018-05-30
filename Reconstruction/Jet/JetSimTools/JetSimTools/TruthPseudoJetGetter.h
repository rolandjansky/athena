// emacs this file is -*- c++ -*-
// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETSIMTOOLS_TRUTHPSEUDOJETGETTER_H
#define JETSIMTOOLS_TRUTHPSEUDOJETGETTER_H

#include "GaudiKernel/ToolHandle.h"
#include "JetRec/PseudoJetGetter.h"
#include "JetSimTools/JetTruthParticleSelectorTool.h"
#include "xAODTruth/TruthParticleContainer.h"

class TruthPseudoJetGetter : public PseudoJetGetter {
  ASG_TOOL_CLASS(TruthPseudoJetGetter, IPseudoJetGetter)
public:
  TruthPseudoJetGetter(const std::string &name);
  

  virtual StatusCode initialize() override;

  /// Method to construct the PseudoJetVector and record in StoreGate
  virtual StatusCode createAndRecord() const override;

  // Kept for backward compatibity
  virtual const PseudoJetVector* get() const override;


protected:

  /// Input collection name.
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_incolltruth;

  ToolHandle<JetTruthParticleSelectorTool> m_selector;

  /// Output collection name.
  SG::WriteHandleKey<PseudoJetContainer> m_outcoll;      

private:
  const PseudoJetContainer* getC() const;

};

#endif
