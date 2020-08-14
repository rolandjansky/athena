/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "../src/T2TrackManager.h"
#include "../src/T2Track.h"
#include "../src/T2BeamSpot.h"
#include "../src/T2SplitVertex.h"
//Athena tools
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
//Tracking
#include "TrkTrack/TrackCollection.h"
#include "TrigInDetToolInterfaces/ITrigPrimaryVertexFitter.h"
//Data handles
#include "StoreGate/ReadCondHandleKey.h"
//Beam data
#include "StoreGate/StoreGateSvc.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
//To get TEVec and HLT::Error messaging
#include "TrigInterfaces/Algo.h"
//Monitoring tool
#include "AthenaMonitoringKernel/Monitored.h"

class TrigVertexCollection;

namespace HLT {
   class TriggerElement;
}
namespace PESA {
   class T2TrackClusterer;
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

         T2VertexBeamSpotTool( const std::string& type, const std::string& name, const IInterface* parent );
         ~T2VertexBeamSpotTool(){ }

         virtual StatusCode initialize() final;


         bool isGoodTrack     ( const T2Track & track  ) const;
         bool isGoodVertex    ( const T2Vertex& vertex ) const;
         bool isGoodVertexBCID( const T2Vertex& vertex ) const;

         //Data dependencies
         SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{ this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

         /** Primary Vertex Fitter Tool */
         ToolHandle<ITrigPrimaryVertexFitter> m_primaryVertexFitterTool;



         /**  Return true if there is at least one high pT track in the event
          * the total number of these high pT tracks is checked in the selectTracks function and the threshold can be set via TrackSeedPt (by default 1 GeV) */
         inline bool isHighPTTrack( unsigned &nTracks ) const{return ( nTracks > 0 );}


         /**  Return true if the number of tracks which passed the selection in goodTrack function is lower than a given threshold
          * The threshold can be set via TotalNTrackMin parameter  */
         inline bool notEnoughTracks(unsigned &nTracks) const{  return ( nTracks < m_totalNTrkMin ) ;}


         void selectTracks( const TrackCollection* trackCollection,
               ConstDataVector<TrackCollection>& mySelectedTrackCollection, std::vector<unsigned> &trackCounter ) const;

         unsigned int reconstructVertices( ConstDataVector<TrackCollection>& mySelectedTrackCollection,
                      TrigVertexCollection& myVertexCollection,
                      DataVector< TrigVertexCollection >& mySplitVertexCollections, const EventContext& ) const;

         void reconstructSplitVertices( ConstDataVector<TrackCollection>& mySelectedTrackCollection,
               DataVector< TrigVertexCollection >& mySplitVertexCollections, T2TrackClusterer& trackClusterer, const EventContext& ) const;

         bool m_passNpvTrigCuts;

         /* Track manager */
         T2TrackManager m_trackManager;

         /* Number of Z blocks */
         double       m_trackClusDZ;
         bool         m_weightSeed;
         bool         m_splitWholeCluster;
         bool         m_reclusterSplit;
         double       m_trackSeedPt;

         /* Track selection criteria */
         unsigned  m_totalNTrkMin;
         int          m_TrackAlgoId;
         double       m_minTrackPt;
         double       m_maxTrackEta;
         double       m_maxTrackZ0;
         double       m_maxTrackD0;
         double       m_maxTrackZ0err;
         double       m_maxTrackD0err;
         double       m_minTrackNDF;
         double       m_minTrackQual;
         double       m_maxTrackQual;
         double       m_minTrackChi2Prob;
         int          m_minSiHits;
         int          m_minPIXHits;                                           
         int          m_minSCTHits;
         int          m_minTRTHits;

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

         /* single interaction trigger requirements */
         unsigned int m_minNpvTrigger;
         unsigned int m_maxNpvTrigger;

         /* Monitor track parameters */
         void monitor_tracks(const std::string& prefix, const std::string& suffix, const std::vector<T2Track>& tracks ) const;

         /* Monitor cluster parameters */
         void monitor_cluster( const T2TrackClusterer& clusterer  ) const;

         /* Monitor  parameters of tracks inside the cluster */
         void monitor_cluster_tracks(T2TrackClusterer& clusterer, const Trk::Track & track  ) const;

         /* Monitor  vertex parameters  */
         void monitor_vertex(const std::string& prefix, const std::string& suffix, const T2Vertex &vertex ) const;

         std::string m_vertexCollName;

      private:
         ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};

         


   };

} // end namespace

#endif
