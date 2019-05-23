/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FatrasRecoTools/ISF_PRD_AssociationTool.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
 
#include <cassert>
#include <vector>
#include "ext/functional"

iFatras::ISF_PRD_AssociationTool::ISF_PRD_AssociationTool(const std::string& t,
							  const std::string& n,
							  const IInterface*  p ) :
  base_class(t,n,p)
{
}

iFatras::ISF_PRD_AssociationTool::~ISF_PRD_AssociationTool() {}

StatusCode iFatras::ISF_PRD_AssociationTool::initialize() {
  return StatusCode::SUCCESS; 
}

StatusCode iFatras::ISF_PRD_AssociationTool::finalize() {
  StatusCode sc = AlgTool::finalize();
  return sc;
}

StatusCode iFatras::ISF_PRD_AssociationTool::addPRDs( const Trk::Track& track )
{
  return addPRDs (m_maps, track);
}

StatusCode iFatras::ISF_PRD_AssociationTool::addPRDs( Maps& maps, const Trk::Track& track ) const
{
  // test caching
  TrackPrepRawDataMap::const_iterator itvec = maps.m_trackPrepRawDataMap.find(&track);
  if (itvec!=maps.m_trackPrepRawDataMap.end())
    {
      msg(MSG::ERROR)<<"track already found in cache, should not happen"<<endmsg;
      return StatusCode::FAILURE;
    }
  
  // get all prds on 'track'
  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack( maps, track );
  
  // loop over PRD
  for (const Trk::PrepRawData* prd : prds) {
    maps.m_prepRawDataTrackMap.emplace(prd, &track);
  }
  
  // cache this using m_trackPrepRawDataMap
  maps.m_trackPrepRawDataMap.emplace(&track, prds);
     
  if (msgLvl(MSG::DEBUG)) msg()<<"Added PRDs from Track at ("<<&track<<") - map now has size: \t"
			       <<maps.m_prepRawDataTrackMap.size()<<endmsg;
  return StatusCode::SUCCESS;
}
 
StatusCode iFatras::ISF_PRD_AssociationTool::removePRDs( const Trk::Track& track )
{
  return removePRDs (m_maps, track);
}

StatusCode iFatras::ISF_PRD_AssociationTool::removePRDs( Maps& maps,
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
      msg(MSG::ERROR)<<"Track not found in cache, this should not happen"<<endmsg;
      return StatusCode::FAILURE;
    }
  
  // get all prds on 'track'
  std::vector< const Trk::PrepRawData* > prds = itvec->second;
  
  // loop over PRD
  for (const Trk::PrepRawData* prd : prds)
    {
      // now get all map elements (i.e. Tracks) that contain this PRD
      Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange 
	range = maps.m_prepRawDataTrackMap.equal_range(prd);
      // get iterators for range
      ConstPRD_MapIt mapIt    = range.first;
      ConstPRD_MapIt mapItEnd = range.second;
      // simple for loop instead of fancier remove_if above
      for ( ;mapIt!=mapItEnd; ++mapIt) {
	if ( mapIt->second==&track ) {
	  maps.m_prepRawDataTrackMap.erase( mapIt );
	  break;//should only ever be one Track
	}
      }
    }
  
  // remove cached PRD vector
  maps.m_trackPrepRawDataMap.erase( itvec );
  
  if (msgLvl(MSG::DEBUG)) msg()<<"Removed  PRDs from track ("
			       <<&track<<") \t- map has changed size from \t"
			       <<oldSize <<" \tto "<<maps.m_prepRawDataTrackMap.size()<<endmsg;
  return StatusCode::SUCCESS;
}
 
Trk::IPRD_AssociationTool::TrackSet
iFatras::ISF_PRD_AssociationTool::findConnectedTracks( const Trk::Track& track ) const
{
  return findConnectedTracks (m_maps, track);
}

Trk::IPRD_AssociationTool::TrackSet
iFatras::ISF_PRD_AssociationTool::findConnectedTracks( const Maps& maps,
                                                       const Trk::Track& track ) const
{
  TrackSet connectedTracks;
  
  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack(maps,track);
  for (const Trk::PrepRawData* prd : prds) {
    Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange range = onTracks(maps, *prd);
    // add them into the list
    for ( ; range.first!=range.second; ++(range.first) )
      connectedTracks.insert((range.first)->second);
  }
  
  // don't forget to remove the input track
  connectedTracks.erase(&track);
  
  if (msgLvl(MSG::VERBOSE)) msg()<<"Added in connected tracks for track "<<&track
				 << "\tsize of list is "<<connectedTracks.size()<<endmsg;
  
  return connectedTracks;
}

std::vector< const Trk::PrepRawData* >
iFatras::ISF_PRD_AssociationTool::getPrdsOnTrack(const Trk::Track& track) const
{
  return getPrdsOnTrack (m_maps, track);
}


std::vector< const Trk::PrepRawData* >
iFatras::ISF_PRD_AssociationTool::getPrdsOnTrack(const Maps& maps,
                                                 const Trk::Track& track) const
{
   typedef std::vector<const Trk::PrepRawData*> PRDs_t;
 
   // test caching
   TrackPrepRawDataMap::const_iterator itvec = maps.m_trackPrepRawDataMap.find(&track);
   if (itvec!=maps.m_trackPrepRawDataMap.end())
   {
     msg(MSG::VERBOSE)<<"found track in cache, return cached PRD vector for track"<<endmsg;
     return itvec->second;
   }
 
   if (track.measurementsOnTrack()==0) {
     msg(MSG::WARNING)<<"Track has no RoTs"<<endmsg;
     return PRDs_t(); // return vector optimization
    }
 
   // output vector
   PRDs_t vec;
   // size it
   vec.reserve(track.measurementsOnTrack()->size());
 
   // get the PRDs for the measuremenst on track
   for (const Trk::MeasurementBase* meas : *track.measurementsOnTrack())
   {
     const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(meas);
     if (0!=rot)
       vec.push_back(rot->prepRawData());
   }
   
   if (msgLvl(MSG::DEBUG)) msg()<<" Getting "<<vec.size()
				<<" PRDs from track at:"<<&track<<endmsg;
   return vec;
}
 
Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange 
iFatras::ISF_PRD_AssociationTool::onTracks(const Trk::PrepRawData& prd) const
{
  return onTracks (m_maps, prd);
}


Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange
iFatras::ISF_PRD_AssociationTool::onTracks(const Maps& maps,
                                           const Trk::PrepRawData& prd) const
{
     return maps.m_prepRawDataTrackMap.equal_range(&prd);
}

void iFatras::ISF_PRD_AssociationTool::reset()
{
  m_maps.m_prepRawDataTrackMap.clear();
  m_maps.m_trackPrepRawDataMap.clear();
}
