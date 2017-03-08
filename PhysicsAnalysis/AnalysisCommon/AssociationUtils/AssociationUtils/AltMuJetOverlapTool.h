#ifndef ASSOCIATIONUTILS_ALTMUJETOVERLAPTOOL_H
#define ASSOCIATIONUTILS_ALTMUJETOVERLAPTOOL_H

// EDM includes
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"

// Local includes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/BaseOverlapTool.h"
#include "AssociationUtils/BJetHelper.h"
#include "AssociationUtils/IObjectAssociator.h"

namespace ORUtils
{

  /// @class AltMuJetOverlapTool
  /// @brief Implements mu-jet OR optimized for high-pt muons in boosted
  /// and highly radiative (jetty) topologies.
  ///
  /// This implementation comes from the strong 2Lep SUSY analysis studies
  /// which were presented in the SUSY meeting here:
  /// https://indico.cern.ch/event/436702
  ///
  /// The procedure is two-staged:
  ///   1. Remove (non-b-tagged) jets overlapping with muons in an inner
  ///      delta-R cone (default 0.2).
  ///   2. Remove muons overlapping with remaining jets using the sliding
  ///      delta-R cone. See the SlidingDeltaRMatcher for more info.
  ///
  /// Special configuration properties
  ///   BJetLabel - input decoration flagging b-jets
  ///   InnerDR - cone in which non-bjets are removed (default 0.2)
  ///   SlidingDRC1 - constant offset for sliding dR (default 0.04)
  ///   SlidingDRC2 - mu-pt dividend for sliding dR (default 10GeV)
  ///   SlidingDRMaxCone - max cone size cutoff for sliding dR (default: off)
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class AltMuJetOverlapTool : public virtual IOverlapTool,
                              public BaseOverlapTool
  {

      /// Create proper constructor for Athena
      ASG_TOOL_CLASS(AltMuJetOverlapTool, IOverlapTool)

    public:

      /// Standalone constructor
      AltMuJetOverlapTool(const std::string& name);

      /// @brief Identify overlapping muons and jets.
      /// First, low-track-multiplicity jets are flagged if they overlap with
      /// muons. Second, muons are flagged if they overlap with the remaining
      /// jets.
      virtual StatusCode
      findOverlaps(const xAOD::IParticleContainer& cont1,
                   const xAOD::IParticleContainer& cont2) const override;

      /// @brief Identify overlapping muons and jets.
      /// The above method calls this one.
      virtual StatusCode
      findOverlaps(const xAOD::MuonContainer& muons,
                   const xAOD::JetContainer& jets) const;

    protected:

      /// Initialize the tool
      virtual StatusCode initializeDerived() override;

      /// Retrieve the primary vertex used to count jet tracks.
      /// TODO: reduce duplication with MuJetOverlapTool.
      const xAOD::Vertex* getPrimVtx() const;

      /// Helper method to get the number of tracks in a jet w.r.t.
      /// the primary vertex. Returns -1 if no primary vertex is found.
      /// TODO: reduce duplication with MuJetOverlapTool.
      int getNumTracks(const xAOD::Jet* jet) const;

    private:

      //
      // Configurable properties
      //

      /// Input jet decoration which labels a bjet
      std::string m_bJetLabel;

      /// Minimum number of tracks to keep an overlapping jet
      int m_numJetTrk;
      /// Maximum ratio of mu/jet PT to keep an overlapping jet
      float m_muJetPtRatio;

      /// Inner dR cone within which jets get removed
      float m_innerDR;
      /// Outer cone C1, the constant offset in sliding dR
      double m_slidingDRC1;
      /// Outer cone C2, the inverse pt factor in sliding dR
      double m_slidingDRC2;
      /// MaxCone, the upper limit of the sliding cone
      double m_slidingDRMaxCone;
      /// Calculate deltaR using rapidity
      bool m_useRapidity;

      //
      // Utilities
      //

      /// BJet helper
      std::unique_ptr<BJetHelper> m_bJetHelper;

      /// Delta-R matcher for the inner cone
      std::unique_ptr<IParticleAssociator> m_dRMatchCone1;
      /// Delta-R matcher for the outer cone
      std::unique_ptr<IParticleAssociator> m_dRMatchCone2;

  }; // class AltMuJetOverlapTool

} // namespace ORUtils

#endif
