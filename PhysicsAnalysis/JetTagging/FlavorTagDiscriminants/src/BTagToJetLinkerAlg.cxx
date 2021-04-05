/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagToJetLinkerAlg.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/ReadDecorHandle.h"

namespace FlavorTagDiscriminants {
  BTagToJetLinkerAlg::BTagToJetLinkerAlg(
    const std::string& name, ISvcLocator* svcloc):
    AthReentrantAlgorithm(name, svcloc)
  {
  }


  StatusCode BTagToJetLinkerAlg::initialize() {
    ATH_CHECK(m_btagLink.initialize());
    ATH_CHECK(m_jetLink.initialize());
    return StatusCode::SUCCESS;
  }


  StatusCode BTagToJetLinkerAlg::execute(const EventContext& cxt) const {
    // Note: BC = BTagging container, JC = JetContainer, see the
    // typedefs in the header file
    SG::ReadDecorHandle<JC,ElementLink<BC>> btagLinks(m_btagLink, cxt);
    if (!btagLinks.isValid()) {
      ATH_MSG_ERROR("no btag container " << btagLinks.key());
      return StatusCode::FAILURE;
    }
    SG::WriteDecorHandle<BC,ElementLink<JC>> jetLinks(m_jetLink, cxt);

    for (const SG::AuxElement* jet: *btagLinks) {

      const auto& btag_link = btagLinks(*jet);

      // let's check to make sure the links are valid and that they
      // point where we think they do
      if (!btag_link.isValid()) {
        ATH_MSG_ERROR("invalid btag link!");
        return StatusCode::FAILURE;
      }
      SG::sgkey_t hashFromContainer = m_jetLink.contHandleKey().hashedKey();
      SG::sgkey_t hashFromLink = btag_link.key();
      if (hashFromLink != hashFromContainer) {
        ATH_MSG_ERROR(
          "Your jet -> btagging link points to '" << btag_link.dataID()
          << "' but you're trying to add this link to '"
          << m_jetLink.contHandleKey().key() << "'");
        return StatusCode::FAILURE;
      }

      const SG::AuxElement* btag = *btag_link;
      ElementLink<JC> jetLink(m_btagLink.contHandleKey().hashedKey(),
                              jet->index(), cxt);
      jetLinks(*btag) = jetLink;

    }
    return StatusCode::SUCCESS;
  }


  StatusCode BTagToJetLinkerAlg::finalize() {
    return StatusCode::SUCCESS;
  }

}
