/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODBTagging/BTagging.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "DerivationFrameworkTop/TrkJetTrackThinning.h"

namespace DerivationFramework {
  TrkJetTrackThinning::TrkJetTrackThinning(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_thinningSvc("ThinningSvc", name) {
    // Declare the tool's interface to the framework:
    declareInterface<DerivationFramework::IThinningTool>(this);

    // Set up the tool's properties:
    declareProperty("ThinningService", m_thinningSvc,
                    "The thinning service to use");
    declareProperty("TrkJetKey", m_trkjetSGKey,
                    "StoreGate key of the jet container to use");
    declareProperty("InDetTrackParticlesKey", m_inDetSGKey,
                    "StoreGate key of the track particle container");
    declareProperty("TrkJetSelectionString", m_selectionString,
                    "Selection string for the track jets");
    declareProperty("EventSelectionString", m_event_selectionString,
                    "Seletion string for the whole event");
    declareProperty("ThinConstituents", m_thinConstituents,
                    "Keep track jet constituents");
    declareProperty("ThinJetFitterTracks", m_thinJetFitter,
                    "Keep track jet JetFitter vertices' tracks");
    declareProperty("ThinSV1Tracks", m_thinSV1,
                    "Keep track jet SV1 vertex tracks");
    declareProperty("ApplyAnd", m_and,
                    "Use IThinningSvc::Operator::And instead of "
                    "IThinningSvc::Operator::Or");
  }

  StatusCode TrkJetTrackThinning::initialize() {
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_inDetSGKey == "") {
      ATH_MSG_FATAL("No inner detector track collection provided for thinning.");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Using " << m_inDetSGKey
                          << " as the source collection for inner detector track particles");

    if (m_trkjetSGKey == "") {
      ATH_MSG_FATAL("No track jet collection provided for thinning.");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Inner detector track particles asssociated with "
                 << m_trkjetSGKey << " thinning. Will retain: ");
    if (m_thinConstituents) ATH_MSG_INFO("Keeping track jet constituents.");
    if (m_thinJetFitter) ATH_MSG_INFO("Keeping track jet JetFitter vertices' tracks.");
    if (m_thinSV1) ATH_MSG_INFO("Keeping track jet SV1 vertex tracks.");

    // Set up the text-parsing machinery for selectiong the jet directly
    // according to user cuts
    if (m_selectionString != "") {
      m_trkjet_parser.reset(new ExpressionParserHelper(evtStore()));
      if (!m_trkjet_parser->parser().loadExpression(m_selectionString)) {
        ATH_MSG_FATAL("Failed to interpret expression: "
                      << m_selectionString);
        return StatusCode::FAILURE;
      }
    }

    if (m_event_selectionString != "") {
      m_event_parser.reset(new ExpressionParserHelper(evtStore()));
      if (!m_event_parser->parser().loadExpression(m_event_selectionString)) {
        ATH_MSG_FATAL("Failed to interpret expression: "
                      << m_event_selectionString);
        return StatusCode::FAILURE;
      }
    }

    // Access the thinning service:
    ATH_CHECK(m_thinningSvc.retrieve());

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode TrkJetTrackThinning::finalize() {
    // Print some statistics:
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed " << m_ntot << " tracks, " << m_npass
                              << " were retained ");

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode TrkJetTrackThinning::doThinning() const {
    // Retrieve main TrackParticle collection
    const xAOD::TrackParticleContainer* importedTrackParticles = nullptr;

    ATH_CHECK(evtStore()->retrieve(importedTrackParticles, m_inDetSGKey));

    // Check that the event contains tracks
    const size_t nTracks = importedTrackParticles->size();
    if (nTracks == 0) {
      return StatusCode::SUCCESS;
    }

    // Set up a mask with the same entries as the full TrackParticle
    // collection
    std::vector<bool> mask;
    mask.assign(nTracks, false); // default: don't keep any tracks
    m_ntot += nTracks;

    // first evaluate the event selection expression
    // if it is false, then there's no use doing anything else
    // just run the thinning expression with all masks false
    if (m_event_parser) {
      bool result = m_event_parser->parser().evaluateAsBool();
      if (!result) {
        // Tell the thinning service what to do:
        const IThinningSvc::Operator::Type opType = (m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or);
        ATH_CHECK(m_thinningSvc->filter(*importedTrackParticles, mask, opType));
        return StatusCode::SUCCESS;
      }
    }

    // Retrieve the jet container:
    const xAOD::JetContainer* importedJets = nullptr;
    ATH_CHECK(evtStore()->retrieve(importedJets, m_trkjetSGKey));
    const size_t nJets = importedJets->size();

    // Select the jets to use:
    std::vector<const xAOD::Jet*> jetsToCheck;
    if (m_trkjet_parser) {
      const std::vector<int> entries = m_trkjet_parser->parser().evaluateAsVector();
      const size_t nEntries = entries.size();
      // check the sizes are compatible
      if (nJets != nEntries) {
        ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection "
                      "string used jets??");
        return StatusCode::FAILURE;
      }
      // identify which jets to keep for the thinning check
      for (size_t i = 0; i < nJets; ++i) {
        if (entries.at(i) == 1) {
          jetsToCheck.push_back(importedJets->at(i));
        }
      }
    } else {
      // If no selection was specified, use all the jet objects:
      for (const xAOD::Jet* jet : *importedJets) {
        jetsToCheck.push_back(jet);
      }
    }

    // what tracks to keep if fullfiling thinning criteria
    // perform track jet constituent thinning if selected
    if (m_thinConstituents) ATH_CHECK(thinConstituents(mask, jetsToCheck));
    // perform JetFitter tracks thinning if selected
    if (m_thinJetFitter) ATH_CHECK(thinJetFitterTracks(mask, jetsToCheck));
    // perform SV1 tracks thinning if selected
    if (m_thinSV1) ATH_CHECK(thinSV1Tracks(mask, jetsToCheck));

    // Count up the mask contents
    for (bool bit : mask) {
      if (bit) {
        ++m_npass;
      }
    }

    // Tell the thinning service what to do:
    const IThinningSvc::Operator::Type opType = (m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or);
    ATH_CHECK(m_thinningSvc->filter(*importedTrackParticles, mask, opType));

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode TrkJetTrackThinning::thinConstituents(
    std::vector<bool>& mask,
    std::vector<const xAOD::Jet*>& jetsToCheck) const {
    // Set elements in the mask to true if there is a corresponding
    // ElementLink from a reconstructed jet object
    for (const xAOD::Jet* jet : jetsToCheck) {
      // Loop over the TrkJet track particles:
      const auto& tpLinks = jet->constituentLinks();
      for (const auto& tpLink : tpLinks) {
        // Check that it is what we expect:
        if (!tpLink.isValid()) {
          continue;
        }
        if (tpLink.dataID() != m_inDetSGKey) {
          ATH_MSG_FATAL("TrkJet track particle does not come from "
                        "container \""
                        << m_inDetSGKey << "\"");
          return StatusCode::FAILURE;
        }
        // If it is, set the mask for it:
        mask.at(tpLink.index()) = true;
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode TrkJetTrackThinning::thinJetFitterTracks(
    std::vector<bool>& mask,
    std::vector<const xAOD::Jet*>& jetsToCheck) const {

    using VtxLnk = ElementLink<xAOD::BTagVertexContainer>;
    // Set elements in the mask to true if there is a corresponding
    // ElementLink from a reconstructed jet object
    for (const xAOD::Jet* jet : jetsToCheck) {
      // Access the BTagging object:
      const xAOD::BTagging* btag = jet->btagging();
      if (!btag) {
        ATH_MSG_WARNING("No BTagging objects found for jet. Skipping.");
        continue;
      }

      // Loop over the JF track particles:
      std::vector<VtxLnk> JFvertices;
      const bool hasJFvertices = btag->variable<std::vector<VtxLnk>>(
        "JetFitter", "JFvertices", JFvertices);
      if (!hasJFvertices) {
        ATH_MSG_WARNING("No JetFitter vertices found for jet. Skipping.");
        continue;
      }
      for (const VtxLnk& vtx : JFvertices) {
        for (const auto& tpLink : (*vtx)->track_links()) {
          // Check that it is what we expect:
          if (!tpLink.isValid()) {
            continue;
          }
          if (tpLink.dataID() != m_inDetSGKey) {
            ATH_MSG_FATAL("JetFitter vtx track particle does not come from "
                          "container \""
                          << m_inDetSGKey << "\"");
            return StatusCode::FAILURE;
          }
          // If it is, set the mask for it:
          mask.at(tpLink.index()) = true;
        }
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode TrkJetTrackThinning::thinSV1Tracks(
    std::vector<bool>& mask,
    std::vector<const xAOD::Jet*>& jetsToCheck) const {
    // Set elements in the mask to true if there is a corresponding
    // ElementLink from a reconstructed jet object
    for (const xAOD::Jet* jet : jetsToCheck) {
      // Access the BTagging object:
      const xAOD::BTagging* btag = jet->btagging();
      if (!btag) {
        ATH_MSG_WARNING("No BTagging objects found for jet");
        continue;
      }

      // Loop over the SV1 track particles:
      const auto& tpLinks = btag->SV1_TrackParticleLinks();
      for (const auto& tpLink : tpLinks) {
        // Check that it is what we expect:
        if (!tpLink.isValid()) {
          continue;
        }
        if (tpLink.dataID() != m_inDetSGKey) {
          ATH_MSG_FATAL("SV1 track particle does not come from "
                        "container \""
                        << m_inDetSGKey << "\"");
          return StatusCode::FAILURE;
        }
        // If it is, set the mask for it:
        mask.at(tpLink.index()) = true;
      }
    }
    return StatusCode::SUCCESS;
  }
} // namespace DerivationFramework
