/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
///============================================================
/// Trigger/TrigAlgorithms/TrigT2BeamSpot/T2VertexBeamSpot
///
/// Online beam spot measurement and monitoring using
/// L2 recontructed primary vertices. Implementation class
/// lifted from T2VertexBeamSpot.h
///
/// Authors : David W. Miller, Rainer Bartoldus,
///           Su Dong
///============================================================
/**********************************************************************************
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2VertexBeamSpot
 *
 * @brief  PESA algorithm that measures and monitors beam spot position using vertices
 *
 * @author David W. Miller    <David.W.Miller@cern.ch>     - SLAC, Stanford University
 **********************************************************************************/
#ifndef TRIGT2BEAMSPOT_T2VERTEXBEAMSPOTTOOL_H
#define TRIGT2BEAMSPOT_T2VERTEXBEAMSPOTTOOL_H
#include <string>
#include <vector>
#include <memory>
// Local tools
#include "T2BSTrackFilterTool.h"
#include "T2BeamSpot.h"
#include "T2SplitVertex.h"
#include "T2Track.h"
#include "T2TrackClusterer.h"
//Athena tools
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
//Tracking
#include "TrkTrack/TrackCollection.h"
#include "TrigInDetToolInterfaces/ITrigPrimaryVertexFitter.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
//Data handles
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
//Beam data
#include "BeamSpotConditionsData/BeamSpotData.h"
//Monitoring tool
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

namespace InDet {
   class BeamSpotData;
}

namespace PESA {
   class T2SplitVertex;
   /**
    *   This class uses primary vertex reconstruction to measure
    *   and monitor the LHC beam as seen by the ATLAS detector. 
    *   Fast, online vertexing is implemented using Dmitry 
    *   Emeliyanov's (D.Emeliyanov@rl.ac.uk) TrigPrimaryVertexFitter.
    *
    *   @author Ignacio Aracena     <ignacio.aracena@cern.ch>
    *   @author Rainer Bartoldus    <bartoldu@slac.stanford.edu>
    *   @author Su Dong             <sudong@slac.stanford.edu>
    *   @author David W. Miller     <David.W.Miller@cern.ch>
    *   
    */
   class T2VertexBeamSpotTool :  public AthAlgTool {
      public:

         using TrackVector = std::vector<const Trk::Track*>;

         T2VertexBeamSpotTool( const std::string& type, const std::string& name, const IInterface* parent );

         virtual StatusCode initialize() override final;

         /**
          * Update beam spot data with new track information.
          *
          * Parameter provides a lists of tracks.
          */
         void updateBS(const TrackCollection& tracks, const EventContext& ctx) const;

      private:

         /**  Return count of high pT track in the track list,
          * the total number of these high pT tracks is checked in the selectTracks function and the threshold can be set via TrackSeedPt (by default 1 GeV) */
         unsigned numHighPTTrack(const TrackVector& tracks) const;

         unsigned int reconstructVertices(TrackVector& tracks,
                      TrigVertexCollection& myVertexCollection,
                      DataVector< TrigVertexCollection >& mySplitVertexCollections, const EventContext& ) const;

         bool isGoodVertex    ( const T2Vertex& vertex ) const;
         bool isGoodVertexBCID( const T2Vertex& vertex ) const;

         void reconstructSplitVertices(TrackVector& tracks,
               DataVector< TrigVertexCollection >& mySplitVertexCollections,
               T2TrackClusterer& trackClusterer,
               const InDet::BeamSpotData* indetBeamSpot,
               const EventContext& ) const;

         /* Monitor cluster parameters */
         void monitor_cluster( const T2TrackClusterer& clusterer  ) const;

         /* Monitor  parameters of tracks inside the cluster */
         void monitor_cluster_tracks(T2TrackClusterer& clusterer, const Trk::Track & track  ) const;

         /* Monitor  vertex parameters  */
         void monitor_vertex(const std::string& prefix, const std::string& suffix, const T2Vertex &vertex, int bcid=-1 ) const;

         /* Monitor split vertex parameters */
         void monitor_split_vertex(const std::string& prefix, const std::string& suffix, const T2SplitVertex& vertex) const;


         /* Number of Z blocks */
         double       m_trackClusDZ;
         bool         m_weightSeed;
         bool         m_splitWholeCluster;
         bool         m_reclusterSplit;
         double       m_trackSeedPt;
         std::string  m_clusterPerigee = "original"; // one of "original", "beamspot", "beamline"
         T2TrackClusterer::TrackPerigee m_clusterTrackPerigee = T2TrackClusterer::perigee_original;

         /* Track selection criteria */
         unsigned  m_totalNTrkMin;

         /* Vertex selection criteria */
         unsigned int m_vtxNTrkMin;
         unsigned int m_vtxNTrkMax;
         double       m_vtxQualMin;
         double       m_vtxQualMax;
         double       m_vtxChi2ProbMin;
         double       m_vtxMassMin;
         double       m_vtxSumPtMin;
         double       m_vtxXposMax;
         double       m_vtxXerrMax;
         double       m_vtxYposMax;
         double       m_vtxYerrMax;
         double       m_vtxZposMax;
         double       m_vtxZerrMax;

         /// Additional vertex selection criteria for BCID measurements
         unsigned int m_vtxBCIDNTrkMin;

         unsigned m_nSplitVertices;

         bool m_filterBS;         // if true then filter tracks against local beamspot estimate

         //Data dependencies
         SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};

         //The same as in Run2 (m_vertexCollName)
         SG::WriteHandleKey<TrigVertexCollection> m_outputVertexCollectionKey{this, "OutputVertexCollection", "myVertices", "SG key for output vertex collection"};
         //TODO: to be added SG::WriteHandleKeyArray<TrigVertexCollection> m_outputSplitVertexCollectionKey;   /*Input list of track collection names which should be used for the algorithms*/

         /** Primary Vertex Fitter Tool */
         ToolHandle<ITrigPrimaryVertexFitter> m_primaryVertexFitterTool;

         ToolHandle<T2BSTrackFilterTool> m_trackFilterTool{this, "TrackFilter", "PESA::T2BSTrackFilterTool/T2BSTrackFilterTool" };
         ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};

   };

} // end namespace

#endif
