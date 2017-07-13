// System includes
#include <typeinfo>

// Framework includes
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"

// Local includes
#include "AssociationUtils/TauAntiTauJetOverlapTool.h"
#include "AssociationUtils/DeltaRMatcher.h"

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  TauAntiTauJetOverlapTool::TauAntiTauJetOverlapTool(const std::string& name)
    : BaseOverlapTool(name),
      m_bJetLabel(""),
      m_antiTauLabel(""),
      m_dR(0.2),
      m_useRapidity(true),
      m_bJetHelper(nullptr),
      m_dRMatcher(nullptr),
      m_antiTauDecHelper(nullptr)
  {
    declareProperty("BJetLabel", m_bJetLabel,
                    "Input b-jet flag. Disabled by default.");
    declareProperty("AntiTauLabel", m_antiTauLabel,
                    "Decoration which labels anti-taus");
    declareProperty("DR", m_dR, "Maximum dR for overlap match");
    declareProperty("UseRapidity", m_useRapidity,
                    "Calculate delta-R using rapidity");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode TauAntiTauJetOverlapTool::initializeDerived()
  {
    using CxxUtils::make_unique;

    // Initialize the b-jet helper
    if(!m_bJetLabel.empty()) {
      ATH_MSG_DEBUG("Configuring btag-aware OR with btag label: " << m_bJetLabel);
      m_bJetHelper = make_unique<BJetHelper>(m_bJetLabel);
    }

    // Initialize the dR matcher
    m_dRMatcher = make_unique<DeltaRMatcher>(m_dR, m_useRapidity);

    // Initialize the anti-tau decoration helper
    if(!m_antiTauLabel.empty()) {
      ATH_MSG_DEBUG("Configuring anti-tau OR with label: " << m_antiTauLabel);
      m_antiTauDecHelper =
        make_unique<OverlapDecorationHelper>
          (m_antiTauLabel, m_outputLabel, m_outputPassValue);
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode TauAntiTauJetOverlapTool::
  findOverlaps(const xAOD::IParticleContainer& cont1,
               const xAOD::IParticleContainer& cont2) const
  {
    // Check the container types
    if(typeid(cont1) != typeid(xAOD::JetContainer) &&
       typeid(cont1) != typeid(ConstDataVector<xAOD::JetContainer>)) {
      ATH_MSG_ERROR("First container arg is not of type JetContainer!");
      return StatusCode::FAILURE;
    }
    if(typeid(cont2) != typeid(xAOD::TauJetContainer) &&
       typeid(cont2) != typeid(ConstDataVector<xAOD::TauJetContainer>)) {
      ATH_MSG_ERROR("Second container arg is not of type TauJetContainer!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( findOverlaps(static_cast<const xAOD::JetContainer&>(cont1),
                            static_cast<const xAOD::TauJetContainer&>(cont2)) );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode TauAntiTauJetOverlapTool::
  findOverlaps(const xAOD::JetContainer& jets,
               const xAOD::TauJetContainer& taus) const
  {
    ATH_MSG_DEBUG("Removing overlapping taus and jets");

    // Initialize output decorations if necessary
    m_decHelper->initializeDecorations(taus);
    m_decHelper->initializeDecorations(jets);

    // Remove bjets overlapping with ID taus
    for(const auto tau : taus) {
      if(!m_decHelper->isSurvivingObject(*tau)) continue;
      for(const auto jet : jets) {
        if(!m_decHelper->isSurvivingObject(*jet)) continue;
        if(!isBJet(*jet)) continue;
        if(m_dRMatcher->objectsMatch(*tau, *jet)){
          ATH_CHECK( handleOverlap(jet, tau) );
        }
      }
    }

    // Remove anti-taus from remaining bjets
    for(const auto jet : jets) {
      if(!m_decHelper->isSurvivingObject(*jet)) continue;
      if(!isBJet(*jet)) continue;
      for(const auto tau : taus) {
        if(!isSurvivingAntiTau(*tau)) continue;
        if(m_dRMatcher->objectsMatch(*tau, *jet)) {
          ATH_CHECK( handleOverlap(tau, jet) );
        }
      }
    }

    // Remove light jets from remaining ID taus and anti-taus.
    for(const auto tau : taus) {
      if(!m_decHelper->isSurvivingObject(*tau) &&
         !isSurvivingAntiTau(*tau) ) continue;
      for(const auto jet : jets) {
        if(!m_decHelper->isSurvivingObject(*jet)) continue;
        // We don't need to check the bjet label, but it might save CPU.
        if(isBJet(*jet)) continue;
        if(m_dRMatcher->objectsMatch(*tau, *jet)){
          ATH_CHECK( handleOverlap(jet, tau) );
        }
      }
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify a user-labeled b-jet
  //---------------------------------------------------------------------------
  bool TauAntiTauJetOverlapTool::isBJet(const xAOD::Jet& jet) const
  {
    if(m_bJetHelper && m_bJetHelper->isBJet(jet)) return true;
    return false;
  }

  //---------------------------------------------------------------------------
  // Identify a user-labeled anti-tau
  //---------------------------------------------------------------------------
  bool TauAntiTauJetOverlapTool::isSurvivingAntiTau(const xAOD::TauJet& tau) const
  {
    if(m_antiTauDecHelper && m_antiTauDecHelper->isSurvivingObject(tau))
      return true;
    return false;
  }

} // namespace ORUtils
