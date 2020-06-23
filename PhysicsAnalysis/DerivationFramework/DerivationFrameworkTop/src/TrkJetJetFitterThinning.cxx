/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTagging.h"

// Local include(s):
#include "DerivationFrameworkTop/TrkJetJetFitterThinning.h"

namespace DerivationFramework {
  TrkJetJetFitterThinning::TrkJetJetFitterThinning(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
    : AthAlgTool(type, name, parent),
    m_trkjetSGKey(""),
    m_verticesSGKey(""),
    m_thinningSvc("ThinningSvc", name) {
    // Declare the tool's interface to the framework:
    declareInterface< DerivationFramework::IThinningTool >(this);

    // Set up the tool's properties:
    declareProperty("ThinningService", m_thinningSvc,
                    "The thinning service to use");
    declareProperty("TrkJetKey", m_trkjetSGKey,
                    "StoreGate key of the jet container to use");
    declareProperty("TrkJetSelectionString", m_selectionString,
                    "Selection string for the track jets");
    declareProperty("EventSelectionString", m_event_selectionString,
                    "Seletion string for the whole event");
    declareProperty("JetFitterContainerKey", m_verticesSGKey,
                    "StoreGate key for the JetFitter vertices container");
    declareProperty("ApplyAnd", m_and,
                    "Use IThinningSvc::Operator::And instead of "
                    "IThinningSvc::Operator::Or");
  }

  StatusCode TrkJetJetFitterThinning::initialize() {
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");

    if (m_trkjetSGKey == "") {
      ATH_MSG_FATAL("No track jet collection provided for thinning.");
      return StatusCode::FAILURE;
    }

    if (m_verticesSGKey == "") {
      ATH_MSG_FATAL("No BTagging_*_JFVtx collection provided for thinning.");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO(m_verticesSGKey << " vertices asssociated with " << m_trkjetSGKey << " will be kept.");

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

    m_opType = (m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or);

    // Access the thinning service:
    ATH_CHECK(m_thinningSvc.retrieve());

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode TrkJetJetFitterThinning::finalize() {
    // Print some statistics:
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed " << m_ntot << " vertices, " << m_npass
                              << " were retained ");

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode TrkJetJetFitterThinning::doThinning() const {

    // Retrieve the vertex containers
    const xAOD::BTagVertexContainer* JetFitterVertices = nullptr;

    // Set up a mask with the same entries as the vertex container
    std::vector<bool> mask;
    ATH_CHECK(evtStore()->retrieve(JetFitterVertices, m_verticesSGKey));
    mask.assign(JetFitterVertices->size(), false);
    m_ntot += JetFitterVertices->size();

    // first evaluate the event selection expression
    // if it is false, then there's no use doing anything else
    // just run the thinning expression with all masks false
    if (m_event_parser) {
      const bool passEvent = m_event_parser->parser().evaluateAsBool();
      if (!passEvent) {
        ATH_CHECK(m_thinningSvc->filter(*JetFitterVertices, mask, m_opType));
        return StatusCode::SUCCESS;
      }
    }

    // Retrieve the jet container:
    const xAOD::JetContainer* importedJets = nullptr;
    ATH_CHECK(evtStore()->retrieve(importedJets, m_trkjetSGKey));
    const size_t nJets = importedJets->size();

    // Select the jets to use:
    std::vector< const xAOD::Jet* > jetsToCheck;
    if (m_trkjet_parser) {
      const std::vector< int > entries =
        m_trkjet_parser->parser().evaluateAsVector();
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

    // perform JetFitter vertex thinning
    ATH_CHECK(thinJetFitterVertices(mask, jetsToCheck));

    // Count up the mask contents
    for (bool bit : mask) {
      if (bit) {
        ++m_npass;
      }
    }

    ATH_CHECK(m_thinningSvc->filter(*JetFitterVertices, mask, m_opType));
    return StatusCode::SUCCESS;
  }

  StatusCode TrkJetJetFitterThinning::thinJetFitterVertices(
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

      // Loop over the JetFitter vertices
      std::vector<VtxLnk> JetFitterVertices;
      const bool hasJFVertices = btag->variable<std::vector<VtxLnk>>(
        "JetFitter", "JFvertices", JetFitterVertices);
      if (!hasJFVertices) {
        continue;
      }
      for (const VtxLnk& vertexLink : JetFitterVertices) {
        if (!vertexLink.isValid()) {
          continue;
        }
        mask.at(vertexLink.index()) = true;
      }
    }
    return StatusCode::SUCCESS;
  }
} // namespace DerivationFramework
