#ifndef ASSOCIATIONUTILS_TAU_ANTITAU_JET_OVERLAPTOOL_H
#define ASSOCIATIONUTILS_TAU_ANTITAU_JET_OVERLAPTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// EDM includes
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/JetContainer.h"

// Local includes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/BaseOverlapTool.h"
#include "AssociationUtils/BJetHelper.h"
#include "AssociationUtils/IObjectAssociator.h"

namespace ORUtils
{

  /// @class TauAntiTauJetOverlapTool
  /// @brief A tool implementing a specialized tau-jet overlap removal.
  ///
  /// This tool was implemented by request for the h->bbtautau analysis which
  /// uses a loose-ish btag working point and also considers anti-tau ID.
  ///
  /// Object precedence:
  ///   ID-tau > bjet > anti-tau > light-jet
  ///
  /// The procedure works as follows.
  ///   1. Remove bjets overlapping with ID taus.
  ///   2. Remove anti-taus from remaining bjets.
  ///   3. Remove light jets from remaining ID-taus and anti-taus.
  ///
  /// To fully utilize the above procedure you have to set the following
  /// properties:
  ///
  /// * AntiTauLabel: The user-set decoration name labeling anti-taus.
  ///   E.g., "isAntiTau". Default is empty, which disables anti-taus.
  ///
  /// * BJetLabel: The usual user-set decoration name labeling bjets.
  ///   E.g., "isBJet". Default is empty, which disables bjet OR.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class TauAntiTauJetOverlapTool : public virtual IOverlapTool,
                                   public BaseOverlapTool
  {

      /// Create proper constructor for Athena
      ASG_TOOL_CLASS(TauAntiTauJetOverlapTool, IOverlapTool)

    public:

      /// Standalone constructor
      TauAntiTauJetOverlapTool(const std::string& name);

      /// @brief Identify overlapping taus and jets.
      virtual StatusCode
      findOverlaps(const xAOD::IParticleContainer& cont1,
                   const xAOD::IParticleContainer& cont2) const override;

      /// @brief Identify overlapping taus and jets.
      /// The above method calls this one.
      virtual StatusCode
      findOverlaps(const xAOD::JetContainer& jets,
                   const xAOD::TauJetContainer& taus) const;

    protected:
      /// @name Helper methods
      /// @{

    protected:

      /// Initialize the tool
      virtual StatusCode initializeDerived() override;

      /// Is this jet a b-jet? Returns false if bjet ID not configured.
      /// Does not check if the jet is "surviving" OR.
      bool isBJet(const xAOD::Jet& jet) const;

      /// Is this an anti-tau? Returns false if anti-tau ID not configured.
      /// This one does check if the tau is "surviving" OR.
      bool isSurvivingAntiTau(const xAOD::TauJet& tau) const;

    private:
      /// @name Configurable properties
      /// @{

      /// Input jet decoration which labels a bjet
      std::string m_bJetLabel;

      /// Decoration labelling an anti-tau
      std::string m_antiTauLabel;

      /// Flat delta-R cone for matching objects
      float m_dR;
      /// Calculate deltaR using rapidity
      bool m_useRapidity;

      /// @}

    private:
      /// @name Utilities
      /// @{

      /// BJet helper
      std::unique_ptr<BJetHelper> m_bJetHelper;

      /// Delta-R matcher
      std::unique_ptr<IParticleAssociator> m_dRMatcher;

      /// Accessor for reading the anti-tau decoration
      //std::unique_ptr<ort::inputAccessor_t> m_antiTauAcc;

      /// Decoration helper for the anti-taus
      std::unique_ptr<OverlapDecorationHelper> m_antiTauDecHelper;

      /// @}

  }; // class TauAntiTauJetOverlapTool

} // namespace ORUtils

#endif
