/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRKALIGNGENTOOLS_BEAMSPOTVERTEXPREPROCESSOR_H
#define  TRKALIGNGENTOOLS_BEAMSPOTVERTEXPREPROCESSOR_H

#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkAlignInterfaces/IAlignTrackPreProcessor.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include <limits>
#include "VxVertex/VxContainer.h"
#include <functional>

#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

/**
   @brief Tool used to create a collection of AlignTracks from Tracks
   while the tracks are being refitted with a vertex/beam-spot constraint.
   The AlignTracks are filled into a collection of Tracks.
   The default control flow is like this:
   firstly do vertex constraint, if can't, then do beamspot again;
   if can't again, then do normal refit.
   if want to do vertex constraint only, set:
   doVertexConstraint = True and doBeamspotConstraint = False 
   if want to do beamspot constraint only, set:
   doVertexConstraint = False and doBeamspotConstraint = True 
   @author Jike Wang <jwang@cern.ch>
   @author Daniel Kollar <daniel.kollar@cern.ch>
   -> updated 2015 Matthias Danninger
*/
 

namespace Trk {
  class IGlobalTrackFitter;
  class IExtrapolator;
  //class ITrackSelectorTool;
  class Track;
  class AlignTrack;
  class AlignVertex;
  class VxCandidate;
  class VertexOnTrack;
  class VxTrackAtVertex;
  class IAlignModuleTool;

  class BeamspotVertexPreProcessor : virtual public Trk::IAlignTrackPreProcessor, public AthAlgTool
  {

  public:

    BeamspotVertexPreProcessor(const std::string & type, const std::string & name, const IInterface * parent);
    virtual ~BeamspotVertexPreProcessor();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual DataVector<Track> * processTrackCollection(const DataVector<Track> * trks);

    void accumulateVTX(const AlignTrack* alignTrack);

    void solveVTX();

    /** Print processing summary to logfile. */
    virtual void printSummary();



  private:

    void  prepareAllTracksVector();

    bool  isAssociatedToPV(const Track * track, const xAOD::VertexContainer* vertices);
    bool  isAssociatedToVertex(const Track * track, const xAOD::Vertex * vertex);

    bool  selectVertices(const xAOD::Vertex * vtx) const;
    bool  selectUpdatedVertices(const xAOD::Vertex * updatedVtx) const;
    
    const xAOD::Vertex* findVertexCandidate(const Track* track) const; // MD: changed to be now only a vertex
    const VertexOnTrack* provideVotFromVertex(const Track* track, const xAOD::Vertex* &vtx) const;
    const VertexOnTrack* provideVotFromBeamspot(const Track* track) const;
    void provideVtxBeamspot(const AlignVertex* b, AmgSymMatrix(3)* q, Amg::Vector3D* v) const;


    const Track* doConstraintRefit(ToolHandle<IGlobalTrackFitter>& fitter, const Track* track,  const VertexOnTrack* vot, const ParticleHypothesis& particleHypothesis) const;
    bool  doBeamspotConstraintTrackSelection(const Track* track);
    AlignTrack* doTrackRefit(const Track* track);



    ToolHandle<IGlobalTrackFitter>    m_trackFitter;     //!< normal track fitter
    ToolHandle<IGlobalTrackFitter>    m_SLTrackFitter;   //! straight line track fitter
    ToolHandle<IExtrapolator>         m_extrapolator;    //!< track extrapolator
    //ToolHandle<ITrackSelectorTool>    m_trkSelector;     //!< track selector tool
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelector; //!< new track selector tool
    //ToolHandle<ITrackSelectorTool>    m_BSTrackSelector; //!< track selector tool for tracks to be used with beam-spot constraint
    ToolHandle<InDet::IInDetTrackSelectionTool> m_BSTrackSelector; //!< new track selector tool for tracks to be used with beam-spot constraint
    //MD:
    PublicToolHandle<ITrackToVertexIPEstimator>        m_ITrackToVertexIPEstimator
       {this,"TrackToVertexIPEstimator","Trk::TrackToVertexIPEstimator",""};  //!< vertex updating tool, such as reomving one track from vertex

    /** Pointer to AlignModuleTool*/
    ToolHandle <Trk::IAlignModuleTool> m_alignModuleTool;
    
    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

    std::string m_PVContainerName;                       //!< the name of the primary vertex container
    bool m_runOutlierRemoval;                            //!< switch whether to run outlier logics or not
    bool m_selectVertices;                 	         //!< do vertex selection  
    int  m_particleNumber;             		         //!< type of material interaction in extrapolation
    bool m_doTrkSelection;                               //!< to activate the preprocessor track selection
    bool m_doBSTrackSelection;                           //!< the selection mechanism which is based on cutting the perigee parameters, pt, etc.
    bool m_doAssociatedToPVSelection;        		 //!< the selection mechanism that only use the tracks associated to PV

    unsigned int m_constraintMode;

    std::string m_compareMethod;			 //!< the method used to judge whether two tracks are the same track
    std::vector<std::string> m_interestedVertexContainers;
    std::vector< std::pair< const xAOD::Vertex*, std::vector<VxTrackAtVertex> > >  m_allTracksVector;
    bool m_doBeamspotConstraint;    			 //!< do beamspot constraint 
    bool m_doPrimaryVertexConstraint;      		 //!< do vertex constraint 
    bool m_doFullVertexConstraint;                       //!< do GX full vertex constraint 
    bool m_doNormalRefit;                                //!< provide tracks in the case failed BS, PV and FullVertex constraints. 
    
    double m_maxPt;                             //!< Max pT range for refitting tracks
 
    bool m_refitTracks;                       //!< flag to refit tracks
    bool m_storeFitMatrices;  			         //!< flag to store derivative and covariance matrices after refit
    bool m_useSingleFitter;                   //!< only use 1 fitter for refitting track
    double m_BSScalingFactor;               //!< scaling factor on beasmpot width
    double m_PVScalingFactor;               //!< scaling factor on primary vertex position error

    int m_minTrksInVtx;                     //!< requirement to the minimal number of tracks in the vertex
    int m_nTracks;
    std::vector<int> m_trackTypeCounter;
    int m_nFailedNormalRefits;
    int m_nFailedBSRefits;
    int m_nFailedPVRefits;

    DataVector<AlignVertex> m_AlignVertices;	         //!< collection of AlignVertices used in FullVertex constraint option


  };



  class CompareTwoTracks {

    public:
         CompareTwoTracks(const Track* track, const std::string& compareMethod)
         :m_method(compareMethod)
         ,m_track(track)
         { //std::cout <<"compareMethod: "<< m_method <<std::endl; 
         }

    bool operator()(VxTrackAtVertex vtxTrk);

    private:
         std::string m_method;
         const Track* m_track;
  };


}

#endif // TRKALIGNGENTOOLS_BEAMSPOTVERTEXPREPROCESSOR_H

