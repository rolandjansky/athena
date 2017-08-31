/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_ELEJETOVERLAPTOOL_H
#define ASSOCIATIONUTILS_ELEJETOVERLAPTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// EDM includes
#include "xAODEgamma/ElectronContainer.h"
#include "xAODJet/JetContainer.h"

// Local includes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/BaseOverlapTool.h"
#include "AssociationUtils/BJetHelper.h"
#include "AssociationUtils/DeltaRMatcher.h"

namespace ORUtils
{

  /// @class EleJetOverlapTool
  /// @brief A tool implementing the recommended ele-jet overlap removal.
  ///
  /// This tool takes electrons and jets and removes their overlaps based on
  /// various criteria include delta-R, and b-tagging results.
  ///
  /// The procedure works as follows:
  ///   1. Remove non-btagged jets that overlap with electrons in an inner
  ///      delta-R cone.
  ///   2. Remove electrons that overlap with surviving non-pileup jets in an
  ///      outer delta-R cone.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class EleJetOverlapTool : public virtual IOverlapTool,
                            public BaseOverlapTool
  {

      /// Create proper constructor for Athena
      ASG_TOOL_CLASS(EleJetOverlapTool, IOverlapTool)

    public:

      /// Standalone constructor
      EleJetOverlapTool(const std::string& name);

      /// @brief Identify overlapping electrons and jets.
      /// First, electrons are flagged for removal if they overlap with
      /// non-b-labeled jets within the inner dR cone. Next, jets are flagged
      /// for removal if they overlap with remaining electrons in the outer dR cone.
      virtual StatusCode
      findOverlaps(const xAOD::IParticleContainer& cont1,
                   const xAOD::IParticleContainer& cont2) const override;

      /// @brief Identify overlapping electrons and jets.
      /// The above method calls this one.
      virtual StatusCode
      findOverlaps(const xAOD::ElectronContainer& electrons,
                   const xAOD::JetContainer& jets) const;

    protected:

      /// Initialize the tool
      virtual StatusCode initializeDerived() override;

    private:

      /// @name Configurable properties
      /// @{

      /// Input jet decoration which labels a bjet
      std::string m_bJetLabel;

      /// Toggle PT ratio criteria
      bool m_applyPtRatio;
      /// Minimum e/jet pt ratio to remove a jet
      double m_eleJetPtRatio;

      /// Inner dR cone within which jets get removed
      float m_innerDR;
      /// Outer dR cone within which electrons get removed
      float m_outerDR;

      /// Activate sliding dR for the cone which removes electrons
      bool m_useSlidingDR;
      /// Sliding cone C1
      double m_slidingDRC1;
      /// Sliding cone C2
      double m_slidingDRC2;
      /// Sliding cone max size
      double m_slidingDRMaxCone;

      /// Calculate deltaR using rapidity
      bool m_useRapidity;

      /// @}

      /// @name Utilities
      /// @{

      /// BJet helper
      std::unique_ptr<BJetHelper> m_bJetHelper;

      /// Delta-R matcher for the inner cone
      std::unique_ptr<IParticleAssociator> m_dRMatchCone1;
      /// Delta-R matcher for the outer cone
      std::unique_ptr<IParticleAssociator> m_dRMatchCone2;

      /// @}

  }; // class EleJetOverlapTool

} // namespace ORUtils

#endif
