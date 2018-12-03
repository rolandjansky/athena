/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetAssociationTools/InDetPRD_AssociationToolGangedPixels.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include <cassert>
#include <vector>
#include "ext/functional"

InDet::InDetPRD_AssociationToolGangedPixels::InDetPRD_AssociationToolGangedPixels(const std::string& t,
										  const std::string& n,
										  const IInterface*  p ) :
  AthAlgTool(t,n,p)
{
  declareInterface<IPRD_AssociationTool>(this);
  declareProperty( "PixelClusterAmbiguitiesMapName", m_pixelClusterAmbiguitiesMapName = "PixelClusterAmbiguitiesMap" );
  declareProperty( "addTRToutliers", m_addTRToutliers = false);
}

InDet::InDetPRD_AssociationToolGangedPixels::~InDetPRD_AssociationToolGangedPixels()
{}

StatusCode InDet::InDetPRD_AssociationToolGangedPixels::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetPRD_AssociationToolGangedPixels::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

StatusCode InDet::InDetPRD_AssociationToolGangedPixels::addPRDs( const Trk::Track& track )
{
  using namespace std;

  // test caching
  TrackPrepRawDataMap::const_iterator itvec = m_trackPrepRawDataMap.find(&track);
  if (itvec!=m_trackPrepRawDataMap.end())
  {
    ATH_MSG_ERROR("track already found in cache, should not happen");
    return StatusCode::FAILURE;
  }
  // get all prds on 'track'
  vector< const Trk::PrepRawData* > prds = getPrdsOnTrack( track );
  vector< const Trk::PrepRawData* >::const_iterator it    = prds.begin();
  vector< const Trk::PrepRawData* >::const_iterator itEnd = prds.end();	

  // loop over PRD
  for (; it!=itEnd; ++it) {
     m_prepRawDataTrackMap.insert(std::make_pair(*it, &track) );

     // test ganged ambiguity
     const PixelCluster* pixel = dynamic_cast<const PixelCluster*> (*it);
     if (pixel!=0) {
       if (pixel->gangedPixel()) {
         ATH_MSG_DEBUG("Found ganged pixel, search for mirror");
         std::pair<PixelGangedClusterAmbiguities::const_iterator,
           PixelGangedClusterAmbiguities::const_iterator> ambi = m_gangedAmbis->equal_range(pixel);
         for (; ambi.first != ambi.second ; ++(ambi.first) ) {
           // add ambiguity as used by this track as well
           if (msgLvl(MSG::DEBUG)) msg() << "Found mirror pixel, add mirror to association map" << endreq;
           m_prepRawDataTrackMap.insert(std::make_pair(ambi.first->second, &track) );
         }
       }
     }
  }

  // cache this using m_trackPrepRawDataMap
  m_trackPrepRawDataMap.insert( std::make_pair(&track, prds) );
    
  ATH_MSG_DEBUG("Added PRDs from Track at ("<<&track<<") - map now has size: \t" <<m_prepRawDataTrackMap.size());
  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetPRD_AssociationToolGangedPixels::removePRDs( const Trk::Track& track )
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
    msg(MSG::ERROR)<<"Track not found in cache, this should not happen"<<endreq;
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

    // test ganged ambiguity
    const PixelCluster* pixel = dynamic_cast<const PixelCluster*> (*it);
    if (pixel!=0) {
      if (pixel->gangedPixel()) {
	std::pair<PixelGangedClusterAmbiguities::const_iterator,
	          PixelGangedClusterAmbiguities::const_iterator> ambi = m_gangedAmbis->equal_range(pixel);
	for (; ambi.first != ambi.second ; ++(ambi.first) ) {
	  // add ambiguity as used by this track as well
    ATH_MSG_DEBUG("Found ganged pixel, remove also mirror from association map");

	  range = m_prepRawDataTrackMap.equal_range(ambi.first->second);
	  // get iterators for range
	  mapIt    = range.first;
	  mapItEnd = range.second;
	  // simple for loop instead of fancier remove_if above
	  for ( ;mapIt!=mapItEnd; ++mapIt) {
	    if ( mapIt->second==&track ) {
	      m_prepRawDataTrackMap.erase( mapIt );
	      break;//should only ever be one Track
	    }
	  }
	}
      }
    }

  }
 
  // remove cached PRD vector
  m_trackPrepRawDataMap.erase( itvec );
 
  ATH_MSG_DEBUG("Removed  PRDs from track ("
			       <<&track<<") \t- map has changed size from \t"
			       <<oldSize <<" \tto "<<m_prepRawDataTrackMap.size());
  return StatusCode::SUCCESS;
}

