/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///============================================================
///
/// $Id: T2VertexBeamSpotImpl.h 760210 2016-07-07 09:33:44Z hartj $
///
/// Trigger/TrigAlgorithms/TrigT2BeamSpot/T2VertexBeamSpot
///
/// Online beam spot measurement and monitoring using
/// L2 recontructed primary vertices. Implementation class
/// lifted from T2VertexBeamSpot.h
///
/// Authors : David W. Miller, Rainer Bartoldus,
///           Su Dong
///
/// Date : 9 Dec, 2010
///
///============================================================

/**********************************************************************************
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2VertexBeamSpot
 *
 * @brief  PESA algorithm that measures and monitors beam spot position using vertices
 *
 * @author David W. Miller    <David.W.Miller@cern.ch>     - SLAC, Stanford University
 *
 * File and Version Information:
 * $Id: T2VertexBeamSpotImpl.h 760210 2016-07-07 09:33:44Z hartj $
 **********************************************************************************/

#ifndef TRIGT2BEAMSPOT_T2VERTEXBEAMSPOTIMPL_H
#define TRIGT2BEAMSPOT_T2VERTEXBEAMSPOTIMPL_H

/// Local tools

#include "T2TrackManager.h"
#include "T2Track.h"
#include "T2BeamSpot.h"
#include "T2Vertex.h"
#include "T2SplitVertex.h"

#include "TrigT2BeamSpot/T2VertexBeamSpot.h"  // Need this only for forwarding methods msg() and msgLvl()

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "DataModel/DataVector.h"

#include "TrkTrack/TrackCollection.h"

#include <string>
#include <vector>
#include <memory>

/// General athena stuff
class MsgStream;

/// Trigger EDM
class TrigInDetTrack;
class TrigInDetTrackCollection;

// Trigger Steering
class AlgoConfig;

/// Vertexing Tools
class ITrigPrimaryVertexFitter;
class TrigVertexCollection;

/// Timer
class TrigTimer;

/// Beam spot service
class IBeamCondSvc;

/// Event statistics
enum Statistics
  {
    allInput      = 0,
    hasTE         = 1,
    hasROI        = 2,
    hasTracks     = 3,
    hasSeedTrack  = 4,
    enoughTracks  = 5,
    hasCluster    = 6,
    hasVertex     = 7,
    hasGoodVertex = 8,
    //
    numStatistics /// Leave this entry at the end
  // When this changes, adjust T2VertexBeamSpotMonitoring.py: EventStatistics histogram
  };

/// Timers
enum Timers
  {
  total          =  0,
  allTE          =  1,
  allROI         =  2,
  allTrack       =  3,
  allVertex      =  4,
  allVertexSplit =  5,
  allOutput      =  6,
  trackSort      =  7,
  trackSortSplit =  8,
  vertexFit      =  9,
  vertexFitSplit = 10,
  zCluster       = 11,
  zClusterSplit  = 12,
  //
  numTimers /// Leave this entry at the end
  // When this changes, adjust T2VertexBeamSpotConfig.py: time.TimerHistLimits
  };

namespace HLT {
  class TriggerElement;
}

namespace PESA {

  class T2VertexBeamSpot;
  class T2TrackClusterer;

  /**
   *   @class T2VertexBeamSpotImpl
   *   This class uses primary vertex reconstruction to measure
   *   and monitor the LHC beam as seen by the ATLAS detector. 
   *   Fast, online vertexing is implemented using Dmitry 
   *   Emeliyanov's (D.Emeliyanov@rl.ac.uk) TrigPrimaryVertexFitter.
   *   
   *   As an AllTEAlgo, this must appear first in the sequence.
   *
   *   @author Ignacio Aracena     <ignacio.aracena@cern.ch>
   *   @author Rainer Bartoldus    <bartoldu@slac.stanford.edu>
   *   @author Su Dong             <sudong@slac.stanford.edu>
   *   @author David W. Miller     <David.W.Miller@cern.ch>
   *   
   */

