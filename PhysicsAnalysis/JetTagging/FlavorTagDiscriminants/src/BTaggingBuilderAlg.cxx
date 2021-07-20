/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTaggingBuilderAlg.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "StoreGate/WriteDecorHandle.h"


namespace FlavorTagDiscriminants {
  BTaggingBuilderAlg::BTaggingBuilderAlg(
    const std::string& name, ISvcLocator* svcloc):
    AthReentrantAlgorithm(name, svcloc)
  {
  }


  StatusCode BTaggingBuilderAlg::initialize() {
    ATH_CHECK(m_jetKey.initialize());
    ATH_CHECK(m_btaggingKey.initialize());
    m_jetLinkKey = m_btaggingKey.key() + ".jetLink";
    ATH_CHECK(m_jetLinkKey.initialize());
    return StatusCode::SUCCESS;
  }


  StatusCode BTaggingBuilderAlg::execute(const EventContext& cxt) const {
    // Note: BC = BTagging container, JC = JetContainer, see the
    // typedefs in the header file

    SG::ReadHandle<JC> jets(m_jetKey, cxt);
    SG::WriteHandle<BC> btagging(m_btaggingKey, cxt);
    ATH_CHECK(btagging.record(
                std::make_unique<xAOD::BTaggingContainer>(),
                std::make_unique<xAOD::BTaggingAuxContainer>())
      );
    SG::WriteDecorHandle<BC,ElementLink<JC>> jetLinks(m_jetLinkKey, cxt);

    for (const xAOD::Jet* jet: *jets) {

      auto btag = std::make_unique<xAOD::BTagging>();
      jetLinks(*btag) = ElementLink<JC>(m_jetKey.hashedKey(), jet->index());
      btagging->push_back(std::move(btag));

    }
    return StatusCode::SUCCESS;
  }

  StatusCode BTaggingBuilderAlg::finalize() {
    return StatusCode::SUCCESS;
  }

}
