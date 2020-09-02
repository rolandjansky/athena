/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System includes
#include <typeinfo>
#include <exception>

// Framework includes
#include "AthContainers/ConstDataVector.h"
#include "FourMomUtils/xAODP4Helpers.h"

// EDM includes
#include "xAODEgamma/EgammaDefs.h"

// Local includes
#include "AssociationUtils/EleEleOverlapTool.h"

namespace
{

  /// Check for dummy float values of variables.
  /// Assuming -999 is the only value of interest for now.
  bool isDummyVal(float x)
  {
    const float epsilon = 1e-5;
    const float dummyVal = -999;
    return std::abs(x - dummyVal) < epsilon;
  }

  // Define simple exception for Dummy/missing values
  class DummyValError : public std::exception
  {};

}

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  EleEleOverlapTool::EleEleOverlapTool(const std::string& name)
    : BaseOverlapTool(name),
      m_useTrackMatch(true),
      m_useClusterMatch(false),
      m_clusterDeltaEta(3*0.025),
      m_clusterDeltaPhi(5*0.025)
  {
    declareProperty("UseTrackMatch", m_useTrackMatch,
                    "Match electrons by shared track");
    declareProperty("UseClusterMatch", m_useClusterMatch,
                    "Match electrons by cluster proximity");
    declareProperty("ClusterDeltaEta", m_clusterDeltaEta,
                    "Cluster matching delta eta");
    declareProperty("ClusterDeltaPhi", m_clusterDeltaPhi,
                    "Cluster matching delta phi");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode EleEleOverlapTool::initializeDerived()
  {
    ATH_MSG_DEBUG("UseTrackMatch " << m_useTrackMatch <<
                  " UseClusterMatch " << m_useClusterMatch <<
                  " ClusterDeltaEta " << m_clusterDeltaEta <<
                  " ClusterDeltaPhi " << m_clusterDeltaPhi);

    // Sanity check
    if(!m_useTrackMatch && !m_useClusterMatch) {
      ATH_MSG_ERROR("You must enable at least one: UseTrackMatch or UseClusterMatch");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode EleEleOverlapTool::
  findOverlaps(const xAOD::IParticleContainer& cont1,
               const xAOD::IParticleContainer& cont2) const
  {
    // I require that the two containers are the same so that I can
    // arbitrarily pick one of them to use.
    if(&cont1 != &cont2) {
      ATH_MSG_ERROR("This tool expects both electron containers to be the " <<
                    "same for now");
      return StatusCode::FAILURE;
    }
    // Check the container type
    if(typeid(cont1) != typeid(xAOD::ElectronContainer) &&
       typeid(cont1) != typeid(ConstDataVector<xAOD::ElectronContainer>)) {
      ATH_MSG_ERROR("Container is not an ElectronContainer!");
      return StatusCode::FAILURE;
    }
    // Call the type-specific method
    ATH_CHECK( findOverlaps(static_cast<const xAOD::ElectronContainer&>(cont1)) );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode EleEleOverlapTool::
  findOverlaps(const xAOD::ElectronContainer& electrons) const
  {
    ATH_MSG_DEBUG("Removing overlapping electrons");

    // Initialize output decorations if necessary
    m_decHelper->initializeDecorations(electrons);

    // TODO: consider adding cluster-based matching also

    // Loop over surviving electron pairs
    for(const auto el1 : electrons) {
      if(!m_decHelper->isSurvivingObject(*el1)) continue;
      for(const auto el2 : electrons) {
        if(el1 == el2) continue;
        if(!m_decHelper->isSurvivingObject(*el2)) continue;

        // Perform the match and decide whether to reject el1
        try {
          if(electronsMatch(*el1, *el2) && rejectFirst(*el1, *el2)) {
            ATH_CHECK( handleOverlap(el1, el2) );
          }
        }
        catch(const DummyValError& e) {
          ATH_MSG_ERROR("Cluster 2nd sampling eta/phi values are -999. " <<
                        "It seems you are missing the neccesary variables to "
                        "do the requested electron-electron cluster matching");
          return StatusCode::FAILURE;
        }
      }
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Apply the ele-ele matching criteria
  //---------------------------------------------------------------------------
  bool EleEleOverlapTool::
  electronsMatch(const xAOD::Electron& el1, const xAOD::Electron& el2) const
  {
    // Look for a shared track
    if(m_useTrackMatch && el1.trackParticleLink() == el2.trackParticleLink())
      return true;

    // Look for overlapping clusters
    if(m_useClusterMatch) {
      const auto& clus1 = *el1.caloCluster();
      const auto& clus2 = *el2.caloCluster();
      using xAOD::P4Helpers::deltaPhi;

      // We use coordinates from 2nd sampling
      const unsigned layer = 2;
      const float eta1 = clus1.etaBE(layer);
      const float eta2 = clus2.etaBE(layer);
      const float phi1 = clus1.phiBE(layer);
      const float phi2 = clus2.phiBE(layer);

      // Check validity of the eta/phi (no dummy -999 values)
      if(isDummyVal(eta1) || isDummyVal(eta2) ||
         isDummyVal(phi1) || isDummyVal(phi2)) {
        throw DummyValError();
      }

      const float dEta = clus1.etaBE(layer) - clus2.etaBE(layer);
      const float dPhi = deltaPhi(clus1.phiBE(layer), clus2.phiBE(layer));

      if( std::abs(dEta) < m_clusterDeltaEta &&
          std::abs(dPhi) < m_clusterDeltaPhi )
      {
        return true;
      }
    }

    return false;
  }

  //---------------------------------------------------------------------------
  // Decide whether to reject the first electron compared to the 2nd.
  // This function assumes a matching criteria has already been applied.
  //---------------------------------------------------------------------------
  bool EleEleOverlapTool::
  rejectFirst(const xAOD::Electron& el1, const xAOD::Electron& el2) const
  {
    // TODO: consider incorporating track-match information in the priority
    // selection.

    // Always reject author "Ambiguous" when compared to author "Electron"
    if(el1.author() == xAOD::EgammaParameters::AuthorAmbiguous &&
       el2.author() == xAOD::EgammaParameters::AuthorElectron)
      return true;
    if(el1.author() == xAOD::EgammaParameters::AuthorElectron &&
       el2.author() == xAOD::EgammaParameters::AuthorAmbiguous)
      return false;

    // Reject the softer electron
    if(el1.pt() < el2.pt()) return true;

    return false;
  }

} // namespace ORUtils
