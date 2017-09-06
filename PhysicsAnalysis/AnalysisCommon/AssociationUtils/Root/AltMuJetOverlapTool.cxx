// System includes
#include <typeinfo>
#include <limits>

// Framework includes
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"

// Local includes
#include "AssociationUtils/AltMuJetOverlapTool.h"
#include "AssociationUtils/DeltaRMatcher.h"

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
  AltMuJetOverlapTool::AltMuJetOverlapTool(const std::string& name)
    : BaseOverlapTool(name)
  {
    declareProperty("BJetLabel", m_bJetLabel = "",
                    "Input b-jet flag. Disabled by default.");
    // Disabled by default
    declareProperty("NumJetTrk", m_numJetTrk = std::numeric_limits<int>::max(),
                    "Min number of jet tracks to keep jet and remove muon");
    // Disabled by default
    declareProperty("MuJetPtRatio", m_muJetPtRatio = 0.,
                    "Max PT ratio to keep jet and remove muon");
    declareProperty("InnerDR", m_innerDR = 0.2,
                    "Inner cone for removing jets");
    declareProperty("SlidingDRC1", m_slidingDRC1 = 0.04,
                    "The constant offset for sliding dR");
    declareProperty("SlidingDRC2", m_slidingDRC2 = 10.*GeV,
                    "The inverse muon pt factor for sliding dR");
    declareProperty("SlidingDRMaxCone",
                    m_slidingDRMaxCone = std::numeric_limits<double>::max(),
                    "Maximum allowed size of sliding dR cone");
    declareProperty("UseRapidity", m_useRapidity = true,
                    "Calculate delta-R using rapidity");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode AltMuJetOverlapTool::initializeDerived()
  {
    using CxxUtils::make_unique;

    // Initialize the b-jet helper
    if(!m_bJetLabel.empty())
      m_bJetHelper = make_unique<BJetHelper> (m_bJetLabel);

    // Initialize the inner cone dR matcher
    m_dRMatchCone1 =
      make_unique<DeltaRMatcher> (m_innerDR, m_useRapidity);
    // Initialize the sliding dR matcher
    m_dRMatchCone2 =
      make_unique<SlidingDeltaRMatcher>
        (m_slidingDRC1, m_slidingDRC2, m_slidingDRMaxCone, m_useRapidity);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode AltMuJetOverlapTool::
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
  StatusCode AltMuJetOverlapTool::
  findOverlaps(const xAOD::MuonContainer& muons,
               const xAOD::JetContainer& jets) const
  {
    ATH_MSG_DEBUG("Removing overlapping muons and jets");

    // Initialize output decorations if necessary
    m_decHelper->initializeDecorations(muons);
    m_decHelper->initializeDecorations(jets);

    // Remove jets that overlap with muons in first cone.
    for(const auto muon : muons){
      if(!m_decHelper->isSurvivingObject(*muon)) continue;

      for(const auto jet : jets){
        if(!m_decHelper->isSurvivingObject(*jet)) continue;
        // User-defined jet criteria include b-tagging,
        // numTrack, and the mu/jet PT ratio
        if(m_bJetHelper && m_bJetHelper->isBJet(*jet)) continue;
        if(getNumTracks(jet) >= m_numJetTrk) continue;
        float ptRatio = muon->pt() / jet->pt();
        if(ptRatio < m_muJetPtRatio) continue;

        if(m_dRMatchCone1->objectsMatch(*jet, *muon)){
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
  const xAOD::Vertex* AltMuJetOverlapTool::getPrimVtx() const
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
    return nullptr;
  }

  //---------------------------------------------------------------------------
  // Retrieve the primary vertex
  //---------------------------------------------------------------------------
  int AltMuJetOverlapTool::getNumTracks(const xAOD::Jet* jet) const
  {
    // Find the primary vertex
    auto vtx = getPrimVtx();
    if(!vtx) return -1;
    static const SG::AuxElement::ConstAccessor< std::vector<int> > acc("NumTrkPt500");
    return acc(*jet)[vtx->index()];
  }

} // namespace ORUtils
