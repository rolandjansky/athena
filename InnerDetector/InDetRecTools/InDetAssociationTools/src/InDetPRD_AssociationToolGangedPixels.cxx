/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetAssociationTools/InDetPRD_AssociationToolGangedPixels.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include <vector>

InDet::InDetPRD_AssociationToolGangedPixels::InDetPRD_AssociationToolGangedPixels(const std::string& t,
										  const std::string& n,
										  const IInterface*  p ) :
  base_class(t,n,p)
{
  declareProperty( "PixelClusterAmbiguitiesMapName", m_pixelClusterAmbiguitiesMapName = "PixelClusterAmbiguitiesMap" );
  declareProperty( "addTRToutliers", m_addTRToutliers = false);
  declareProperty( "SetupCorrect",  m_setupCorrect=false);
}

InDet::InDetPRD_AssociationToolGangedPixels::~InDetPRD_AssociationToolGangedPixels()
{}

StatusCode InDet::InDetPRD_AssociationToolGangedPixels::initialize()
{
  ATH_CHECK( m_pixelClusterAmbiguitiesMapName.initialize() );
  if (!m_setupCorrect) {
    ATH_MSG_WARNING("Tool " << name() << " not configured.");
  }
  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetPRD_AssociationToolGangedPixels::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetPRD_AssociationToolGangedPixels::addPRDs( const Trk::Track& track )
{
  return addPRDs (m_maps, track);
}

StatusCode InDet::InDetPRD_AssociationToolGangedPixels::addPRDs( Maps& maps,
                                                                 const Trk::Track& track ) const
{
  // test caching
  TrackPrepRawDataMap::const_iterator itvec = maps.m_trackPrepRawDataMap.find(&track);
  if (itvec!=maps.m_trackPrepRawDataMap.end())
  {
    ATH_MSG_ERROR("track already found in cache, should not happen");
    return StatusCode::FAILURE;
  }
  // get all prds on 'track'
  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack( maps, track );

  SG::ReadHandle<PixelGangedClusterAmbiguities> gangedAmbis (m_pixelClusterAmbiguitiesMapName);

  // loop over PRD
  for (const Trk::PrepRawData* prd : prds) {
     maps.m_prepRawDataTrackMap.emplace(prd, &track);
     // test ganged ambiguity
     if (prd->type(Trk::PrepRawDataType::PixelCluster)) {
       const PixelCluster* pixel = static_cast<const PixelCluster*> (prd);
       if (pixel->gangedPixel()) {
	       ATH_MSG_DEBUG( "Found ganged pixel, search for mirror" );
	       std::pair<PixelGangedClusterAmbiguities::const_iterator,
	           PixelGangedClusterAmbiguities::const_iterator> ambi = gangedAmbis->equal_range(pixel);
	       for (; ambi.first != ambi.second ; ++(ambi.first) ) {
	       // add ambiguity as used by this track as well
	         ATH_MSG_DEBUG( "Found mirror pixel, add mirror to association map" );
	         maps.m_prepRawDataTrackMap.emplace(ambi.first->second, &track);
	       }
       }
     }
  }

  // cache this using maps.m_trackPrepRawDataMap
  maps.m_trackPrepRawDataMap.emplace(&track, prds);
    
  ATH_MSG_DEBUG("Added PRDs from Track at ("<<&track<<") - map now has size: \t"
			       <<maps.m_prepRawDataTrackMap.size());
  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetPRD_AssociationToolGangedPixels::removePRDs( const Trk::Track& track )
{
  return removePRDs (m_maps, track);
}

StatusCode InDet::InDetPRD_AssociationToolGangedPixels::removePRDs( Maps& maps,
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
    ATH_MSG_ERROR("Track not found in cache, this should not happen");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<PixelGangedClusterAmbiguities> gangedAmbis (m_pixelClusterAmbiguitiesMapName);

  // get all prds on 'track'
  std::vector< const Trk::PrepRawData* > prds = itvec->second;
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

    // test ganged ambiguity
    
    if (prd->type(Trk::PrepRawDataType::PixelCluster)) {
      const PixelCluster* pixel = static_cast<const PixelCluster*> (prd);
      if (pixel->gangedPixel()) {
	      std::pair<PixelGangedClusterAmbiguities::const_iterator,
	          PixelGangedClusterAmbiguities::const_iterator> ambi = gangedAmbis->equal_range(pixel);
        for (; ambi.first != ambi.second ; ++(ambi.first) ) {
          // add ambiguity as used by this track as well
          ATH_MSG_DEBUG("Found ganged pixel, remove also mirror from association map");

          range = maps.m_prepRawDataTrackMap.equal_range(ambi.first->second);
          // get iterators for range
          mapIt    = range.first;
          mapItEnd = range.second;
          // simple for loop instead of fancier remove_if above
          for ( ;mapIt!=mapItEnd; ++mapIt) {
            if ( mapIt->second==&track ) {
              maps.m_prepRawDataTrackMap.erase( mapIt );
              break;//should only ever be one Track
            }
          }
        }
      }
    }

  }
 
  // remove cached PRD vector
  maps.m_trackPrepRawDataMap.erase( itvec );
 
  ATH_MSG_DEBUG("Removed  PRDs from track ("
			       <<&track<<") \t- map has changed size from \t"
			       <<oldSize <<" \tto "<<maps.m_prepRawDataTrackMap.size());
  return StatusCode::SUCCESS;
}

Trk::IPRD_AssociationTool::TrackSet
InDet::InDetPRD_AssociationToolGangedPixels::findConnectedTracks( const Trk::Track& track ) const
{
  return findConnectedTracks (m_maps, track);
}

Trk::IPRD_AssociationTool::TrackSet
InDet::InDetPRD_AssociationToolGangedPixels::findConnectedTracks( const Maps& maps,
                                                                  const Trk::Track& track )  const
{
  TrackSet connectedTracks;
  
  SG::ReadHandle<PixelGangedClusterAmbiguities> gangedAmbis (m_pixelClusterAmbiguitiesMapName);

  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack(maps, track);
  for (const Trk::PrepRawData* prd : prds) {
    Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange range = onTracks(maps, *prd);
    // add them into the list
    for ( ; range.first!=range.second; ++(range.first) )
      connectedTracks.insert((range.first)->second);

    // test ganged ambiguity
    
    if (prd->type(Trk::PrepRawDataType::PixelCluster)) {
      const PixelCluster* pixel = static_cast<const PixelCluster*> (prd);
      if (pixel->gangedPixel()) {
	      std::pair<PixelGangedClusterAmbiguities::const_iterator,
	          PixelGangedClusterAmbiguities::const_iterator> ambi = gangedAmbis->equal_range(pixel);
        for (; ambi.first != ambi.second ; ++(ambi.first) ) {
          range = onTracks( maps, *(ambi.first->second) );
          // add them into the list
          for ( ; range.first!=range.second; ++(range.first) )
            connectedTracks.insert((range.first)->second);
        }
      }
    }
  }

  // don't forget to remove the input track
  connectedTracks.erase(&track);

  ATH_MSG_VERBOSE("Added in connected tracks for track "<<&track
				 << "\tsize of list is "<<connectedTracks.size());

  return connectedTracks;
}


std::vector< const Trk::PrepRawData* >
InDet::InDetPRD_AssociationToolGangedPixels::getPrdsOnTrack(const Trk::Track& track) const
{
  return getPrdsOnTrack (m_maps, track);
}


std::vector< const Trk::PrepRawData* >
InDet::InDetPRD_AssociationToolGangedPixels::getPrdsOnTrack(const Maps& maps,
                                                            const Trk::Track& track) const
{
  using PRDs_t = std::vector<const Trk::PrepRawData *>;

  // test caching
  TrackPrepRawDataMap::const_iterator itvec = maps.m_trackPrepRawDataMap.find(&track);
  if (itvec!=maps.m_trackPrepRawDataMap.end())
  {
    ATH_MSG_VERBOSE("found track in cache, return cached PRD vector for track");
    return itvec->second;
  }

  if (track.measurementsOnTrack()==nullptr) {
    ATH_MSG_WARNING("Track has no RoTs");
    return {}; // return vector optimization
   }


  // output vector
  PRDs_t vec;
  // size it
  vec.reserve(track.measurementsOnTrack()->size());

  // get the PRDs for the measuremenst on track
  DataVector<const Trk::MeasurementBase>::const_iterator it    = track.measurementsOnTrack()->begin();
  DataVector<const Trk::MeasurementBase>::const_iterator itEnd = track.measurementsOnTrack()->end();
  for (;it!=itEnd;++it){
    const auto  pThisMeasurement(*it);
    if (pThisMeasurement->type(Trk::MeasurementBaseType::RIO_OnTrack)){
      const Trk::RIO_OnTrack* rot = static_cast<const Trk::RIO_OnTrack*>(pThisMeasurement);
      vec.push_back(rot->prepRawData());
    }
  }
  ATH_MSG_DEBUG(" Getting "<<vec.size()<<" PRDs from track at:"<<&track);
  // new mode, we add the outliers in the TRT
  if (m_addTRToutliers) {
    // get the PRDs for the measuremenst on track
    for (const Trk::MeasurementBase* meas : *track.outliersOnTrack()){
      // get the ROT, make sure it is not a pseudo measurment
      if (meas->type(Trk::MeasurementBaseType::RIO_OnTrack)) {
        const Trk::RIO_OnTrack* rot = static_cast<const Trk::RIO_OnTrack*>(meas);
        // check if outlier is TRT ?
        if (rot->rioType(Trk::RIO_OnTrackType::TRT_DriftCircle)) {
          // add to the list, it is TRT
          vec.push_back(rot->prepRawData());
        }
      }
    }
    ATH_MSG_DEBUG(" Getting "<<vec.size()<<" PRDs including TRT outlier from track at:"<<&track);
  }

  return vec;
}

Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange 
InDet::InDetPRD_AssociationToolGangedPixels::onTracks(const Trk::PrepRawData& prd) const
{
  return onTracks (m_maps, prd);
}

Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange 
InDet::InDetPRD_AssociationToolGangedPixels::onTracks(const Maps& maps,
                                                      const Trk::PrepRawData& prd) const
{
  return maps.m_prepRawDataTrackMap.equal_range(&prd);
}

void InDet::InDetPRD_AssociationToolGangedPixels::reset()
{
  m_maps.m_prepRawDataTrackMap.clear();
  m_maps.m_trackPrepRawDataMap.clear();
}

