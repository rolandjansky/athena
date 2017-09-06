// System includes
#include <typeinfo>

// Framework includes
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"

// EDM includes
#include "xAODEgamma/ElectronxAODHelpers.h"

// Local includes
#include "AssociationUtils/EleMuSharedTrkOverlapTool.h"

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  EleMuSharedTrkOverlapTool::EleMuSharedTrkOverlapTool(const std::string& name)
    : BaseOverlapTool(name)
  {

    declareProperty("RemoveCaloMuons", m_removeCaloMuons = true,
                    "Turn on removal of overlapping calo muons");
  }

  //---------------------------------------------------------------------------
  // Initialize the tool
  //---------------------------------------------------------------------------
  StatusCode EleMuSharedTrkOverlapTool::initializeDerived()
  {
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode EleMuSharedTrkOverlapTool::
  findOverlaps(const xAOD::IParticleContainer& cont1,
               const xAOD::IParticleContainer& cont2) const
  {
    // Check the container types
    if(typeid(cont1) != typeid(xAOD::ElectronContainer) &&
       typeid(cont1) != typeid(ConstDataVector<xAOD::ElectronContainer>)) {
      ATH_MSG_ERROR("First container arg is not of type ElectronContainer!");
      return StatusCode::FAILURE;
    }
    if(typeid(cont2) != typeid(xAOD::MuonContainer) &&
       typeid(cont2) != typeid(ConstDataVector<xAOD::MuonContainer>)) {
      ATH_MSG_ERROR("Second container arg is not of type MuonContainer!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( findOverlaps(static_cast<const xAOD::ElectronContainer&>(cont1),
                            static_cast<const xAOD::MuonContainer&>(cont2)) );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps between electrons and muons
  //---------------------------------------------------------------------------
  StatusCode EleMuSharedTrkOverlapTool::
  findOverlaps(const xAOD::ElectronContainer& electrons,
               const xAOD::MuonContainer& muons) const
  {
    ATH_MSG_DEBUG("Removing overlapping electrons and muons");

    // Initialize output decorations if necessary
    m_decHelper->initializeDecorations(electrons);
    m_decHelper->initializeDecorations(muons);

    // If removing calo-muons that overlap with electrons,
    // then we need to do it in a separate loop first.
    if(m_removeCaloMuons) {

      // Loop over electrons
      for(const auto electron : electrons){
        if(!m_decHelper->isSurvivingObject(*electron)) continue;

        // Get the original ID track
        const xAOD::TrackParticle* elTrk =
          xAOD::EgammaHelpers::getOriginalTrackParticle(electron);

        // Loop over input calo muons
        for(const auto muon : muons) {
          if(!m_decHelper->isSurvivingObject(*muon)) continue;
          if(muon->muonType() != xAOD::Muon::CaloTagged) continue;

          // Get the muon ID track
          const xAOD::TrackParticle* muTrk =
            muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);

          // Flag the calo muon as overlapping if they share the track
          if(elTrk == muTrk) {
            ATH_CHECK( handleOverlap(muon, electron) );
          }
        }
      }
    }

    // Loop over muons
    for(const auto muon : muons){
      if(!m_decHelper->isSurvivingObject(*muon)) continue;

      // Get the muon ID track
      const xAOD::TrackParticle* muTrk =
        muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);

      // Loop over electrons
      for(const auto electron : electrons) {
        if(!m_decHelper->isSurvivingObject(*electron)) continue;

        // Get the original ID track
        const xAOD::TrackParticle* elTrk =
          xAOD::EgammaHelpers::getOriginalTrackParticle(electron);

        // Flag the electron as overlapping if they share the track
        if(elTrk == muTrk) {
          ATH_CHECK( handleOverlap(electron, muon) );
        }
      }
    }
    return StatusCode::SUCCESS;
  }

} // namespace ORUtils