  class T2VertexBeamSpotImpl
  {
  public:

    T2VertexBeamSpotImpl( T2VertexBeamSpot* self )
      : m_timer( numTimers, 0 )
      , m_beamCondSvc( 0 )
      , m_primaryVertexFitter( 0 )
      , m_self( self )
    {}

    ~T2VertexBeamSpotImpl() {}

    /// Methods

    void processTEs( const std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
                     TrigInDetTrackCollection& mySelectedTrackCollection );

    void processTEs( const std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
                     TrackCollection& mySelectedTrackCollection );

    void processROIs( const HLT::TEVec& myTEVec,
                      TrigInDetTrackCollection& mySelectedTrackCollection );

    void processROIs( const HLT::TEVec& myTEVec,
                      TrackCollection& mySelectedTrackCollection );

    void selectTracks( const std::vector< const TrigInDetTrackCollection* >& vectorOfTrackCollections,
                       TrigInDetTrackCollection& mySelectedTrackCollection );

    void selectTracks( const std::vector< const TrackCollection* >& vectorOfTrackCollections,
                       TrackCollection& mySelectedTrackCollection );

    void reconstructVertices( TrigInDetTrackCollection& mySelectedTrackCollection,
                              TrigVertexCollection& myVertexCollection,
                              DataVector< TrigVertexCollection >& mySplitVertexCollections );

    void reconstructVertices( TrackCollection& mySelectedTrackCollection,
                              TrigVertexCollection& myVertexCollection,
                              DataVector< TrigVertexCollection >& mySplitVertexCollections );

    void reconstructSplitVertices( TrigInDetTrackCollection& mySelectedTrackCollection,
                                   DataVector< TrigVertexCollection >& mySplitVertexCollections );

    void reconstructSplitVertices( TrackCollection& mySelectedTrackCollection,
                                   DataVector< TrigVertexCollection >& mySplitVertexCollections );

    void createOutputTEs( TrigVertexCollection& myVertexCollection,
                          DataVector< TrigVertexCollection >& mySplitVertexCollections,
                          unsigned int type_out, const std::vector<std::vector<HLT::TriggerElement*> >& tes_in  );

    bool isGoodTrack     ( const T2Track & track  ) const;
    bool isGoodVertex    ( const T2Vertex& vertex ) const;
    bool isGoodVertexBCID( const T2Vertex& vertex ) const;

    bool eventStage( Statistics stage );

    void resetMonitoredVariables();

    /// Public data members

    /* Timers */
    std::vector<TrigTimer*> m_timer;
    
    /* Beam conditions service */
    IBeamCondSvc* m_beamCondSvc; //!< pointer to the beam condition service
    std::string   m_beamCondSvcName;
    
    /* Primary Vertex fitter tool */
    ITrigPrimaryVertexFitter* m_primaryVertexFitter;

    /* Track manager */
    T2TrackManager m_trackManager;

    /* Track clusterer */
    std::auto_ptr< T2TrackClusterer > m_trackClusterer;
    
    /* Choose to save all track params */
    bool         m_saveAllVertices;
    
    /* Number of Z blocks */
    unsigned int m_nZfitBins;
    double       m_ZUpperLimit;
    double       m_ZLowerLimit;
    double       m_ZBinSize;
    double       m_trackClusDZ;
    bool         m_weightSeed;
    bool         m_splitWholeCluster;
    bool         m_reclusterSplit;
    double       m_trackSeedPt;
    
    /* Track selection criteria */
    unsigned int m_totalNTrkMin;
    int          m_TrackAlgoId;
    double       m_minTrackPt;
    double       m_maxTrackPt;
    double       m_minTrackEta;
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

    /// Counters
    unsigned int m_TotalEvents = 0;
    
    bool         m_doVertexFit;
    bool         m_AcceptAll;
  
    /* Vertex Collection */
    bool m_attachVertices;
    std::string m_vertexCollName;
    
    bool     m_attachSplitVertices;
    unsigned m_nSplitVertices;
    
