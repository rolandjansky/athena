/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System includes
#include <typeinfo>

// Framework includes
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"

// Local includes
#include "AssociationUtils/MuJetOverlapTool.h"
#include "AssociationUtils/DeltaRMatcher.h"
#include "AssociationUtils/MuJetGhostDRMatcher.h"

namespace
{
  /// Unit conversion constants
  const double GeV = 1e3;
}

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  MuJetOverlapTool::MuJetOverlapTool(const std::string& name)
    : BaseOverlapTool(name)
  {
    declareProperty("BJetLabel", m_bJetLabel = "",
                    "Input b-jet flag. Disabled by default.");
    declareProperty("NumJetTrk", m_numJetTrk = 3,
                    "Min number of jet tracks to keep jet and remove muon");
    declareProperty("ApplyRelPt", m_applyRelPt = false,
                    "Toggle mu/jet relative PT requirements to prioritize jet");
    declareProperty("MuJetPtRatio", m_muJetPtRatio = 0.5,
                    "Mu/jet PT ratio threshold to remove a jet");
    declareProperty("MuJetTrkPtRatio", m_muJetTrkPtRatio = 0.7,
                    "Mu/jetTrk PT ratio threshold to remove a jet");
    declareProperty("JetNumTrackDecoration", m_jetNumTrkDec = "",
                    "User-defined decoration for jet numTrack");
    declareProperty("JetSumTrackPTDecoration", m_jetSumTrkPtDec = "",
                    "User-defined decoration for jet sumTrackPT");
    declareProperty("UseGhostAssociation", m_useGhostAssociation = true,
                    "Activate ghost-association mapping for jet removals");
    declareProperty("InnerDR", m_innerDR = 0.2,
                    "Flat inner cone for removing jets");
    declareProperty("OuterDR", m_outerDR = 0.4,
                    "Flat outer cone for removing muons");
    declareProperty("UseSlidingDR", m_useSlidingDR = false,
                    "Enables sliding dR outer cone = c1 + c2/MuPt");
    declareProperty("SlidingDRC1", m_slidingDRC1 = 0.04,
                    "The constant offset for sliding dR");
    declareProperty("SlidingDRC2", m_slidingDRC2 = 10.*GeV,
                    "The inverse muon pt factor for sliding dR");
    declareProperty("SlidingDRMaxCone", m_slidingDRMaxCone = 0.4,
                    "Maximum allowed size of sliding dR cone");
    declareProperty("UseRapidity", m_useRapidity = true,
                    "Calculate delta-R using rapidity");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode MuJetOverlapTool::initializeDerived()
  {
    using CxxUtils::make_unique;

    // Initialize the b-jet helper
    if(!m_bJetLabel.empty()) {
      ATH_MSG_DEBUG("Configuring btag-aware OR with btag label: " << m_bJetLabel);
      m_bJetHelper = make_unique<BJetHelper>(m_bJetLabel);
    }

    // Initialize the matcher for the 'inner' cone.
    if(m_useGhostAssociation) {
      ATH_MSG_DEBUG("Configuring ghost association + dR matching for jet-mu OR "
                    "with inner cone size " << m_innerDR);
      m_dRMatchCone1 = make_unique<MuJetGhostDRMatcher>(m_innerDR, m_useRapidity);
    }
    else {
      ATH_MSG_DEBUG("Configuring mu-jet inner cone size " << m_innerDR);
      m_dRMatchCone1 = make_unique<DeltaRMatcher>(m_innerDR, m_useRapidity);
    }

    // Use sliding dR or flat dR for the 'outer' cone.
    if(m_useSlidingDR) {
      ATH_MSG_DEBUG("Configuring sliding outer cone for mu-jet OR with " <<
                    "constants C1 = " << m_slidingDRC1 << ", C2 = " <<
                    m_slidingDRC2 << ", MaxCone = " << m_slidingDRMaxCone);
      m_dRMatchCone2 =
        make_unique<SlidingDeltaRMatcher>
          (m_slidingDRC1, m_slidingDRC2, m_slidingDRMaxCone, m_useRapidity);
    }
    else {
      ATH_MSG_DEBUG("Configuring mu-jet outer cone size " << m_outerDR);
      m_dRMatchCone2 = make_unique<DeltaRMatcher>(m_outerDR, m_useRapidity);
    }

    // Additional config printouts
    ATH_MSG_DEBUG("Mu-jet matching config: NumJetTrk " << m_numJetTrk <<
                  " ApplyRelPt " << m_applyRelPt <<
                  " MuJetPtRatio " << m_muJetPtRatio <<
                  " MuJetTrkPtRatio " << m_muJetTrkPtRatio);
    if(!m_jetNumTrkDec.empty()) {
      ATH_MSG_DEBUG("Using user-defined JetNumTrackDecoration " << m_jetNumTrkDec);
    }
    if(!m_jetSumTrkPtDec.empty()) {
      ATH_MSG_DEBUG("Using user-defined JetSumTrackPTDecoration " << m_jetSumTrkPtDec);
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode MuJetOverlapTool::
  findOverlaps(const xAOD::IParticleContainer& cont1,
               const xAOD::IParticleContainer& cont2) const
  {
    // Check the container types
    if(typeid(cont1) != typeid(xAOD::MuonContainer) &&
       typeid(cont1) != typeid(ConstDataVector<xAOD::MuonContainer>)) {
      ATH_MSG_ERROR("First container arg is not of type MuonContainer!");
      return StatusCode::FAILURE;
    }
    if(typeid(cont2) != typeid(xAOD::JetContainer) &&
       typeid(cont2) != typeid(ConstDataVector<xAOD::JetContainer>)) {
      ATH_MSG_ERROR("Second container arg is not of type JetContainer!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( findOverlaps(static_cast<const xAOD::MuonContainer&>(cont1),
                            static_cast<const xAOD::JetContainer&>(cont2)) );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode MuJetOverlapTool::
  findOverlaps(const xAOD::MuonContainer& muons,
               const xAOD::JetContainer& jets) const
  {
    ATH_MSG_DEBUG("Removing overlapping muons and jets");

    // Initialize output decorations if necessary
    m_decHelper->initializeDecorations(muons);
    m_decHelper->initializeDecorations(jets);

    // Retrieve the primary vertex for later reference
    size_t vtxIdx = 0;
    if(m_jetNumTrkDec.empty() && m_jetSumTrkPtDec.empty()) {
      auto vtx = getPrimVtx();
      ATH_CHECK(vtx != nullptr);
      vtxIdx = vtx->index();
    }

    // Remove suspicious jets that overlap with muons.
    for(const auto muon : muons){
      if(!m_decHelper->isSurvivingObject(*muon)) continue;

      for(const auto jet : jets){
        if(!m_decHelper->isSurvivingObject(*jet)) continue;

        // Don't reject user-defined b-tagged jets
        if(m_bJetHelper && m_bJetHelper->isBJet(*jet)) continue;

        // Get the number of tracks and the sumPT of those tracks
        int nTrk = getNumTracks(*jet, vtxIdx);
        float sumTrkPt = getSumTrackPt(*jet, vtxIdx);

        // Don't reject jets with high track multiplicity and
        // high relative PT ratio
        bool highNumTrk = nTrk >= m_numJetTrk;
        bool highRelPt = (muon->pt()/jet->pt() < m_muJetPtRatio ||
                          muon->pt()/sumTrkPt < m_muJetTrkPtRatio);
        if(highNumTrk && (!m_applyRelPt || highRelPt)) continue;

        if(m_dRMatchCone1->objectsMatch(*muon, *jet)){
          ATH_CHECK( handleOverlap(jet, muon) );
        }
      }
    }

    // Remove muons from remaining overlapping jets
    for(const auto jet : jets){
      if(!m_decHelper->isSurvivingObject(*jet)) continue;

      for(const auto muon : muons){
        if(!m_decHelper->isSurvivingObject(*muon)) continue;

        if(m_dRMatchCone2->objectsMatch(*muon, *jet)){
          ATH_CHECK( handleOverlap(muon, jet) );
        }
      }
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Retrieve the primary vertex
  //---------------------------------------------------------------------------
  const xAOD::Vertex* MuJetOverlapTool::getPrimVtx() const
  {
    const char* contName = "PrimaryVertices";
    const xAOD::VertexContainer* vertices = nullptr;
    if(evtStore()->retrieve(vertices, contName).isSuccess()) {
      for(auto vtx : *vertices) {
        if(vtx->vertexType() == xAOD::VxType::PriVtx)
          return vtx;
      }
    }
    else {
      ATH_MSG_WARNING("Failed to retrieve " << contName);
    }
    // No PV found. We cannot execute the OR recommendations.
    ATH_MSG_FATAL("No primary vertex in the PrimaryVertices container!");
    return nullptr;
  }

  //---------------------------------------------------------------------------
  // Get the number of tracks in a jet
  //---------------------------------------------------------------------------
  int MuJetOverlapTool::getNumTracks(const xAOD::Jet& jet, size_t vtxIdx) const
  {
    // Use the user decoration if configured
    if(!m_jetNumTrkDec.empty()) {
      return jet.auxdata<int>(m_jetNumTrkDec);
    }
    static const SG::AuxElement::ConstAccessor< std::vector<int> > acc("NumTrkPt500");
    return acc(jet)[vtxIdx];
  }

  //---------------------------------------------------------------------------
  // Get the sum track pt of a jet
  //---------------------------------------------------------------------------
  float MuJetOverlapTool::getSumTrackPt(const xAOD::Jet& jet, size_t vtxIdx) const
  {
    // Use the user decoration if configured
    if(!m_jetSumTrkPtDec.empty()) {
      return jet.auxdata<float>(m_jetSumTrkPtDec);
    }
    static const SG::AuxElement::ConstAccessor< std::vector<float> > acc("SumPtTrkPt500");
    return acc(jet)[vtxIdx];
  }

} // namespace ORUtils
