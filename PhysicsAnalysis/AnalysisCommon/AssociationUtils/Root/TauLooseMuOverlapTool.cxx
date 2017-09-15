// System includes
#include <typeinfo>

// Framework includes
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"

// Local includes
#include "AssociationUtils/TauLooseMuOverlapTool.h"

namespace
{
  /// Unit conversion constants
  const float GeV = 1e3;
}

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  TauLooseMuOverlapTool::TauLooseMuOverlapTool(const std::string& name)
    : BaseOverlapTool(name)
  {
    declareProperty("DR", m_maxDR = 0.2,
                    "Delta-R cone for flagging overlaps");
    declareProperty("UseRapidity", m_useRapidity = true,
                    "Calculate delta-R using rapidity");
    declareProperty("MinMuonPt", m_minMuPt = 2.*GeV,
                    "Minimum muon PT for rejecting taus");
    declareProperty("MinTauPtCombinedMuon", m_minTauPtMuComb = 50.*GeV,
                    "Tau PT threshold to compare to combined muons only");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode TauLooseMuOverlapTool::initializeDerived()
  {
    using CxxUtils::make_unique;

    // Initialize the dR matcher
    m_dRMatcher = make_unique<DeltaRMatcher> (m_maxDR, m_useRapidity);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode TauLooseMuOverlapTool::
  findOverlaps(const xAOD::IParticleContainer& cont1,
               const xAOD::IParticleContainer& cont2) const
  {
    // Check the container types
    if(typeid(cont1) != typeid(xAOD::TauJetContainer) &&
       typeid(cont1) != typeid(ConstDataVector<xAOD::TauJetContainer>)) {
      ATH_MSG_ERROR("First container arg is not of type TauJetContainer!");
      return StatusCode::FAILURE;
    }
    if(typeid(cont2) != typeid(xAOD::MuonContainer) &&
       typeid(cont2) != typeid(ConstDataVector<xAOD::MuonContainer>)) {
      ATH_MSG_ERROR("Second container arg is not of type MuonContainer!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( findOverlaps(static_cast<const xAOD::TauJetContainer&>(cont1),
                            static_cast<const xAOD::MuonContainer&>(cont2)) );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode TauLooseMuOverlapTool::
  findOverlaps(const xAOD::TauJetContainer& taus,
               const xAOD::MuonContainer& muons) const
  {
    ATH_MSG_DEBUG("Removing taus from loose muons");

    // Initialize output decorations if necessary
    m_decHelper->initializeDecorations(taus);
    m_decHelper->initializeDecorations(muons);

    // Loop over loose surviving muons
    for(auto muon : muons){

      // It's not obvious that I should be skipping objects that were
      // flagged as overlap. Perhaps this deserves more thought/study.
      if(m_decHelper->isRejectedObject(*muon)) continue;
      if(muon->pt() < m_minMuPt) continue;
      bool isCombined = (muon->muonType() == xAOD::Muon::Combined);

      // Loop over surviving taus
      for(auto tau : taus){
        if(!m_decHelper->isSurvivingObject(*tau)) continue;

        // High-PT taus are only compared to combined muons
        if((tau->pt() > m_minTauPtMuComb) && !isCombined) continue;

        // Test for overlap
        if(m_dRMatcher->objectsMatch(*muon, *tau)){
          ATH_CHECK( handleOverlap(tau, muon) );
        }
      }
    }  

    return StatusCode::SUCCESS;
  }

} // namespace ORUtils
