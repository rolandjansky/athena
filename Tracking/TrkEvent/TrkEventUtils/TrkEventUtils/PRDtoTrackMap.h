/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _Trk_PRDtoTrackMap_H_
#define _Trk_PRDtoTrackMap_H_

#include <unordered_map>

namespace Trk {

class PrepRawData;
class Track;

class PRDtoTrackMap
{
public:
  using TrackPrepRawDataMap =
    std::unordered_map<const Track*, std::vector<const PrepRawData*>>;

  using PrepRawDataTrackMap =
    std::unordered_multimap<const PrepRawData*, const Track*>;

  using PrepRawDataTrackMapRange =
    std::pair<PrepRawDataTrackMap::iterator, PrepRawDataTrackMap::iterator>;

  using ConstPrepRawDataTrackMapRange =
    std::pair<PrepRawDataTrackMap::const_iterator,
              PrepRawDataTrackMap::const_iterator>;

  PRDtoTrackMap() = default;
  virtual ~PRDtoTrackMap() = default;
  PRDtoTrackMap(const PRDtoTrackMap& a) = default;
  PRDtoTrackMap(PRDtoTrackMap&& a) noexcept = default;
  PRDtoTrackMap& operator=(const PRDtoTrackMap& a) = default;
  PRDtoTrackMap& operator=(PRDtoTrackMap&& a) noexcept = default;

  virtual const std::type_info& getType() const;

  /** does this PRD belong to at least one track?
      @param prd the PrepRawData in question
      @return true if 'prd' exists in at least one track (of course
     PRD_AssociationTool can only give information about tracks it knows about
     i.e. that were added with addPRDs()*/
  bool isUsed(const PrepRawData& prd) const;

  /** does this PRD belong to more than one track?
      @param prd the PrepRawData in question
      @return true if 'prd' exists on more than one track (of course
     PRD_AssociationTool can only give information about tracks it knows about
     i.e. that were added with addPRDs()*/
  bool isShared(const PrepRawData& prd) const;

  /** get the Tracks associated with this PrepRawData.
      IMPORTANT: Please use the typedef PrepRawDataTrackMapRange to
      access the tracks, as the internal representation may change.*/
  PrepRawDataTrackMapRange onTracks(const PrepRawData& prd);

  /** get the Tracks associated with this PrepRawData.
      IMPORTANT: Please use the typedef ConstPrepRawDataTrackMapRange to
      access the tracks, as the internal representation may change.*/
  ConstPrepRawDataTrackMapRange onTracks(const PrepRawData& prd) const;

  /** returns a vector of PRDs belonging to the passed track.
   */
  std::vector<const Trk::PrepRawData*> getPrdsOnTrack(const Track& track) const;

  void clear();

protected:
  /**holds the tracks associated with each PRD (i.e. the PRD* is the key)*/
  PrepRawDataTrackMap m_prepRawDataTrackMap;

  /**holds the PRDs associated with each Track (i.e. the Track* is the key)*/
  TrackPrepRawDataMap m_trackPrepRawDataMap;
};

}

#include "PRDtoTrackMap.icc"
#ifndef CLIDSVC_CLASSDEF_H
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(Trk::PRDtoTrackMap, 51736277, 1)
#endif
#endif
