/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_BUILDER_ALG_H
#define BTAGGING_BUILDER_ALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

namespace FlavorTagDiscriminants {
  class BTaggingBuilderAlg : public AthReentrantAlgorithm
  {
  public:
    BTaggingBuilderAlg(const std::string& name, ISvcLocator* svcloc);
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& cxt) const override;
    virtual StatusCode finalize() override;
  private:

    // Typedef things here in case I ever want to make this more generic.
    using BC = xAOD::BTaggingContainer;
    using JC = xAOD::JetContainer;

    SG::ReadHandleKey<JC> m_jetKey {
      this, "jetCollectionName", "", "name of the jets to b-tag"};
    SG::WriteHandleKey<BC> m_btaggingKey {
      this, "btaggingName", "", "Name for the b-tagging container"};
    SG::WriteDecorHandleKey<BC> m_jetLinkKey {
      this, "jetLinkName", "", "Destination container name + '.jetLink'"};
  };
}

#endif
