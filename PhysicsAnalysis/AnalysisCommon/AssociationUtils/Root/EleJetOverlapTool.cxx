// System includes
#include <typeinfo>

// Framework includes
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"

// Local includes
#include "AssociationUtils/EleJetOverlapTool.h"

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
  EleJetOverlapTool::EleJetOverlapTool(const std::string& name)
    : BaseOverlapTool(name)
  {
    declareProperty("BJetLabel", m_bJetLabel = "",
                    "Input b-jet flag. Disabled by default.");
    declareProperty("ApplyPtRatio", m_applyPtRatio = false,
                    "Toggle ele/jet PT ratio requirement");
    declareProperty("EleJetPtRatio", m_eleJetPtRatio = 0.8,
                    "Ele/jet PT ratio threshold to remove a jet");
    declareProperty("InnerDR", m_innerDR = 0.2,
                    "Inner cone for removing jets");
    declareProperty("OuterDR", m_outerDR = 0.4,
                    "Outer cone for removing electrons");
    declareProperty("UseSlidingDR", m_useSlidingDR = false,
                    "Use sliding dR cone to reject electrons");
    declareProperty("SlidingDRC1", m_slidingDRC1 = 0.04,
                    "The constant offset for sliding dR");
    declareProperty("SlidingDRC2", m_slidingDRC2 = 10.*GeV,
                    "The inverse muon pt factor for sliding dR");
    declareProperty("SlidingDRMaxCone", m_slidingDRMaxCone = 0.4,
                    "Maximum size of sliding dR cone");
    declareProperty("UseRapidity", m_useRapidity = true,
                    "Calculate delta-R using rapidity");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode EleJetOverlapTool::initializeDerived()
  {
    using CxxUtils::make_unique;

    // Initialize the b-jet helper
    if(!m_bJetLabel.empty()) {
      ATH_MSG_DEBUG("Configuring btag-aware OR with btag label: " << m_bJetLabel);
      m_bJetHelper = make_unique<BJetHelper>(m_bJetLabel);
    }

    // Initialize the dR matchers
    m_dRMatchCone1 = make_unique<DeltaRMatcher>(m_innerDR, m_useRapidity);
    if(m_useSlidingDR) {
      ATH_MSG_DEBUG("Configuring sliding outer cone for ele-jet OR with " <<
                    "constants C1 = " << m_slidingDRC1 << ", C2 = " <<
                    m_slidingDRC2 << ", MaxCone = " << m_slidingDRMaxCone);
      m_dRMatchCone2 =
        make_unique<SlidingDeltaRMatcher>
          (m_slidingDRC1, m_slidingDRC2, m_slidingDRMaxCone, m_useRapidity);
    }
    else {
      m_dRMatchCone2 = make_unique<DeltaRMatcher>(m_outerDR, m_useRapidity);
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode EleJetOverlapTool::
  findOverlaps(const xAOD::IParticleContainer& cont1,
               const xAOD::IParticleContainer& cont2) const
  {
    // Check the container types
    if(typeid(cont1) != typeid(xAOD::ElectronContainer) &&
       typeid(cont1) != typeid(ConstDataVector<xAOD::ElectronContainer>)) {
      ATH_MSG_ERROR("First container arg is not an ElectronContainer!");
      return StatusCode::FAILURE;
    }
    if(typeid(cont2) != typeid(xAOD::JetContainer) &&
       typeid(cont2) != typeid(ConstDataVector<xAOD::JetContainer>)) {
      ATH_MSG_ERROR("Second container arg is not of type JetContainer!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( findOverlaps(static_cast<const xAOD::ElectronContainer&>(cont1),
                            static_cast<const xAOD::JetContainer&>(cont2)) );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode EleJetOverlapTool::
  findOverlaps(const xAOD::ElectronContainer& electrons,
               const xAOD::JetContainer& jets) const
  {
    ATH_MSG_DEBUG("Removing overlapping electrons and jets");

    // Initialize output decorations if necessary
    m_decHelper->initializeDecorations(electrons);
    m_decHelper->initializeDecorations(jets);

    // First flag overlapping jets
    for(const auto electron : electrons){
      if(!m_decHelper->isSurvivingObject(*electron)) continue;

      for(const auto jet : jets){
        if(!m_decHelper->isSurvivingObject(*jet)) continue;
        // Don't reject user-defined b-tagged jets
        if(m_bJetHelper && m_bJetHelper->isBJet(*jet)) continue;
        // Don't reject jets with high relative PT
        if(m_applyPtRatio && (electron->pt()/jet->pt() < m_eleJetPtRatio)) continue;

        if(m_dRMatchCone1->objectsMatch(*jet, *electron)){
          ATH_CHECK( handleOverlap(jet, electron) );
        }
      }
    }

    // Now flag overlapping electrons
    for(const auto jet: jets){
      if(!m_decHelper->isSurvivingObject(*jet)) continue;

      for(const auto electron : electrons){
        if(!m_decHelper->isSurvivingObject(*electron)) continue;

        if(m_dRMatchCone2->objectsMatch(*electron, *jet)){
          ATH_CHECK( handleOverlap(electron, jet) );
        }
      }
    }

    return StatusCode::SUCCESS;
  }

} // namespace ORUtils
