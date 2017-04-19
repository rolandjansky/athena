#ifndef ASSOCIATIONUTILS_TAULOOSEELEOVERLAPTOOL_H
#define ASSOCIATIONUTILS_TAULOOSEELEOVERLAPTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// EDM includes
#include "xAODTau/TauJetContainer.h"
#include "xAODEgamma/ElectronContainer.h"

// Local includes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/BaseOverlapTool.h"
#include "AssociationUtils/DeltaRMatcher.h"

namespace ORUtils
{

  /// @class TauLooseEleOverlapTool
  /// @brief A tool implementing the recommended tau-electron overlap removal.
  ///
  /// This implementation has a custom loose-electron selection as recommended
  /// in the harmonization document. Note: this functionality should be already
  /// available in the TauAnalysisTools package, but I'm adding it here for
  /// compatibility with the old OverlapRemovalTool.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class TauLooseEleOverlapTool : public virtual IOverlapTool,
                                 public BaseOverlapTool
  {

      /// Create proper constructor for Athena
      ASG_TOOL_CLASS(TauLooseEleOverlapTool, IOverlapTool)

    public:

      /// Standalone constructor
      TauLooseEleOverlapTool(const std::string& name);

      /// @brief Identify overlapping taus and loose electrons.
      /// TODO: add description of the method.
      virtual StatusCode
      findOverlaps(const xAOD::IParticleContainer& cont1,
                   const xAOD::IParticleContainer& cont2) const override;

      /// @brief Identify overlapping taus and loose electrons.
      /// See the documentation in the above method.
      virtual StatusCode
      findOverlaps(const xAOD::TauJetContainer& taus,
                   const xAOD::ElectronContainer& electrons) const;

    protected:

      /// Initialize the tool
      virtual StatusCode initializeDerived() override;

    protected:

      StatusCode checkElectronID(const xAOD::Electron& electron, bool& pass) const;

    private:

      //
      // Configurable properties
      //

      /// Maximum dR for objects flagged as overlap
      float m_maxDR;
      /// Calculate delta-R using rapidity
      bool m_useRapidity;

      /// Loose electron selection criteria string (e.g. Loose)
      std::string m_eleID;

      /// Alternate fallback loose ID string; mainly convenient for testing.
      /// You should only set this if you really know what you're doing.
      std::string m_altEleID;

      //
      // Utilities
      //

      /// Delta-R matcher
      std::unique_ptr<DeltaRMatcher> m_dRMatcher;

  }; // class TauLooseEleOverlapTool

} // namespace ORUtils

#endif
