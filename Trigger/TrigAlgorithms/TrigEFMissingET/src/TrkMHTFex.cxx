/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @file TrkMHTFex.cxx
 *
 * Implementation of trkmht fex class
 * @author Jon Burr
 *****************************************************************************/

#include "TrkMHTFex.h"
#include "TrigEFMissingET/METComponent.h"
#include "StoreGate/DecorKeyHelpers.h"
#include "StoreGate/ReadDecorHandle.h"
#include "AthLinks/ElementLink.h"
#include "xAODBase/IParticleContainer.h"
#include <unordered_set>
#include <numeric>

namespace {
  bool isCentral(const xAOD::Jet* ijet) {
    return fabs(ijet->eta() ) < 2.4;
  }
}

namespace HLT { namespace MET {
  TrkMHTFex::TrkMHTFex(const std::string& name, ISvcLocator* pSvcLocator):
    FexBase(name, pSvcLocator) 
  {}

  StatusCode TrkMHTFex::initialize()
  {
    CHECK( m_jetKey.initialize() );
    CHECK( m_trackKey.initialize() );
    CHECK( m_vertexKey.initialize() );
    CHECK( m_tvaKey.initialize() );
    CHECK( m_trackSelTool.retrieve() );

    // Update the decor keys if necessary
      if (m_jvtKey.key().find(".") == std::string::npos)
      m_jvtKey = m_jetKey.key() + "." + m_jvtKey.key();
      else if (SG::contKeyFromKey(m_jvtKey.key()) != m_jetKey.key())
      {
      ATH_MSG_ERROR("JVT container key does not match jet key!");
      return StatusCode::FAILURE;
    }
      CHECK(m_jvtKey.initialize());
    m_trackGA.emplace(m_trackGAName);

    return initializeBase(
        {"JETB1", "JETB2", "JETE1", "JETE2", "TrackSoftTerm"});
  }

  StatusCode TrkMHTFex::fillMET(
      xAOD::TrigMissingET& met,
      const EventContext& context,
      MonGroupBuilder&) const
  {
    // Retrieve the inputs
    auto jets = SG::makeHandle(m_jetKey, context);
      if (!jets.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_jetKey);
        return StatusCode::FAILURE;
      }
    auto tracks = SG::makeHandle(m_trackKey, context);
      if (!tracks.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_trackKey);
        return StatusCode::FAILURE;
      }
    auto vertices = SG::makeHandle(m_vertexKey, context);
      if (!vertices.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_vertexKey);
        return StatusCode::FAILURE;
      }
    auto tva = SG::makeHandle(m_tvaKey, context);
    auto jvtAcc = SG::makeHandle<float>(m_jvtKey, context);
    auto& trackLinksAcc = *m_trackGA;

    // Work out which is the primary vertex
    // I do not know if there is a way to see which vertex was counted as
    // primary by the JVT calculation but I see no reason to suspect that it
    // would be anything other than the one found by this method
    const xAOD::Vertex* priVtx = nullptr;
    for (const xAOD::Vertex* vtx : *vertices)
      if (vtx->vertexType() == xAOD::VxType::PriVtx) {
        priVtx = vtx;
        break;
      }

    // Prepare the output values
    std::array<METComponent, 4> mhtSums;
    METComponent tstSum;

    // If no primary vertex is identified, the algorithm will return 0
    if (priVtx) {
      // Prepare a list of primary vertex tracks
      std::vector<const xAOD::TrackParticle*> tstTracks;
      tstTracks.reserve(tracks->size() );
      for (const xAOD::TrackParticle* itrk : *tracks)
        if (tva->associatedVertex(itrk) == priVtx &&
            itrk->pt() < m_tstPtCeil &&
            m_trackSelTool->accept(*itrk, priVtx) )
          tstTracks.push_back(itrk);
      // Keep indices of tracks associated to hard-scatter jets
      std::unordered_set<std::size_t> jetTrackIndices;
      // Iterate over the jets
      for (const xAOD::Jet* ijet : *jets) {
        if (isCentral(ijet) ) {
          if (ijet->pt() < m_minJvtJetPt)
            // Skip central jets below the JVT pT threshold
            continue;
          if (ijet->pt() < m_maxJvtJetPt && jvtAcc(*ijet) < m_jvtSelection)
            // Skip central jets below the maximum JVT pT threshold and with JVT
            // below threshold
            continue;
        } // end if isCentral(ijet)
        else {
          if (ijet->pt() < m_forwardJetPt)
            // Skip forward jets below the forward jet pT threshold
            continue;
        }
        // The index of the mht METComponent
        std::size_t componentIdx = 0;
        if (ijet->eta() < 0)
          ++componentIdx;
        if (!isCentral(ijet) )
          componentIdx += 2;
        // Add the jet into the correct component
        mhtSums.at(componentIdx) += ijet->p4();
        // Iterate over the ghost-associated tracks
        for (const auto& link : trackLinksAcc(*ijet) ) {
          if (link.getDataPtr() != tracks.ptr() ) {
            ATH_MSG_ERROR("Tracks linked with key '" << m_trackGAName
                << "' do not match tracks retrieved with '"
                << m_trackKey << "'!");
            return StatusCode::FAILURE;
          }
          jetTrackIndices.insert(link.index() );
        }
      }
      // Build up the track soft term
      for (const xAOD::TrackParticle* itrk : tstTracks)
        if (jetTrackIndices.count(itrk->index() ) == 0)
          // Only include tracks that weren't associated to a hard-scatter jet
          tstSum += itrk->p4();
    } //> end if priVtx
    // Save the sum over components
    std::accumulate(mhtSums.begin(), mhtSums.end(), tstSum).fillMET(met);
    // Save each component
    for (std::size_t ii = 0; ii < 4; ++ii)
      mhtSums.at(ii).fillMETComponent(ii, met);
    // Save the tst
    tstSum.fillMETComponent(4, met);

    // Debugging information
    ATH_MSG_DEBUG("Jets: "
        << std::accumulate(mhtSums.begin(), mhtSums.end(), METComponent{}) );

    return StatusCode::SUCCESS;
  }

} } //> end namespace HLT::MET
