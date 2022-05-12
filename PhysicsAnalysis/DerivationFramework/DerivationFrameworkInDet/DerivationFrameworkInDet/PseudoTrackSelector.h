/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PseudoTrackSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_PseudoTrackSelector_H
#define DERIVATIONFRAMEWORK_PseudoTrackSelector_H

#include <string>
#include <map>
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "InDetTrackSystematicsTools/IInDetTrackTruthOriginTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "ExpressionEvaluation/ExpressionParserUser.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"

namespace DerivationFramework {
  
  /** @brief Class-algorithm for pseudo track selection */
  class PseudoTrackSelector : public AthAlgTool, public IAugmentationTool
    {
    public:      
      ///////////////////////////////////////////////////////////////////
      /** @brief Standard Algotithm methods:                           */
      ///////////////////////////////////////////////////////////////////

      PseudoTrackSelector(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~PseudoTrackSelector() {}
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();
      virtual StatusCode addBranches() const;
 
    protected:
      ///////////////////////////////////////////////////////////////////
      /** @brief Protected data:                                       */
      ///////////////////////////////////////////////////////////////////
      SG::ReadHandleKey<xAOD::TrackParticleContainer>  m_in_recoTrackParticleLocation;       /** Reco track collection.   */
      SG::ReadHandleKey<xAOD::TrackParticleContainer>  m_in_pseudoTrackParticleLocation;     /** Pseudo track collection. */
      SG::WriteHandleKey<xAOD::TrackParticleContainer> m_out_recoReplacedWithPseudo;         /** Output track collection. */
      SG::WriteHandleKey<xAOD::TrackParticleContainer> m_out_recoReplacedWithPseudoFromB;    /** Output track collection. */
      SG::WriteHandleKey<xAOD::TrackParticleContainer> m_out_recoReplacedWithPseudoNotFromB; /** Output track collection. */
      SG::WriteHandleKey<xAOD::TrackParticleContainer> m_out_recoPlusPseudo;                 /** Output track collection. */
      SG::WriteHandleKey<xAOD::TrackParticleContainer> m_out_recoPlusPseudoFromB;            /** Output track collection. */
      SG::WriteHandleKey<xAOD::TrackParticleContainer> m_out_recoPlusPseudoNotFromB;         /** Output track collection. */
      SG::WriteHandleKey<xAOD::TrackParticleContainer> m_out_recoNoFakes;                    /** Output track collection. */
      SG::WriteHandleKey<xAOD::TrackParticleContainer> m_out_recoNoFakesFromB;               /** Output track collection. */
      SG::WriteHandleKey<xAOD::TrackParticleContainer> m_out_recoNoFakesNotFromB;            /** Output track collection. */

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected methods:                                    */
      ///////////////////////////////////////////////////////////////////

      /** @brief Routines that selects the relevant (pseudo) tracks. */
      void fillRecoReplacedWithPseudo(const xAOD::TrackParticleContainer* recoTrackParticleCol,
                                      const xAOD::TrackParticleContainer* pseudoTrackParticleCol,
                                      xAOD::TrackParticleContainer* outputCol,
                                      bool onlyFromB = false,
                                      bool onlyNotFromB = false) const;
      void fillRecoPlusPseudo(const xAOD::TrackParticleContainer* recoTrackParticleCol,
                              const xAOD::TrackParticleContainer* pseudoTrackParticleCol,
                              xAOD::TrackParticleContainer* outputCol,
                              bool onlyFromB = false,
                              bool onlyNotFromB = false) const;
      void fillRecoNoFakes(const xAOD::TrackParticleContainer* recoTrackParticleCol,
                           xAOD::TrackParticleContainer* outputCol,
                           bool onlyFromB = false,
                           bool onlyNotFromB = false) const;
      // Get truth particle associated to a given track particle
      const xAOD::TruthParticle* getTruth( const xAOD::TrackParticle* track ) const;

    private:
        // convience types
        typedef ElementLink<xAOD::TruthParticleContainer> TruthLink;
        // Track origin tool
        ToolHandle< InDet::IInDetTrackTruthOriginTool > m_trackOriginTool;
    };
}
#endif // DERIVATIONFRAMEWORK_PseudoTrackSelector_H