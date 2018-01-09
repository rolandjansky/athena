/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAssociationTools/PRD_AssociationTool.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventUtils/CreatePRD_MapPairFromTrack.h"
#include "TrkEventUtils/CreatePRD_VectorFromTrack.h"

#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include <cassert>
#include <vector>
//#include <functional>
#include "ext/functional"
//#include "boost/mpl/select1st.hpp"

Trk::PRD_AssociationTool::PRD_AssociationTool(const std::string& t,
  const std::string& n,
  const IInterface*  p )
  :
  AthAlgTool(t,n,p)
{
  declareInterface<IPRD_AssociationTool>(this);
}

Trk::PRD_AssociationTool::~PRD_AssociationTool()
{
}

StatusCode Trk::PRD_AssociationTool::initialize()
{
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;
  return StatusCode::SUCCESS;
}

StatusCode Trk::PRD_AssociationTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

StatusCode Trk::PRD_AssociationTool::addPRDs( const Trk::Track& track )
{
  using namespace std;

  // test caching
  TrackPrepRawDataMap::const_iterator itvec = m_trackPrepRawDataMap.find(&track);
  if (itvec!=m_trackPrepRawDataMap.end())
  {
    ATH_MSG_ERROR ("track already found in cache, should not happen");
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
    
  ATH_MSG_DEBUG ("Added PRDs from Track at ("<<&track<<") - map now has size: \t"<<
                 m_prepRawDataTrackMap.size());
  return StatusCode::SUCCESS;
}

StatusCode Trk::PRD_AssociationTool::removePRDs( const Trk::Track& track )
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
    ATH_MSG_ERROR ("Track not found in cache, this should not happen");
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
    PrepRawData* prd = const_cast<PrepRawData*>(*it);
    IPRD_AssociationTool::PrepRawDataTrackMapRange 
        range = m_prepRawDataTrackMap.equal_range(prd);

    // get iterators for range
    ConstPRD_MapIt mapIt    = range.first;
    ConstPRD_MapIt mapItEnd = range.second;

// FIXME - doesn't compile. Out of time -  come back to it later
// 		remove_if(
//  			mapIt, mapItEnd, 
//  			compose1(
//  				bind2nd(equal_to<const Track*>(), &track), 
//  				select2nd<PrepRawDataTrackMap::value_type>()
//  				)
//  			);

    // simple for loop instead of fancier remove_if above
    for ( ;mapIt!=mapItEnd; ++mapIt)
    {
      if ( mapIt->second==&track ) 
      {
        m_prepRawDataTrackMap.erase( mapIt );
        break;//should only ever be one Track
      }
    }
  }
 
  // remove cached PRD vector
  m_trackPrepRawDataMap.erase( itvec );
 
  ATH_MSG_DEBUG ("Removed  PRDs from track (" <<&track<<") \t- map has changed "<<
                 "size from \t"<<oldSize <<" \tto "<<m_prepRawDataTrackMap.size());
  return StatusCode::SUCCESS;
}

Trk::IPRD_AssociationTool::TrackSet
    Trk::PRD_AssociationTool::findConnectedTracks( const Trk::Track& track ) 
{
  using namespace std;
  //using namespace __gnu_cxx;
  
  TrackSet connectedTracks;
  
  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack(track);
  std::vector< const Trk::PrepRawData* >::const_iterator it    = prds.begin();
  std::vector< const Trk::PrepRawData* >::const_iterator itEnd = prds.end();
  for ( ; it!=itEnd; it++)
  {
    IPRD_AssociationTool::PrepRawDataTrackMapRange range = onTracks(**it);
   
    // TODO use remove_copy_if instead.
    for ( ; range.first!=range.second; ++(range.first) )
    {
      const Track* conTrack = (range.first)->second;
      // don't copy this track!
      if (conTrack!=&track) {
	// this does actually not allow for double entries
        connectedTracks.insert(conTrack);
        ATH_MSG_VERBOSE ("Track "<<&track<<" \tshares PRD "<<*it<<" \twith track:"<<conTrack);
      }
    }
    ATH_MSG_VERBOSE ("Added in connected tracks for PRD:"<<*it<<
                     "\tsize of list now:"<<connectedTracks.size());
  }

  return connectedTracks;
}


std::vector< const Trk::PrepRawData* > Trk::PRD_AssociationTool::getPrdsOnTrack(const Trk::Track& track) const
{
  typedef std::vector<const PrepRawData*> PRDs_t;

  // test caching
  TrackPrepRawDataMap::const_iterator itvec = m_trackPrepRawDataMap.find(&track);
  if (itvec!=m_trackPrepRawDataMap.end())
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

  DataVector<const MeasurementBase>::const_iterator it    = track.measurementsOnTrack()->begin();
  DataVector<const MeasurementBase>::const_iterator itEnd = track.measurementsOnTrack()->end();
  PRDs_t vec;
  vec.reserve(track.measurementsOnTrack()->size());
  for (;it!=itEnd;it++)
  {
    const RIO_OnTrack* rot = dynamic_cast<const RIO_OnTrack*>(*it);
    if (0!=rot)
      vec.push_back(rot->prepRawData());
  }
  ATH_MSG_DEBUG (" Getting "<<vec.size()<<" PRDs from track at:"<<&track);

  return vec;
}

Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange 
    Trk::PRD_AssociationTool::onTracks(const PrepRawData& prd) const
{
//	std::pair<IPRD_AssociationTool::PRD_MapIt, IPRD_AssociationTool::PRD_MapIt>       range = 
  return m_prepRawDataTrackMap.equal_range(&prd);
}

void Trk::PRD_AssociationTool::reset()
{
  m_prepRawDataTrackMap.clear();
  m_trackPrepRawDataMap.clear();
}

