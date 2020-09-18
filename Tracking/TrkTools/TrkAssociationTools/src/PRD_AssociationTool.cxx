/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAssociationTools/PRD_AssociationTool.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include <cassert>
#include <vector>
#include <stdexcept>
#include "ext/functional"

Trk::PRD_AssociationTool::PRD_AssociationTool(const std::string& t,
  const std::string& n,
  const IInterface*  p )
  :
  base_class(t,n,p)
{
  declareProperty( "SetupCorrect",  m_setupCorrect=false);
}

StatusCode Trk::PRD_AssociationTool::initialize()
{
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_CHECK( m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty() ) );
  ATH_CHECK( m_idHelperSvc.retrieve());
  if (!m_setupCorrect) {
    ATH_MSG_INFO("initialize: Tool " << name() << " not configured for PRD usage.");
  }

  return StatusCode::SUCCESS;
}

StatusCode Trk::PRD_AssociationTool::addPRDs( const Trk::Track& track )
{
  if (!m_prdToTrackMap.key().empty()) throw std::runtime_error("Cannot modify(addPRDs) the PRDtoTrackMap that was read from storegate!");
  return addPRDs (m_maps, track);
}

StatusCode Trk::PRD_AssociationTool::addPRDs( Maps& maps, const Trk::Track& track ) const
{
  // test caching
  TrackPrepRawDataMap::const_iterator itvec = maps.m_trackPrepRawDataMap.find(&track);
  if (itvec!=maps.m_trackPrepRawDataMap.end())
  {
    ATH_MSG_ERROR ("track already found in cache, should not happen");
    return StatusCode::FAILURE;
  }
  // get all prds on 'track'
  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack( maps, track );
  for (const Trk::PrepRawData* prd : prds) {
     maps.m_prepRawDataTrackMap.emplace(prd, &track);
  }

  // cache this using m_trackPrepRawDataMap
  maps.m_trackPrepRawDataMap.emplace(&track, prds);

  ATH_MSG_DEBUG ("Added PRDs from Track at ("<<&track<<") - map now has size: \t"<<
                 maps.m_prepRawDataTrackMap.size());
  return StatusCode::SUCCESS;
}

StatusCode Trk::PRD_AssociationTool::removePRDs( const Trk::Track& track )
{
  if (!m_prdToTrackMap.key().empty()) throw std::runtime_error("Cannot modify(removePRDs) the PRDtoTrackMap that was read from storegate!");
  return removePRDs (m_maps, track);
}

StatusCode Trk::PRD_AssociationTool::removePRDs( Maps& maps,
                                                 const Trk::Track& track ) const
{
  // This is NOT pretty code!
  // At the moment I'm brute-forcing, but maybe I need a second map, containing <Track*, iterator>
  // The problem is that I think filling such a map is also time-consuming.
  // Since removes happen much less frequently than add, then the slow bit should be here.
  // EJWM

  // save for debugging purposes
  int oldSize = maps.m_prepRawDataTrackMap.size();//used in debug output at end.

  // test caching
  TrackPrepRawDataMap::iterator itvec = maps.m_trackPrepRawDataMap.find(&track);
  if (itvec==maps.m_trackPrepRawDataMap.end())
  {
    ATH_MSG_ERROR ("Track not found in cache, this should not happen");
    return StatusCode::FAILURE;
  }

  // get all prds on 'track'
  std::vector< const Trk::PrepRawData* > prds = itvec->second;
  for (const Trk::PrepRawData* prd : prds)
  {
    // now get all map elements (i.e. Tracks) that contain this PRD
    IPRD_AssociationTool::PrepRawDataTrackMapRange
        range = maps.m_prepRawDataTrackMap.equal_range(prd);

    // get iterators for range
    ConstPRD_MapIt mapIt    = range.first;
    ConstPRD_MapIt mapItEnd = range.second;

    // simple for loop instead of fancier remove_if above
    for ( ;mapIt!=mapItEnd; ++mapIt)
    {
      if ( mapIt->second==&track )
      {
        maps.m_prepRawDataTrackMap.erase( mapIt );
        break;//should only ever be one Track
      }
    }
  }

  // remove cached PRD vector
  maps.m_trackPrepRawDataMap.erase( itvec );

  ATH_MSG_DEBUG ("Removed  PRDs from track (" <<&track<<") \t- map has changed "<<
                 "size from \t"<<oldSize <<" \tto "<<maps.m_prepRawDataTrackMap.size());
  return StatusCode::SUCCESS;
}

