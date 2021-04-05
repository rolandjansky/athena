/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_TO_JET_LINKER_ALG_H
#define BTAG_TO_JET_LINKER_ALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"

namespace FlavorTagDiscriminants {
  class BTagToJetLinkerAlg : public AthReentrantAlgorithm
  {
  public:
    BTagToJetLinkerAlg(const std::string& name, ISvcLocator* svcloc);
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& cxt) const override;
    virtual StatusCode finalize() override;
  private:

    // Typedef things here in case I ever want to make this more generic.
    using BC = xAOD::BTaggingContainer;
    using JC = xAOD::JetContainer;

    // Better would be to make everything work with AuxElement, e.g.
    //
    //  using EC = DataVector<SG::AuxElement>;
    //  using BC = EC;
    //  using JC = EC;
    //
    // But I haven't figured out how to make this work (I'd need to
    // assign a class ID, and even then I'm not sure it would be
    // supported).

    SG::ReadDecorHandleKey<JC> m_btagLink {
      this, "jetToBtagLink", "", "Jet container name + . + btaggingLink"};
    SG::WriteDecorHandleKey<BC> m_jetLink {
      this, "btagToJetLink", "", "Link from btag container to jet"};
  };
}

#endif
