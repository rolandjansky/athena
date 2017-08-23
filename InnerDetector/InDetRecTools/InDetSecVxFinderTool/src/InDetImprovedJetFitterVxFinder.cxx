/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetImprovedJetFitterVxFinder.cxx  -  Description
                             -------------------

    begin : January 2008
    authors: Giacinto Piacquadio (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch
    changes: new version in January 2008!
    
 
  2008 (c) Atlas Detector Software

  Look at the header file for more information.
     
 ***************************************************************************/

#include "InDetSecVxFinderTool/InDetImprovedJetFitterVxFinder.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxJetVertex/PairOfVxVertexOnJetAxis.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/VxClusteringTable.h"
#include "TrkJetVxFitter/JetFitterHelper.h"
#include "TrkJetVxFitter/JetFitterInitializationHelper.h"
#include "TrkJetVxFitter/JetFitterRoutines.h"
#include "VxSecVertex/VxJetFitterVertexInfo.h"
#include <TMath.h>
#include "TrkEventPrimitives/FitQuality.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/RecVertex.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkVertexSeedFinderUtils/IMode3dFinder.h"
#include "TrkVertexFitters/SequentialVertexFitter.h"
#include "TrkEventPrimitives/VertexType.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "InDetSecVxFinderTool/InDetJetFitterUtils.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkVertexSeedFinderTools/CrossDistancesSeedFinder.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "VxJetVertex/TwoTrackVerticesInJet.h"
#include "VxJetVertex/SelectedTracksInJet.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkVertexFitterInterfaces/INeutralParticleParameterCalculator.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"

//17.5.2015 This is for the EDM migration
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"

#define InDetImprovedJetFitterVxFinder_DEBUGAddOns

namespace InDet
{

  struct TrackParticle_pair
  {
    double first;
    const Trk::TrackParticleBase* second;
    TrackParticle_pair(double p1, const Trk::TrackParticleBase* p2)
      : first (p1), second (p2) {}
    bool operator< (const TrackParticle_pair& other) const
    { return first > other.first; }
  };

  struct Track_pair
  {
    double first;
    const Trk::Track* second;
    Track_pair(double p1, const Trk::Track* p2)
      : first (p1), second (p2) {}
    bool operator< (const Track_pair& other) const
    { return first > other.first; }
  };  

  InDetImprovedJetFitterVxFinder::InDetImprovedJetFitterVxFinder(const std::string& t, const std::string& n, const IInterface*  p) :
    AthAlgTool(t,n,p),
    m_initializationHelper("Trk::JetFitterInitializationHelper"),
    m_helper("Trk::JetFitterHelper"),
    m_routines("Trk::JetFitterRoutines"),
    m_trkFilter("InDet::InDetDetailedTrackSelectorTool"),
    m_extrapolator("Trk::Extrapolator/InDetExtrapolator"),
    m_jetFitterUtils("InDet::InDetJetFitterUtils/InDetJetFitterUtils"),
    m_CrossDistancesSeedFinder("Trk::CrossDistancesSeedFinder/CrossDistancesSeedFinder"),
    m_mode3dfinder("Trk::Mode3dTo1dFinder/Mode3dTo1dFinder"),
    m_SequentialVertexFitter("Trk::SequentialVertexFitter/SequentialVertexFitter"),
    m_convUtils("Trk::NeutralParticleParameterCalculator"),
    m_VertexEdmFactory("Trk::VertexInternalEdmFactory"),
    m_maxNumDeleteIterations(30),
    m_vertexProbCut(0.001),
    m_maxClusteringIterations(30),
    m_vertexClusteringProbabilityCut(0.005),//GP 11/6/16: changed to default in job option file
    m_vertexClusteringProbabilityCutWithMass(0.05),//GP 11/7/16: keep default to not active
    m_vertexClusteringProbabilityCutWithMass0010(0.002),//MB Mass dependent cut
    m_vertexClusteringProbabilityCutWithMass1015(0.002),//MB Mass dependent cut
    m_vertexClusteringProbabilityCutWithMass1520(0.050),//MB Mass dependent cut
    m_vertexClusteringProbabilityCutWithMass2025(0.100),//MB Mass dependent cut
    m_vertexClusteringProbabilityCutWithMass2530(0.200),//MB Mass dependent cut
    m_vertexClusteringProbabilityCutWithMass3040(0.500),//MB Mass dependent cut
    m_vertexClusteringProbabilityCutWithMass4050(0.700),//MB Mass dependent cut
    m_vertexClusteringProbabilityCutWithMass5060(0.900),//MB Mass dependent cut
    m_vertexClusteringProbabilityCutWithMass6070(0.900),//MB Mass dependent cut
    m_vertexClusteringTwoVtxMassForProbCut(2000),
    m_useFastClustering(false),
    m_cutCompatibilityPrimaryVertexForPositiveLifetimeTracks(1e-1),
    m_cutCompatibilityPrimaryVertexForNegativeLifetimeTracks(5e-2),
    m_twoVertexProbabilityCut(3.4e-2),
    m_cutDeltaMKs(18.),
    m_cutDeltaMLambda(7.),
    m_cutDeltaMGamma(30.),
    m_cutCompatibilityPrimaryVertexSingleTrackForKsAndLambda(1e-3),
    m_cutCompatibilityPrimaryVertexBothTracksForKsAndLambda(1e-4),
    m_cutTwoTrkVtxLifetimeSignificanceForKs(4.5),
    m_cutTwoTrkVtxLifetimeSignificanceForLambda(4.5),
    m_cutIPD0BothTracksCriteriumOneForKsAndLambda(1.),
    m_cutIPZ0BothTracksCriteriumOneForKsAndLambda(2.),
    m_cutTransverseTwoTrkVtxDistanceCriteriumTwoForKsAndLambda(20.),
    m_cutCompatibilityChi2ToPrimaryNeutralTrackFromConversion(36),
    m_cutCompatibilityChi2ToPrimaryNeutralTrackFromKs(36),
    m_cutCompatibilityChi2ToPrimaryNeutralTrackFromLambda(36),
    m_cutCompatibilityPrimaryVertexSingleTrackForBFirstSelection(1e-1),
    m_cutCompatibilityPrimaryVertexBothTracksForBFirstSelection(1e-2),
    m_cutIPD0BothTracksForBFirstSelection(3.5),
    m_cutIPZ0BothTracksForBFirstSelection(5.),
    m_cutPtBothTracksForBFirstSelection(500.),
    m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionFirstCriterium(1.),
    m_cutTwoTrkVtxVertexProbForBFirstSelectionFirstCriterium(0.05),
    m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionSecondCriterium(1.5),
    m_cutTwoTrkVtxVertexProbForBFirstSelectionSecondCriterium(0.034),
    m_firstBeam_min(23), //29.35-1.5*0.68),
    m_firstBeam_max(25), //35+1.5*0.68),
    m_secondBeam_min(23), //34.18-1.5*0.65),
    m_secondBeam_max(25), //34.18+1.5*0.65),
    m_firstLayer_min(34.0-2.5), //57.5)
    m_firstLayer_max(34.0+2.5), //46.5),
    m_secondLayer_min(51.5-3), //70.5-1.5*2.1),
    m_secondLayer_max(51.5+3), //70.5+1.5*2.1),
    m_cutCompatibilityToPrimarySingleTrackForMatInteractions(1e-4),
    m_cutCompatibilityToPrimaryBothTracksForMatInteractions(1e-6),
    m_cutIPD0SingleTrackForBSecondSelection(1.5), 
    m_cutIPZ0SingleTrackForBSecondSelection(3.),  
    m_cutPtSingleTrackForBSecondSelection(750.),
    m_cutIPD0SigBoxSingleTrackForBSecondSelection(2.), //0 effectively disables the cut
    m_cutIPZ0SigBoxSingleTrackForBSecondSelection(5.),
    m_cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection(5e-2),
    m_cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection(1e-2),
    m_doNotUsePrimaryVertexCombatibilityInfo(false),
    m_revertFromPositiveToNegativeTags(false),
    m_maxR(1150.),//max R of ID
    m_maxZ(2727.),//max Z of ID
    m_VxContainerName("PrimaryVertices"),
    m_maxTracksForDetailedClustering(25),
    m_usepTDepTrackSel(false),
    m_pTMinOffset(800),//MeV
    m_pTMinSlope(0.01),//MeV(track cut)/MeV(sum of track pT)
    m_pTMax(600e3)//MeV--> stop at 600 GeV sum of track pT
  { 
    //JetFitter tools
    declareProperty("JetFitterHelper",m_helper);
    declareProperty("JetFitterInitializationHelper",m_initializationHelper);
    declareProperty("JetFitterRoutines",m_routines);

    //JetFinder tools
    declareProperty("InDetJetFitterUtils",m_jetFitterUtils);
    
    //Tracking tools
    declareProperty("Extrapolator",m_extrapolator);
    declareProperty("TrackSelector",m_trkFilter);

    //Vertexing tools
    declareProperty("CrossDistancesSeedFinder",m_CrossDistancesSeedFinder);
    declareProperty("Mode3dFinder",m_mode3dfinder);
    declareProperty("SequentialVertexFitter",m_SequentialVertexFitter);

    //Name of primary vertex container
    declareProperty("VxPrimaryContainer", m_VxContainerName);

    //Properties which steer the JetFitter clustering operations
    declareProperty("MaxNumDeleteIterations",m_maxNumDeleteIterations);
    declareProperty("VertexProbCut",m_vertexProbCut);
    declareProperty("MaxClusteringIterations",m_maxClusteringIterations);
    declareProperty("VertexClusteringProbabilityCut",m_vertexClusteringProbabilityCut);
    declareProperty("VertexClusteringProbabilityCutWithMass",m_vertexClusteringProbabilityCutWithMass);
    declareProperty("vertexClusteringTwoVtxMassForProbCut",m_vertexClusteringTwoVtxMassForProbCut);
    declareProperty("UseFastClustering",m_useFastClustering);
    
    declareProperty("VertexClusteringProbabilityCutWithMass0010",m_vertexClusteringProbabilityCutWithMass0010);
    declareProperty("VertexClusteringProbabilityCutWithMass1015",m_vertexClusteringProbabilityCutWithMass1015);
    declareProperty("VertexClusteringProbabilityCutWithMass1520",m_vertexClusteringProbabilityCutWithMass1520);
    declareProperty("VertexClusteringProbabilityCutWithMass2025",m_vertexClusteringProbabilityCutWithMass2025);
    declareProperty("VertexClusteringProbabilityCutWithMass2530",m_vertexClusteringProbabilityCutWithMass2530);
    declareProperty("VertexClusteringProbabilityCutWithMass3040",m_vertexClusteringProbabilityCutWithMass3040);
    declareProperty("VertexClusteringProbabilityCutWithMass4050",m_vertexClusteringProbabilityCutWithMass4050);
    declareProperty("VertexClusteringProbabilityCutWithMass5060",m_vertexClusteringProbabilityCutWithMass5060);
    declareProperty("VertexClusteringProbabilityCutWithMass6070",m_vertexClusteringProbabilityCutWithMass6070);

    //Cuts which steer the finding in the seeding phase before JetFitter (yes, so many!!!)
    declareProperty("cutCompPrimaryVertexForPosLifetimeTracks",m_cutCompatibilityPrimaryVertexForPositiveLifetimeTracks);
    declareProperty("cutCompPrimaryVertexForNegLifetimeTracks",m_cutCompatibilityPrimaryVertexForNegativeLifetimeTracks);
    declareProperty("twoVertexProbabilityCut",m_twoVertexProbabilityCut);
    declareProperty("cutDeltaMKs",m_cutDeltaMKs);
    declareProperty("cutDeltaMLambda",m_cutDeltaMLambda);
    declareProperty("cutDeltaMGamma",m_cutDeltaMGamma);
    declareProperty("cutCompPrimaryVertexSingleTrackForKsAndLambda",m_cutCompatibilityPrimaryVertexSingleTrackForKsAndLambda);
    declareProperty("cutCompPrimaryVertexBothTracksForKsAndLambda",m_cutCompatibilityPrimaryVertexBothTracksForKsAndLambda);
    declareProperty("cutTwoTrkVtxLifetimeSignificanceForKs",m_cutTwoTrkVtxLifetimeSignificanceForKs);
    declareProperty("cutTwoTrkVtxLifetimeSignificanceForLambda",m_cutTwoTrkVtxLifetimeSignificanceForLambda);
    declareProperty("cutIPD0BothTracksCriteriumAForKsAndLambda",m_cutIPD0BothTracksCriteriumOneForKsAndLambda);
    declareProperty("cutIPZ0BothTracksCriteriumAForKsAndLambda",m_cutIPZ0BothTracksCriteriumOneForKsAndLambda);
    declareProperty("cutTransvTwoTrkVtxDistCriteriumBForKsAndLambda",m_cutTransverseTwoTrkVtxDistanceCriteriumTwoForKsAndLambda);
    declareProperty("cutCompChi2ToPrimaryNeutTrkFromConversion",m_cutCompatibilityChi2ToPrimaryNeutralTrackFromConversion);
    declareProperty("cutCompChi2ToPrimaryNeutTrkFromKs",m_cutCompatibilityChi2ToPrimaryNeutralTrackFromKs);
    declareProperty("cutCompChi2ToPrimaryNeutTrkFromLambda",m_cutCompatibilityChi2ToPrimaryNeutralTrackFromLambda);
    declareProperty("cutCompPrimaryVertexSingleTrackForBFirstSelect",m_cutCompatibilityPrimaryVertexSingleTrackForBFirstSelection);
    declareProperty("cutCompPrimaryVertexBothTracksForBFirstSelect",m_cutCompatibilityPrimaryVertexBothTracksForBFirstSelection);
    declareProperty("cutIPD0BothTracksForBFirstSelection",m_cutIPD0BothTracksForBFirstSelection);
    declareProperty("cutIPZ0BothTracksForBFirstSelection",m_cutIPZ0BothTracksForBFirstSelection);
    declareProperty("cutPtBothTracksForBFirstSelection",m_cutPtBothTracksForBFirstSelection);
    declareProperty("cutIPD0SigBoxSingleTrackForBSecondSelection",m_cutIPD0SigBoxSingleTrackForBSecondSelection);
    declareProperty("cutIPZ0SigBoxSingleTrackForBSecondSelection",m_cutIPZ0SigBoxSingleTrackForBSecondSelection);
    declareProperty("cutTwoTrkVtxLifeSignForBFirstSelectCriteriumA",m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionFirstCriterium);
    declareProperty("cutTwoTrkVtxVtxProbForBFirstSelectCriteriumA",m_cutTwoTrkVtxVertexProbForBFirstSelectionFirstCriterium);
    declareProperty("cutTwoTrkVtxLifeSignForBFirstSelectCriteriumB",m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionSecondCriterium);
    declareProperty("cutTwoTrkVtxVtxProbForBFirstSelectCriteriumB",m_cutTwoTrkVtxVertexProbForBFirstSelectionSecondCriterium);
    declareProperty("firstBeam_min",m_firstBeam_min);
    declareProperty("secondBeam_min",m_secondBeam_min);
    declareProperty("secondBeam_max",m_secondBeam_max);
    declareProperty("firstLayer_min",m_firstLayer_min);
    declareProperty("firstLayer_max",m_firstLayer_max);
    declareProperty("secondLayer_min",m_secondLayer_min);
    declareProperty("secondLayer_max",m_secondLayer_max);
    declareProperty("cutCompToPrimarySingleTrackForMatInterac",m_cutCompatibilityToPrimarySingleTrackForMatInteractions);
    declareProperty("cutCompToPrimaryBothTracksForMatInterac",m_cutCompatibilityToPrimaryBothTracksForMatInteractions);
    declareProperty("cutIPD0SingleTrackForBSecondSelection",m_cutIPD0SingleTrackForBSecondSelection);
    declareProperty("cutIPZ0SingleTrackForBSecondSelection",m_cutIPZ0SingleTrackForBSecondSelection);
    declareProperty("cutPtSingleTrackForBSecondSelection",m_cutPtSingleTrackForBSecondSelection);
    declareProperty("cutCompPVSinglePosLifeTrackForBSecondSelect",m_cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection);
    declareProperty("cutCompPVSingleNegLifeTrackForBSecondSelect",m_cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection);
    declareProperty("doNotUsePVCombInfo",m_doNotUsePrimaryVertexCombatibilityInfo);
    declareProperty("ID_maxR",m_maxR);
    declareProperty("ID_maxZ",m_maxZ);

    declareProperty("revertFromPositiveToNegativeTags",m_revertFromPositiveToNegativeTags);

    declareProperty("maxTracksForDetailedClustering",m_maxTracksForDetailedClustering);

    declareProperty("ConversionUtils", m_convUtils,"Name of conversion utility");
 
    declareProperty("VertexEdmFactory", m_VertexEdmFactory);

    declareProperty("usepTDepTrackSel",m_usepTDepTrackSel);
    declareProperty("pTMinOffset",m_pTMinOffset);
    declareProperty("pTMinSlope",m_pTMinSlope);
    declareProperty("pTMax",m_pTMax);
    
   
    declareInterface< ISecVertexInJetFinder >(this) ;

  }
  

  InDetImprovedJetFitterVxFinder::~InDetImprovedJetFitterVxFinder() {}
  

  StatusCode InDetImprovedJetFitterVxFinder::initialize() {
    
    
    StatusCode sc = AlgTool::initialize();
    if(sc.isFailure())
      {
	msg(MSG::ERROR)<<" Unable to initialize the AlgTool"<<endmsg;
	return sc;
      }
    
    //retrieving the udator itself 	 
    sc =  m_helper.retrieve();
    if(sc.isFailure()) 	  { 	 
      	msg(MSG::ERROR) << " Unable to retrieve "<<m_helper<<endmsg; 	 
      return StatusCode::FAILURE; 	 
    }else msg(MSG::INFO) << "JetFitter Helper retrieved"<<endmsg; 
    
    
    sc = m_initializationHelper.retrieve();
    if(sc.isFailure()) 	  { 	 
      msg(MSG::ERROR) << " Unable to retrieve "<<m_initializationHelper<<endmsg; 	 
      return StatusCode::FAILURE; 	 
    }else msg(MSG::INFO) << "JetFitter Initialization Helper retrieved"<<endmsg; 
    
    sc = m_routines.retrieve();
    if(sc.isFailure()) 	  
    { 	 
      msg(MSG::ERROR) << " Unable to retrieve the JetFitter routines"<<m_routines<<endmsg; 	 
      return StatusCode::FAILURE; 	 
    }
    else msg(MSG::INFO) << "JetFitter Routines class retrieved"<<endmsg; 
    
    if(m_trkFilter.retrieve().isFailure()) 
    {
      msg(MSG::ERROR) << " Unable to retrieve "<<m_trkFilter<<endmsg;
      return StatusCode::FAILURE;
    } 
    else msg(MSG::INFO) << "Track filter retrieved"<<endmsg; 
    
    
    if (m_jetFitterUtils.retrieve().isFailure()) 
    {
      msg(MSG::ERROR) << "Could not find JetFitterUtils tool." << endmsg;
      return StatusCode::FAILURE;
    } 
    else msg(MSG::INFO) << " JetFitterUtils retrieved" << endmsg;

    if (m_CrossDistancesSeedFinder.retrieve().isFailure())
    {
      msg(MSG::ERROR) <<  "Could not find CrossDistancesSeedFinder tool." << endmsg;
      return StatusCode::FAILURE;
    } else msg(MSG::INFO) << " CrossDistancesSeedFinder tool retrieved" << endmsg;
      
    if (m_mode3dfinder.retrieve().isFailure())
    {
      msg(MSG::ERROR) <<  "Could not find mode3dfinder tool." << endmsg;
      return StatusCode::FAILURE;
    } else msg(MSG::INFO) << " Mode3DFinder tool retrieved" << endmsg;

    if (m_SequentialVertexFitter.retrieve().isFailure())
    {
      msg(MSG::ERROR) <<  "Could not get the sequential vertex fitter tool."<<endmsg;
      return StatusCode::FAILURE;
    } else msg(MSG::INFO) << " Sequential vertex fitter retrieved " << endmsg;

    if (m_extrapolator.retrieve().isFailure())
    {
    msg(MSG::ERROR) << "Could not find extrapolator tool." << endmsg;
    return StatusCode::FAILURE;
    } else msg(MSG::INFO) <<  " Extrapolator retrieved " << endmsg;
    
    sc = m_VertexEdmFactory.retrieve();
    if(sc.isFailure()) {
      msg(MSG::ERROR) << "Unable to retrieve" << m_VertexEdmFactory <<endmsg;
      return StatusCode::FAILURE;
    } else msg(MSG::INFO) << " xAOD vertex converter retrieved " <<endmsg;

    msg(MSG::INFO) << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }
  

   StatusCode InDetImprovedJetFitterVxFinder::finalize() {

     msg(MSG::INFO)  << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;

  } 

  const Trk::VxSecVertexInfo* InDetImprovedJetFitterVxFinder::findSecVertex(const xAOD::Vertex & primaryVertex,
                                                                            const TLorentzVector & jetMomentum,
                                                                            const std::vector<const xAOD::IParticle*> & inputTracks) const
  {
    
    std::vector<const Trk::ITrackLink*> selectedTracks;
           
    std::vector<const xAOD::IParticle*>::const_iterator   trk_iter;
    //std::vector<const xAOD::IParticle*>::const_iterator   trk_end=inputTracks.end();


    double sumpT=0;

    for (trk_iter= inputTracks.begin(); trk_iter != inputTracks.end(); ++trk_iter)
    {
      
      const xAOD::TrackParticle * tmp=dynamic_cast<const xAOD::TrackParticle *> ((*trk_iter));
     
      if (!m_trkFilter->decision(*tmp,&primaryVertex)) 
      {
        continue;
      }

      if (!m_usepTDepTrackSel)
      {

        ElementLink<xAOD::TrackParticleContainer> linkTP;
        linkTP.setElement(const_cast<xAOD::TrackParticle*>(tmp));
      
        Trk::LinkToXAODTrackParticle* link= new Trk::LinkToXAODTrackParticle(linkTP);
        selectedTracks.push_back(link);
        
      }
      else
      {
        sumpT+=tmp->pt();
      }
    }
    
    if (m_usepTDepTrackSel)
    {
      for (trk_iter= inputTracks.begin(); trk_iter != inputTracks.end(); ++trk_iter)
      {

        const xAOD::TrackParticle * tmp=dynamic_cast<const xAOD::TrackParticle *> ((*trk_iter));
       
        if (!m_trkFilter->decision(*tmp,&primaryVertex)) 
        {
          continue;
        }
        
        if (!((tmp->pt()>m_pTMinOffset + sumpT * m_pTMinSlope) 
              || 
              (sumpT > m_pTMax && tmp->pt()>m_pTMinOffset+m_pTMax* m_pTMinSlope)))
        {
          continue;
        }
        
        ElementLink<xAOD::TrackParticleContainer> linkTP;
        linkTP.setElement(const_cast<xAOD::TrackParticle*>(tmp));
      
        Trk::LinkToXAODTrackParticle* link= new Trk::LinkToXAODTrackParticle(linkTP);
        selectedTracks.push_back(link);

      }
    }

    Trk::RecVertex dummy;
    
    const Trk::VxSecVertexInfo* secVxInfo=doTheFinding(primaryVertex,
                                                       jetMomentum,
                                                       selectedTracks);

    return secVxInfo;

  }
  
  
  const Trk::VxSecVertexInfo* InDetImprovedJetFitterVxFinder::findSecVertex(const Trk::RecVertex & primaryVertex,
                                                                            const TLorentzVector & jetMomentum,
                                                                            const std::vector<const Trk::TrackParticleBase*> & myTracks) const {
    if (msgLvl(MSG::VERBOSE)) msg() << " Starting findSecVertex " << endmsg;

    std::vector<const Trk::ITrackLink*> selectedTracks;

    std::vector<const Trk::TrackParticleBase*>::const_iterator tracksBegin=myTracks.begin();
    std::vector<const Trk::TrackParticleBase*>::const_iterator tracksEnd=myTracks.end();
    for (std::vector<const Trk::TrackParticleBase*>::const_iterator tracksIter=tracksBegin;
	 tracksIter!=tracksEnd;++tracksIter) {
      if (m_trkFilter->decision(**tracksIter,&primaryVertex)==true) {
        
        if (msgLvl(MSG::VERBOSE)) msg() << " Adding new ElementLink " << endmsg;
        
        ElementLink<Trk::TrackParticleBaseCollection> link;
        link.setElement(const_cast<Trk::TrackParticleBase*>(*tracksIter));
        Trk::LinkToTrackParticleBase * linkTT = new Trk::LinkToTrackParticleBase(link);
//      linkTT->setStorableObject(*trackTES);
        selectedTracks.push_back(linkTT);
      }
    }

    xAOD::Vertex dummy;
    return doTheFinding(dummy,
                        jetMomentum,
                        selectedTracks);
  }
  

  
  const Trk::VxSecVertexInfo* InDetImprovedJetFitterVxFinder::doTheFinding(const xAOD::Vertex & primaryVertex, //const Trk::RecVertex & /* primaryVertex */,
                                                                            const TLorentzVector & jetMomentum,
                                                                           const std::vector<const Trk::ITrackLink*> & myTracks) const
  {
  
    if (msgLvl(MSG::VERBOSE)) msg() <<  " doTheFinding called " << endmsg;

    //some fundamental masses
    //I know they are hard coded here... 
    //sorry for this!!
    const double s_masspion=139.57018;
    const double s_masselectron=0.511;
    const double s_massproton=938.272;
    const double s_massks=497.648;
    const double s_masslambda=1115.683;

    StatusCode sc;

//    //get the primary vertex information
//    const xAOD::VertexContainer* myVxContainer(0);
//    StatusCode sc = evtStore()->retrieve(myVxContainer,m_VxContainerName);
//    if (sc.isFailure()) {
//      msg(MSG::ERROR) << m_VxContainerName << " not found in StoreGate." << endmsg;
//      return 0;
//    }  
//
//    xAOD::Vertex* xAODprimaryVertex(0);
//    
//    xAOD::VertexContainer::const_iterator fz_end=myVxContainer->end();
//    for (xAOD::VertexContainer::const_iterator fz = myVxContainer->begin(); fz != fz_end; ++fz) 
//    {
//      if ((*fz)->vertexType() == xAOD::VxType::PriVtx) 
//      {
//        xAODprimaryVertex = *fz;
//        break;
//      }
//    }
//    
//    if (!xAODprimaryVertex) 
//    {
//      if (myVxContainer->size() > 0) 
//      {
//        xAODprimaryVertex = *myVxContainer->begin();
//      }
//    }

//    xAOD::Vertex xAODprimaryVertex(primaryVertex);


    Trk::VxCandidate* signalVertex=0;
    sc = m_VertexEdmFactory->createVxCandidate(primaryVertex, signalVertex);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << m_VertexEdmFactory << " fail to createVxCandidate." << endmsg;
      return 0;
    }  


    if (signalVertex==0)
    {
      if (msgLvl(MSG::WARNING)) msg() << " No primary vertex in collection for jetfitter... No secondary vertices fit can be performed" << endmsg;
    }
    const Trk::RecVertex& primaryVertexRecVertex = signalVertex->recVertex();

    //create a Vector where you will store the compatibilites to the tracks in myTracks
    std::map<const Trk::ITrackLink*,double> compatibilityOfTrack;

    //create other variables for later: primary tracks and secondary tracks
    std::vector<const Trk::ITrackLink*> primaryTracks;
    std::vector<const Trk::ITrackLink*> secondaryTracks;

    //here all the V0candidates will be stored
    std::vector<const xAOD::Vertex*> V0candidates;
    std::map<const Trk::ITrackLink*,const Trk::ITrackLink*> fromLinkInV0CandidateToOriginalLink;

    //here the Neutral Tracks to store as TrackParticleBase
    //(identified conversions,Lambdas and Ks)
    //std::vector<const Trk::TrackParticleBase*> neutralTracks;
    std::vector<const Trk::LinkToTrackParticleBase*> neutralTracks;
    
    //one further information you absolutely need is:
    //the mass information... (horrible map again)
    std::map<const Trk::TrackParticleBase*,double> massOfNeutralTrack;
    
    //here the tracks for the first fit (higher purity) and for the second fit will be stored
    std::vector<const Trk::ITrackLink*> tracksToUseInFirstFit;
    std::vector<const Trk::ITrackLink*> tracksToUseInSecondFit;

    //positionAndWeight for the vertices which will be used as 
    //a first estimate of the B-flight direction to increase 
    //the fit reliability (JetFitter's linearization is strongly 
    //affected by the direction of the B-hardon flight axis)
    std::vector<Trk::PositionAndWeight> positionsOfSeedingVertices;

    //store the number of found material interactions
    int numberFirstBeamInteractions=0;
    int numberSecondBeamInteractions=0;
    int numberFirstLayerInteractions=0;
    int numberSecondLayerInteractions=0;
    

    //now you have the list of tracks...
    //that's already something!
    //now you need to get the compatibility of the tracks
    //(you want to use the primary vertex chi2 if this is NEEDED)

    if (msgLvl(MSG::VERBOSE)) msg() << " JetFitterFinding phase 1: getting the compatibilities of all tracks to the primary vertex... " << endmsg;

    std::vector<Trk::VxTrackAtVertex*>::const_iterator trkBegin=signalVertex->vxTrackAtVertex()->begin();
    std::vector<Trk::VxTrackAtVertex*>::const_iterator trkEnd=signalVertex->vxTrackAtVertex()->end();

    std::vector<const Trk::ITrackLink*>::const_iterator qualitytracksBegin=myTracks.begin();
    std::vector<const Trk::ITrackLink*>::const_iterator qualitytracksEnd=myTracks.end();

    int countDebug=0;
    
    for (std::vector<const Trk::ITrackLink*>::const_iterator qualitytracksIter=qualitytracksBegin;
         qualitytracksIter!=qualitytracksEnd;++qualitytracksIter) 
    {
      
      bool matched=false;

      const Trk::ITrackLink * trkTrackLink=*qualitytracksIter;

      const Trk::LinkToTrackParticleBase * trkCastedLinkTPBase=
          dynamic_cast<const Trk::LinkToTrackParticleBase *>(trkTrackLink);
      
//      const Trk::LinkToTrack * trkCastedLinkT=
//          dynamic_cast<const Trk::LinkToTrack *>(trkTrackLink);

      const Trk::LinkToXAODTrackParticle* trkCastedLinkTPxAOD=
          dynamic_cast<const Trk::LinkToXAODTrackParticle *>(trkTrackLink);      

      if (!trkCastedLinkTPxAOD && !trkCastedLinkTPBase)
      {
        if (msgLvl(MSG::WARNING)) msg() <<  " One track among the selected ones is neither an xAOD::TrackParticle nor a TrackParticle. Skipping track..." << endmsg;
        continue;
      }
      
      //obtain the wanted perigee now
      
      //needed for extrapolation (later to get the IP 3d sign)
      Trk::PerigeeSurface mySurface(primaryVertexRecVertex.position());
     
      //const Trk::Perigee* myMeasuredPerigee=0;

      if (msgLvl(MSG::VERBOSE)) msg() <<  "PV position " << primaryVertexRecVertex.position().x() << " " << primaryVertexRecVertex.position().y() << " " << primaryVertexRecVertex.position().z() << endmsg;

       const Trk::TrackParameters* myMeasuredPerigee=nullptr;
      
      if (trkCastedLinkTPBase)
      {

        const Trk::TrackParticleBase & myTrackParticleBase=***trkCastedLinkTPBase;

        const Trk::TrackParameters * definingParameters=&(myTrackParticleBase.definingParameters());
        //const Trk::Perigee* perigeeBeforeExtrapolation= definingParameters.perigee();
        
        
        //if (perigeeBeforeExtrapolation==0)
        //{
        //  if (msgLvl(MSG::WARNING)) msg() <<  "No Perigee at TrackParticleBase: this is a problem!" << endmsg;
        //  continue;
        //}

//        myMeasuredPerigee=dynamic_cast<const Trk::MeasuredPerigee*>(m_extrapolator->extrapolate(myTrackParticleBase,
//       mySurface));
        
	myMeasuredPerigee = m_extrapolator->extrapolateDirectly(*definingParameters,mySurface);


      }
      
      if (trkCastedLinkTPxAOD)
      {
        const xAOD::TrackParticle & myTrack=***trkCastedLinkTPxAOD;
        myMeasuredPerigee=m_extrapolator->extrapolate(myTrack,mySurface);
      }

      if (!myMeasuredPerigee)
      {
        msg(MSG::WARNING) << " Extrapolation to primary vertex failed. Skipping track " << endmsg;
        continue;
      }

      if (!m_doNotUsePrimaryVertexCombatibilityInfo)
      {
        
      for (std::vector<Trk::VxTrackAtVertex*>::const_iterator trkIter=trkBegin;trkIter!=trkEnd;++trkIter) 
      {

        const  Trk::ITrackLink * vtxTrackLink=(*trkIter)->trackOrParticleLink();

        
        const Trk::LinkToTrackParticleBase * vtxCastedLinkTPBase=
            dynamic_cast<const Trk::LinkToTrackParticleBase *>(vtxTrackLink);
        

      const Trk::LinkToXAODTrackParticle* vtxCastedLinkTPxAOD=
          dynamic_cast<const Trk::LinkToXAODTrackParticle *>(vtxTrackLink);      


        bool case11=vtxCastedLinkTPBase&&trkCastedLinkTPBase&&
            &(***vtxCastedLinkTPBase)==&(***trkCastedLinkTPBase);


        bool case22=trkCastedLinkTPxAOD&&vtxCastedLinkTPxAOD&&
            &(***vtxCastedLinkTPxAOD)==&(***trkCastedLinkTPxAOD);


        if (!case11 && !case22)
        {
#ifdef InDetImprovedJetFitterVxFinder_DEBUGAddOns
          if ((trkCastedLinkTPBase&&vtxCastedLinkTPxAOD)||(trkCastedLinkTPxAOD&&vtxCastedLinkTPBase))
          {
            if (countDebug%500==0)
            {
              if (msgLvl(MSG::DEBUG)) msg() <<  " Jet Fitter Vertex Finding is done on TrackParticleBase, primary vertex information is stored based on xAOD::TrackParticleBase or viceversa." << endmsg;
              if (msgLvl(MSG::DEBUG)) msg() <<  " Cannot use the fit of primary vertex for estimating the compatibility to the primary vertex. No correlation between primary vertex fit and track will be considered. Please activate PV REFITTING on AOD..." << endmsg;
            }
            countDebug+=1;
          }
#endif
          continue;
        }
        
        if (case11&&case22)
        {
          msg(MSG::WARNING) << " Throwing exception " << endmsg;
          throw std::string("Internal algorithmic error in InDetImprovedJetFitterVxFinder");
        }
        
        
        const Trk::FitQuality & fitQuality=(*trkIter)->trackQuality();
        double withoutSign=fitQuality.chiSquared();


        if (fitQuality.chiSquared()==0.)
        {
          withoutSign=m_jetFitterUtils->compatibility(*myMeasuredPerigee,primaryVertexRecVertex).first;
        }
        


        //now assign a sign to the found track (respect to the jet momentum direction)
	Amg::Vector3D jetMomSpatial(jetMomentum.X(),jetMomentum.Y(),jetMomentum.Z());
        withoutSign=fabs(withoutSign)*m_jetFitterUtils->get3DLifetimeSignOfTrack(*myMeasuredPerigee,
                                                                                 jetMomSpatial,
                                                                                 primaryVertexRecVertex);
        
        compatibilityOfTrack[*qualitytracksIter]=withoutSign;

        //temporary: to check that everything is fine, notice if the result without considering 
        //that the track was part in the fit is consistent/similar to the correct onw...
#ifdef InDetImprovedJetFitterVxFinder_DEBUGAddOns


        if (fitQuality.chiSquared()>0.)
        {
          double WouldBe=m_jetFitterUtils->compatibility(*myMeasuredPerigee,primaryVertexRecVertex).first;

          if (fabs(WouldBe/fitQuality.chiSquared()-1)>0.02)
          {
            if (msgLvl(MSG::VERBOSE)) msg() << " Probability mismatch: 1st vertex chi2 is: " << fitQuality.chiSquared() << 
                                          " while a posteriori chi2 is: " << WouldBe << endmsg;
            
	    if (msgLvl(MSG::VERBOSE)) msg() << "d0/s(d0)" << myMeasuredPerigee->parameters()[Trk::d0]/Amg::error(*myMeasuredPerigee->covariance(),Trk::d0) 
					    << "z0/s(z0)" << myMeasuredPerigee->parameters()[Trk::z0]/Amg::error(*myMeasuredPerigee->covariance(),Trk::z0) << endmsg;
            
          }
        }
	//	else
	//	  {
	//	    if (msgLvl(MSG::VERBOSE)) msg() << " Probability match: 1st vertex chi2 is: " << fitQuality.chiSquared() <<
	//					" while a posteriori chi2 is: " << WouldBe << endmsg;
	//	  }
        
#endif
        matched=true;
      }
      
      } 
      else 
      {
        if (msgLvl(MSG::VERBOSE)) msg() << " Not using any primary vertex information for track compatibilities." << endmsg;
      }
      
      if (!matched) 
      {
        double actualcomp=m_jetFitterUtils->compatibility(*myMeasuredPerigee,
                                                          primaryVertexRecVertex).first;
#ifdef InDetImprovedJetFitterVxFinder_DEBUGAddOns
        
        if (msgLvl(MSG::DEBUG)) msg() << " the compatibility which will be stored for a non primary vtx track is: " << 
            actualcomp << endmsg;
        if (msgLvl(MSG::VERBOSE)) 
        {
	  //Amg::Vector3D trackParameters = myMeasuredPerigee->parameters();
          
          msg(MSG::VERBOSE) << "d0/s(d0)" << myMeasuredPerigee->parameters()[Trk::d0]/Amg::error(*myMeasuredPerigee->covariance(),Trk::d0) 
                            << "z0/s(z0)" << myMeasuredPerigee->parameters()[Trk::z0]/Amg::error(*myMeasuredPerigee->covariance(),Trk::z0) << endmsg;
        }
#endif
        
        //get the sign (abs not needed but...)

	Amg::Vector3D jetMomSpatial(jetMomentum.X(),jetMomentum.Y(),jetMomentum.Z());
        actualcomp=fabs(actualcomp)*m_jetFitterUtils->get3DLifetimeSignOfTrack(*myMeasuredPerigee,
                                                                               jetMomSpatial,
                                                                               primaryVertexRecVertex);
        
        compatibilityOfTrack[*qualitytracksIter]=actualcomp;
        
      }

      delete myMeasuredPerigee;
      myMeasuredPerigee=0;

    }
    
    //so now you have the following inputs:
    //1) primary vertex: --> signalVertex (VxCandidate*) --> primaryVertexRecVertex (const RecVertex&)
    //2) jetMomentum (const CLHEP::HepLorentzVector &)
    //3) myTracks (const std::vector<const Trk::ITrackLink*>)
    //4) compatibilityOfTrack (std::map<const Trk::ITrackLink*,double>)

    if (msgLvl(MSG::VERBOSE)) msg() <<  " JetFitterFinding phase 2: distinguish between primary and secondary tracks " << endmsg;

    //now distinguish between primary and secondary tracks

    std::vector<const Trk::ITrackLink*>::const_iterator myTracksBegin=myTracks.begin();
    std::vector<const Trk::ITrackLink*>::const_iterator myTracksEnd=myTracks.end();

    for (std::vector<const Trk::ITrackLink*>::const_iterator myTracksIter=myTracksBegin;
         myTracksIter!=myTracksEnd;++myTracksIter)
    {

      double compatibilityOfActualTrack=compatibilityOfTrack[*myTracksIter];

      double cutCompatibilityPVforNegTracks=m_cutCompatibilityPrimaryVertexForNegativeLifetimeTracks;
      double cutCompatibilityPVforPosTracks=m_cutCompatibilityPrimaryVertexForPositiveLifetimeTracks;

      if (m_revertFromPositiveToNegativeTags)//revert cuts if needed
      {
        cutCompatibilityPVforNegTracks=m_cutCompatibilityPrimaryVertexForPositiveLifetimeTracks;
        cutCompatibilityPVforPosTracks=m_cutCompatibilityPrimaryVertexForNegativeLifetimeTracks;
      }

      if ( ( compatibilityOfActualTrack<0 && TMath::Prob(fabs(compatibilityOfActualTrack),2)< cutCompatibilityPVforNegTracks) ||
           ( compatibilityOfActualTrack>=0 && TMath::Prob(fabs(compatibilityOfActualTrack),2)< cutCompatibilityPVforPosTracks) )
      {
        secondaryTracks.push_back(*myTracksIter);
      }
      else 
      {
        primaryTracks.push_back(*myTracksIter);
      }
    }

    //now you also have primaryTracks and secondaryTracks

    if (msgLvl(MSG::VERBOSE)) msg() << " JetFitterFinding phase 3: finding all two track vertices combinations " << endmsg;
    
    //iterate only on the secondaryTracks and go through all the 2-tracks combinations
    
    //temporary vector used to accumulate the parameters to fit...
    //std::vector<const Trk::ParametersBase*> perigeeToFit(0);
    //const Trk::ParametersBase* firstPerigee(0);
    //const Trk::ParametersBase* secondPerigee(0);

    std::vector<const Trk::TrackParameters*> perigeeToFit(0);
    const Trk::TrackParameters* firstPerigee(0);
    const Trk::TrackParameters* secondPerigee(0);


    std::vector<const Trk::ITrackLink*>::const_iterator secondaryTracksBegin=secondaryTracks.begin();
    std::vector<const Trk::ITrackLink*>::const_iterator secondaryTracksEnd=secondaryTracks.end();
    
    for (std::vector<const Trk::ITrackLink*>::const_iterator secondaryTracksIter=secondaryTracksBegin;
         secondaryTracksIter!=secondaryTracksEnd;++secondaryTracksIter) 
    {
      
      //this is independent of the kind of track: Kirill and Andi did this --> well done!
      firstPerigee=(*secondaryTracksIter)->parameters();

      for (std::vector<const Trk::ITrackLink*>::const_iterator secondaryTracksIter2=secondaryTracksBegin;
           secondaryTracksIter2!=secondaryTracksIter;
	   ++secondaryTracksIter2) {
        
        perigeeToFit.clear();
        secondPerigee=(*secondaryTracksIter2)->parameters();
     	perigeeToFit.push_back(firstPerigee);
	perigeeToFit.push_back(secondPerigee);
        
	//MU Trk::Vertex seedVertex;
	Amg::Vector3D seedVertex;
	try {
	  seedVertex=m_CrossDistancesSeedFinder->findSeed(perigeeToFit);

	  //MU	  Amg::Vector3D seedVertexPos=seedVertex.position();
          
	  //MU          if (seedVertex.position().perp()>m_maxR || fabs(seedVertex.position().z())>m_maxZ)
          if (seedVertex.perp()>m_maxR || fabs(seedVertex.z())>m_maxZ)
          {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "Vertex seed outside ID. R=" << seedVertex.perp() << " Z=" << seedVertex.z() << endmsg;   
            seedVertex[0] = (primaryVertexRecVertex.position().x());
            seedVertex[1] = (primaryVertexRecVertex.position().y());
            seedVertex[2] = (primaryVertexRecVertex.position().z());
          }
	} catch (...) {
	  if (msgLvl(MSG::WARNING)) msg() <<  "Seed finding failed. Using primary vertex as seed... (clearly not optimal)" << endmsg;
	  seedVertex=Amg::Vector3D(primaryVertexRecVertex.position());
	}
        
        //N*(N-1) combinations are considered (this is why a cut on primary tracks is done)
	//MU	m_SequentialVertexFitter->fit(perigeeToFit, seedVertex);
	xAOD::Vertex* myCandidate(m_SequentialVertexFitter->fit(perigeeToFit,
								seedVertex));
	//MU  			  );
	//MU  Trk::VxCandidate* myCandidate(m_SequentialVertexFitter->fit(perigeeToFit,
        //MU                                                              seedVertex));
	if (myCandidate==0) {
	  if (msgLvl(MSG::DEBUG)) msg() << " Sequential fit failed. shouldn't happen... Skipping V0 candidate... " << endmsg;
          continue;
	}


        //now you are cloning the ITrackLink*
        //so you need to update the map of the compatibilities
        //(not great programming style, but this class is C not C++ :-)
        Trk::ITrackLink* newLinkTrack1=(*secondaryTracksIter)->clone();
        Trk::ITrackLink* newLinkTrack2=(*secondaryTracksIter2)->clone();

//        compatibilityOfTrack[newLinkTrack1]=compatibilityOfTrack[*secondaryTracksIter];
//        compatibilityOfTrack[newLinkTrack2]=compatibilityOfTrack[*secondaryTracksIter2];

        ((myCandidate->vxTrackAtVertex()))[0].setOrigTrack ( newLinkTrack1  );
        ((myCandidate->vxTrackAtVertex()))[1].setOrigTrack ( newLinkTrack2 );

        if (myCandidate->chiSquared()<0 ||
            myCandidate->numberDoF()<0)
        {
          msg(MSG::WARNING) << " Fit for V0 candidate failed: chi2 or ndf negative. Deleting candidate..." << endmsg;
          delete myCandidate;
          myCandidate=0;
          continue;
        }
	
	Amg::Vector3D jetMomSpatial(jetMomentum.X(),jetMomentum.Y(),jetMomentum.Z());
        Amg::Vector3D twoTrkVtxPos(myCandidate->x(),myCandidate->y(),myCandidate->z());
	double sign=(twoTrkVtxPos-primaryVertexRecVertex.position()).dot(jetMomSpatial);

        if (TMath::Prob(myCandidate->chiSquared(),
                        myCandidate->numberDoF())>m_twoVertexProbabilityCut 
	    && (sign<0 || !m_revertFromPositiveToNegativeTags)) 
        {
#ifdef InDetImprovedJetFitterVxFinder_DEBUGAddOns
          if (msgLvl(MSG::DEBUG)) msg() << " passed probability vertex: " << TMath::Prob(myCandidate->chiSquared(),
                                                                                         myCandidate->numberDoF()) << endmsg;
#endif
	  V0candidates.push_back(myCandidate);
          fromLinkInV0CandidateToOriginalLink[newLinkTrack1]=*secondaryTracksIter;
          fromLinkInV0CandidateToOriginalLink[newLinkTrack2]=*secondaryTracksIter2;
        }
        else
        {
#ifdef InDetImprovedJetFitterVxFinder_DEBUGAddOns
          if (msgLvl(MSG::DEBUG)) msg() << " didn't pass probability cut for 2-tr vtx: " << 
              std::setprecision(10) <<
              TMath::Prob(myCandidate->chiSquared(),
                          myCandidate->numberDoF()) << 
              endmsg;
#endif        
          delete myCandidate;
          myCandidate=0;
        }
      }
    }
    
    //now the two track vertices were found

    if (msgLvl(MSG::VERBOSE)) msg() <<  " JetFitterFinding phase 4: create neutral tracks out of recognized conversions, KS and Lambdas " << endmsg;

#ifdef InDetImprovedJetFitterVxFinder_DEBUGAddOns
    if (msgLvl(MSG::DEBUG)) msg() <<  "Found " << V0candidates.size() << " V0 candidates. " << endmsg;
#endif

    //now iterate over all the two track vertices and realize if neutral tracks can be created from these...
    
    std::vector<const xAOD::Vertex*>::const_iterator v0candBegin=V0candidates.begin();
    std::vector<const xAOD::Vertex*>::const_iterator v0candEnd=V0candidates.end();
    
    for (std::vector<const xAOD::Vertex*>::const_iterator v0candIter=v0candBegin;v0candIter!=v0candEnd;++v0candIter)
    {
      
      const xAOD::Vertex * fittedVertex = *v0candIter;

      double radius=fittedVertex->position().perp();

      if (msgLvl(MSG::VERBOSE)) msg() <<   " radius of 2-tr vertex is: " << radius;
      
//      double vertexprob=
//          (fittedVertex.fitQuality().chiSquared()>0&&fittedVertex.fitQuality().numberDoF()>0) ?
//          TMath::Prob(fittedVertex.fitQuality().chiSquared(),fittedVertex.fitQuality().numberDoF()) : -1;
      
      //now obtain the daughters of the two vertex
      std::vector<Trk::VxTrackAtVertex>::const_iterator vtxIter = fittedVertex->vxTrackAtVertex().begin();
      //obtain first track
      auto const & firstTrack = *vtxIter;
      //obtain second track
      ++vtxIter;
      auto const & secondTrack = *vtxIter;

      int charge=m_jetFitterUtils->getTwoTrackVtxCharge(**v0candIter);

      if (msgLvl(MSG::VERBOSE)) msg() << " charge " << charge;
      
      double massee=m_jetFitterUtils->getTwoTrackVtxMass(**v0candIter,
                                                         s_masselectron,
                                                         s_masselectron);

      if (msgLvl(MSG::VERBOSE)) msg() << " massee " << massee;

      double masspipi=m_jetFitterUtils->getTwoTrackVtxMass(**v0candIter,
                                                         s_masspion,
                                                         s_masspion);

      if (msgLvl(MSG::VERBOSE)) msg() << " masspipi " << masspipi;

      double massprpi=m_jetFitterUtils->getTwoTrackVtxMass(**v0candIter,
                                                           s_massproton,
                                                           s_masspion);

      if (msgLvl(MSG::VERBOSE)) msg() << " massprpi " << massprpi << endmsg;
      
      std::pair<double,double> distanceAndError=
          m_jetFitterUtils->getDistanceAndErrorBetweenTwoVertices(*fittedVertex,primaryVertexRecVertex);
      
      if (msgLvl(MSG::VERBOSE)) msg() <<" distance " << distanceAndError.first << " error " <<  distanceAndError.second << endmsg;

      //now assign a sign to the distance (decay in front or behind the primary vertex)
      double signeddistance=distanceAndError.first;
      
      Amg::Vector3D jetMomSpatial(jetMomentum.X(),jetMomentum.Y(),jetMomentum.Z());
      double sign=(fittedVertex->position() - primaryVertexRecVertex.position()).dot(jetMomSpatial);
      if (sign<0) 
      {
        signeddistance*=-1.;
      }

      if (msgLvl(MSG::VERBOSE)) msg() <<" signed distance " << signeddistance << endmsg;

      //now to understand if the 2-track vertex can represent a neutral track (conversion,Ks,Lambda)
      bool isKs=fabs(masspipi-s_massks)<m_cutDeltaMKs;
      bool isLambda=fabs(massprpi-s_masslambda)<m_cutDeltaMLambda;
      bool isPhoton=fabs(massee)<m_cutDeltaMGamma;
      
      const Trk::ITrackLink* originalTrackLink1=fromLinkInV0CandidateToOriginalLink[firstTrack.trackOrParticleLink()];
      const Trk::ITrackLink* originalTrackLink2=fromLinkInV0CandidateToOriginalLink[secondTrack.trackOrParticleLink()];
     
      double compatibilityTrack1=compatibilityOfTrack[originalTrackLink1];
      double compatibilityTrack2=compatibilityOfTrack[originalTrackLink2];

      if (msgLvl(MSG::VERBOSE)) msg() <<" compatibility of track 1 " << compatibilityTrack1 << " compatibility of track 2 " << compatibilityTrack2 << endmsg;
      
      bool flightSignificance= 
          TMath::Prob(fabs(compatibilityTrack1),2)<m_cutCompatibilityPrimaryVertexSingleTrackForKsAndLambda&&
          TMath::Prob(fabs(compatibilityTrack2),2)<m_cutCompatibilityPrimaryVertexSingleTrackForKsAndLambda&&
          TMath::Prob(fabs(compatibilityTrack1)+
                      fabs(compatibilityTrack2),4)<m_cutCompatibilityPrimaryVertexBothTracksForKsAndLambda;

      if (msgLvl(MSG::VERBOSE)) msg() <<" flightSignificance " << flightSignificance << endmsg;
      
      const Trk::TrackParameters* initialPerigee1=firstTrack.initialPerigee();
      const Trk::TrackParameters* initialPerigee2=secondTrack.initialPerigee();
      
      if (!initialPerigee1 || !initialPerigee2)
      {
        msg(MSG::WARNING) << "No refitted parameters available for fitted v0 candidate. Discarding candidate..." << endmsg;
        continue;
      }

      std::pair<double,double> track1_IPd0z0=m_jetFitterUtils->getD0andZ0IP(*initialPerigee1,
                                                                            primaryVertexRecVertex);

      std::pair<double,double> track2_IPd0z0=m_jetFitterUtils->getD0andZ0IP(*initialPerigee2,
                                                                            primaryVertexRecVertex);
      

      if (msgLvl(MSG::VERBOSE)) msg() <<" tr 1 d0ip " << track1_IPd0z0.first << " z0 ip " << track1_IPd0z0.second << endmsg;
      if (msgLvl(MSG::VERBOSE)) msg() <<" tr 2 d0ip " << track2_IPd0z0.first << " z0 ip " << track2_IPd0z0.second << endmsg;
      
      bool tightIPcut1=fabs(track1_IPd0z0.first)<m_cutIPD0BothTracksCriteriumOneForKsAndLambda&&fabs(track1_IPd0z0.second)<m_cutIPZ0BothTracksCriteriumOneForKsAndLambda;
      bool tightIPcut2=fabs(track2_IPd0z0.first)<m_cutIPD0BothTracksCriteriumOneForKsAndLambda&&fabs(track2_IPd0z0.second)<m_cutIPZ0BothTracksCriteriumOneForKsAndLambda;

      bool atLeastOneLoose=( !tightIPcut1 ) || ( !tightIPcut2 );

      bool requirementForKs=
          isKs&&charge==0&&
          signeddistance/distanceAndError.second>m_cutTwoTrkVtxLifetimeSignificanceForKs&&
          flightSignificance&&
          ((radius>m_cutTransverseTwoTrkVtxDistanceCriteriumTwoForKsAndLambda&&(!atLeastOneLoose))
           ||(atLeastOneLoose));

      if (m_revertFromPositiveToNegativeTags)
      {
        requirementForKs=
            isKs&&charge==0&&
            signeddistance/distanceAndError.second<-m_cutTwoTrkVtxLifetimeSignificanceForKs&&
            flightSignificance&&
            ((radius>m_cutTransverseTwoTrkVtxDistanceCriteriumTwoForKsAndLambda&&(!atLeastOneLoose))
           ||(atLeastOneLoose));
      }
      
      
      bool requirementForLambda=
          isLambda&&charge==0&&
          signeddistance/distanceAndError.second>m_cutTwoTrkVtxLifetimeSignificanceForLambda&&
          flightSignificance&&
          ((radius>m_cutTransverseTwoTrkVtxDistanceCriteriumTwoForKsAndLambda&&(!atLeastOneLoose))
           ||(atLeastOneLoose));

      if (m_revertFromPositiveToNegativeTags)
      {
        requirementForLambda=
            isLambda&&charge==0&&
            signeddistance/distanceAndError.second<-m_cutTwoTrkVtxLifetimeSignificanceForLambda&&
            flightSignificance&&
            ((radius>m_cutTransverseTwoTrkVtxDistanceCriteriumTwoForKsAndLambda&&(!atLeastOneLoose))
             ||(atLeastOneLoose));
      }
      

      bool requirementForConversion=
          isPhoton&&
          signeddistance>0&&
          charge==0;
      
      if (m_revertFromPositiveToNegativeTags)
      {
        requirementForConversion=
            isPhoton&&
            signeddistance<0&&
            charge==0;
      }
      

      if (msgLvl(MSG::VERBOSE)) msg() <<" is Ks " << (requirementForKs?"yes":"no")<< " is conversion "  << (requirementForConversion?"yes":"no")<<
          " is Lambda " << (requirementForLambda?"yes":"no") << endmsg;

    if (requirementForKs||requirementForLambda||requirementForConversion)
      {
	
        //const Trk::NeutralPerigee* neutralParticle=  m_convUtils->createNeutralTrackFromVertex(**v0candIter);
				//const Trk::ParametersBase* myParametersBase = neutralParticle; NeutralPerigee is already just a typedef to Trk::ParametersBase<5,Trk::Neutral>, doesn't make sense to convert it again...
        //const Trk::FitQuality* myFitQuality=new Trk::FitQuality( (*v0candIter)->recVertex().fitQuality() );
        //const Trk::TrackSummary* myTrackSummary=0;//new Trk::TrackSummary();
        //const Trk::VxCandidate* myVxCand=*v0candIter;
        //const Trk::Track* myTrack=0;

        std::vector<const Trk::TrackParameters*> myDummyVector;
        if (msgLvl(MSG::DEBUG)) msg() << "creating new neutral track particle base" << endmsg;
        
	///THIS WILL NOT WORK IN NEW EDM! NEEDS A FIX!!!
	/*
        const Trk::TrackParticleBase* myTrackParticleBase=new Trk::TrackParticleBase(myTrack,
                                                                                     Trk::NoVtx,
                                                                                     myVxCand,
                                                                                     myTrackSummary,//summary
                                                                                     myDummyVector,
                                                                                     myParametersBase,
                                                                                     myFitQuality);

        
        ElementLink<Trk::TrackParticleBaseCollection> link;
        link.setElement(const_cast<Trk::TrackParticleBase*>(myTrackParticleBase));
        Trk::LinkToTrackParticleBase * myNeutralTrackLink = new Trk::LinkToTrackParticleBase(link);
        neutralTracks.push_back(myNeutralTrackLink);

        if (requirementForKs)
        {
          massOfNeutralTrack[myTrackParticleBase]=masspipi;
        }
        else if (requirementForConversion)
        {
          massOfNeutralTrack[myTrackParticleBase]=massee;
        }
        else if (requirementForLambda)
        {
          massOfNeutralTrack[myTrackParticleBase]=massprpi;
        }
        else
        {
          msg(MSG::WARNING) << " Internal error. Neutral candidate is discarded. " << endmsg;
          continue;
        }
	*/
      }
    }
    

    if (msgLvl(MSG::VERBOSE)) msg() <<" JetFitterFinding phase 5: try to further analyze conversions,Ks and Lambda and do some sort of overlap removal" << endmsg;
    
    //now you have the following things
    //as before
    //1) primary vertex: --> signalVertex (VxCandidate*) --> primaryVertexRecVertex (const RecVertex&)
    //2) jetMomentum (const CLHEP::HepLorentzVector &)
    //3) myTracks (const std::vector<const Trk::ITrackLink*>)
    //4) compatibilityOfTrack (std::map<const Trk::ITrackLink*,double>)
    //
    //5)primaryTracks,secondaryTracks
    //6)V0candidates
    //7)neutralTracks + masses (massOfNeutralTrack)

    std::vector<const Trk::ITrackLink*> tracksToVeto;
    std::vector<const xAOD::Vertex*> candidatesToVeto;
    std::vector<const Trk::LinkToTrackParticleBase*> selectedNeutralTracks;
    std::vector<const Trk::LinkToTrackParticleBase*> selectedNeutralTracksForIP;

    
    
    //v0candBegin
    //v0candIter

    //first iteration to understand if there are conversions
    for (std::vector<const xAOD::Vertex*>::const_iterator v0candIter=v0candBegin;v0candIter!=v0candEnd;++v0candIter)
     {
       const xAOD::Vertex* v0candPtr=*v0candIter;      
     
       if (msgLvl(MSG::DEBUG)) msg() << " Analyzing new neutral candidate for conversion " << endmsg;
       
       if (v0candPtr==0) 
       {
         msg(MSG::WARNING) << " Pointer for V0 is null pointer. Skipping V0 candidate..." << endmsg;
         continue;
       }

       //is there a neutral particle associated to this? Iterate over neutrals to understand this
       //reurning 0 for the moment anyway, this is broken in new EDM. Needs Fixing!
       const Trk::LinkToTrackParticleBase* correspondingNeutralLink=
           m_jetFitterUtils->findNeutralTrackParticleBase(neutralTracks,
                                                          **v0candIter);

       if (!correspondingNeutralLink)
       {
         continue;
       }

       const Trk::TrackParticleBase * correspondingNeutral=**correspondingNeutralLink;
       

       //Can't get a neutral from a link to TrackParticleBase anymore...dynamic_cast to get this to compile, but this will not do anthing sensible. FIX ME!
       const Trk::NeutralPerigee& myMeasuredNeutralPerigee=dynamic_cast<const Trk::NeutralPerigee&>(correspondingNeutral->definingParameters());
         
       if (fabs(massOfNeutralTrack[correspondingNeutral])>m_cutDeltaMGamma)
       {
         continue;
       }
       
       double compatibilityToPrimaryOfNeutralTrack=
           m_jetFitterUtils->compatibility(myMeasuredNeutralPerigee,primaryVertexRecVertex).first;
           
#ifdef InDetImprovedJetFitterVxFinder_DEBUGAddOns
       if (msgLvl(MSG::DEBUG)) msg() << " The compatibility of the conversion is: " << compatibilityToPrimaryOfNeutralTrack << endmsg;
#endif

       selectedNeutralTracksForIP.push_back(correspondingNeutralLink);

       if (fabs(compatibilityToPrimaryOfNeutralTrack)<m_cutCompatibilityChi2ToPrimaryNeutralTrackFromConversion)
       {
         tracksToVeto.push_back(fromLinkInV0CandidateToOriginalLink[((v0candPtr->vxTrackAtVertex()))[0].trackOrParticleLink()]);
         tracksToVeto.push_back(fromLinkInV0CandidateToOriginalLink[((v0candPtr->vxTrackAtVertex()))[1].trackOrParticleLink()]);
       }
     }
     
     //now do the same but trying to find out KSs
    for (std::vector<const xAOD::Vertex*>::const_iterator v0candIter=v0candBegin;v0candIter!=v0candEnd;++v0candIter)
     {
       
       const xAOD::Vertex * v0candPtr=*v0candIter;      
     
       if (msgLvl(MSG::DEBUG)) msg() << " Analyzing new neutral candidate for KS " << endmsg;
       
       if (v0candPtr==0) 
       {
         msg(MSG::WARNING) << " Pointer for V0 is null pointer. Skipping KS candidate..." << endmsg;
         continue;
       }

       //is there a neutral particle associated to this? Iterate over neutrals to understand this
       //reurning 0 for the moment anyway, this is broken in new EDM. Needs Fixing!
       const Trk::LinkToTrackParticleBase* correspondingNeutralLink=
           m_jetFitterUtils->findNeutralTrackParticleBase(neutralTracks,
                                                          **v0candIter);
       if (!correspondingNeutralLink)
       {
         continue;
       }

       const Trk::TrackParticleBase * correspondingNeutral=**correspondingNeutralLink;

       //See above... FIX ME!
       const Trk::NeutralPerigee& myMeasuredNeutralPerigee=
           dynamic_cast<const Trk::NeutralPerigee&>(correspondingNeutral->definingParameters());
         
       if (fabs(massOfNeutralTrack[correspondingNeutral]-s_massks)>m_cutDeltaMKs)
       {
         continue;
       }
       
       double compatibilityToPrimaryOfNeutralTrack=
           m_jetFitterUtils->compatibility(myMeasuredNeutralPerigee,primaryVertexRecVertex).first;
           
#ifdef InDetImprovedJetFitterVxFinder_DEBUGAddOns
       if (msgLvl(MSG::DEBUG)) msg() << " The compatibility of the KS is: " << compatibilityToPrimaryOfNeutralTrack << endmsg;
#endif

       auto const &  firstTrack = v0candPtr->vxTrackAtVertex()[0];
       auto const & secondTrack = v0candPtr->vxTrackAtVertex()[1];       

       if (firstTrack==0 || secondTrack ==0)
       {
         msg(MSG::WARNING) << " Zero pointer (VxTrackAtVertex): skipping Ks candidate " << endmsg;
         continue;
       }
       

       const Trk::ITrackLink* trackLink1=firstTrack.trackOrParticleLink();
       const Trk::ITrackLink* trackLink2=secondTrack.trackOrParticleLink();

       if (trackLink1==0 || trackLink2==0)
       {
         msg(MSG::WARNING) << " Zero pointer (ITrackLink): skipping Ks candidate " << endmsg;
         continue;
       }

       //check if the candidate was vetoed
       if (m_jetFitterUtils->checkIfVxCandidateIsInVector(v0candPtr,candidatesToVeto))
           continue;

       const Trk::ITrackLink* originalTrackLink1=fromLinkInV0CandidateToOriginalLink[trackLink1];
       const Trk::ITrackLink* originalTrackLink2=fromLinkInV0CandidateToOriginalLink[trackLink2];


       //check if the tracks were already vetoed (by conversions)
       if (m_jetFitterUtils->checkIfTrackIsInVector(originalTrackLink1,tracksToVeto))
           continue;
       if (m_jetFitterUtils->checkIfTrackIsInVector(originalTrackLink2,tracksToVeto))
           continue;

       if (fabs(compatibilityToPrimaryOfNeutralTrack)<m_cutCompatibilityChi2ToPrimaryNeutralTrackFromKs)
       {

         if (msgLvl(MSG::VERBOSE)) msg() <<" Add neutral to the candidates to veto " << endmsg;

         //add neutral to the candidates to veto
         candidatesToVeto.push_back(v0candPtr);
         selectedNeutralTracksForIP.push_back(correspondingNeutralLink);
       }
       else
       {


         const Trk::TrackParameters * trackPerigee1=firstTrack.initialPerigee();
         const Trk::TrackParameters * trackPerigee2=secondTrack.initialPerigee();
         
         if (trackPerigee1==0||trackPerigee2==0)
         {
           msg(MSG::WARNING) << " Null Pointer in Perigee returned. Skipping KS candidate... " << endmsg;
           continue;
         }
         

         std::pair<double,double> track1IPd0z0=
             m_jetFitterUtils->getD0andZ0IP(*trackPerigee1,primaryVertexRecVertex);

         std::pair<double,double> track2IPd0z0=
             m_jetFitterUtils->getD0andZ0IP(*trackPerigee2,primaryVertexRecVertex);

         bool tightIPcut1=fabs(track1IPd0z0.first)<m_cutIPD0BothTracksCriteriumOneForKsAndLambda&&fabs(track1IPd0z0.second)<m_cutIPZ0BothTracksCriteriumOneForKsAndLambda;
         bool tightIPcut2=fabs(track2IPd0z0.first)<m_cutIPD0BothTracksCriteriumOneForKsAndLambda&&fabs(track2IPd0z0.second)<m_cutIPZ0BothTracksCriteriumOneForKsAndLambda;

         if (msgLvl(MSG::VERBOSE)) msg() <<  " tightIPcut1 " << (tightIPcut1?"yes":"no") << " tightIPcut2: " << (tightIPcut2?"yes":"no") << endmsg;
         
         bool atLeastOneLoose=( !tightIPcut1 ) || ( !tightIPcut2 );

         if (msgLvl(MSG::VERBOSE)) msg() <<" at least one loose " << (atLeastOneLoose?"yes":"no") << endmsg;
         
         if (atLeastOneLoose)
         {

           //////////////////////

           bool hasTrackInPreviousNeutral=false;
           
//           const Trk::ITrackLink* originalTrackLink1=fromLinkInV0CandidateToOriginalLink[trackLink1];
//           const Trk::ITrackLink* originalTrackLink2=fromLinkInV0CandidateToOriginalLink[trackLink2];
//         already calculated!

           //go from original ITrackLink to all other possible ITrackLink
           std::vector<const Trk::ITrackLink*> vectorOfV0ITrackLink;
       
           std::map<const Trk::ITrackLink*,const Trk::ITrackLink*>::const_iterator fromLinkInV0CandidateToOriginalLinkBegin=fromLinkInV0CandidateToOriginalLink.begin();
           std::map<const Trk::ITrackLink*,const Trk::ITrackLink*>::const_iterator fromLinkInV0CandidateToOriginalLinkEnd=fromLinkInV0CandidateToOriginalLink.end();
       
           for (std::map<const Trk::ITrackLink*,const Trk::ITrackLink*>::const_iterator fromLinkInV0CandidateToOriginalLinkIter=fromLinkInV0CandidateToOriginalLinkBegin;
                fromLinkInV0CandidateToOriginalLinkIter!=fromLinkInV0CandidateToOriginalLinkEnd;
                ++fromLinkInV0CandidateToOriginalLinkIter)
           {
             if ((*fromLinkInV0CandidateToOriginalLinkIter).second==originalTrackLink1||
                 (*fromLinkInV0CandidateToOriginalLinkIter).second==originalTrackLink2)
             {
               if (msgLvl(MSG::VERBOSE)) msg() <<" Found one of the original ITrackLink " << endmsg;
               vectorOfV0ITrackLink.push_back((*fromLinkInV0CandidateToOriginalLinkIter).first);
             }
           }

           std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLinkBegin=vectorOfV0ITrackLink.begin();
           std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLinkEnd=vectorOfV0ITrackLink.end();
           
           for (std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLinkIter=vectorOfV0ITrackLinkBegin;
                vectorOfV0ITrackLinkIter!=vectorOfV0ITrackLinkEnd;
                ++vectorOfV0ITrackLinkIter)
           {
             
             if (m_jetFitterUtils->checkIfTrackIsInNeutralTrackVector(*vectorOfV0ITrackLinkIter,neutralTracks))
             {
               hasTrackInPreviousNeutral=true;
               if (msgLvl(MSG::DEBUG)) msg() << " Found track in previous neutral. Skipping new neutral candidate... " << endmsg;
               break;
             }
           }
           
           if (hasTrackInPreviousNeutral)
               continue;

           //check if the tracks are already in one neutral track (previous found KS)
           //THIS CANNOT WORK... (tracks in different V0 candidates have different links)
           //if (m_jetFitterUtils->checkIfTrackIsInNeutralTrackVector(trackLink1,neutralTracks))
           //continue;
           //THIS CANNOT WORK... 
           //if (m_jetFitterUtils->checkIfTrackIsInNeutralTrackVector(trackLink2,neutralTracks))
           //    continue;

           //PLEASE NOTE: when adding to tracksToUseInFirstFit, you need 
           //ALWAYS
           
           if (msgLvl(MSG::VERBOSE)) msg() <<" Add to the tracks to use in the first fit...! " << endmsg;

           tracksToUseInFirstFit.push_back(correspondingNeutralLink);
           selectedNeutralTracks.push_back(correspondingNeutralLink);
           selectedNeutralTracksForIP.push_back(correspondingNeutralLink);
         }
         // if not at Least One Loose, then it's probably a two track vertex which 
         // comes directly from the B continous...
       }
     }
  
     //now do the same but trying to find out Lambdas
    for (std::vector<const xAOD::Vertex*>::const_iterator v0candIter=v0candBegin;v0candIter!=v0candEnd;++v0candIter)
     {
       
       const xAOD::Vertex* v0candPtr=*v0candIter;      
     
       if (msgLvl(MSG::DEBUG)) msg() << " Analyzing new neutral candidate for Lambdas " << endmsg;
       
       if (v0candPtr==0) 
       {
         msg(MSG::WARNING) << " Pointer for V0 is null pointer. Skipping Lambdas candidate..." << endmsg;
         continue;
       }

       //is there a neutral particle associated to this? Iterate over neutrals to understand this
       //reurning 0 for the moment anyway, this is broken in new EDM. Needs Fixing!
       const Trk::LinkToTrackParticleBase* correspondingNeutralLink=
           m_jetFitterUtils->findNeutralTrackParticleBase(neutralTracks,
                                                          **v0candIter);

       if (!correspondingNeutralLink)
       {
         continue;
       }

       const Trk::TrackParticleBase * correspondingNeutral=**correspondingNeutralLink;
       
       //See comments above... FIX ME!
       const Trk::NeutralPerigee& myMeasuredNeutralPerigee=
           dynamic_cast<const Trk::NeutralPerigee&>(correspondingNeutral->definingParameters());
         
       if (fabs(massOfNeutralTrack[correspondingNeutral]-s_masslambda)>m_cutDeltaMLambda)
       {
         continue;
       }
       
       double compatibilityToPrimaryOfNeutralTrack=
           m_jetFitterUtils->compatibility(myMeasuredNeutralPerigee,primaryVertexRecVertex).first;
           
#ifdef InDetImprovedJetFitterVxFinder_DEBUGAddOns
       if (msgLvl(MSG::DEBUG)) msg() << " The compatibility of the Lambda is: " << compatibilityToPrimaryOfNeutralTrack << endmsg;
#endif
       
       auto const &  firstTrack = v0candPtr->vxTrackAtVertex()[0];
       auto const & secondTrack = v0candPtr->vxTrackAtVertex()[1];

       if (firstTrack==0 || secondTrack ==0)
       {
         msg(MSG::WARNING) << " Zero pointer (VxTrackAtVertex): skipping Lambda candidate " << endmsg;
         continue;
       }
       

       const Trk::ITrackLink* trackLink1=firstTrack.trackOrParticleLink();
       const Trk::ITrackLink* trackLink2=secondTrack.trackOrParticleLink();

       if (trackLink1==0 || trackLink2==0)
       {
         msg(MSG::WARNING) << " Zero pointer (ITrackLink): skipping Lambda candidate " << endmsg;
         continue;
       }

       //check if the candidate was vetoed
       if (m_jetFitterUtils->checkIfVxCandidateIsInVector(v0candPtr,candidatesToVeto))
       {
         if (msgLvl(MSG::VERBOSE)) msg() <<" candidate has to be vetoed " << endmsg;
         continue;
       }
       

       //check if the tracks were already vetoed (by conversions or KS)
       if (m_jetFitterUtils->checkIfTrackIsInVector(fromLinkInV0CandidateToOriginalLink[trackLink1],tracksToVeto))
           continue;
       if (m_jetFitterUtils->checkIfTrackIsInVector(fromLinkInV0CandidateToOriginalLink[trackLink2],tracksToVeto))
           continue;
       
       const Trk::TrackParameters * trackPerigee1=firstTrack.initialPerigee();
       const Trk::TrackParameters * trackPerigee2=secondTrack.initialPerigee();
       
       if (trackPerigee1==0||trackPerigee2==0)
       {
         msg(MSG::WARNING) << " Null Pointer in Perigee returned. Skipping Lambda candidate... " << endmsg;
         continue;
       }

       std::pair<double,double> track1IPd0z0=
           m_jetFitterUtils->getD0andZ0IP(*trackPerigee1,primaryVertexRecVertex);
       
       std::pair<double,double> track2IPd0z0=
           m_jetFitterUtils->getD0andZ0IP(*trackPerigee2,primaryVertexRecVertex);
       
      bool tightIPcut1=fabs(track1IPd0z0.first)<m_cutIPD0BothTracksCriteriumOneForKsAndLambda&&fabs(track1IPd0z0.second)<m_cutIPZ0BothTracksCriteriumOneForKsAndLambda;
      bool tightIPcut2=fabs(track2IPd0z0.first)<m_cutIPD0BothTracksCriteriumOneForKsAndLambda&&fabs(track2IPd0z0.second)<m_cutIPZ0BothTracksCriteriumOneForKsAndLambda;
       
       bool atLeastOneLoose=( !tightIPcut1 ) || ( !tightIPcut2 );


       if (fabs(compatibilityToPrimaryOfNeutralTrack)<m_cutCompatibilityChi2ToPrimaryNeutralTrackFromLambda)
       {

         if (msgLvl(MSG::VERBOSE)) msg() <<" Adding Lambda to candidates to veto or veto the tracks directly " << endmsg;

         //add neutral to the candidates to veto
         candidatesToVeto.push_back(v0candPtr);

         selectedNeutralTracksForIP.push_back(correspondingNeutralLink);

         if (atLeastOneLoose)
         {
           tracksToVeto.push_back(fromLinkInV0CandidateToOriginalLink[((v0candPtr->vxTrackAtVertex()))[0].trackOrParticleLink()]);
           tracksToVeto.push_back(fromLinkInV0CandidateToOriginalLink[((v0candPtr->vxTrackAtVertex()))[1].trackOrParticleLink()]);
         }
         else
         {
           candidatesToVeto.push_back(v0candPtr);
         }
       }
       else
       {
         if (atLeastOneLoose)
         {
           selectedNeutralTracksForIP.push_back(correspondingNeutralLink);
         }
       }
     }

     if (msgLvl(MSG::VERBOSE)) msg() <<" JetFitterFinding phase 6: use two-track vertices as a first seed for JetFitter " << endmsg;
 
     //now you can add the tracks from the two-track vertices for a first fit with 
     //only the "best tracks"
     for (std::vector<const xAOD::Vertex*>::const_iterator v0candIter=v0candBegin;v0candIter!=v0candEnd;++v0candIter)
     {
       
       const xAOD::Vertex * v0candPtr = *v0candIter;      
     
       if (msgLvl(MSG::DEBUG)) msg() << " Analyzing two track vertices to select the best tracks" << endmsg;
       
       if (v0candPtr==0) 
       {
         msg(MSG::WARNING) << " Pointer for V0 is null pointer. Skipping V0 candidate..." << endmsg;
         continue;
       }
       
       if (m_jetFitterUtils->checkIfVxCandidateIsInVector(v0candPtr,candidatesToVeto))
           continue;
       
       auto const &  firstTrack = v0candPtr->vxTrackAtVertex()[0];
       auto const & secondTrack = v0candPtr->vxTrackAtVertex()[1];
       
       if (firstTrack==0 || secondTrack ==0)
       {
         msg(MSG::WARNING) << " Zero pointer (VxTrackAtVertex): skipping 2-track candidate " << endmsg;
         continue;
       }
       
       const Trk::ITrackLink* trackLink1=firstTrack.trackOrParticleLink();
       const Trk::ITrackLink* trackLink2=secondTrack.trackOrParticleLink();
       
       if (trackLink1==0 || trackLink2==0)
       {
         msg(MSG::WARNING) << " Zero pointer (ITrackLink): skipping 2-track candidate " << endmsg;
         continue;
       }
       
       bool hasTrack1HardVetoed=m_jetFitterUtils->checkIfTrackIsInVector(fromLinkInV0CandidateToOriginalLink[trackLink1],
                                                                         tracksToVeto);

       bool hasTrack2HardVetoed=m_jetFitterUtils->checkIfTrackIsInVector(fromLinkInV0CandidateToOriginalLink[trackLink2],
                                                                         tracksToVeto);

       if (hasTrack1HardVetoed || hasTrack2HardVetoed)
           continue;
        
       //MU       const Trk::FitQuality & vertexQuality= v0candPtr->fitQuality();

       double vertexProb=TMath::Prob(v0candPtr->chiSquared(),v0candPtr->numberDoF());

       if (vertexProb<m_cutTwoTrkVtxVertexProbForBFirstSelectionFirstCriterium && 
           vertexProb<m_cutTwoTrkVtxVertexProbForBFirstSelectionSecondCriterium)
           continue;

       const Trk::ITrackLink* originalTrackLink1=fromLinkInV0CandidateToOriginalLink[trackLink1];
       const Trk::ITrackLink* originalTrackLink2=fromLinkInV0CandidateToOriginalLink[trackLink2];
       
       double compatibilityTrack1=compatibilityOfTrack[originalTrackLink1];
       double compatibilityTrack2=compatibilityOfTrack[originalTrackLink2];

       bool flightSignificance= 
           TMath::Prob(fabs(compatibilityTrack1),2)<m_cutCompatibilityPrimaryVertexSingleTrackForBFirstSelection&&
           TMath::Prob(fabs(compatibilityTrack2),2)<m_cutCompatibilityPrimaryVertexSingleTrackForBFirstSelection&&
           TMath::Prob(fabs(compatibilityTrack1)+
                       fabs(compatibilityTrack2),4)<m_cutCompatibilityPrimaryVertexBothTracksForBFirstSelection;

       if (!flightSignificance)
           continue;
       
       const Trk::TrackParameters* initialPerigee1=firstTrack.initialPerigee();
       const Trk::TrackParameters* initialPerigee2=secondTrack.initialPerigee();
       
       if (!initialPerigee1 || !initialPerigee2)
       {
         msg(MSG::WARNING) << "No refitted parameters available for 2-track vertex. Candidate not accepted..." << endmsg;
         continue;
       }
      
      std::pair<double,double> track1_IPd0z0=m_jetFitterUtils->getD0andZ0IP(*initialPerigee1,
                                                                            primaryVertexRecVertex);

      std::pair<double,double> track2_IPd0z0=m_jetFitterUtils->getD0andZ0IP(*initialPerigee2,
                                                                            primaryVertexRecVertex);
      
      bool goodQUALITYcut1=fabs(track1_IPd0z0.first)<m_cutIPD0BothTracksForBFirstSelection&&fabs(track1_IPd0z0.second)<m_cutIPZ0BothTracksForBFirstSelection&&initialPerigee1->momentum().perp()>m_cutPtBothTracksForBFirstSelection;
      bool goodQUALITYcut2=fabs(track2_IPd0z0.first)<m_cutIPD0BothTracksForBFirstSelection&&fabs(track2_IPd0z0.second)<m_cutIPZ0BothTracksForBFirstSelection&&initialPerigee2->momentum().perp()>m_cutPtBothTracksForBFirstSelection;
      
      bool bothGoodQUALITY=goodQUALITYcut1&&goodQUALITYcut2;
      
      if (!bothGoodQUALITY)
          continue;
      
      const xAOD::Vertex * fittedVertex = *v0candIter;

      std::pair<double,double> distanceAndError=
	m_jetFitterUtils->getDistanceAndErrorBetweenTwoVertices(*fittedVertex,primaryVertexRecVertex);
      
      //now assign a sign to the distance (decay in front or behind the primary vertex)
      double signeddistance=distanceAndError.first;
      Amg::Vector3D jetMomSpatial(jetMomentum.X(),jetMomentum.Y(),jetMomentum.Z());
      double sign=(fittedVertex->position()-primaryVertexRecVertex.position()).dot(jetMomSpatial);

      if (sign<0) 
      {
        signeddistance*=-1.;
      }

      if (!m_revertFromPositiveToNegativeTags)
      {
        if (!((vertexProb>m_cutTwoTrkVtxVertexProbForBFirstSelectionFirstCriterium &&
               signeddistance/distanceAndError.second>m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionFirstCriterium)
              ||
              (vertexProb>m_cutTwoTrkVtxVertexProbForBFirstSelectionSecondCriterium && 
               signeddistance/distanceAndError.second>m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionSecondCriterium)))
            continue;
      }
      else
      {
        if (!((vertexProb>m_cutTwoTrkVtxVertexProbForBFirstSelectionFirstCriterium &&
               signeddistance/distanceAndError.second<-m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionFirstCriterium)
              ||
              (vertexProb>m_cutTwoTrkVtxVertexProbForBFirstSelectionSecondCriterium && 
               signeddistance/distanceAndError.second<-m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionSecondCriterium)))
             continue;
      }
      
      
      double radius=fittedVertex->position().perp();
      int interactiontype=-1;

      if (radius>m_firstBeam_min && radius<m_firstBeam_max)
      {
        interactiontype=0;
        numberFirstBeamInteractions+=1;
      }
      else if (radius>m_secondBeam_min && radius<m_secondBeam_max)
      {
        interactiontype=1;
        numberSecondBeamInteractions+=1;
      }
      else if (radius>m_firstLayer_min && radius<m_firstLayer_max)
      {
        interactiontype=2;
        numberFirstLayerInteractions+=1;
      }
      else if (radius>m_secondLayer_min && radius<m_secondLayer_max)
      {
        interactiontype=3;
        numberSecondLayerInteractions+=1;
      }

      if (interactiontype==2 || interactiontype==3)
      {
        bool signifCutTight=
            TMath::Prob(fabs(compatibilityTrack1),2)<m_cutCompatibilityToPrimarySingleTrackForMatInteractions&&
            TMath::Prob(fabs(compatibilityTrack2),2)<m_cutCompatibilityToPrimarySingleTrackForMatInteractions&&
            TMath::Prob(fabs(compatibilityTrack1)+
                        fabs(compatibilityTrack2),4)<m_cutCompatibilityToPrimaryBothTracksForMatInteractions;
        if (signifCutTight)
        {
          if (msgLvl(MSG::DEBUG)) msg() << " Material interaction in layer... Skipping candidate..." << endmsg;
          continue;
        }
      }

///////////////////////
      
      //problem: you need to iterate over all V0 vertices in order to get all possible "names" (new clones)
      //of the original ITrackLinks

      bool hasTrack2InCommonWithNeutral=false;
      bool hasTrack1InCommonWithNeutral=false;
      
      
      //go from original ITrackLink to all other possible ITrackLink
      std::vector<const Trk::ITrackLink*> vectorOfV0ITrackLink1;
      std::vector<const Trk::ITrackLink*> vectorOfV0ITrackLink2;
      
      std::map<const Trk::ITrackLink*,const Trk::ITrackLink*>::const_iterator fromLinkInV0CandidateToOriginalLinkBegin=fromLinkInV0CandidateToOriginalLink.begin();
      std::map<const Trk::ITrackLink*,const Trk::ITrackLink*>::const_iterator fromLinkInV0CandidateToOriginalLinkEnd=fromLinkInV0CandidateToOriginalLink.end();
      
      for (std::map<const Trk::ITrackLink*,const Trk::ITrackLink*>::const_iterator fromLinkInV0CandidateToOriginalLinkIter=fromLinkInV0CandidateToOriginalLinkBegin;
           fromLinkInV0CandidateToOriginalLinkIter!=fromLinkInV0CandidateToOriginalLinkEnd;
           ++fromLinkInV0CandidateToOriginalLinkIter)
      {
        if ((*fromLinkInV0CandidateToOriginalLinkIter).second==originalTrackLink1)
        {
          if (msgLvl(MSG::VERBOSE)) msg() <<" Found one of the original ITrackLink " << endmsg;
          vectorOfV0ITrackLink1.push_back((*fromLinkInV0CandidateToOriginalLinkIter).first);
        }
        if ((*fromLinkInV0CandidateToOriginalLinkIter).second==originalTrackLink2)
        {
          if (msgLvl(MSG::VERBOSE)) msg() <<" Found one of the original ITrackLink " << endmsg;
          vectorOfV0ITrackLink2.push_back((*fromLinkInV0CandidateToOriginalLinkIter).first);
        }
      }

      std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLink1Begin=vectorOfV0ITrackLink1.begin();
      std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLink1End=vectorOfV0ITrackLink1.end();
           
      for (std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLink1Iter=vectorOfV0ITrackLink1Begin;
           vectorOfV0ITrackLink1Iter!=vectorOfV0ITrackLink1End;
           ++vectorOfV0ITrackLink1Iter)
      {
             
        if (m_jetFitterUtils->checkIfTrackIsInNeutralTrackVector(*vectorOfV0ITrackLink1Iter,selectedNeutralTracks))
        {
          hasTrack1InCommonWithNeutral=true;
          break;
        }
        
      }
      
      std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLink2Begin=vectorOfV0ITrackLink2.begin();
      std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLink2End=vectorOfV0ITrackLink2.end();
           
      for (std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLink2Iter=vectorOfV0ITrackLink2Begin;
           vectorOfV0ITrackLink2Iter!=vectorOfV0ITrackLink2End;
           ++vectorOfV0ITrackLink2Iter)
      {
        if (m_jetFitterUtils->checkIfTrackIsInNeutralTrackVector(*vectorOfV0ITrackLink2Iter,selectedNeutralTracks))
        {
          hasTrack2InCommonWithNeutral=true;
          continue;
        }
      }
      

///////////////////////

      //THIS AGAIN CANNOT WORK!
//      bool hasTrack2InCommonWithNeutral=m_jetFitterUtils->checkIfTrackIsInNeutralTrackVector(trackLink1,selectedNeutralTracks);
//      bool hasTrack1InCommonWithNeutral=m_jetFitterUtils->checkIfTrackIsInNeutralTrackVector(trackLink2,selectedNeutralTracks);

      if (hasTrack2InCommonWithNeutral  && hasTrack1InCommonWithNeutral ) 
      {
        if (msgLvl(MSG::DEBUG)) msg() << " Both tracks in common with neutral " << endmsg;
        continue;
      }
      else if (hasTrack1InCommonWithNeutral)
      {
        const Trk::ITrackLink* trackLink2Original=fromLinkInV0CandidateToOriginalLink[trackLink2];
        if (!m_jetFitterUtils->checkIfTrackIsInVector(trackLink2Original,
                                                      tracksToUseInFirstFit)) 
        {
          if (msgLvl(MSG::DEBUG)) msg() << " Track 1 in common with neutrals. Adding track 2... " << endmsg;          
          tracksToUseInFirstFit.push_back(trackLink2Original);
        }
        else
        {
          if (msgLvl(MSG::DEBUG)) msg() << " Track 1 in common with neutrals.BUT NOT Adding track 2 because already there... " << endmsg;          
        }
        positionsOfSeedingVertices.push_back(Trk::PositionAndWeight(v0candPtr->position(),1));
      }
      else if (hasTrack2InCommonWithNeutral)
      {
        const Trk::ITrackLink* trackLink1Original=fromLinkInV0CandidateToOriginalLink[trackLink1];
        if (!m_jetFitterUtils->checkIfTrackIsInVector(trackLink1Original,
                                                      tracksToUseInFirstFit)) 
        {
          if (msgLvl(MSG::DEBUG)) msg() << " Track 2 in common with neutrals. Adding track 1... " << endmsg;
          tracksToUseInFirstFit.push_back(trackLink1Original);
        }
        else
        {
          if (msgLvl(MSG::DEBUG)) msg() << " Track 2 in common with neutrals.BUT NOT Adding track 1 because already there... " << endmsg;          
        }
        
        positionsOfSeedingVertices.push_back(Trk::PositionAndWeight(v0candPtr->position(),1));
      }
      else //no tracks in common with neutrals
      {

        const Trk::ITrackLink* trackLink2Original=fromLinkInV0CandidateToOriginalLink[trackLink2];
        const Trk::ITrackLink* trackLink1Original=fromLinkInV0CandidateToOriginalLink[trackLink1];
        
        if (msgLvl(MSG::DEBUG)) msg() << " No track in common with neutrals. Considering adding both of them... " << endmsg;
        
        if (!m_jetFitterUtils->checkIfTrackIsInVector(trackLink2Original,
                                                      tracksToUseInFirstFit)) 
        {
          if (msgLvl(MSG::DEBUG)) msg() << " Adding track 2... " << endmsg;          
          tracksToUseInFirstFit.push_back(trackLink2Original);
        }
        
        if (!m_jetFitterUtils->checkIfTrackIsInVector(trackLink1Original,
                                                      tracksToUseInFirstFit)) 
        {
          if (msgLvl(MSG::DEBUG)) msg() << " Adding track 1... " << endmsg;          
          tracksToUseInFirstFit.push_back(trackLink1Original);
        }
        positionsOfSeedingVertices.push_back(Trk::PositionAndWeight(v0candPtr->position(),1));
      }
     }

     if (msgLvl(MSG::VERBOSE)) msg() <<" JetFitterFinding phase 7: determine single good tracks to add in the fit in a second step " << endmsg;
     
     //now the tracksToUseInFirstFit are filled, what is missing are the single tracks to be used 
     //in a further step of the fit

     for (std::vector<const Trk::ITrackLink*>::const_iterator secondaryTracksIter=secondaryTracksBegin;
          secondaryTracksIter!=secondaryTracksEnd;++secondaryTracksIter) 
     {

       double compatibilityOfActualTrack=compatibilityOfTrack[*secondaryTracksIter];
       
       const Trk::TrackParameters* perigee=(*secondaryTracksIter)->parameters();
       
       const AmgSymMatrix(5)* measPerigee=perigee->covariance();
       
       if (measPerigee==0)
       {
         msg(MSG::WARNING) << " Track parameters have no covariance. skipping single track candidate... " << endmsg;
         continue;
       }
       
       std::pair<double,double> track_IPd0z0=m_jetFitterUtils->getD0andZ0IP(*perigee,
                                                                            primaryVertexRecVertex);
       std::pair<double,double> track_IPd0z0Sig=m_jetFitterUtils->getD0andZ0IPSig(*perigee,
										  primaryVertexRecVertex);

       const double & IPd0=track_IPd0z0.first;
       const double & IPz0=track_IPd0z0.second;
       const double & IPd0Sig=track_IPd0z0Sig.first;
       const double & IPz0Sig=track_IPd0z0Sig.second;
       const double pT=perigee->momentum().perp();
       
       double cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection=m_cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection;
       double cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection=m_cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection;

       if (m_revertFromPositiveToNegativeTags)
       {
         cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection=m_cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection;
         cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection=m_cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection;
       }
       
       bool passBoxCut=( fabs(IPd0Sig)<m_cutIPD0SigBoxSingleTrackForBSecondSelection && fabs(IPz0Sig)>m_cutIPZ0SigBoxSingleTrackForBSecondSelection);

       if (fabs(IPd0)>m_cutIPD0SingleTrackForBSecondSelection||
           fabs(IPz0)>m_cutIPZ0SingleTrackForBSecondSelection||
           ( compatibilityOfActualTrack>=0 && TMath::Prob(fabs(compatibilityOfActualTrack),2)>cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection) ||
           ( compatibilityOfActualTrack<0 && TMath::Prob(fabs(compatibilityOfActualTrack),2)>cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection) ||
           pT< m_cutPtSingleTrackForBSecondSelection || passBoxCut) 
       {
         if (msgLvl(MSG::DEBUG)) msg() << " Candidate didn't pass one of the selection cuts " << endmsg;
         continue;
       }

       bool alreadyUsed=m_jetFitterUtils->checkIfTrackIsInVector(*secondaryTracksIter,
                                                                 tracksToUseInFirstFit);
       
       if (alreadyUsed)
       {
         if (msgLvl(MSG::VERBOSE)) msg() <<" Track was already used " << endmsg;
         continue;
       }
       
       bool isVetoed=m_jetFitterUtils->checkIfTrackIsInVector(*secondaryTracksIter,
                                                              tracksToVeto);
       
       if (isVetoed)
       {
         if (msgLvl(MSG::VERBOSE)) msg() <<" Track was vetoed " << endmsg;
         continue;
       }

       bool isInVetoedCandidate=false;
       
       //go from original ITrackLink to all other possible ITrackLink
       std::vector<const Trk::ITrackLink*> vectorOfV0ITrackLink;
       
       std::map<const Trk::ITrackLink*,const Trk::ITrackLink*>::const_iterator fromLinkInV0CandidateToOriginalLinkBegin=fromLinkInV0CandidateToOriginalLink.begin();
       std::map<const Trk::ITrackLink*,const Trk::ITrackLink*>::const_iterator fromLinkInV0CandidateToOriginalLinkEnd=fromLinkInV0CandidateToOriginalLink.end();
       
       for (std::map<const Trk::ITrackLink*,const Trk::ITrackLink*>::const_iterator fromLinkInV0CandidateToOriginalLinkIter=fromLinkInV0CandidateToOriginalLinkBegin;
            fromLinkInV0CandidateToOriginalLinkIter!=fromLinkInV0CandidateToOriginalLinkEnd;
            ++fromLinkInV0CandidateToOriginalLinkIter)
       {
         if ((*fromLinkInV0CandidateToOriginalLinkIter).second==*secondaryTracksIter)
         {
           if (msgLvl(MSG::VERBOSE)) msg() <<" Found one of the original ITrackLink " << endmsg;
           vectorOfV0ITrackLink.push_back((*fromLinkInV0CandidateToOriginalLinkIter).first);
         }
       }
       
       std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLinkBegin=vectorOfV0ITrackLink.begin();
       std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLinkEnd=vectorOfV0ITrackLink.end();

       for (std::vector<const Trk::ITrackLink*>::const_iterator vectorOfV0ITrackLinkIter=vectorOfV0ITrackLinkBegin;
            vectorOfV0ITrackLinkIter!=vectorOfV0ITrackLinkEnd;
            ++vectorOfV0ITrackLinkIter)
       {
         
         isInVetoedCandidate=isInVetoedCandidate||
             m_jetFitterUtils->checkIfTrackIsInV0CandidatesVector(*vectorOfV0ITrackLinkIter,
                                                                  candidatesToVeto);
         if (isInVetoedCandidate)
         {
           break;
         }
       }
       
       //now isInVetoedCandidate has the correct answer...

       if (isInVetoedCandidate)
       {
         if (msgLvl(MSG::DEBUG)) msg() << " vetoed by existing V0 candidate veto " << endmsg;
         continue;
       }

       
       bool alreadyUsedSecondFit=m_jetFitterUtils->checkIfTrackIsInVector(*secondaryTracksIter,
                                                                          tracksToUseInSecondFit);
       
       if (alreadyUsedSecondFit)
       {
         msg(MSG::WARNING) << " Already used in second fit. Should this be really possible??? " << endmsg;
         continue;
       }
       
       tracksToUseInSecondFit.push_back(*secondaryTracksIter);
     }
     
     int firstFitTracksNum=tracksToUseInFirstFit.size();
     int secondFitTracksNum=tracksToUseInSecondFit.size();
     
     if (msgLvl(MSG::DEBUG)) msg() << " First fit with : " << firstFitTracksNum << 
         " tracks. Second fit with : " << secondFitTracksNum << endmsg;

    //now take the positionsOfSeedingVertices and try to obtain a good seed direction...
     Amg::Vector3D JFseedDirection(jetMomentum.X(),jetMomentum.Y(),jetMomentum.Z());
     JFseedDirection.normalize();
     if (msgLvl(MSG::VERBOSE)) msg() <<" Jet Direction would be: " << JFseedDirection << endmsg;
     
     if (positionsOfSeedingVertices.size()!=0) 
     {
       Amg::Vector3D theSeedVertex=m_mode3dfinder->getMode(positionsOfSeedingVertices);
       if (m_revertFromPositiveToNegativeTags==false)
       {
         if ((theSeedVertex-primaryVertexRecVertex.position()).dot(JFseedDirection)>0) 
         {
           JFseedDirection=(theSeedVertex-primaryVertexRecVertex.position()).unit();
           if (msgLvl(MSG::DEBUG)) msg() << " Using twotrkvtx direction for start: " << JFseedDirection << endmsg;
         }
         else 
         {
           msg(MSG::WARNING) << " NORMAL SEEDING: Seed vertex is on negative side... Using Jet Direction!" << endmsg;
         }
       }
       else
       {
         if ((theSeedVertex-primaryVertexRecVertex.position()).dot(JFseedDirection)<0) 
         {
           JFseedDirection=-(theSeedVertex-primaryVertexRecVertex.position()).unit();
           if (msgLvl(MSG::DEBUG)) msg() << " Using twotrkvtx direction for start: " << JFseedDirection << endmsg;
         }
         else 
         {
           msg(MSG::WARNING) << " REVERSE SEEDING: Seed vertex is on positive side... Using Jet Direction!" << endmsg;
         }         
       }
     }

     if (msgLvl(MSG::VERBOSE)) msg() <<" JetFitterFinding phase 8: do the real finding with JetFitter " << endmsg;

     Trk::VxJetCandidate* myJetCandidate=findSecVertex(primaryVertexRecVertex,
                                                       jetMomentum,
                                                       tracksToUseInFirstFit,
                                                       tracksToUseInSecondFit,
                                                       JFseedDirection);
     
     if (msgLvl(MSG::VERBOSE)) msg() <<" JetFitterFinding phase 9: store the results " << endmsg;


     //now in principle you just need to store the RESULT...


     //create new neutral tracks (the one you want to store...)
     std::vector<const Trk::TrackParticleBase*> selectedNeutralTracksToStore;

     std::vector<const Trk::LinkToTrackParticleBase*>::iterator selectedNeutralTracksBegin=selectedNeutralTracksForIP.begin();
     std::vector<const Trk::LinkToTrackParticleBase*>::iterator selectedNeutralTracksEnd=selectedNeutralTracksForIP.end();

     for (std::vector<const Trk::LinkToTrackParticleBase*>::iterator selectedNeutralTracksIter=selectedNeutralTracksBegin;
          selectedNeutralTracksIter!=selectedNeutralTracksEnd;++selectedNeutralTracksIter) 
     {
       selectedNeutralTracksToStore.push_back(new Trk::TrackParticleBase(****selectedNeutralTracksIter));
     }

       //create the auxiliary objects...
       const Trk::TwoTrackVerticesInJet* myTwoTrackVerticesInJet=new 
           Trk::TwoTrackVerticesInJet(V0candidates,
                                      selectedNeutralTracksToStore);
     
       const Trk::SelectedTracksInJet* mySelectedTracksInJet=new 
           Trk::SelectedTracksInJet(primaryTracks,
                                    secondaryTracks);
     
     //delete the old objects.. pay some attention here!

     //delete the neutralTracks that you didn't use...
     std::vector<const Trk::LinkToTrackParticleBase*>::iterator neutralTracksBegin=neutralTracks.begin();
     std::vector<const Trk::LinkToTrackParticleBase*>::iterator neutralTracksEnd=neutralTracks.end();

     for (std::vector<const Trk::LinkToTrackParticleBase*>::iterator neutralTracksIter=neutralTracksBegin;
          neutralTracksIter!=neutralTracksEnd;++neutralTracksIter) 
     {
       //if (!m_jetFitterUtils->checkIfTrackIsInVector(*neutralTracksIter,selectedNeutralTracks))
       //{
       delete ***neutralTracksIter;
       //}
       delete *neutralTracksIter;
       *neutralTracksIter=0;
     }


     //there shouldn't be much more...
     //     int numberFirstBeamInteractions=0;
     //    int numberSecondBeamInteractions=0;
     //    int numberFirstLayerInteractions=0;
     //    int numberSecondLayerInteractions=0;
     
     //for the moment do not store material interactions...

     //now create the output object
     //MU      std::vector<Trk::VxCandidate*> myCandidates;
     std::vector<Trk::VxJetCandidate*> myCandidates;
     // This push_back is problematic for the migration to xAOD::Vertex, it works simply because VxJetCandidate inherits from VxCandidate
     myCandidates.push_back(myJetCandidate);

     /* SERIOUS PROBLEM */
     /* 17.05.2014 Problem here: can't store jetfitter vertex in xAOD:VxCandidate! */
//     const Trk::VxJetFitterVertexInfo* myOutputInfo=new Trk::VxJetFitterVertexInfo(myCandidates,
//                                                                                   myTwoTrackVerticesInJet,
//                                                                                   mySelectedTracksInJet);
     
     Trk::VxJetFitterVertexInfo* myOutputInfo=new Trk::VxJetFitterVertexInfo(myCandidates,
                                                                             myTwoTrackVerticesInJet,
                                                                             mySelectedTracksInJet);
      
     myOutputInfo->getSVOwnership(true);


     delete signalVertex;
     signalVertex=0;

     return myOutputInfo;
  }
  

  Trk::VxJetCandidate* InDetImprovedJetFitterVxFinder::findSecVertex(const Trk::RecVertex & primaryVertex,
                                                                     const TLorentzVector & jetMomentum,
                                                                     const std::vector<const Trk::ITrackLink*> & firstInputTracks,
                                                                     const std::vector<const Trk::ITrackLink*> & secondInputTracks,
                                                                     const Amg::Vector3D & vtxSeedDirection) const 
  {


    if (msgLvl(MSG::VERBOSE)) msg() << " entered findSecVertex(). Applying JetFitter finding to the found sets of tracks and performing clustering (pattern recognition)  " << endmsg;

    Amg::Vector3D myDirection(jetMomentum.X(),jetMomentum.Y(),jetMomentum.Z());

    std::vector<std::vector<const Trk::ITrackLink*> > bunchesOfTracks;
    
    std::vector<const Trk::ITrackLink*> tracksToAdd;
    
    std::vector<const Trk::ITrackLink*>::const_iterator tracks2Begin=firstInputTracks.begin();
    std::vector<const Trk::ITrackLink*>::const_iterator tracks2End=firstInputTracks.end();
    for (std::vector<const Trk::ITrackLink*>::const_iterator tracks2Iter=tracks2Begin;
	 tracks2Iter!=tracks2End;++tracks2Iter) {
      if (msgLvl(MSG::VERBOSE)) msg() <<" adding track to fit " << endmsg;
      tracksToAdd.push_back(*tracks2Iter);
    }
    
    bunchesOfTracks.push_back(tracksToAdd);
    tracksToAdd.clear();
  
    std::vector<const Trk::ITrackLink*>::const_iterator tracks3Begin=secondInputTracks.begin();
    std::vector<const Trk::ITrackLink*>::const_iterator tracks3End=secondInputTracks.end();
    for (std::vector<const Trk::ITrackLink*>::const_iterator tracks3Iter=tracks3Begin;
	 tracks3Iter!=tracks3End;++tracks3Iter) {
      if (msgLvl(MSG::VERBOSE)) msg() <<" adding track to fit " << endmsg;
      tracksToAdd.push_back(*tracks3Iter);
    }

    if (tracksToAdd.size()!=0) 
    {
      bunchesOfTracks.push_back(tracksToAdd);
    }
    tracksToAdd.clear();
    

    //now it just uses these bunches...
    //now I have just to make sure that no clustering is done at first iteration
    //while it needs to be done at second iteration (there will be only two iterations)


    std::vector<std::vector<const Trk::ITrackLink*> >::const_iterator BunchesBegin=bunchesOfTracks.begin();
    std::vector<std::vector<const Trk::ITrackLink*> >::const_iterator BunchesEnd=bunchesOfTracks.end();

    std::vector<const Trk::ITrackLink*>::const_iterator tracksToAddBegin;
    std::vector<const Trk::ITrackLink*>::const_iterator tracksToAddEnd;
    std::vector<const Trk::ITrackLink*>::const_iterator tracksToAddIter;


    Trk::VxJetCandidate* myJetCandidate=0;

    for (std::vector<std::vector<const Trk::ITrackLink*> >::const_iterator BunchesIter=BunchesBegin;
	 BunchesIter!=BunchesEnd;++BunchesIter) {
      
      if (BunchesIter==BunchesBegin) {
	if (msgLvl(MSG::VERBOSE)) msg() <<" initial fit with  " << (*BunchesIter).size() << " tracks " << endmsg;
	myJetCandidate=m_initializationHelper->initializeJetCandidate(*BunchesIter,&primaryVertex,&myDirection,&vtxSeedDirection);
	m_routines->initializeToMinDistancesToJetAxis(myJetCandidate);
        if ((*BunchesIter).size()>0) 
        {
          doTheFit(myJetCandidate,true);
        }
      } else {
	if (msgLvl(MSG::VERBOSE)) msg() <<" other fit with " << (*BunchesIter).size() << " tracks " << endmsg;
	std::vector<Trk::VxVertexOnJetAxis*> setOfVertices=myJetCandidate->getVerticesOnJetAxis();
	std::vector<Trk::VxTrackAtVertex*>* setOfTracks=myJetCandidate->vxTrackAtVertex();
	tracksToAddBegin=(*BunchesIter).begin();
	tracksToAddEnd=(*BunchesIter).end();
	for (tracksToAddIter=tracksToAddBegin;tracksToAddIter!=tracksToAddEnd;++tracksToAddIter) {
	  std::vector<Trk::VxTrackAtVertex*> temp_vector_tracksAtVertex;
	  Trk::VxTrackAtVertex* newVxTrack=new Trk::VxTrackAtVertex((*tracksToAddIter)->clone());
	  temp_vector_tracksAtVertex.push_back(newVxTrack);
	  setOfTracks->push_back(newVxTrack);
	  setOfVertices.push_back(new Trk::VxVertexOnJetAxis(temp_vector_tracksAtVertex));
	}
	if (msgLvl(MSG::VERBOSE)) msg() <<" new overall number of tracks to fit : " << setOfVertices.size() << endmsg;
	myJetCandidate->setVerticesOnJetAxis(setOfVertices);
	m_initializationHelper->updateTrackNumbering(myJetCandidate);
        //question: should this be done???
	m_routines->initializeToMinDistancesToJetAxis(myJetCandidate);
	doTheFit(myJetCandidate);
      }
    }
    

    //    std::vector<Trk::VxCandidate*> myCandidates;
    //    myCandidates.push_back(myJetCandidate);
    //return new Trk::VxSecVertexInfo(myCandidates);//ownership of the single objects is taken over!

    return myJetCandidate;
    
  }

  void InDetImprovedJetFitterVxFinder::doTheFit(Trk::VxJetCandidate* myJetCandidate,
                                                bool performClustering) const {

    
    int numClusteringLoops=0;
    bool noMoreVerticesToCluster(false);

    do {//reguards clustering

      if (msgLvl(MSG::VERBOSE)) msg() <<"InDetImprovedJetFitterVxFinder:      ------>>>>         new cycle of fit" << endmsg;

      int numLoops=0;
      bool noMoreTracksToDelete(false);
      do {//reguards eliminating incompatible tracks...
	
	m_routines->performTheFit(myJetCandidate,15,false,30,0.001);
	
	const std::vector<Trk::VxVertexOnJetAxis*> & vertices=myJetCandidate->getVerticesOnJetAxis();
	
	std::vector<Trk::VxVertexOnJetAxis*>::const_iterator verticesBegin=vertices.begin();
	std::vector<Trk::VxVertexOnJetAxis*>::const_iterator verticesEnd=vertices.end();
	
	
	//delete incompatible tracks...
	float max_prob(1.);
	Trk::VxVertexOnJetAxis* worseVertex(0);
	for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator verticesIter=verticesBegin;
	     verticesIter!=verticesEnd;++verticesIter) {
	  if (*verticesIter==0) {
	    msg(MSG::WARNING) << "One vertex is empy. Problem when trying to delete incompatible vertices. No further vertices deleted." << endmsg;
	  } else {
	    const Trk::FitQuality & fitQuality=(*verticesIter)->fitQuality();
	    if (TMath::Prob(fitQuality.chiSquared(),(int)std::floor(fitQuality.numberDoF()+0.5))<max_prob) {
	      max_prob=TMath::Prob(fitQuality.chiSquared(),(int)std::floor(fitQuality.numberDoF()+0.5));
	      worseVertex=*verticesIter;
	    }
	  }
	}
	if (max_prob<m_vertexProbCut) {
	  if (msgLvl(MSG::DEBUG)) msg() << "Deleted vertex " << worseVertex->getNumVertex() << " with probability " << max_prob << endmsg;
	  //	  std::cout << "Deleted vertex " << worseVertex->getNumVertex() << " with probability " << max_prob << std::endl;
	  if (worseVertex==myJetCandidate->getPrimaryVertex()) {
	    if (msgLvl(MSG::VERBOSE)) msg() << " It's the primary" << endmsg;
	  }

	  m_routines->deleteVertexFromJetCandidate(worseVertex,myJetCandidate);

	} else {
	  noMoreTracksToDelete=true;
	  if (msgLvl(MSG::VERBOSE)) msg() <<"No tracks to delete: maximum probability is " << max_prob << endmsg;
	}
	
	numLoops+=1;
      } while (numLoops<m_maxNumDeleteIterations&&!(noMoreTracksToDelete));
      
      if (!performClustering) break;

      if (!m_useFastClustering && (int)myJetCandidate->getVerticesOnJetAxis().size()<m_maxTracksForDetailedClustering) {
	m_routines->fillTableWithFullProbOfMerging(myJetCandidate,8,false,10,0.01);
      } else {
	m_routines->fillTableWithFastProbOfMerging(myJetCandidate);
      }
      const Trk::VxClusteringTable* clusteringTablePtr(myJetCandidate->getClusteringTable());


      

      if (clusteringTablePtr==0) {
	msg(MSG::WARNING) << " No Clustering Table while it should have been calculated... no more clustering performed during vertexing " << endmsg;
	noMoreVerticesToCluster=true;
      } else {

	if (msgLvl(MSG::VERBOSE)) msg() <<" clustering table is " << *clusteringTablePtr << endmsg;

	//now iterate over the full map and decide wether you want to do the clustering OR not...
	float probVertex(0.);
	Trk::PairOfVxVertexOnJetAxis pairOfVxVertexOnJetAxis=clusteringTablePtr->getMostCompatibleVertices(probVertex);
	//a PairOfVxVertexOnJetAxis is a std::pair<VxVertexOnJetAxis*,VxVertexOnJetAxis*>
	
	float probVertexExcludingPrimary(0.);
	Trk::PairOfVxVertexOnJetAxis pairOfVxVertexOnJetAxisExcludingPrimary=clusteringTablePtr->getMostCompatibleVerticesExcludingPrimary(probVertexExcludingPrimary);

	bool firstProbIsWithPrimary= ( fabs(probVertex-probVertexExcludingPrimary)>1e-6 );

	if (probVertex>0.&&probVertex>m_vertexClusteringProbabilityCut&&firstProbIsWithPrimary) {
	  if (msgLvl(MSG::VERBOSE)) msg() <<" merging vtx number " << (*pairOfVxVertexOnJetAxis.first).getNumVertex() << 
                                        " and " << (*pairOfVxVertexOnJetAxis.second).getNumVertex() << " (should be PV)." << endmsg;
          //	  const Trk::VxVertexOnJetAxis & mergedVertex=
          m_helper->mergeVerticesInJetCandidate(*pairOfVxVertexOnJetAxis.first,
                                                *pairOfVxVertexOnJetAxis.second,
                                                *myJetCandidate);
	  //now you need to update the numbering scheme
	  m_initializationHelper->updateTrackNumbering(myJetCandidate);//maybe this should be moved to a lower level...
          continue;
	}

        if (probVertexExcludingPrimary>0.)
	{

	  //GP suggested by Marco Battaglia, use vertex mass in order to decide wether to split or not, so derive vertex masses first
	  const Trk::VxVertexOnJetAxis* firstVertex=pairOfVxVertexOnJetAxisExcludingPrimary.first;
	  const Trk::VxVertexOnJetAxis* secondVertex=pairOfVxVertexOnJetAxisExcludingPrimary.second;
          
	  CLHEP::HepLorentzVector massVector1=m_jetFitterUtils->fourMomentumAtVertex(*firstVertex);//MeV
	  CLHEP::HepLorentzVector massVector2=m_jetFitterUtils->fourMomentumAtVertex(*secondVertex);//MeV

	  CLHEP::HepLorentzVector sumMassVector=massVector1+massVector2;

	  double massTwoVertex=sumMassVector.mag();//MeV
	  
	  bool doMerge(false);

	  double vertexClusteringProbabilityCutWithMass;

	  if(massTwoVertex< 1000.){
	    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass0010;
	  }else if(massTwoVertex< 1500.){
	    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass1015;
	  }else if(massTwoVertex< 2000.){
	    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass1520;
	  }else if(massTwoVertex< 2500.){
	    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass2025;
	  }else if(massTwoVertex< 3000.){
	    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass2530;
	  }else if(massTwoVertex< 4000.){
	    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass3040;
	  }else if(massTwoVertex< 5000.){
	    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass4050;
	  }else if(massTwoVertex< 6000.){
	    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass5060;
	  }else{
	    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass6070;
	  }

	  if (probVertexExcludingPrimary>vertexClusteringProbabilityCutWithMass)
          {
            doMerge=true;
          }

	  if (doMerge)
	  {

	    if (msgLvl(MSG::VERBOSE)) msg() <<" merging vtx number " << (*pairOfVxVertexOnJetAxis.first).getNumVertex() <<
					" and " << (*pairOfVxVertexOnJetAxis.second).getNumVertex() << " mass merged vertex: " << massTwoVertex << endmsg;
	    
	    m_helper->mergeVerticesInJetCandidate(*pairOfVxVertexOnJetAxisExcludingPrimary.first,
						  *pairOfVxVertexOnJetAxisExcludingPrimary.second,
						  *myJetCandidate);
	    
	    m_initializationHelper->updateTrackNumbering(myJetCandidate);//maybe this should be moved to a lower level...                                                                   
	    continue;//go to next cycle, after a succesful merging
	  }
	}
	  
	noMoreVerticesToCluster=true;


      }
      numClusteringLoops+=1;
    } while (numClusteringLoops<m_maxClusteringIterations&&!(noMoreVerticesToCluster));
    
    //now a section should follow where the "complicate" VxJetCandidate is transformed in a conventional "VxCandidate" 
    //so that it can be used also by the normal B-Tagging algorithms...
    //TO BE COMPLETED
    
    //return myJetCandidate;

  }


}//end namespace Rec
