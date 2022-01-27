/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetSecVertexTruthMatchTool_h
#define InDetSecVertexTruthMatchTool_h

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ITHistSvc.h"



// EDM include(s):
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTruth/TruthParticleFwd.h"
#include "xAODTruth/TruthVertexFwd.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "InDetSecVertexValidation/IInDetSecVertexTruthMatchTool.h"

// standard includes
#include<vector>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>



namespace InDetSecVertexTruthMatchUtils {

  //Namespace for useful analysis things on the truth matching decorations applied to the VertexContainer

  //How the matching info is stored; link to the truth vertex, a float with its contribution to the relative track weight, and a float with its contribution to the track sumpt2 of the truth vertex
  typedef std::tuple<ElementLink<xAOD::TruthVertexContainer>, float, float> VertexTruthMatchInfo;

  //type codes for vertex matching on all vertices
  enum VertexMatchType {
    MATCHED, // > threshold (default 70%) from one truth interaction
    MERGED,  // not matched
    SPLIT,   // highest weight truth interaction contributes to >1 vtx (vtx with highest fraction of sumpT2 remains matched/merged)
    FAKE,    // highest contribution is fake (if pile-up MC info not present those tracks end up as "fakes")
    NTYPES   // access to number of types
  };

  //type codes for truth vertices
  //NOTE: types are subsets of subsequent types
  enum TruthVertexMatchType {
    INCLUSIVE, // all vertices
    RECONSTRUCTABLE, // fiducial cuts, >= 2 charged daughters
    ACCEPTED, // >= 2 reco tracks
    SEEDED, // tracks pass cuts
    RECONSTRUCTED, // matched to reco vtx
    RECONSTRUCTEDSPLIT, // matched to multiple vtx
  };

}

/** Class for vertex truth matching.
 * Match reconstructed vertices to truth level decay vertices
 * Categorize reconstructed vertices depending on their composition.
 */
