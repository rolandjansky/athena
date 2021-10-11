/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKAMBIGUITYPROCESSOR_IPRD_ASSOCIATIONTOOL_H
#define TRKAMBIGUITYPROCESSOR_IPRD_ASSOCIATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include <map>
#include <set>

class AtlasDetectorID;
class Identifier;

namespace Trk {

class Track;
class PrepRawData;

/** @brief Interface for handling the mapping between Trk::Track and
Trk::PrepRawData.

  The tool is used to keep track of a collection of PrepRawData belonging to
Tracks. The user is responsible for informing the tool about the Tracks in
question, by using :
  - addPRDs() method to add tracks (and the PrepRawData they contain) and
  - removePRDs() to remove them (for example if the Track has failed some cut
and is being rejected

  Once the information is in the store, various methods can be used to query the
relationship between tracks and the prep raw data - for example, isShared().

  Currently, reset() should be used to clear the tool before use.
@author Edward.Moyse@cern.ch */
class IPRD_AssociationTool : virtual public IAlgTool
{
public:
  DeclareInterfaceID(IPRD_AssociationTool, 1, 0);

  //      typedef std::pair<const PrepRawData*, const Track*>
  //      PrepRawDataTrackMapPair;
  using PrepRawDataTrackMap = PRDtoTrackMap::PrepRawDataTrackMap;
  using PRD_MapIt = PrepRawDataTrackMap::iterator;
  using ConstPRD_MapIt = PrepRawDataTrackMap::const_iterator;
  /**the first element is the beginning iterator of the range, the second is the
   * end*/
  using PrepRawDataTrackMapRange = std::pair<ConstPRD_MapIt, ConstPRD_MapIt>;
  using TrackPrepRawDataMap = PRDtoTrackMap::TrackPrepRawDataMap;

  /** */
  typedef std::set<const Track*> TrackSet;

  /// The mutable state of the tool.
  struct Maps
  {
    // holds the tracks associated with each PRD (i.e. the PRD* is the key)
    IPRD_AssociationTool::PrepRawDataTrackMap m_prepRawDataTrackMap;
    // holds the PRDs associated with each Track (i.e. the Track* is the key)
    IPRD_AssociationTool::TrackPrepRawDataMap m_trackPrepRawDataMap;
  };

  /** add the PRDs from this track to maps*/
  virtual StatusCode addPRDs(Maps& maps, const Track& track) const = 0;

  /** add the PRDs from this track to the store*/
  virtual StatusCode addPRDs(const Track& track) = 0;

  /** remove the PRDs from this track from the maps
      @param track all PRDs from 'track' will be removed from maps */
  virtual StatusCode removePRDs(Maps& maps, const Track& track) const = 0;

  /** remove the PRDs from this track from the store
      @param track all PRDs from 'track' will be removed from the
     PRD_AssociationTool's internal store.*/
  virtual StatusCode removePRDs(const Track& track) = 0;

  /** does this PRD belong to at least one track in maps?
      @param prd the PrepRawData in question
      @return true if 'prd' exists in at least one track (of course
     PRD_AssociationTool can only give information about tracks it knows about
     i.e. that were added to maps with addPRDs()*/
  virtual bool isUsed(const Maps& maps, const PrepRawData& prd) const = 0;

  /** does this PRD belong to at least one track?
      @param prd the PrepRawData in question
      @return true if 'prd' exists in at least one track (of course
     PRD_AssociationTool can only give information about tracks it knows about
     i.e. that were added with addPRDs()*/
  virtual bool isUsed(const PrepRawData& prd) const = 0;

  /** does this PRD belong to more than one track in maps?
      @param prd the PrepRawData in question
      @return true if 'prd' exists on more than one track (of course
     PRD_AssociationTool can only give information about tracks it knows about
     i.e. that were added to maps with addPRDs()*/
  virtual bool isShared(const Maps& maps, const PrepRawData& prd) const = 0;

  /** does this PRD belong to more than one track?
      @param prd the PrepRawData in question
      @return true if 'prd' exists on more than one track (of course
     PRD_AssociationTool can only give information about tracks it knows about
     i.e. that were added with addPRDs()*/
  virtual bool isShared(const PrepRawData& prd) const = 0;

  /** returns a vector of PRDs belonging to the passed track.
      It's basically for the convenience of users and is created purely from the
     passed track. i.e. there is no caching if you do it multiple times on the
     same track, you're being inefficient!!
      @param track  will be iterated through and all PrepRawData added to a
     vector
      @return vector of PrepRawData* belonging to 'track'. The PrepRawData  must
     NOT be deleted - they belong to the Track (and thus the event).*/
  virtual std::vector<const PrepRawData*> getPrdsOnTrack(
    const Track& track) const = 0;

  // getPrdsOnTrack with state passed explicitly.
  virtual std::vector<const PrepRawData*> getPrdsOnTrack(
    const Maps& maps,
    const Track& track) const = 0;

  /** Return transient sets of connected and disconnected tracks, *AND* remove
     disconnected tracks from this tool. i.e. get (transient) set of tracks
     which share/don't share PrepRawData, and remove those tracks that are
     disconnected from this tool's caches.
      @return transient set of connected and disconnected tracks.
      NB the sets are returned by reference
      for speed, but they will be reset each time this method is called (whether
     in your algorithm or that of another), hence they are transient. You
     **MUST** copy them if you need  them to have an extended lifetime.*/
  virtual TrackSet findConnectedTracks(const Track& track) const = 0;

  // findConnectedTracks with state passed explicitly.
  virtual TrackSet findConnectedTracks(const Maps& maps,
                                       const Track& track) const = 0;

  /** get the Tracks associated with this PrepRawData.
      IMPORTANT: Please use the typedefs
     IPRD_AssociationTool::PrepRawDataTrackMapRange and
      IPRD_AssociationTool::ConstPRD_MapIt to access the tracks, as the internal
      representation may change.*/
  virtual PrepRawDataTrackMapRange onTracks(const PrepRawData& prd) const = 0;

  // onTracks with explicit state
  virtual PrepRawDataTrackMapRange onTracks(const Maps& maps,
                                            const PrepRawData& prd) const = 0;

  /**this may well go, but at the moment it resets the tool*/
  virtual void reset() = 0;
};

} // end of ns

#endif
