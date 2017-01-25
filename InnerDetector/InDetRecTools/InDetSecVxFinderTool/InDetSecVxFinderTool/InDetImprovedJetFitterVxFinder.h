/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/***************************************************************************
                          InDetImprovedJetFitterVxFinder.cxx  -  Description
                             -------------------

    begin : January 2008!
    authors: Giacinto Piacquadio (University of Freiburg),
             Christian Weiser (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch,
            christian.weiser@cern.ch
    changes: second version in January 2008!
 
  2008 (c) Atlas Detector Software

  This class implements the finder functionality for using 
  the JetFitter algorithm. It uses as input a ParticleJet and 
  then gives the VxJetCandidate out after having performed the 
  fit.

  SECOND VERSION: many improvements, detailed reconstruction of conversions,
  KS and Lambdas. KS are even used in the multi vertex fit in JetFitter in a 
  region of space of enough purity to allow it.
     
 ***************************************************************************/

#ifndef TrkJetVxFitter_InDetImprovedJetFitterVxFinder_H
#define TrkJetVxFitter_InDetImprovedJetFitterVxFinder_H

#include "InDetRecToolInterfaces/ISecVertexInJetFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrackLink/ITrackLink.h"

namespace Trk {
  class IVertexFitter;
  class TrackParticleBase;
  class VxSecVertexInfo;
  class VxJetCandidate;
  class RecVertex;
  class JetFitterInitializationHelper;
  class JetFitterHelper;
  class JetFitterRoutines;
  class ITrackSelectorTool;
  class IExtrapolator;
  class IVertexSeedFinder;
  class IMode3dFinder;
  class INeutralParticleParameterCalculator;
  class IVxCandidateXAODVertex;
}

namespace InDet 
{
  class InDetJetFitterUtils;
}

namespace xAOD
{
  class IParticle;  
}

namespace InDet {

  //  static const InterfaceID IID_InDetImprovedJetFitterVxFinder("InDetImprovedJetFitterVxFinder", 1, 0);

  class InDetImprovedJetFitterVxFinder : public AthAlgTool, public ISecVertexInJetFinder
  {

  public:

    //static const InterfaceID& interfaceID() {
    //return IID_InDetImprovedJetFitterVxFinder;
    //}
 

    StatusCode initialize();
    StatusCode finalize();

    InDetImprovedJetFitterVxFinder(const std::string& t, const std::string& n, const IInterface*  p);

    ~InDetImprovedJetFitterVxFinder();


    const Trk::VxSecVertexInfo* findSecVertex(const xAOD::Vertex & primaryVertex,
                                              const TLorentzVector & jetMomentum,
                                              const std::vector<const xAOD::IParticle*> & inputTracks) const;

    const Trk::VxSecVertexInfo* findSecVertex(const Trk::RecVertex & primaryVertex,
                                              const TLorentzVector & jetMomentum,
					      const std::vector<const Trk::TrackParticleBase*> & inputTracks) const;

  private:
    
    const Trk::VxSecVertexInfo* doTheFinding(const xAOD::Vertex & primaryVertex, //const Trk::RecVertex & primaryVertex ,
                                              const TLorentzVector & jetMomentum,
                                             const std::vector<const Trk::ITrackLink*> & myTracks) const;
    

    Trk::VxJetCandidate* findSecVertex(const Trk::RecVertex & primaryVertex,
                                       const TLorentzVector & jetMomentum,
                                       const std::vector<const Trk::ITrackLink*> & firstInputTracks,
                                       const std::vector<const Trk::ITrackLink*> & secondInputTracks,
                                       const Amg::Vector3D & vtxSeedDirection) const;
        

    void doTheFit(Trk::VxJetCandidate* myJetCandidate,
                  bool performClustering=true) const;


    ToolHandle<Trk::JetFitterInitializationHelper> m_initializationHelper;
    ToolHandle<Trk::JetFitterHelper> m_helper;
    ToolHandle<Trk::JetFitterRoutines> m_routines;
    ToolHandle<Trk::ITrackSelectorTool> m_trkFilter; 
    ToolHandle<Trk::IExtrapolator> m_extrapolator;
    ToolHandle<InDet::InDetJetFitterUtils> m_jetFitterUtils;
    ToolHandle<Trk::IVertexSeedFinder> m_CrossDistancesSeedFinder;
    ToolHandle<Trk::IMode3dFinder> m_mode3dfinder;
    ToolHandle<Trk::IVertexFitter> m_SequentialVertexFitter;
    ToolHandle<Trk::INeutralParticleParameterCalculator> m_convUtils;
    ToolHandle< Trk::IVxCandidateXAODVertex > m_VertexEdmFactory;
    

