/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAssociationTools/PRDtoTrackMapTool.h"

#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include <cassert>
#include <vector>
#include "ext/functional"

Trk::PRDtoTrackMapTool::PRDtoTrackMapTool(const std::string& t,
                                          const std::string& n,
                                          const IInterface*  p)
  : base_class(t,n,p)
{
}

StatusCode Trk::PRDtoTrackMapTool::initialize() {
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}

std::unique_ptr<Trk::PRDtoTrackMap> Trk::PRDtoTrackMapTool::createPRDtoTrackMap() const {
  return std::unique_ptr<Trk::PRDtoTrackMap>(new Trk::PRDtoTrackMapTool::PRDtoTrackMap);
}

std::unique_ptr<Trk::PRDtoTrackMap> Trk::PRDtoTrackMapTool::reduceToStorableMap(std::unique_ptr<Trk::PRDtoTrackMap> &&obj_in) const {
  return std::move(obj_in);
}

void Trk::PRDtoTrackMapTool::ensureType(Trk::PRDtoTrackMap &virt_prd_to_track_map) const
{
  if (virt_prd_to_track_map.getType() != typeid(PRDtoTrackMapTool::PRDtoTrackMap)) {
    ATH_MSG_FATAL( "Type mismap between tool and map. Expecting " << typeid(PRDtoTrackMap).name()
                   << " but got " << virt_prd_to_track_map.getType().name()
                   << " . The tool can only handle a map created by the same tool. ");
    throw std::logic_error("Type mismap between tool and map");
  }
  assert( dynamic_cast<PRDtoTrackMap *>(&virt_prd_to_track_map) != nullptr );
}

StatusCode Trk::PRDtoTrackMapTool::addPRDs(Trk::PRDtoTrackMap &virt_prd_to_track_map,  const Trk::Track& track ) const
{
  ensureType(virt_prd_to_track_map);
  PRDtoTrackMap &prd_to_track_map = static_cast<PRDtoTrackMap&>(virt_prd_to_track_map);

  // test caching
  PRDtoTrackMap::TrackPrepRawDataMap::const_iterator itvec = prd_to_track_map.m_trackPrepRawDataMap.find(&track);
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


Trk::PRDtoTrackMapTool::TrackSet
    Trk::PRDtoTrackMapTool::findConnectedTracks(Trk::PRDtoTrackMap &virt_prd_to_track_map, const Trk::Track& track ) const
{
  ensureType(virt_prd_to_track_map);
  PRDtoTrackMap &prd_to_track_map = static_cast<PRDtoTrackMap&>(virt_prd_to_track_map);

  TrackSet connectedTracks;

  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack(prd_to_track_map, track);
  for (const Trk::PrepRawData* a_prd : prds )
  {
    PRDtoTrackMap::PrepRawDataTrackMapRange range = prd_to_track_map.onTracks(*a_prd);

    // TODO use remove_copy_if instead.
    for ( ; range.first!=range.second; ++(range.first) )
    {
      const Track* conTrack = (range.first)->second;
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
Trk::PRDtoTrackMapTool::getPrdsOnTrack(Trk::PRDtoTrackMap &virt_prd_to_track_map,
                                      const Trk::Track& track) const
{
  using PRDs_t = std::vector<const Trk::PrepRawData *>;

  ensureType(virt_prd_to_track_map);
  PRDtoTrackMap &prd_to_track_map = static_cast<PRDtoTrackMap&>(virt_prd_to_track_map);

  // test caching
  PRDtoTrackMap::TrackPrepRawDataMap::const_iterator itvec = prd_to_track_map.m_trackPrepRawDataMap.find(&track);
  if (itvec!=prd_to_track_map.m_trackPrepRawDataMap.end())
  {
    ATH_MSG_VERBOSE ("found track in cache, return cached PRD vector for track");
    return itvec->second;
  }

  if (track.measurementsOnTrack()==nullptr) {
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
  for (const MeasurementBase* meas : *track.measurementsOnTrack())
  {
    const RIO_OnTrack* rot = dynamic_cast<const RIO_OnTrack*>(meas);
    if (rot){
      if(m_idHelperSvc->isMuon(rot->identify())){
        //only use precision hits for muon track overlap
        if(   !m_idHelperSvc->isMdt(rot->identify())
           && !(m_idHelperSvc->isCsc(rot->identify())
           && !m_idHelperSvc->measuresPhi(rot->identify()))) continue;
      }
      vec.push_back(rot->prepRawData());
    }
    else{
      const Trk::CompetingRIOsOnTrack* competingROT = dynamic_cast <const Trk::CompetingRIOsOnTrack*> (meas);
      if(competingROT){
        const unsigned int numROTs = competingROT->numberOfContainedROTs();
        for( unsigned int i=0;i<numROTs;++i ){
          const Trk::RIO_OnTrack* rot = &competingROT->rioOnTrack(i);
          if( !rot || !rot->prepRawData() || !m_idHelperSvc->isMuon(rot->identify()) ) continue;
          //only use precision hits for muon track overlap
          if(   !m_idHelperSvc->isMdt(rot->identify())
             && !(    m_idHelperSvc->isCsc(rot->identify())
                  && !m_idHelperSvc->measuresPhi(rot->identify())) ) continue;
          vec.push_back(rot->prepRawData());
        }
      }
    }
  }
  ATH_MSG_DEBUG (" Getting "<<vec.size()<<" PRDs from track at:"<<&track);

  return vec;
}
