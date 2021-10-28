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
  TH1F* m_matchType;
  TH1F* m_truth_Ntrk;
  TH1F* m_seedPosition_r;
  TH1F* m_seedPosition_z;
  TH1F* m_seedBadPosition_r;
  TH1F* m_seedBadPosition_z;
  TH1F* m_seedAllPosition_r;
  TH1F* m_seedAllPosition_z;
  TH1F* m_positionRes_R;
  TH1F* m_positionRes_Z;

  TH1F* m_truthInclusive_r;
  TH1F* m_truthReconstructable_r;
  TH1F* m_truthAccepted_r;
  TH1F* m_truthSeeded_r;
  TH1F* m_truthReconstructed_r;
  TH1F* m_truthSplit_r;

  TH1F* m_truthReconstructable_trkSel;
  TH1F* m_truthReconstructed_trkSel;

  TH1F* m_recoX;
  TH1F* m_recoY;
  TH1F* m_recoZ;
  TH1F* m_recoR;
  TH1F* m_recodistFromPV;
  TH1F* m_recoPt;
  TH1F* m_recoEta;
  TH1F* m_recoPhi;
  TH1F* m_recoMass;
  TH1F* m_recoMu;
  TH1F* m_recoChi2;
  TH1F* m_recoDir;
  TH1F* m_recoCharge;
  TH1F* m_recoH;
  TH1F* m_recoHt;
  TH1F* m_recoMinOpAng;
  TH1F* m_recoMaxOpAng;
  TH1F* m_recoMaxDR;
  TH1F* m_recoMinD0;
  TH1F* m_recoMaxD0;
  TH1F* m_recoNtrk;

  TH1F* m_recoX_LLP;
  TH1F* m_recoY_LLP;
  TH1F* m_recoZ_LLP;
  TH1F* m_recoR_LLP;
  TH1F* m_recodistFromPV_LLP;
  TH1F* m_recoPt_LLP;
  TH1F* m_recoEta_LLP;
  TH1F* m_recoPhi_LLP;
  TH1F* m_recoMass_LLP;
  TH1F* m_recoMu_LLP;
  TH1F* m_recoChi2_LLP;
  TH1F* m_recoDir_LLP;
  TH1F* m_recoCharge_LLP;
  TH1F* m_recoH_LLP;
  TH1F* m_recoHt_LLP;
  TH1F* m_recoMinOpAng_LLP;
  TH1F* m_recoMaxOpAng_LLP;
  TH1F* m_recoMaxDR_LLP;
  TH1F* m_recoMinD0_LLP;
  TH1F* m_recoMaxD0_LLP;
  TH1F* m_recoNtrk_LLP;

  TH1F* m_recoX_Fake;
  TH1F* m_recoY_Fake;
  TH1F* m_recoZ_Fake;
  TH1F* m_recoR_Fake;
  TH1F* m_recodistFromPV_Fake;
  TH1F* m_recoPt_Fake;
  TH1F* m_recoEta_Fake;
  TH1F* m_recoPhi_Fake;
  TH1F* m_recoMass_Fake;
  TH1F* m_recoMu_Fake;
  TH1F* m_recoChi2_Fake;
  TH1F* m_recoDir_Fake;
  TH1F* m_recoCharge_Fake;
  TH1F* m_recoH_Fake;
  TH1F* m_recoHt_Fake;
  TH1F* m_recoMinOpAng_Fake;
  TH1F* m_recoMaxOpAng_Fake;
  TH1F* m_recoMaxDR_Fake;
  TH1F* m_recoMinD0_Fake;
  TH1F* m_recoMaxD0_Fake;
  TH1F* m_recoNtrk_Fake;

  TH1F* m_recoX_Split;
  TH1F* m_recoY_Split;
  TH1F* m_recoZ_Split;
  TH1F* m_recoR_Split;
  TH1F* m_recodistFromPV_Split;
  TH1F* m_recoPt_Split;
  TH1F* m_recoEta_Split;
  TH1F* m_recoPhi_Split;
  TH1F* m_recoMass_Split;
  TH1F* m_recoMu_Split;
  TH1F* m_recoChi2_Split;
  TH1F* m_recoDir_Split;
  TH1F* m_recoCharge_Split;
  TH1F* m_recoH_Split;
  TH1F* m_recoHt_Split;
  TH1F* m_recoMinOpAng_Split;
  TH1F* m_recoMaxOpAng_Split;
  TH1F* m_recoMaxDR_Split;
  TH1F* m_recoMinD0_Split;
  TH1F* m_recoMaxD0_Split;
  TH1F* m_recoNtrk_Split;


  TH1F* m_recoTrk_qOverP;
  TH1F* m_recoTrk_theta; 
  TH1F* m_recoTrk_E; 
  TH1F* m_recoTrk_M; 
  TH1F* m_recoTrk_Pt;  
  TH1F* m_recoTrk_Px; 
  TH1F* m_recoTrk_Py; 
  TH1F* m_recoTrk_Pz;  
  TH1F* m_recoTrk_Eta; 
  TH1F* m_recoTrk_Phi;
  TH1F* m_recoTrk_D0; 
  TH1F* m_recoTrk_Z0;
  TH1F* m_recoTrk_errD0; 
  TH1F* m_recoTrk_errZ0; 
  TH1F* m_recoTrk_Chi2;
  TH1F* m_recoTrk_nDoF; 
  TH1F* m_recoTrk_charge;
  
  TH1F* m_matchScore_weight;
  TH1F* m_matchScore_pt;
  TH1F* m_matchedTruthID;  
  
  TH1F* m_truthX;  
  TH1F* m_truthY;
  TH1F* m_truthZ;  
  TH1F* m_truthR;
  TH1F* m_truthdistFromPV; 
  TH1F* m_truthEta;  
  TH1F* m_truthPhi; 
  TH1F* m_truthNtrk_out;
  TH1F* m_truthParent_E;
  TH1F* m_truthParent_M; 
  TH1F* m_truthParent_Pt;
  TH1F* m_truthParent_Eta; 
  TH1F* m_truthParent_Phi;
  TH1F* m_truthParent_charge; 
  TH1F* m_truthParentProdX; 
  TH1F* m_truthParentProdY; 
  TH1F* m_truthParentProdZ; 
  TH1F* m_truthParentProdR;
  TH1F* m_truthParentProddistFromPV;
  TH1F* m_truthParentProdEta; 
  TH1F* m_truthParentProdPhi;
  

};

#endif