Trk::IPRD_AssociationTool::TrackSet
Trk::PRD_AssociationTool::findConnectedTracks( const Trk::Track& track ) const
{
  if (!m_prdToTrackMap.key().empty()) throw std::runtime_error("findConnectedTracks not supported when using a PRDtoTrackMap from storegate.");
  return findConnectedTracks (m_maps, track);
}

Trk::IPRD_AssociationTool::TrackSet
Trk::PRD_AssociationTool::findConnectedTracks( const Maps& maps,
                                               const Trk::Track& track ) const
{
  TrackSet connectedTracks;

  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack(maps, track);
  for (const Trk::PrepRawData* prd : prds)
  {
    IPRD_AssociationTool::PrepRawDataTrackMapRange range = onTracks(maps, *prd);

    // TODO use remove_copy_if instead.
    for ( ; range.first!=range.second; ++(range.first) )
    {
      const Track* conTrack = (range.first)->second;
      // don't copy this track!
      if (conTrack!=&track) {
        // this does actually not allow for double entries
        connectedTracks.insert(conTrack);
        ATH_MSG_VERBOSE ("Track "<<&track<<" \tshares PRD "<<prd<<" \twith track:"<<conTrack);
      }
    }
    ATH_MSG_VERBOSE ("Added in connected tracks for PRD:"<<prd<<
                     "\tsize of list now:"<<connectedTracks.size());
  }

  return connectedTracks;
}


std::vector< const Trk::PrepRawData* >
Trk::PRD_AssociationTool::getPrdsOnTrack(const Trk::Track& track) const
{
  if (!m_prdToTrackMap.key().empty()) throw std::runtime_error("getPrdsOnTrack not supported when using a PRDtoTrackMap from storegate.");
  return getPrdsOnTrack (m_maps, track);
}


std::vector< const Trk::PrepRawData* >
Trk::PRD_AssociationTool::getPrdsOnTrack(const Maps& maps,
                                         const Trk::Track& track) const
{
  using PRDs_t = std::vector<const PrepRawData *>;

  // test caching
  TrackPrepRawDataMap::const_iterator itvec = maps.m_trackPrepRawDataMap.find(&track);
  if (itvec!=maps.m_trackPrepRawDataMap.end())
  {
    ATH_MSG_VERBOSE ("found track in cache, return cached PRD vector for track");
    return itvec->second;
  }

  if (track.measurementsOnTrack()==nullptr) {
    ATH_MSG_WARNING ("Track has no RoTs");
    return PRDs_t();
  }

  PRDs_t vec;
  vec.reserve(track.measurementsOnTrack()->size());
  for (const MeasurementBase* meas : *track.measurementsOnTrack())
  {
    const RIO_OnTrack* rot = dynamic_cast<const RIO_OnTrack*>(meas);
    if (rot){
      if(m_idHelperSvc->isMuon(rot->identify())){
        //only use precision hits for muon track overlap
        if(!m_idHelperSvc->isMdt(rot->identify()) && !(m_idHelperSvc->isCsc(rot->identify()) && !m_idHelperSvc->measuresPhi(rot->identify()))) continue;
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
          if(!m_idHelperSvc->isMdt(rot->identify()) && !(m_idHelperSvc->isCsc(rot->identify()) && !m_idHelperSvc->measuresPhi(rot->identify()))) continue;
          vec.push_back(rot->prepRawData());
        }
      }
    }
  }
  ATH_MSG_DEBUG (" Getting "<<vec.size()<<" PRDs from track at:"<<&track);

  return vec;
}

Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange
Trk::PRD_AssociationTool::onTracks(const PrepRawData& prd) const
{
  if (!m_prdToTrackMap.key().empty()) {return SG::ReadHandle<Trk::PRDtoTrackMap>(m_prdToTrackMap)->onTracks(prd);}
  return onTracks (m_maps, prd);
}

Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange
Trk::PRD_AssociationTool::onTracks(const Maps& maps, const PrepRawData& prd) const
{
//	std::pair<IPRD_AssociationTool::PRD_MapIt, IPRD_AssociationTool::PRD_MapIt>       range =
  return maps.m_prepRawDataTrackMap.equal_range(&prd);
}

void Trk::PRD_AssociationTool::reset()
{
  if (!m_prdToTrackMap.key().empty()) throw std::runtime_error("cannot reset PRDtoTrack map when it is taken from storegate.");
  m_maps.m_prepRawDataTrackMap.clear();
  m_maps.m_trackPrepRawDataMap.clear();
}
