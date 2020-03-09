/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _Trk_IPRDtoTrackMapTool_h_
#define _Trk_IPRDtoTrackMapTool_h_

#include "GaudiKernel/IAlgTool.h"
#include <map>
#include <set>
#include "TrkEventUtils/PRDtoTrackMap.h"

class AtlasDetectorID;
class Identifier;

static const InterfaceID IID_IPRDtoTrackMapTool("Trk::IPRDtoTrackMapTool", 1, 0);

namespace Trk {

  class Track;
  class PrepRawData;
  class PRDtoTrackMap;

/** @brief Interface for handling the mapping between Trk::Track and Trk::PrepRawData.

  The tool is used to keep track of a collection of PrepRawData belonging to Tracks.
The user is responsible for informing the tool about the Tracks in question, by using :
  - addPRDs() method to add tracks (and the PrepRawData they contain) and 
  - removePRDs() to remove them (for example if the Track has failed some cut and is being rejected

  Once the information is in the store, various methods can be used to query the relationship between tracks and the prep raw data - for example, isShared().

  Currently, reset() should be used to clear the tool before use.
  @author Edward.Moyse@cern.ch */

  //@TODO turn into a service
  class IPRDtoTrackMapTool : virtual public IAlgTool
  {
    public:
      DeclareInterfaceID(IPRDtoTrackMapTool, 1, 0);
      /** */
      typedef std::set<const Track*>   TrackSet;

      /** Create a structure which holds the PRD to track map.
       * @TODO should take event context as an argument.
       */ 
      virtual std::unique_ptr<Trk::PRDtoTrackMap> createPRDtoTrackMap() const = 0;

      /** Return an object which is storeable.
       *
       * May create a new object which is stripped off data that is not to be stored.
       */
      virtual std::unique_ptr<Trk::PRDtoTrackMap> reduceToStorableMap(std::unique_ptr<Trk::PRDtoTrackMap> &&obj_in) const = 0;

      /** Add the PRDs from this track to the store*/
      virtual StatusCode addPRDs(Trk::PRDtoTrackMap &prd_to_track_map, const Track& track) const = 0;

      /** returns a vector of PRDs belonging to the passed track.
          It's basically for the convenience of users and is created purely from the passed track.
          i.e. there is no caching if you do it multiple times on the same track, you're being 
          inefficient!!
          @param track  will be iterated through and all PrepRawData added to a vector
          @return vector of PrepRawData* belonging to 'track'. The PrepRawData  must NOT be 
          deleted - they belong to the Track (and thus the event).*/
      virtual std::vector< const PrepRawData* > getPrdsOnTrack(Trk::PRDtoTrackMap &prd_to_track_map, const Track& track) const=0;

      /** Return transient sets of connected and disconnected tracks, *AND* remove disconnected 
          tracks from this tool.
          i.e. get (transient) set of tracks which share/don't share PrepRawData, and remove those 
          tracks that are disconnected from this tool's caches. 
          @return transient set of connected and disconnected tracks. 
          NB the sets are returned by reference 
          for speed, but they will be reset each time this method is called (whether in your 
          algorithm or that of another), hence they are transient. You **MUST** copy them if
          you need  them to have an extended lifetime.*/
      virtual TrackSet findConnectedTracks( Trk::PRDtoTrackMap &prd_to_track_map, const Track& track) const  =0;


  };

}//end of ns

#endif 
