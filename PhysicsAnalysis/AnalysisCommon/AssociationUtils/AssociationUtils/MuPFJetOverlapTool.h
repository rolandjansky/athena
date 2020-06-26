/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_MUPFJETOVERLAPTOOL_H
#define ASSOCIATIONUTILS_MUPFJETOVERLAPTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// EDM includes
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexContainer.h"

// Local includes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/BaseOverlapTool.h"
#include "AssociationUtils/BJetHelper.h"
#include "AssociationUtils/IObjectAssociator.h"

namespace ORUtils
{

  /// @class MuPFJetOverlapTool
  /// @brief A tool implementing the recommended removal of PFlow jets which 
  ///        are reconstructed from muon ID tracks.
  ///
  /// This tool takes muons and jets and removes jets based on
  /// various criteria including delta-R, jet track multiplicity, mu_ID_pt/jet_SumTrkPt
  /// ratio and muon calo isolation (topoetcone40 variable).
  ///
  /// The variable used for calo isolation is topoetcone40 and must be present.
  /// For the jets the variables NumTrkPt500 and SumPtTrkPt500 must be present.
  ///
  /// The procedure works as follows.
  ///   - Remove jets that overlap with muons which satisfy a variety of criteria.
  ///     Two different windows in nTrk are used, and in each a cut is performed of the form
  ///     topoetcone40<y0
  ///     OR topoetcone40<y0+(y2-y1)/(x2-x1)*(mu_id_pt/jet_SumPtTrkPt500-x1)
  ///     OR mu_id_pt/jet_SumPtTrkPt500>x2
  ///    
  ///   - Note no muons are removed in this procedure
  ///
  /// To determine jet track properties, the primary vertex is retrieved
  /// and used to query the NumTrkPt500 and SumPtTrkPt500 jet attribute.
  ///
  /// Some special configuration properties
  ///   UseSlidingDR     - activate sliding dR cone (c1 + c2/muPt)
  ///   
  ///
  /// These properties are useful for specifying PV-related quantities so that
  /// the tool doesn't need to retrieve the PV:
  ///   JetNumTrackDecoration   - user-defined numTrk decoration
  ///   JetSumTrackPTDecoration - user-defined sumTrkPt decoration
  ///
  /// @author Chris Young <christopher.young@cern.ch>
  ///
  class MuPFJetOverlapTool : public virtual IOverlapTool,
                           public BaseOverlapTool
  {

      /// Create proper constructor for Athena
      ASG_TOOL_CLASS(MuPFJetOverlapTool, IOverlapTool)

    public:

      /// Standalone constructor
      MuPFJetOverlapTool(const std::string& name);

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

    private:

      /// Retrieve the primary vertex used to count jet tracks
      const xAOD::Vertex* getPrimVtx() const;

      /// Get the number of tracks in a jet w.r.t. requested vertex
      int getNumTracks(const xAOD::Jet& jet, size_t vtxIdx) const;

      /// Get the sum trk pt in a jet w.r.t. requested vertex
      float getSumTrackPt(const xAOD::Jet& jet, size_t vtxIdx) const;

      /// @name Configurable properties
      /// @{

      /// Input jet decoration which labels a bjet
      std::string m_bJetLabel;

      /// Minimum number of jet tracks to use the looser jet rejection criteria
      int m_numJetTrk;
      /// The 5 parameters that define the low nTrk criteria for removing jets.
      float m_muPFJet_lowNtrk_x1;
      float m_muPFJet_lowNtrk_x2;
      float m_muPFJet_lowNtrk_y0;
      float m_muPFJet_lowNtrk_y1;
      float m_muPFJet_lowNtrk_y2;
      /// The 5 parameters that define the high nTrk criteria for removing jets.
      float m_muPFJet_highNtrk_x1;
      float m_muPFJet_highNtrk_x2;
      float m_muPFJet_highNtrk_y0;
      float m_muPFJet_highNtrk_y1;
      float m_muPFJet_highNtrk_y2;

      /// Optional user decoration for jet numTrack of type 'int'
      std::string m_jetNumTrkDec;
      /// Optional user decoration for jet sumTrackPT of type 'float'
      std::string m_jetSumTrkPtDec;

      /// dR cone within which jets can get removed
      float m_coneDR;

      /// Calculate deltaR using rapidity
      bool m_useRapidity;

      /// @}

      /// @name Utilities
      /// @{

      /// BJet helper
      std::unique_ptr<BJetHelper> m_bJetHelper;
      /// Delta-R matcher for the cone
      std::unique_ptr<IParticleAssociator> m_dRMatchCone;

      /// @}

  }; // class MuPFJetOverlapTool

} // namespace ORUtils

#endif
