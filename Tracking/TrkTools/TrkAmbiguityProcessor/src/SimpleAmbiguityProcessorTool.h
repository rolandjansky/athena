/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMPLEAMBIGUITYPROCESSORTOOL_H
#define SIMPLEAMBIGUITYPROCESSORTOOL_H


#include "AmbiguityProcessorBase.h"
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



class AtlasDetectorID;
class PixelID;

namespace Trk {

  class ITrackScoringTool;
  class IPRD_AssociationTool;
  class ITruthToTrack;

  class SimpleAmbiguityProcessorTool : public AmbiguityProcessorBase {
    public:
      // public types
      typedef std::multimap< TrackScore, TrackPtr > TrackScoreMap;
      typedef std::vector<const PrepRawData*> PrdSignature;
      typedef std::set<PrdSignature> PrdSignatureSet;

      // default methods
      SimpleAmbiguityProcessorTool(const std::string&,const std::string&,const IInterface*);
      virtual ~SimpleAmbiguityProcessorTool ();
      virtual StatusCode initialize() override final;
      virtual StatusCode finalize  () override final;

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

      TrackCollection*  
      processVector(const TrackCollection &tracks, Trk::PRDtoTrackMap *prdToTrackMap) const;

      /**Add passed TrackCollection, and Trk::PrepRawData from tracks to caches
      @param tracks the TrackCollection is looped over, 
      and each Trk::Track is added to the various caches. 
      The Trk::PrepRawData from each Trk::Track are added to the IPRD_AssociationTool*/
      void 
      addNewTracks(const TrackCollection &tracks,
                  TrackScoreMap& trackScoreTrackMap,
                  Trk::PRDtoTrackMap &prdToTrackMap) const;

      TrackCollection *
      solveTracks(TrackScoreMap &trackScoreTrackMap,
                 Trk::PRDtoTrackMap &prdToTrackMap,
                 std::vector<std::unique_ptr<const Trk::Track> > &trackDustbin,
                 Counter &stat) const;

      /** add subtrack to map */
      void 
      addSubTrack( const std::vector<const TrackStateOnSurface*>& tsos) const;

      /** do a refit assuming electron hypothesis **/
      virtual std::unique_ptr<Trk::Track>
      doBremRefit(const Trk::Track & track) const override final;

      /** refit PRDs */
      Track* 
      refitPrds( const Track* track, Trk::PRDtoTrackMap &prdToTrackMap, Counter &stat) const override final;

      /** print out tracks and their scores for debugging*/
      void 
      dumpTracks(const TrackCollection& tracks) const;

      /** dump the accumulated statistics */
      void 
      dumpStat(MsgStream &out) const;
      
      std::unique_ptr<Trk::Track>
      fit(const Track &track, bool flag, Trk::ParticleHypothesis hypo) const override final;

      // private data members
      /** by default drop double tracks before refit*/
      bool m_dropDouble;
  
      /** refitting tool - used to refit tracks once shared hits are removed. 
      Refitting tool used is configured via jobOptions.*/
      ToolHandle<ITrackFitter> m_fitterTool;
      /** association tool **/
      ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoTool{this, "AssociationTool", "Trk::PRDtoTrackMapTool" };
      /** selection tool - here the decision which hits remain on a track and which are removed are made */
      ToolHandle<IAmbiTrackSelectionTool> m_selectionTool;

    };
} //end ns

#endif // TrackAmbiguityProcessorTool_H