class InDetSecVertexTruthMatchTool : public virtual IInDetSecVertexTruthMatchTool,
                                     public asg::AsgTool {

  ASG_TOOL_CLASS( InDetSecVertexTruthMatchTool, IInDetSecVertexTruthMatchTool )

 public:

  InDetSecVertexTruthMatchTool( const std::string & name );

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override;

  //take const collection of vertices, match them, and decorate with matching info
  virtual StatusCode matchVertices( const xAOD::VertexContainer& vtxContainer, const xAOD::TruthVertexContainer& truthVtxContainer ) const override;
  //take const collection of truth vertices and decorate with type info
  virtual StatusCode labelTruthVertices( const xAOD::TruthVertexContainer & truthVtxContainer ) const override;

 private:

  //required MC match probability to consider track a good match
  float m_trkMatchProb;
  //relative weight threshold to consider vertex matched
  float m_vxMatchWeight;
  //pt cut to use on tracks
  float m_trkPtCut;
  //comma separated list of pdgids to consider in the matchiing
  std::string m_pdgIds;
  //turn on/off histogram output
  bool m_fillHist;

  //private methods to check if particles are good to use
  //returns barcode of LLP production truth vertex
  int checkProduction( const xAOD::TruthParticle& truthPart ) const;
  void countReconstructibleDescendentParticles(const xAOD::TruthVertex& signalTruthVertex,
                                               std::vector<const xAOD::TruthParticle*>& set) const;
  std::vector<int> checkParticle( const xAOD::TruthParticle& part, const xAOD::TrackParticleContainer &tkCont ) const;

  // (optional) write out histograms
  virtual StatusCode fillRecoPlots( const xAOD::Vertex& secVtx ) const;
  virtual StatusCode fillTruthPlots( const xAOD::TruthVertex& truthVtx ) const;

  //private internal variables (not properties)
  std::vector<int> m_pdgIdList;

  //histograms
  ITHistSvc* m_thistSvc{nullptr};
  TH1F* m_matchType = nullptr;
  TH1F* m_truth_Ntrk = nullptr;
  TH1F* m_positionRes_R = nullptr;
  TH1F* m_positionRes_Z = nullptr;

  TH1F* m_truthInclusive_r = nullptr;
  TH1F* m_truthReconstructable_r = nullptr;
  TH1F* m_truthAccepted_r = nullptr;
  TH1F* m_truthSeeded_r = nullptr;
  TH1F* m_truthReconstructed_r = nullptr;
  TH1F* m_truthSplit_r = nullptr;

  TH1F* m_truthReconstructable_trkSel = nullptr;
  TH1F* m_truthReconstructed_trkSel = nullptr;

  TH1F* m_recoX = nullptr;
  TH1F* m_recoY = nullptr;
  TH1F* m_recoZ = nullptr;
  TH1F* m_recoR = nullptr;
  TH1F* m_recodistFromPV = nullptr;
  TH1F* m_recoPt = nullptr;
  TH1F* m_recoEta = nullptr;
  TH1F* m_recoPhi = nullptr;
  TH1F* m_recoMass = nullptr;
  TH1F* m_recoMu = nullptr;
  TH1F* m_recoChi2 = nullptr;
  TH1F* m_recoDir = nullptr;
  TH1F* m_recoCharge = nullptr;
  TH1F* m_recoH = nullptr;
  TH1F* m_recoHt = nullptr;
  TH1F* m_recoMinOpAng = nullptr;
  TH1F* m_recoMaxOpAng = nullptr;
  TH1F* m_recoMaxDR = nullptr;
  TH1F* m_recoMinD0 = nullptr;
  TH1F* m_recoMaxD0 = nullptr;
  TH1F* m_recoNtrk = nullptr;

  TH1F* m_recoR_LLP = nullptr;
  TH1F* m_recoPt_LLP = nullptr;
  TH1F* m_recoEta_LLP = nullptr;
  TH1F* m_recoPhi_LLP = nullptr;
  TH1F* m_recoMass_LLP = nullptr;
  TH1F* m_recoMu_LLP = nullptr;
  TH1F* m_recoChi2_LLP = nullptr;
  TH1F* m_recoDir_LLP = nullptr;
  TH1F* m_recoCharge_LLP = nullptr;
  TH1F* m_recoH_LLP = nullptr;
  TH1F* m_recoHt_LLP = nullptr;
  TH1F* m_recoMinOpAng_LLP = nullptr;
  TH1F* m_recoMaxOpAng_LLP = nullptr;
  TH1F* m_recoMaxDR_LLP = nullptr;
  TH1F* m_recoMinD0_LLP = nullptr;
  TH1F* m_recoMaxD0_LLP = nullptr;
  TH1F* m_recoNtrk_LLP = nullptr;

  TH1F* m_recoR_Fake = nullptr;
  TH1F* m_recoPt_Fake = nullptr;
  TH1F* m_recoEta_Fake = nullptr;
  TH1F* m_recoPhi_Fake = nullptr;
  TH1F* m_recoMass_Fake = nullptr;
  TH1F* m_recoMu_Fake = nullptr;
  TH1F* m_recoChi2_Fake = nullptr;
  TH1F* m_recoDir_Fake = nullptr;
  TH1F* m_recoCharge_Fake = nullptr;
  TH1F* m_recoH_Fake = nullptr;
  TH1F* m_recoHt_Fake = nullptr;
  TH1F* m_recoMinOpAng_Fake = nullptr;
  TH1F* m_recoMaxOpAng_Fake = nullptr;
  TH1F* m_recoMaxDR_Fake = nullptr;
  TH1F* m_recoMinD0_Fake = nullptr;
  TH1F* m_recoMaxD0_Fake = nullptr;
  TH1F* m_recoNtrk_Fake = nullptr;

  TH1F* m_recoR_Split = nullptr;
  TH1F* m_recoPt_Split = nullptr;
  TH1F* m_recoEta_Split = nullptr;
  TH1F* m_recoPhi_Split = nullptr;
  TH1F* m_recoMass_Split = nullptr;
  TH1F* m_recoMu_Split = nullptr;
  TH1F* m_recoChi2_Split = nullptr;
  TH1F* m_recoDir_Split = nullptr;
  TH1F* m_recoCharge_Split = nullptr;
  TH1F* m_recoH_Split = nullptr;
  TH1F* m_recoHt_Split = nullptr;
  TH1F* m_recoMinOpAng_Split = nullptr;
  TH1F* m_recoMaxOpAng_Split = nullptr;
  TH1F* m_recoMaxDR_Split = nullptr;
  TH1F* m_recoMinD0_Split = nullptr;
  TH1F* m_recoMaxD0_Split = nullptr;
  TH1F* m_recoNtrk_Split = nullptr;


  TH1F* m_recoTrk_qOverP = nullptr;
  TH1F* m_recoTrk_theta = nullptr; 
  TH1F* m_recoTrk_E = nullptr; 
  TH1F* m_recoTrk_M = nullptr; 
  TH1F* m_recoTrk_Pt = nullptr;  
  TH1F* m_recoTrk_Px = nullptr; 
  TH1F* m_recoTrk_Py = nullptr; 
  TH1F* m_recoTrk_Pz = nullptr;  
  TH1F* m_recoTrk_Eta = nullptr; 
  TH1F* m_recoTrk_Phi = nullptr;
  TH1F* m_recoTrk_D0 = nullptr; 
  TH1F* m_recoTrk_Z0 = nullptr;
  TH1F* m_recoTrk_errD0 = nullptr; 
  TH1F* m_recoTrk_errZ0 = nullptr; 
  TH1F* m_recoTrk_Chi2 = nullptr;
  TH1F* m_recoTrk_nDoF = nullptr; 
  TH1F* m_recoTrk_charge = nullptr;
  
  TH1F* m_matchScore_weight = nullptr;
  TH1F* m_matchScore_pt = nullptr;
  TH1F* m_matchedTruthID = nullptr;  
  
  TH1F* m_truthX = nullptr;  
  TH1F* m_truthY = nullptr;
  TH1F* m_truthZ = nullptr;  
  TH1F* m_truthR = nullptr;
  TH1F* m_truthdistFromPV = nullptr; 
  TH1F* m_truthEta = nullptr;  
  TH1F* m_truthPhi = nullptr; 
  TH1F* m_truthNtrk_out = nullptr;
  TH1F* m_truthParent_E = nullptr;
  TH1F* m_truthParent_M = nullptr; 
  TH1F* m_truthParent_Pt = nullptr;
  TH1F* m_truthParent_Eta = nullptr; 
  TH1F* m_truthParent_Phi = nullptr;
  TH1F* m_truthParent_charge = nullptr; 
  TH1F* m_truthParentProdX = nullptr; 
  TH1F* m_truthParentProdY = nullptr; 
  TH1F* m_truthParentProdZ = nullptr; 
  TH1F* m_truthParentProdR = nullptr;
  TH1F* m_truthParentProddistFromPV = nullptr;
  TH1F* m_truthParentProdEta = nullptr; 
  TH1F* m_truthParentProdPhi = nullptr;
  

};

#endif
