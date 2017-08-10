#ifndef ASSOCIATIONUTILS_TAULOOSEMUOVERLAPTOOL_H
#define ASSOCIATIONUTILS_TAULOOSEMUOVERLAPTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// EDM includes
#include "xAODTau/TauJetContainer.h"
#include "xAODMuon/MuonContainer.h"

// Local includes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/BaseOverlapTool.h"
#include "AssociationUtils/DeltaRMatcher.h"

namespace ORUtils
{

  /// @class TauLooseMuOverlapTool
  /// @brief A tool implementing the recommended tau-muon overlap removal.
  ///
  /// This implementation has a custom loose-muon selection as recommended
  /// in the harmonization document. Note: this functionality should be already
  /// available in the TauAnalysisTools package, but I'm adding it here for
  /// compatibility with the old OverlapRemovalTool.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class TauLooseMuOverlapTool : public virtual IOverlapTool,
                                public BaseOverlapTool
  {

      /// Create proper constructor for Athena
      ASG_TOOL_CLASS(TauLooseMuOverlapTool, IOverlapTool)

    public:

      /// Standalone constructor
      TauLooseMuOverlapTool(const std::string& name);

      /// @brief Identify overlapping taus and loose muons.
      /// TODO: add description of the method.
      virtual StatusCode
      findOverlaps(const xAOD::IParticleContainer& cont1,
                   const xAOD::IParticleContainer& cont2) const override;

      /// @brief Identify overlapping taus and loose muons.
      /// See the documentation in the above method.
      virtual StatusCode
      findOverlaps(const xAOD::TauJetContainer& taus,
                   const xAOD::MuonContainer& muons) const;

    protected:

      /// Initialize the tool
      virtual StatusCode initializeDerived() override;

    private:

      //
      // Configurable properties
      //

      /// Maximum dR for objects flagged as overlap
      float m_maxDR;
      /// Calculate delta-R using rapidity
      bool m_useRapidity;

      /// Minimum muon PT to reject a tau
      float m_minMuPt;
      /// Tau PT threshold to compare to combined muons only
      float m_minTauPtMuComb;

      //
      // Utilities
      //

      /// Delta-R matcher
      std::unique_ptr<DeltaRMatcher> m_dRMatcher;

  }; // class TauLooseMuOverlapTool

} // namespace ORUtils

#endif
