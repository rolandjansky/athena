/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKSELECTIONPROCESSORTOOL_H
#define TRACKSELECTIONPROCESSORTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"

#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

//need to include the following, since its a typedef and can't be forward declared.
#include "TrkTrack/TrackCollection.h"

#include "TrackPtr.h"

#include <map>
#include <set>
#include <vector>


namespace Trk {

  class ITrackScoringTool;
  class IPRD_AssociationTool;

  class TrackSelectionProcessorTool 
    : 
  public AthAlgTool,
    virtual public ITrackAmbiguityProcessorTool
    {
    public:

      // public types
      typedef std::multimap< TrackScore, TrackPtr > TrackScoreMap;
    
      typedef std::set<const PrepRawData*> PrdSignature;
      typedef std::set<PrdSignature> PrdSignatureSet;

      // default methods
      TrackSelectionProcessorTool(const std::string&,const std::string&,const IInterface*);
      virtual ~TrackSelectionProcessorTool ();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize  () override;

      /**Returns a processed TrackCollection from the passed 'tracks' WITHOUT copying or refitting the input tracks.
   The pointers of the tracks in the input collection are copied into the output collection which does NOT
   own the pointers. Clients must ensure that the Input Collection is not deleted before the output collection
   else all pointers in the output collection will be invalid!

   @param tracks collection of tracks which will have ambiguities resolved. Will not be
   modified.
         @param prdToTrackMap on optional prd-to-track map being filled by the processor.

   @return new collections of tracks, with ambiguities resolved.
    The returned track collection is a SG::VIEW_ELEMENTS container, meaning that it should NOT be written to disk.
    Ownership of the collection is passed on (i.e. client handles deletion)

         If no prd-to-track map is given the processor might create one internally (for internal
         use only, or exported to storegate).
      */
      virtual TrackCollection*  process(const TrackCollection* tracksCol,Trk::PRDtoTrackMap *prdToTrackMap) const override;

      virtual TrackCollection*  process(const TracksScores* /*trackScoreTrackMap*/) const override { return nullptr; }

      /** statistics output to be called by algorithm during finalize. */
      void statistics() override {}
    private:

      /**Add passed TrackCollection, and Trk::PrepRawData from tracks to caches
   @param tracks the TrackCollection is looped over, 
   and each Trk::Track is added to the various caches. 
   The Trk::PrepRawData from each Trk::Track are added to the IPRD_AssociationTool*/
      void addNewTracks(TrackScoreMap &trackScoreTrackMap, Trk::PRDtoTrackMap &prdToTrackMap, const std::vector<const Track*> &tracks ) const;


      void solveTracks(TrackScoreMap &trackScoreTrackMap, Trk::PRDtoTrackMap &prdToTrackMap, TrackCollection &final_tracks) const;

      /** print out tracks and their scores for debugging*/
      void dumpTracks(const TrackCollection& tracks) const;
  
      // private data members

      /** by default drop double tracks before refit*/
      bool m_dropDouble;
   
      /**Scoring tool
   This tool is used to 'score' the tracks, i.e. to quantify what a good track is.
   @todo The actual tool that is used should be configured through job options*/
      ToolHandle<ITrackScoringTool> m_scoringTool;

      /** selection tool - here the decision which hits remain on a track and
    which are removed are made
      */
      ToolHandle<IAmbiTrackSelectionTool> m_selectionTool;

      ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoTool
         {this, "AssociationTool", "Trk::PRDtoTrackMapTool" };

      /** key for the PRDtoTrackMap to filled by the ambiguity score processor.**/
      SG::ReadHandleKey<Trk::PRDtoTrackMap>  m_assoMapName
         {this,"AssociationMapName",""};  ///< the key given to the newly created association map

      /** option to disable sorting based on track score and 
    use the ordering provided externally*/
      bool m_disableSorting;
    };

} //end ns

#endif // TrackAmbiguityProcessorTool_H

