/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// InDetRecStatisticsAlg.h
// Authors: Sven Vahsen, Lauren Tompkins
///////////////////////////////////////////////////////////////////

#ifndef INDETRECSTATISTICS_TrackStatistics_H
#define INDETRECSTATISTICS_TrackStatistics_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "HepPDT/ParticleDataTable.hh"
#include "InDetRecStatistics/TrackStatHelper.h"
#include "Identifier/Identifier.h"
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkToolInterfaces/IUpdator.h"

#include <vector>
#include <atomic>

// forward declarations

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;
class IdDictManager;
class TrackTruthCollection;
class McEventCollection;
class GenParticle;

namespace Trk {
  class ITrackSummaryTool;
  class TrackSummary;
}

namespace InDet {

    class InDetRecStatisticsAlg:public AthReentrantAlgorithm {


 /**
  @class InDetRecStatisticsAlg
  This class is used to compute statistics to compare multiple track finding algorithms.  It prints a statistics table which shows efficiencies and fake rates for input track collections.
  @author lauren.tompkin@cern.ch, sven.vahsen@cern.ch
  */

     public:
      /**Default Constructor*/
      InDetRecStatisticsAlg (const std::string& name, ISvcLocator* pSvcLocator);
      /** Initialization of services, track collections, creates TrackStatHelper for each Track Collection*/
      StatusCode    initialize();
      /**Calculation of statistics*/
      StatusCode    execute(const EventContext &ctx) const;
      /**Statistics table printed, collection cleared*/
      StatusCode    finalize();
      /**Get various services such as StoreGate, dictionaries, detector managers etc.*/
      StatusCode    getServices();
      /** Clear statistics counters, called before each track collection is processed*/
      StatusCode    resetStatistics();
      /** Print tracking statistics calculated with TrackStatHelper */
      void          printStatistics();
      /** Print track statistics for all and low proability tracks */
      void          printTrackSummary   (MsgStream &out, enum eta_region );
      


     private:

      template <typename T_Int=int>
      struct Counter;
      using CounterLocal = Counter<int>;

      /** Select charged,stable particles which pass pt and eta cuts for analysis*/
      /** Classify truth tracks as primary, secondary or truncated */
      void          selectGenSignal     (const McEventCollection*,
					 std::vector <std::pair<HepMC::ConstGenParticlePtr,int> > &,
					 unsigned int,
                                         unsigned int,
                                         CounterLocal &counter) const;

      /**Select for analysis reconstructed tracks passing Pt and eta cuts*/
      void          selectRecSignal     (const TrackCollection*,
					 std::vector <const Trk::Track *> & ,
					 std::vector <const Trk::Track *> &,
                                         CounterLocal &counter) const;



      std::vector <class TrackStatHelper *> m_SignalCounters;//!< Vector of TrackStatHelper objects, one for each track collection
      const HepPDT::ParticleDataTable*      m_particleDataTable; //!< Atlas particle ID table
      const AtlasDetectorID*                m_idHelper;          //!< Used to find out the sub-det from PRD->identify().
      const PixelID*                        m_pixelID; //!< get pixel layer from hit ID
      const SCT_ID*                         m_sctID; //!< get sct layer from hit ID
      const TRT_ID*                         m_trtID; //!< get trt layer from hit ID
      // added to check TRT existence (SLHC geo check)
      const IdDictManager * m_idDictMgr;
      ToolHandle<Trk::ITruthToTrack>              m_truthToTrack; //!< tool to create track parameters from a gen particle
      ToolHandle<Trk::IExtendedTrackSummaryTool>  m_trkSummaryTool;  //!< tool to get track summary information from track 
      ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoTool
         {this, "AssociationTool", "InDet::InDetPRDtoTrackMapToolGangedPixels" };

      ToolHandle<Trk::IUpdator>                   m_updatorHandle;            //!< Tool handle of updator for unbiased states
      Trk::IUpdator*                              m_updator;                  //!< updator for unbiased states
      ToolHandle<Trk::IResidualPullCalculator>    m_residualPullCalculator;   //!< The residual and pull calculator tool handle

      // steering parameters of Algorithm
      SG::ReadHandleKey<McEventCollection>  m_McTrackCollection_key;
      ToolHandle<Trk::ITrackSelectorTool> m_trackSelectorTool;
     
