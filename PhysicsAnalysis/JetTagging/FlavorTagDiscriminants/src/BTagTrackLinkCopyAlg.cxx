/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagTrackLinkCopyAlg.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/ReadDecorHandle.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace FlavorTagDiscriminants {
  BTagTrackLinkCopyAlg::BTagTrackLinkCopyAlg(
    const std::string& name, ISvcLocator* svcloc):
    AthReentrantAlgorithm(name, svcloc)
  {
  }


  StatusCode BTagTrackLinkCopyAlg::initialize() {
    ATH_CHECK(m_jetTracks.initialize());
    ATH_CHECK(m_btagTracks.initialize());
    ATH_CHECK(m_jetLinks.initialize());
    const auto& key_from_btag_tracks = m_btagTracks.contHandleKey().key();
    const auto& key_from_jet_link = m_jetLinks.contHandleKey().key();
    if (key_from_btag_tracks != key_from_jet_link) {
        ATH_MSG_ERROR(
          "Inconsistent keys for btag tracks '"
          << key_from_btag_tracks << "' and btag -> jet links '"
          << key_from_jet_link << "'");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }


  StatusCode BTagTrackLinkCopyAlg::execute(const EventContext& cxt) const {
    // Note: BC = BTagging container, JC = JetContainer, see the
    // typedefs in the header file

    using TC = xAOD::TrackParticleContainer;
    using IPC = xAOD::IParticleContainer;

    // Note that the jet tracks might have to be changed back to IPC
    // after Bill's megamerge.
    SG::ReadDecorHandle<JC,std::vector<ElementLink<IPC>>> jetTracks(
      m_jetTracks, cxt);
    if (!jetTracks.isValid()) {
      ATH_MSG_ERROR("no incoming tracks " << m_jetTracks.key());
      return StatusCode::FAILURE;
    }
    SG::WriteDecorHandle<BC,std::vector<ElementLink<TC>>> btagTracks(
      m_btagTracks, cxt);

    SG::ReadDecorHandle<BC,ElementLink<JC>> jetLinks(m_jetLinks, cxt);

    for (const SG::AuxElement* btag: *jetLinks) {

      const auto& jet_link = jetLinks(*btag);

      if (!jet_link.isValid()) {
        ATH_MSG_ERROR("invalid jet link!");
        return StatusCode::FAILURE;
      }
      SG::sgkey_t hash_from_jc = m_jetTracks.contHandleKey().hashedKey();
      SG::sgkey_t hash_from_link = jet_link.key();
      if (hash_from_link != hash_from_jc) {
        ATH_MSG_ERROR(
          "Your btag -> jet link points to '" << jet_link.dataID()
          << "' but you're trying to copy tracks from '"
          << m_jetTracks.contHandleKey().key() << "'");
        return StatusCode::FAILURE;
      }
      const SG::AuxElement* jet = *jet_link;

      std::vector<ElementLink<TC> > tmpTracks;
      for (const auto& elpart: jetTracks(*jet)) {
        tmpTracks.push_back(ElementLink<TC>(elpart.key(), elpart.index()));
      }
      btagTracks(*btag) = tmpTracks;

    }
    return StatusCode::SUCCESS;
  }


  StatusCode BTagTrackLinkCopyAlg::finalize() {
    return StatusCode::SUCCESS;
  }

}
