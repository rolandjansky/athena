/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRK_PRD_ASSOCIATIONTOOL_H
#define TRK_PRD_ASSOCIATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include <set>
#include <map>

class AtlasDetectorID;
class Identifier;

namespace Trk {

  class Track;
  class PrepRawData;

  /** Concrete Implementation of the IPRD_AssociationTool interface.*/
  class PRD_AssociationTool : virtual public IPRD_AssociationTool, public AthAlgTool
{

  public:
    PRD_AssociationTool(const std::string&,const std::string&,const IInterface*);
    virtual ~PRD_AssociationTool ();
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();

  /** add the PRDs from this track to the store
    @param track all PRDs from 'track' will be added to PRD_AssociationTool's internal store.*/
    virtual StatusCode addPRDs(const Track& track);

  /** remove the PRDs from this track from the store
    @param track all PRDs from 'track' will be removed from the PRD_AssociationTool's 
    internal store.*/
    virtual StatusCode removePRDs(const Track& track);

  /** does this PRD belong to at least one track?
    @param prd the PrepRawData in question
    @return true if 'prd' exists in at least one track (of course PRD_AssociationTool can only
    give information about tracks it knows about i.e. that were added with addPRDs()*/
    virtual bool isUsed(const PrepRawData& prd) const;

  /** does this PRD belong to more than one track?
    @param prd the PrepRawData in question
    @return true if 'prd' exists on more than one track (of course PRD_AssociationTool can only
    give information about tracks it knows about i.e. that were added with addPRDs()*/
    virtual bool isShared(const PrepRawData& prd) const;

  /**returns a vector of PRDs belonging to the passed track.
    It's basically for the convenience of users and is created purely from the passed track.
    i.e. there is no caching if you do it multiple times on the same track, you're being 
    inefficient!!
    @param track this Track will be iterated through and all PrepRawData added to a vector
    @return vector of PrepRawData* belonging to 'track'. The PrepRawData should NOT be deleted 
    - they belong to the Track (and thus the event).*/
    virtual std::vector< const PrepRawData* > getPrdsOnTrack(const Track& track) const;

  /** returns set of tracks which share PRD with this one
  @param track this Track must be known to this tool. 
  @return a set of tracks which share PRD/hits with the passed 'track'*/
    virtual IPRD_AssociationTool::TrackSet findConnectedTracks( const Track& track) ;

  /** get the Tracks associated with this Trk::PrepRawData. 
    IMPORTANT: Please use the typedefs IPRD_AssociationTool::PrepRawDataRange and 
    IPRD_AssociationTool::ConstPRD_MapIt (defined in the interface) to access the 
    tracks, as the way the data is stored internally may change.*/
    virtual IPRD_AssociationTool::PrepRawDataTrackMapRange onTracks(const PrepRawData& prd) const;

      /** resets the tool - should be called before using tool (and maybe afterwards to free up 
    memory)*/
    virtual void reset();

  private:
    /**holds the tracks associated with each PRD (i.e. the PRD* is the key)*/
    IPRD_AssociationTool::PrepRawDataTrackMap m_prepRawDataTrackMap;

    /**holds the PRDs associated with each Track (i.e. the Track* is the key)*/
    IPRD_AssociationTool::TrackPrepRawDataMap m_trackPrepRawDataMap;
};

  inline bool Trk::PRD_AssociationTool::isUsed(const PrepRawData& prd) const
{
  return (m_prepRawDataTrackMap.count(&prd)>0);
}

  inline bool Trk::PRD_AssociationTool::isShared(const PrepRawData& prd) const
{
  return (m_prepRawDataTrackMap.count(&prd)>1);
}

}
#endif