      bool          m_doSharedHits;//!< 
      bool          m_UseTrackSummary;//!< Flag to print detailed statistics for each track collection
      bool          m_printSecondary; //!< Flag to print hit information for secondary tracks
      float         m_minPt;//!< Minimum Pt cut for tracks used by the algorithm
      float         m_maxEta; //!< Maximum Eta cut for tracks used by the algorithm
      float         m_maxEtaBarrel; //!< define max eta of barrel region
      float         m_maxEtaTransition; //!< define max eta of transition region
      float         m_maxEtaEndcap; //!< define max eta of eta region
      float         m_fakeTrackCut; //!< Maximum probability for which a track will be considered a fake.
      float         m_fakeTrackCut2;//!< Second definition of maximum probability for which a track will be considered a fake
      float         m_matchTrackCut;//!< Minimum number of hits from a truth track to be considered a matched reco track
      //      float         m_maxRIndet;
      float         m_maxRStartPrimary;//!< Maximum R of start vertex to be considered primary.
      float         m_maxRStartSecondary;//!< Maximum R of start vertex to be considered secondary.
      float         m_maxZStartPrimary;//!< Maximum Z of start vertex to be considered primary.
      float         m_maxZStartSecondary;//!< Maximum Z of start vertex to be considered secondary.
      float         m_minREndPrimary;//!< If track has end vertex, this is min R of end vertex to be considered primary.
      float         m_minREndSecondary;//!< If track has end vertex, this is min R of end vertex to be considered secondary.
      float         m_minZEndPrimary;//!< If track has end vertex, this is min Z of end vertex to be considered primary.
      float         m_minZEndSecondary;//!< If track has end vertex, this is min Z of end vertex to be considered secondary.
      bool          m_useTrackSelection; //!< Use track selector tool
      bool          m_doTruth;  //!< Use truth information
      float m_minEtaDBM;
      float m_maxEtaDBM;
      SG::ReadHandleKeyArray<TrackCollection>        m_RecTrackCollection_keys;
      SG::ReadHandleKeyArray<TrackTruthCollection>   m_TrackTruthCollection_keys;

      mutable std::atomic<bool>      m_pullWarning    ATLAS_THREAD_SAFE; //!< warn only once, if pull cannot be calculated
      mutable std::atomic<bool>      m_UpdatorWarning ATLAS_THREAD_SAFE; //!< warn only once, if unbiased track states can not be calculated
      mutable std::atomic<int>       m_isUnbiased     ATLAS_THREAD_SAFE;  //!< if can get unbiased residuals

      mutable std::atomic<long> m_events_processed    ATLAS_THREAD_SAFE;//!< number of events processed
         // statistics counters
      enum ECounter {
         kN_rec_tracks_without_perigee, //!< number of tracks w/o perigee
         kN_unknown_hits,               //!< number of hits without track
         kN_rec_tracks_processed,       //!< number of reconstructed tracks processed
         kN_gen_tracks_processed,       //!< number of generated tracks processed
         kN_spacepoints_processed,      //!< number of space points processed
         kNCounter
      };

      template <typename T_Int>
      struct Counter {
         Counter() { reset(); }
         void reset() {
            for (unsigned int i=0; i<kNCounter; ++i) { m_counter[i]=0; }
         }
         template <typename T_IntB>
         Counter &operator+=(const Counter<T_IntB> &b) {
            for (unsigned int i=0; i<kNCounter; ++i) { m_counter[i] += b.m_counter[i]; }
            return *this;
         }
         T_Int m_counter[kNCounter];
      };

      mutable Counter<std::atomic<long> > m_counter ATLAS_THREAD_SAFE;
      /** Calculate pull from residual, track and hit error.*/
      float calculatePull(const float, const float, const float);   //vv
      /** Get Unbiased Track Parameters */
      const Trk::TrackParameters *  getUnbiasedTrackParameters(const Trk::TrackParameters* , const Trk::MeasurementBase* );
      Identifier  getIdentifier(const Trk::MeasurementBase* measurement );
    };

} // close of namespace

#endif // INDETRECSTATISTICS_TrackStatistics_H
