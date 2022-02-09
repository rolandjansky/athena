/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_SEEDJETBUILDER_H
#define DITAUREC_SEEDJETBUILDER_H

#include "DiTauToolBase.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/ReadHandle.h"


class SeedJetBuilder : public DiTauToolBase {
 public:

  //-------------------------------------------------------------
  //! Constructor
  //-------------------------------------------------------------
  SeedJetBuilder(const std::string& type,
		 const std::string& name,
		 const IInterface * parent);

  //-------------------------------------------------------------
  //! Destructor
  //-------------------------------------------------------------
  virtual ~SeedJetBuilder();

  virtual StatusCode initialize() override;

  virtual StatusCode execute(DiTauCandidateData * data,
			     const EventContext& ctx) const override;

 private:
  SG::ReadHandleKey<xAOD::JetContainer> m_jetContainerName
    { this, "JetCollection", "AntiKt10LCTopoJets", "" };
};

#endif  // DITAUREC_SEEDJETBUILDER_H