    int m_maxNumDeleteIterations;
    double m_vertexProbCut;
    int m_maxClusteringIterations;
    double m_vertexClusteringProbabilityCut;
    double m_vertexClusteringProbabilityCutWithMass;
    double m_vertexClusteringProbabilityCutWithMass0010;
    double m_vertexClusteringProbabilityCutWithMass1015;
    double m_vertexClusteringProbabilityCutWithMass1520;
    double m_vertexClusteringProbabilityCutWithMass2025;
    double m_vertexClusteringProbabilityCutWithMass2530;
    double m_vertexClusteringProbabilityCutWithMass3040;
    double m_vertexClusteringProbabilityCutWithMass4050;
    double m_vertexClusteringProbabilityCutWithMass5060;
    double m_vertexClusteringProbabilityCutWithMass6070;
    double m_vertexClusteringTwoVtxMassForProbCut;

    bool m_useFastClustering;

    double m_cutCompatibilityPrimaryVertexForPositiveLifetimeTracks;
    double m_cutCompatibilityPrimaryVertexForNegativeLifetimeTracks;
    double m_twoVertexProbabilityCut;
    double m_cutDeltaMKs;
    double m_cutDeltaMLambda;
    double m_cutDeltaMGamma;
    double m_cutCompatibilityPrimaryVertexSingleTrackForKsAndLambda;
    double m_cutCompatibilityPrimaryVertexBothTracksForKsAndLambda;
    double m_cutTwoTrkVtxLifetimeSignificanceForKs;
    double m_cutTwoTrkVtxLifetimeSignificanceForLambda;
    double m_cutIPD0BothTracksCriteriumOneForKsAndLambda;
    double m_cutIPZ0BothTracksCriteriumOneForKsAndLambda;
    double m_cutTransverseTwoTrkVtxDistanceCriteriumTwoForKsAndLambda;
    //double m_cutCompatibilityToPrimaryNeutralTrackFromConversion;
    //double m_cutCompatibilityToPrimaryNeutralTrackFromKs;
    //double m_cutCompatibilityToPrimaryNeutralTrackFromLambda;
    double m_cutCompatibilityChi2ToPrimaryNeutralTrackFromConversion;
    double m_cutCompatibilityChi2ToPrimaryNeutralTrackFromKs;
    double m_cutCompatibilityChi2ToPrimaryNeutralTrackFromLambda;
    double m_cutCompatibilityPrimaryVertexSingleTrackForBFirstSelection;
    double m_cutCompatibilityPrimaryVertexBothTracksForBFirstSelection;
    double m_cutIPD0BothTracksForBFirstSelection;
    double m_cutIPZ0BothTracksForBFirstSelection;
    double m_cutPtBothTracksForBFirstSelection;
    double m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionFirstCriterium;
    double m_cutTwoTrkVtxVertexProbForBFirstSelectionFirstCriterium;
    double m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionSecondCriterium;
    double m_cutTwoTrkVtxVertexProbForBFirstSelectionSecondCriterium;
//material interaction removal... (is it usefull at all???)
    double m_firstBeam_min;
    double m_firstBeam_max;
    double m_secondBeam_min;
    double m_secondBeam_max;
    double m_firstLayer_min;
    double m_firstLayer_max;
    double m_secondLayer_min;
    double m_secondLayer_max;
    double m_cutCompatibilityToPrimarySingleTrackForMatInteractions;
    double m_cutCompatibilityToPrimaryBothTracksForMatInteractions;
//cuts for additional tracks (SecondSelection)
    double m_cutIPD0SingleTrackForBSecondSelection;
    double m_cutIPZ0SingleTrackForBSecondSelection;
    double m_cutPtSingleTrackForBSecondSelection;
    double m_cutIPD0SigBoxSingleTrackForBSecondSelection; //box cut for PU rejection in 1-t vtx
    double m_cutIPZ0SigBoxSingleTrackForBSecondSelection; //box cut for PU rejection in 1-t vtx
    double m_cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection;
    double m_cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection;
//don't use the primary vertex finder information to estimate the compatibility! 
    double m_doNotUsePrimaryVertexCombatibilityInfo;
//invert the cuts for estimating the negative tags!
    bool m_revertFromPositiveToNegativeTags;
    
    /*
     * Avoid vertex to be initialize outside ID envelope given bu m_maxR and m_maxZ
     */

    double m_maxR;
    double m_maxZ;

    std::string  m_VxContainerName;

    
    /*
     * Number of tracks above which do not perform detailed clustering 
     * (to avoid explosion of combinatorics)
     */    

    int m_maxTracksForDetailedClustering;

    /*
     * sum pT dependent cuts on track pT
     * pT(track)< pTMinOffset + pTsum * m_pTMinSlope
     * OR pTsum
     */
    bool m_usepTDepTrackSel;
    double m_pTMinOffset;
    double m_pTMinSlope;
    double m_pTMax;


  };//end class declaration

}//end namespace Rec


#endif

