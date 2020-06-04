/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_PRDtoTrackMapTool.h"

#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include <cassert>
#include <vector>
#include "ext/functional"

iFatras::ISF_PRDtoTrackMapTool::ISF_PRDtoTrackMapTool(const std::string& t,
                                          const std::string& n,
                                          const IInterface*  p)
  : base_class(t,n,p)
{
}

std::unique_ptr<Trk::PRDtoTrackMap> iFatras::ISF_PRDtoTrackMapTool::createPRDtoTrackMap() const {
  return std::unique_ptr<Trk::PRDtoTrackMap>(new ISF_PRDtoTrackMap);
}

std::unique_ptr<Trk::PRDtoTrackMap> iFatras::ISF_PRDtoTrackMapTool::reduceToStorableMap(std::unique_ptr<Trk::PRDtoTrackMap> &&obj_in) const {
  return std::move(obj_in);
}

void iFatras::ISF_PRDtoTrackMapTool::ensureType(Trk::PRDtoTrackMap &virt_prd_to_track_map) const
{
  if (virt_prd_to_track_map.getType() != typeid(ISF_PRDtoTrackMap)) {
    ATH_MSG_FATAL( "Type mismap between tool and map. Expecting " << typeid(ISF_PRDtoTrackMap).name()
                   << " but got " << virt_prd_to_track_map.getType().name()
                   << " . The tool can only handle a map created by the same tool. ");
    throw std::logic_error("Type mismap between tool and map");
  }
  assert( dynamic_cast<ISF_PRDtoTrackMap *>(&virt_prd_to_track_map) != nullptr );
}

StatusCode iFatras::ISF_PRDtoTrackMapTool::addPRDs(Trk::PRDtoTrackMap &virt_prd_to_track_map,  const Trk::Track& track ) const
{
  ensureType(virt_prd_to_track_map);
  ISF_PRDtoTrackMap &prd_to_track_map = static_cast<ISF_PRDtoTrackMap&>(virt_prd_to_track_map);

  // test caching
  ISF_PRDtoTrackMap::TrackPrepRawDataMap::const_iterator itvec = prd_to_track_map.m_trackPrepRawDataMap.find(&track);
  if (itvec!=prd_to_track_map.m_trackPrepRawDataMap.end())
  {
    ATH_MSG_ERROR ("track already found in cache, should not happen");
    return StatusCode::FAILURE;
  }
  // add all prds on track to map
  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack(prd_to_track_map, track );
  for(const Trk::PrepRawData*a_prd : prds) {
    if (prd_to_track_map.m_prepRawDataTrackMap.insert(std::make_pair(a_prd, &track) ) == prd_to_track_map.m_prepRawDataTrackMap.end()) {
      ATH_MSG_ERROR ("failed to associate track to prd.");
    }
  }

  // cache this using m_trackPrepRawDataMap
  prd_to_track_map.m_trackPrepRawDataMap.insert( std::make_pair(&track, std::move(prds)) );

  ATH_MSG_DEBUG ("Added PRDs from Track at ("<<&track<<") - map now has size: \t"<<
                 prd_to_track_map.m_prepRawDataTrackMap.size());
  return StatusCode::SUCCESS;
}


Trk::IPRDtoTrackMapTool::TrackSet
    iFatras::ISF_PRDtoTrackMapTool::findConnectedTracks(Trk::PRDtoTrackMap &virt_prd_to_track_map, const Trk::Track& track ) const
{
  ensureType(virt_prd_to_track_map);
  ISF_PRDtoTrackMap &prd_to_track_map = static_cast<ISF_PRDtoTrackMap&>(virt_prd_to_track_map);

  //using namespace __gnu_cxx;

  TrackSet connectedTracks;

  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack(prd_to_track_map, track);
  for (const Trk::PrepRawData* a_prd : prds )
  {
    ISF_PRDtoTrackMap::PrepRawDataTrackMapRange range = prd_to_track_map.onTracks(*a_prd);

    // TODO use remove_copy_if instead.
    for ( ; range.first!=range.second; ++(range.first) )
    {
      const Trk::Track* conTrack = (range.first)->second;
      // don't copy this track!
      if (conTrack!=&track) {
        // this does actually not allow for double entries
        connectedTracks.insert(conTrack);
        ATH_MSG_VERBOSE ("Track "<<&track<<" \tshares PRD "<<a_prd<<" \twith track:"<<conTrack);
      }
    }
    ATH_MSG_VERBOSE ("Added in connected tracks for PRD:"<<a_prd<<
                     "\tsize of list now:"<<connectedTracks.size());
  }

  return connectedTracks;
}


std::vector< const Trk::PrepRawData* >
iFatras::ISF_PRDtoTrackMapTool::getPrdsOnTrack(Trk::PRDtoTrackMap &virt_prd_to_track_map,
                                               const Trk::Track& track) const
{
  typedef std::vector<const Trk::PrepRawData*> PRDs_t;

  ensureType(virt_prd_to_track_map);
  ISF_PRDtoTrackMap &prd_to_track_map = static_cast<ISF_PRDtoTrackMap&>(virt_prd_to_track_map);

  // test caching
  ISF_PRDtoTrackMap::TrackPrepRawDataMap::const_iterator itvec = prd_to_track_map.m_trackPrepRawDataMap.find(&track);
  if (itvec!=prd_to_track_map.m_trackPrepRawDataMap.end())
  {
    ATH_MSG_VERBOSE ("found track in cache, return cached PRD vector for track");
    return itvec->second;
  }

  if (track.measurementsOnTrack()==0) {
    ATH_MSG_WARNING ("Track has no RoTs");
    return PRDs_t();
  }

  // FIXME can I do this without copying the vector?
  /*  transform(
      track.measurementsOnTrack()->begin(),
      track.measurementsOnTrack()->end() ,
      back_inserter(vec),
      bind2nd(CreatePRD_VectorFromTrack(), &track) );*/

  PRDs_t vec;
  vec.reserve(track.measurementsOnTrack()->size());
  for (const Trk::MeasurementBase* meas : *track.measurementsOnTrack())
  {
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(meas);
    if (rot){
      vec.push_back(rot->prepRawData());
    }
  }
  ATH_MSG_DEBUG (" Getting "<<vec.size()<<" PRDs from track at:"<<&track);

  return vec;
}
