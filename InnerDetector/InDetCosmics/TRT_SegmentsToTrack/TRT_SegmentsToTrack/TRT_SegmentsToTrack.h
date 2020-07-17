/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_SEGMENTSTOTRACK_H
#define TRT_SEGMENTSTOTRACK_H

//======================================================
//
// Algorithm for creating Tracks out of TrackSegments
// by feeding them through a TrackFitter
//
// Author: Christian Schmitt <Christian.Schmitt@cern.ch>
//
//=======================================================
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkFitterInterfaces/ITrackFitter.h" 
#include "TrkExInterfaces/IExtrapolator.h"
// for shared hits:
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

class PRD_MultiTruthCollection;
class AtlasDetectorID;
class TRT_ID;

namespace Trk {
  class Track;
  class Segment;
}

namespace InDet{
  class TRT_DriftCircle;
}


/** @class TRT_SegmentsToTrack

    Algorithm that creates Tracks out of TrackSegments by feeding them
    to a TrackFitter

    @author Christian.Schmitt@Cern.ch
*/
namespace InDet 
{
 class TRT_SegmentsToTrack : public AthAlgorithm
  {
  public:
    TRT_SegmentsToTrack(const std::string& name, ISvcLocator* pSvcLocator);
    ~TRT_SegmentsToTrack();
    
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:
    int getNumberReal(const InDet::TRT_DriftCircle*,const EventContext& ctx) const; //!< Get the number of truth particles associated with this hit


    double getRealFractionTRT(const Trk::Track *track,const EventContext& ctx) const; //!< Get the fraction of truth TRT hits on this Track
    
    double getNoiseProbability(const Trk::Track *track) const; //!< Get the fraction of noise TRT hits on this Track
    
    int nTRTHits(const Trk::Track *track) const; //!< Count number of TRT Hits on track
    
    int nHTHits(const Trk::Track *track) const; //!< Count number of TRT HT Hits on track

    void combineSegments(const EventContext& ctx) const;

    
    SG::ReadHandleKey<Trk::SegmentCollection> m_inputSegmentCollectionName{this,"InputSegmentsCollection","TrackSegments","RHK to retrieve input track collection"}; //!< Name of the TrackSegment Collection to read in

    SG::WriteHandleKey<TrackCollection> m_outputTrackCollectionName{this,"OutputTrackCollection","SegmentTracks","WHK to store output tracks"};  //!< Name of the TrackCollection to write out 

    ToolHandle<Trk::ITrackFitter> m_trackFitter
       {this,"TrackFitter","Trk::KalmanFitter/TrkKalmanFitter",""};          //!< The TrackFitter

    ToolHandle<Trk::IExtrapolator> m_extrapolator
       {this,"ExtrapolationTool","Trk::Extrapolator/InDetExtrapolator",""};  //!< The Extrapolator

    ToolHandle<Trk::IExtendedTrackSummaryTool> m_trkSummaryTool
       {this, "SummaryTool","",""};
    ToolHandle<Trk::IPRDtoTrackMapTool>        m_assoTool
       {this, "AssociationTool", "","" };
    /** key for the PRDtoTrackMap to filled by the ambiguity score processor.**/
    SG::WriteHandleKey<Trk::PRDtoTrackMap>    m_assoMapName
       {this,"AssociationMapName","",""};  ///< key to be set to optionally store PRD to track association map
    SG::ReadHandleKey<Trk::PRDtoTrackMap>     m_inputAssoMapName
       {this,"InputAssociationMapName","",""};  ///< key to be set to optionally store PRD to track association map

    
    double m_noiseCut;                        //!< All tracks with a TRT Noise fraction larger than this variable will be thrown away
    int  m_minTRTHits;                        //!< All tracks with less Hits (after the track fit) will be thrown away
    
    const AtlasDetectorID*                       m_idHelper; //<! Detector ID helper
    
    const TRT_ID* m_trtid ;
    

    SG::ReadHandleKey<PRD_MultiTruthCollection> m_multiTruthCollectionTRTName{this,"PRDTruthCollectionTRT","PRD_MultiTruthTRT","RHK to retrieve TRT turth info"}; //!< Name of the TRT MultiTruthCollection 

    int m_nTracksReal;                        //!< Counter for real reconstructed Tracks
    int m_nTracksFake;                        //!< Counter for fake reconstructed Track
    
    double m_noiseratio;                      //!< average percentage of noise in real tracks
    
    mutable std::atomic<int> m_events;        //!< Event counter
    std::map<int,int> m_MapReal;              //!< Map of hits and real tracks
    std::map<int,int> m_MapFake;              //!< Map of hits and fake tracks
    
    bool m_materialEffects;                   //!< Flag to switch on Material Effects in the Fitter
    bool m_outlierRemoval;                    //!< Flag to switch on the outlier removal in the track fitter
    

    bool m_combineSegments;                   //!< Try to combine segments from Barrel and Endcap

    SG::ReadHandleKey<Trk::SegmentCollection> m_barrelSegments{this,"BarrelSegments","TRTBarrelSegments","RHK to retrieve barrel track segments"}; //!< Name of Barrel segment collection
    SG::ReadHandleKey<Trk::SegmentCollection> m_endcapSegments{this,"EndcapSegments","TRTEndcapSegments","RHK to retrieve endcap track segments"}; //!< Name of Endcap segment collection

    SG::WriteHandleKey<TrackCollection> m_BECCollectionName{this,"BarrelEndcapTracks","TRT_Barrel_EC","WHK to write tracks"};  //!< Name of the combined (TRT Barrel+EC) TrackCollection to write out

    mutable std::atomic<int> m_n_combined_fit;

    

  };
}

#endif 