Trk::IPRD_AssociationTool::TrackSet
    InDet::InDetPRD_AssociationToolGangedPixels::findConnectedTracks( const Trk::Track& track ) 
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

    // test ganged ambiguity
    const PixelCluster* pixel = dynamic_cast<const PixelCluster*> (*it);
    if (pixel!=0) {
      if (pixel->gangedPixel()) {
	std::pair<PixelGangedClusterAmbiguities::const_iterator,
	          PixelGangedClusterAmbiguities::const_iterator> ambi = m_gangedAmbis->equal_range(pixel);
	for (; ambi.first != ambi.second ; ++(ambi.first) ) {
	  range = onTracks( *(ambi.first->second) );
	  // add them into the list
	  for ( ; range.first!=range.second; ++(range.first) )
	    connectedTracks.insert((range.first)->second);
	}
      }
    }
  }

  // don't forget to remove the input track
  connectedTracks.erase(&track);

  ATH_MSG_VERBOSE("Added in connected tracks for track "<<&track << "\tsize of list is "<<connectedTracks.size());

  return connectedTracks;
}


std::vector< const Trk::PrepRawData* > InDet::InDetPRD_AssociationToolGangedPixels::getPrdsOnTrack(const Trk::Track& track) const
{
  typedef std::vector<const Trk::PrepRawData*> PRDs_t;

  // test caching
  TrackPrepRawDataMap::const_iterator itvec = m_trackPrepRawDataMap.find(&track);
  if (itvec!=m_trackPrepRawDataMap.end())
  {
    ATH_MSG_VERBOSE("found track in cache, return cached PRD vector for track");
    return itvec->second;
  }

  if (track.measurementsOnTrack()==0) {
    ATH_MSG_WARNING("Track has no RoTs");
    return PRDs_t(); // return vector optimization
   }

  // FIXME can I do this without copying the vector?
  /*  transform(
      track.measurementsOnTrack()->begin(), 
      track.measurementsOnTrack()->end() ,
      back_inserter(vec), 
      bind2nd(CreatePRD_VectorFromTrack(), &track) );*/

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
  
  ATH_MSG_DEBUG(" Getting "<<vec.size() <<" PRDs from track at:"<<&track);
  
  // new mode, we add the outleirs in the TRT
  if (m_addTRToutliers) {

    // get the PRDs for the measuremenst on track
    DataVector<const Trk::MeasurementBase>::const_iterator it    = track.outliersOnTrack()->begin();
    DataVector<const Trk::MeasurementBase>::const_iterator itEnd = track.outliersOnTrack()->end();
    for (;it!=itEnd;it++)
      {

	// get the ROT, make sure it is not a pseudo measurment
	const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*it);
	if (0!=rot) {

	  // check if outlier is TRT ?
	  const TRT_DriftCircleOnTrack* trt = dynamic_cast<const TRT_DriftCircleOnTrack*> (rot);
	  if (trt) {
	    // add to the list, it is TRT
	    vec.push_back(rot->prepRawData());
	  }
	}
      }
  
    ATH_MSG_DEBUG(" Getting "<<vec.size() <<" PRDs including TRT outlier from track at:"<<&track);
  }

  return vec;
}

Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange 
InDet::InDetPRD_AssociationToolGangedPixels::onTracks(const Trk::PrepRawData& prd) 
{
  //	std::pair<IPRD_AssociationTool::PRD_MapIt, IPRD_AssociationTool::PRD_MapIt>       range = 
  return m_prepRawDataTrackMap.equal_range(&prd);
}

void InDet::InDetPRD_AssociationToolGangedPixels::reset()
{
  if (evtStore()->contains<PixelGangedClusterAmbiguities>( m_pixelClusterAmbiguitiesMapName ) )
    {      
      StatusCode sc = evtStore()->retrieve(m_gangedAmbis, m_pixelClusterAmbiguitiesMapName );
      
      if (sc.isFailure()) {
        ATH_MSG_ERROR("Could not retrieve "<< m_pixelClusterAmbiguitiesMapName);
      } else {
	ATH_MSG_DEBUG("Retrieved " << m_pixelClusterAmbiguitiesMapName
				      << ", number of entries for this event:"
				      << m_gangedAmbis->size());
      }
    } else {
      ATH_MSG_DEBUG("Could not retrieve "<< m_pixelClusterAmbiguitiesMapName << " this is ok if pixel is off.");
    } 
  m_prepRawDataTrackMap.clear();
  m_trackPrepRawDataMap.clear();
}

