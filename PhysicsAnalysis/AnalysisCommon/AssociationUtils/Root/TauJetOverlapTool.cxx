// System includes
#include <typeinfo>

// Framework includes
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"

// Local includes
#include "AssociationUtils/TauJetOverlapTool.h"
#include "AssociationUtils/DeltaRMatcher.h"

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  TauJetOverlapTool::TauJetOverlapTool(const std::string& name)
    : BaseOverlapTool(name)
  {
    declareProperty("BJetLabel", m_bJetLabel = "",
                    "Input b-jet flag. Disabled by default.");
    declareProperty("DR", m_dR = 0.2, "Maximum dR for overlap match");
    declareProperty("UseRapidity", m_useRapidity = true,
                    "Calculate delta-R using rapidity");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode TauJetOverlapTool::initializeDerived()
  {
    using CxxUtils::make_unique;

    // Initialize the b-jet helper
    if(!m_bJetLabel.empty()) {
      ATH_MSG_DEBUG("Configuring btag-aware OR with btag label: " << m_bJetLabel);
      m_bJetHelper = make_unique<BJetHelper>(m_bJetLabel);
    }

    // Initialize the dR matcher
    m_dRMatcher = make_unique<DeltaRMatcher>(m_dR, m_useRapidity);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode TauJetOverlapTool::
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
  StatusCode TauJetOverlapTool::
  findOverlaps(const xAOD::JetContainer& jets,
               const xAOD::TauJetContainer& taus) const
  {
    ATH_MSG_DEBUG("Removing overlapping taus and jets");

    // Initialize output decorations if necessary
    m_decHelper->initializeDecorations(taus);
    m_decHelper->initializeDecorations(jets);

    // TODO: add anti-tau support.

    // Remove non-btagged jets that overlap with taus.
    for(const auto tau : taus){
      if(!m_decHelper->isSurvivingObject(*tau)) continue;
      for(const auto jet : jets){
        if(!m_decHelper->isSurvivingObject(*jet)) continue;

        // Don't reject user-defined b-tagged jets
        if(m_bJetHelper && m_bJetHelper->isBJet(*jet)) continue;

        if(m_dRMatcher->objectsMatch(*tau, *jet)){
          ATH_CHECK( handleOverlap(jet, tau) );
        }
      }
    }

    // Remove taus that overlap with remaining jets
    for(const auto jet : jets) {
      if(!m_decHelper->isSurvivingObject(*jet)) continue;
      for(const auto tau : taus){
        if(!m_decHelper->isSurvivingObject(*tau)) continue;

        if(m_dRMatcher->objectsMatch(*jet, *tau)) {
          ATH_CHECK( handleOverlap(tau, jet) );
        }
      }
    }

    return StatusCode::SUCCESS;
  }

} // namespace ORUtils
