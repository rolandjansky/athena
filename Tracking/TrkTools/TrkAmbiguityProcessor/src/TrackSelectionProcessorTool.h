/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKSELECTIONPROCESSORTOOL_H
#define TRACKSELECTIONPROCESSORTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"

//need to include the following, since its a typedef and can't be forward declared.
#include "TrkTrack/TrackCollection.h"

#include <map>
#include <set>
#include <vector>
#include <functional>


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
      typedef std::pair< const Track*, const bool > TrackFittedPair;
      typedef std::multimap< TrackScore, TrackFittedPair > TrackScoreMap;
    
      typedef std::set<const PrepRawData*> PrdSignature;
      typedef std::set<PrdSignature> PrdSignatureSet;

      // default methods
      TrackSelectionProcessorTool(const std::string&,const std::string&,const IInterface*);
      virtual ~TrackSelectionProcessorTool ();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      /**Returns a processed TrackCollection from the passed 'tracks' WITHOUT copying or refitting the input tracks.
	 The pointers of the tracks in the input collection are copied into the output collection which does NOT 
	 own the pointers. Clients must ensure that the Input Collection is not deleted before the output collection
	 else all pointers in the output collection will be invalid!
	 
	 @param tracks collection of tracks which will have ambiguities resolved. Will not be 
	 modified.
	 @return new collections of tracks, with ambiguities resolved. 
	  The returned track collection is a SG::VIEW_ELEMENTS container, meaning that it should NOT be written to disk. 
	  Ownership of the collection is passed on (i.e. client handles deletion)*/
      virtual TrackCollection*  process(const TrackCollection* tracks);
    
    private:
      
      void reset();
    
      /**Add passed TrackCollection, and Trk::PrepRawData from tracks to caches
	 @param tracks the TrackCollection is looped over, 
	 and each Trk::Track is added to the various caches. 
	 The Trk::PrepRawData from each Trk::Track are added to the IPRD_AssociationTool*/
      void addNewTracks(const TrackCollection* tracks );


      void solveTracks();

      /** print out tracks and their scores for debugging*/
      void dumpTracks(const TrackCollection& tracks);
	
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

      /** unsorted container of track and track scores.*/
      TrackScoreMap m_trackScoreTrackMap;
	
      /** signature map to drop double track. */
      PrdSignatureSet m_prdSigSet;

      /**Tracks that will be passed out of AmbiProcessor. 
	 Recreated anew each time process() is called*/ 
      TrackCollection* m_finalTracks;
	
      /** option to disable sorting based on track score and 
	  use the ordering provided externally*/
      bool m_disableSorting;
    };

} //end ns

#endif // TrackAmbiguityProcessorTool_H