    /* ------------------------- */
    /* activate trigger elements */
    /* ------------------------- */
    bool m_activateTE;
    bool m_activateAllTE;
    bool m_activateSI;
    
    /* --------------------------------------- */
    /* single interaction trigger requirements */
    /* --------------------------------------- */
    unsigned int m_minNpvTrigger;
    unsigned int m_maxNpvTrigger;
    
    /* --------------------- */
    /* Monitoring histograms */
    /* --------------------- */
    
     /* monitored run & event variables*/
    unsigned int m_RunID = 0;
    unsigned int m_EventID = 0;
    unsigned int m_LumiBlock = 0;
    unsigned int m_bcid = 0;

    /* monitoried event-level variables */
    unsigned int m_TotalTracks = 0;
    unsigned int m_TotalTracksPass = 0;
    unsigned int m_TotalHiPTTracks = 0;
    unsigned int m_TotalROIs = 0;
    unsigned int m_TotalTEs = 0;  

    // FIXME: these should be encapsulated with the eventStage method
    // and declareStdContainer be extended to take member function pointers
    std::vector<bool>         m_eventStageFlag;
    std::vector<unsigned int> m_eventStage;
    
    std::vector<unsigned int> m_ROIperTE;
    std::vector<unsigned int> m_TracksPerTE;
    std::vector<unsigned int> m_TracksPerTEPass;
    std::vector<unsigned int> m_HiPTTracksPerTE;
    
    std::vector<unsigned int> m_TracksPerROI;
    std::vector<unsigned int> m_TracksPerROIPass;
    std::vector<unsigned int> m_HiPTTracksPerROI;
     
    /* monitored ROI variables*/
    std::vector<double> m_RoIEta;
    std::vector<double> m_RoIPhi; 
    std::vector<double> m_RoIID; 
    
    MonitoredTrack m_trackPass;
    
    /* monitored cluster variables*/
    std::vector<double> m_clusterNTracks;
    std::vector<double> m_clusterNTracksUnused;
    std::vector<double> m_clusterZ;
    std::vector<double> m_clusterDeltaZ0;
    std::vector<double> m_clusterZ0Pull;
    std::vector<double> m_clusterDeltaVertexZ;
    std::vector<double> m_clusterVertexZPull;

    unsigned int m_NClusters;
    unsigned int m_Nvtx;
    unsigned int m_NvtxPass;
    unsigned int m_NvtxPassBCID;

    MonitoredVertex m_vertex;
    MonitoredVertex m_vertexPass;
    MonitoredVertex m_vertexPassBCID;

    /* Split vertex information */
    MonitoredSplitVertex m_splitVertexPass;
 
    /* Bunch-by-bunch information */
    std::vector<unsigned int> m_bunch_vertexNTracks;
    std::vector<double> m_bunch_vertexX;
    std::vector<double> m_bunch_vertexY;
    std::vector<double> m_bunch_vertexZ;
    
  private:
    
    /// Pointer to visible class
    T2VertexBeamSpot* m_self;

    // Forwarding methods

    // From TrigInterfaces/Algo.h
    MsgStream&   msg()    const { return m_self->msg();    }
    unsigned int msgLvl() const { return m_self->msgLvl(); }

    template<class T> HLT::ErrorCode getFeature(const HLT::TriggerElement* te,
                                                const T*& feature,
                                                const std::string& label = "")
      {
        return m_self->getFeature( te, feature, label );
      }
	
    template<class T> HLT::ErrorCode getFeatures(const HLT::TriggerElement* te,
                                                 std::vector<const T*>& feature,
                                                 const std::string& label = "")
      {
        return m_self->getFeatures( te, feature, label );
      }

    HLT::AlgoConfig* config() const { return m_self->config(); }

    // From TrigInterfaces/TECreateAlgo.h
    template<class T> HLT::ErrorCode attachFeature(HLT::TriggerElement* te,
                                                   T* feature,
                                                   const std::string& label = "")
      {
        return m_self->attachFeature( te, feature, label );
      }

  };

} // end namespace

#endif
