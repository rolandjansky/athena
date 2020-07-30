/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMPLEAMBIGUITYPROCESSORTOOL_H
#define SIMPLEAMBIGUITYPROCESSORTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"

#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"

//need to include the following, since its a typedef and can't be forward declared.
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackSeedMap.h"

#include <set>
#include <vector>
#include <mutex>
#include <array>
#include "AmbiCounter.icc"

#include "TrackPtr.h"
class AtlasDetectorID;
class PixelID;

namespace Trk {

  class ITrackScoringTool;
  class IPRD_AssociationTool;
  class ITruthToTrack;

  class SimpleAmbiguityProcessorTool : public AthAlgTool, virtual public ITrackAmbiguityProcessorTool 
    {
    public:
      enum class ECounter {kNcandidates, kNcandScoreZero, kNcandDouble,
                  kNscoreOk,kNscoreZeroBremRefit,kNscoreZeroBremRefitFailed,kNscoreZeroBremRefitScoreZero,kNscoreZero,
                  kNaccepted,kNsubTrack,kNnoSubTrack,kNacceptedBrem,
                  kNbremFits,kNfits,kNrecoveryBremFits,kNgoodFits,kNfailedFits, kNCounter};
      using Counter = AmbiCounter<ECounter>;
      // public types
      typedef std::multimap< TrackScore, TrackPtr > TrackScoreMap;
    
      typedef std::set<const PrepRawData*> PrdSignature;
      typedef std::set<PrdSignature> PrdSignatureSet;

      // default methods
      SimpleAmbiguityProcessorTool(const std::string&,const std::string&,const IInterface*);
      virtual ~SimpleAmbiguityProcessorTool ();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize  () override;

      /**Returns a processed TrackCollection from the passed 'tracks'
      @param tracks collection of tracks which will have ambiguities resolved. Will not be 
      modified.
      The tracks will be refitted if no fitQuality is given at input.
      @param prdToTrackMap on optional prd-to-track map being filled by the processor.
      @return new collections of tracks, with ambiguities resolved. Ownership is passed on 
      (i.e. client handles deletion).

      If no prd-to-track map is given the processor will create
      one internally (exported to storegate).*/
      virtual TrackCollection*  process(const TrackCollection*, Trk::PRDtoTrackMap *prdToTrackMap) const override;
      virtual TrackCollection*  process(const TracksScores* scoredTracks) const override;

      /** statistics output to be called by algorithm during finalize. */
      virtual void statistics() override;
    private:

      TrackCollection*  processVector(const TrackCollection &tracks, Trk::PRDtoTrackMap *prdToTrackMap) const;

      /**Add passed TrackCollection, and Trk::PrepRawData from tracks to caches
      @param tracks the TrackCollection is looped over, 
      and each Trk::Track is added to the various caches. 
      The Trk::PrepRawData from each Trk::Track are added to the IPRD_AssociationTool*/
      //void addNewTracks(const std::vector<const Track*> &tracks,
      void addNewTracks(const TrackCollection &tracks,
                        TrackScoreMap& trackScoreTrackMap,
                        Trk::PRDtoTrackMap &prdToTrackMap,
                        Counter &stat) const;

      void addTrack(Track* track, const bool fitted,
                    TrackScoreMap &trackScoreMap,
                    Trk::PRDtoTrackMap &prdToTrackMap,
                    std::vector<std::unique_ptr<const Trk::Track> >& cleanupTracks,
                    Counter &stat) const;

      TrackCollection *solveTracks(TrackScoreMap &trackScoreTrackMap,
                                   Trk::PRDtoTrackMap &prdToTrackMap,
                                   std::vector<std::unique_ptr<const Trk::Track> > &cleanupTracks,
                                   Counter &stat) const;

      /** add subtrack to map */
      void addSubTrack( const std::vector<const TrackStateOnSurface*>& tsos) const;

      /** refit track */
      void refitTrack( const Trk::Track* track,
                       TrackScoreMap &trackScoreMap,
                       Trk::PRDtoTrackMap &prdToTrackMap,
                       std::vector<std::unique_ptr<const Trk::Track> >& cleanupTracks,
                       Counter &stat) const;

      /** refit PRDs */
      Track* refitPrds( const Track* track, Trk::PRDtoTrackMap &prdToTrackMap,
                        Counter &stat) const;

      /** refit ROTs corresponding to PRDs */
      Track* refitRots( const Track* track,
                        Counter &stat) const;

      /** print out tracks and their scores for debugging*/
      void dumpTracks(const TrackCollection& tracks) const;

      /** dump the accumulated statistics */
      void dumpStat(MsgStream &out) const;

      // private data members

      /** brem recovery mode with brem fit ? */
      bool  m_tryBremFit;
      bool  m_caloSeededBrem;
      float m_pTminBrem;

      /** by default drop double tracks before refit*/
      bool m_dropDouble;

      /** by default tracks at input get refitted */
      bool m_forceRefit;

      /** by default refit tracks from PRD */
      bool m_refitPrds;

      /** suppress Hole Search */ 
      bool m_suppressHoleSearch;

      /** suppress Track Fit */ 
      bool m_suppressTrackFit;

      /** control material effects (0=non-interacting, 1=pion, 2=electron, 3=muon, 4=pion) read in as an integer 
      read in as an integer and convert to particle hypothesis */
      int m_matEffects;
      Trk::ParticleHypothesis m_particleHypothesis;
   
      /**Scoring tool
      This tool is used to 'score' the tracks, i.e. to quantify what a good track is.
      @todo The actual tool that is used should be configured through job options*/
      ToolHandle<ITrackScoringTool> m_scoringTool;

      /** refitting tool - used to refit tracks once shared hits are removed. 
      Refitting tool used is configured via jobOptions.*/
      ToolHandle<ITrackFitter> m_fitterTool;

      ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoTool
         {this, "AssociationTool", "Trk::PRDtoTrackMapTool" };

      ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool
        {this, "TrackSummaryTool", "InDetTrackSummaryToolNoHoleSearch"};

      /** selection tool - here the decision which hits remain on a track and
       which are removed are made
      */
      ToolHandle<IAmbiTrackSelectionTool> m_selectionTool;

      /** monitoring statistics */
      //enum RegionIndex {iBarrel , iTransi , iEndcap , iDBM = 4, nRegions=4};
      std::vector<float> m_etabounds;           //!< Four eta intervals for internal monitoring
      mutable std::mutex m_statMutex;
      mutable Counter m_stat ATLAS_THREAD_SAFE;

    };
} //end ns

#endif // TrackAmbiguityProcessorTool_H

