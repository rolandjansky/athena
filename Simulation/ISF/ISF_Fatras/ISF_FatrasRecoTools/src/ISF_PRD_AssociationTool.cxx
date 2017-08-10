/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  AthAlgTool(t,n,p)
{
  declareInterface<IPRD_AssociationTool>(this);
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
  using namespace std;
  
  // test caching
  TrackPrepRawDataMap::const_iterator itvec = m_trackPrepRawDataMap.find(&track);
  if (itvec!=m_trackPrepRawDataMap.end())
    {
      msg(MSG::ERROR)<<"track already found in cache, should not happen"<<endmsg;
      return StatusCode::FAILURE;
    }
  
  // get all prds on 'track'
  vector< const Trk::PrepRawData* > prds = getPrdsOnTrack( track );
  vector< const Trk::PrepRawData* >::const_iterator it    = prds.begin();
  vector< const Trk::PrepRawData* >::const_iterator itEnd = prds.end(); 
  
  // loop over PRD
  for (; it!=itEnd; ++it) 
    m_prepRawDataTrackMap.insert(std::make_pair(*it, &track) );
  
  // cache this using m_trackPrepRawDataMap
  m_trackPrepRawDataMap.insert( std::make_pair(&track, prds) );
     
  if (msgLvl(MSG::DEBUG)) msg()<<"Added PRDs from Track at ("<<&track<<") - map now has size: \t"
			       <<m_prepRawDataTrackMap.size()<<endmsg;
  return StatusCode::SUCCESS;
}
 
StatusCode iFatras::ISF_PRD_AssociationTool::removePRDs( const Trk::Track& track )
{
  using namespace std;
 
  // This is NOT pretty code!
  // At the moment I'm brute-forcing, but maybe I need a second map, containing <Track*, iterator>
  // The problem is that I think filling such a map is also time-consuming.
  // Since removes happen much less frequently than add, then the slow bit should be here. 
  // EJWM
 
  // save for debugging purposes
  int oldSize = m_prepRawDataTrackMap.size();//used in debug output at end.
 
  // test caching
  TrackPrepRawDataMap::iterator itvec = m_trackPrepRawDataMap.find(&track);
  if (itvec==m_trackPrepRawDataMap.end())
    {
      msg(MSG::ERROR)<<"Track not found in cache, this should not happen"<<endmsg;
      return StatusCode::FAILURE;
    }
  
  // get all prds on 'track'
  vector< const Trk::PrepRawData* > prds = itvec->second;
  vector< const Trk::PrepRawData* >::const_iterator it    = prds.begin();
  vector< const Trk::PrepRawData* >::const_iterator itEnd = prds.end(); 
  
  // loop over PRD
  for (; it!=itEnd; ++it)
    {
      // now get all map elements (i.e. Tracks) that contain this PRD
      Trk::PrepRawData* prd = const_cast<Trk::PrepRawData*>(*it);
      
      Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange 
	range = m_prepRawDataTrackMap.equal_range(prd);
      // get iterators for range
      PRD_MapIt mapIt    = range.first;
      PRD_MapIt mapItEnd = range.second;
      // simple for loop instead of fancier remove_if above
      for ( ;mapIt!=mapItEnd; ++mapIt) {
	if ( mapIt->second==&track ) {
	  m_prepRawDataTrackMap.erase( mapIt );
	  break;//should only ever be one Track
	}
      }
    }
  
  // remove cached PRD vector
  m_trackPrepRawDataMap.erase( itvec );
  
  if (msgLvl(MSG::DEBUG)) msg()<<"Removed  PRDs from track ("
			       <<&track<<") \t- map has changed size from \t"
			       <<oldSize <<" \tto "<<m_prepRawDataTrackMap.size()<<endmsg;
  return StatusCode::SUCCESS;
}
 
Trk::IPRD_AssociationTool::TrackSet iFatras::ISF_PRD_AssociationTool::findConnectedTracks( const Trk::Track& track ) 
{
  using namespace std;
  TrackSet connectedTracks;
  
  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack(track);
  std::vector< const Trk::PrepRawData* >::const_iterator it    = prds.begin();
  std::vector< const Trk::PrepRawData* >::const_iterator itEnd = prds.end();
  for ( ; it!=itEnd; it++) {
    Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange range = onTracks(**it);
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

std::vector< const Trk::PrepRawData* > iFatras::ISF_PRD_AssociationTool::getPrdsOnTrack(const Trk::Track& track) const
{
   typedef std::vector<const Trk::PrepRawData*> PRDs_t;
 
   // test caching
   TrackPrepRawDataMap::const_iterator itvec = m_trackPrepRawDataMap.find(&track);
   if (itvec!=m_trackPrepRawDataMap.end())
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
   DataVector<const Trk::MeasurementBase>::const_iterator it    = track.measurementsOnTrack()->begin();
   DataVector<const Trk::MeasurementBase>::const_iterator itEnd = track.measurementsOnTrack()->end();
   for (;it!=itEnd;it++)
     {
     const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*it);
     if (0!=rot)
       vec.push_back(rot->prepRawData());
   }
   
   if (msgLvl(MSG::DEBUG)) msg()<<" Getting "<<vec.size()
				<<" PRDs from track at:"<<&track<<endmsg;
   return vec;
}
 
Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange iFatras::ISF_PRD_AssociationTool::onTracks(const Trk::PrepRawData& prd) 
{
     return m_prepRawDataTrackMap.equal_range(&prd);
}

void iFatras::ISF_PRD_AssociationTool::reset()
{
  m_prepRawDataTrackMap.clear();
  m_trackPrepRawDataMap.clear();
}
