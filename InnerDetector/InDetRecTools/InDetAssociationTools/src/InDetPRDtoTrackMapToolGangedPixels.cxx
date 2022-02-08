/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetAssociationTools/InDetPRDtoTrackMapToolGangedPixels.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include <memory>

#include <vector>
InDet::InDetPRDtoTrackMapToolGangedPixels::InDetPRDtoTrackMapToolGangedPixels(const std::string& t,
                                                                              const std::string& n,
                                                                              const IInterface*  p)
  : base_class(t,n,p)
{
  declareProperty( "PixelClusterAmbiguitiesMapName", m_pixelClusterAmbiguitiesMapName = "PixelClusterAmbiguitiesMap" );
  declareProperty( "addTRToutliers", m_addTRToutliers = false);
}

InDet::InDetPRDtoTrackMapToolGangedPixels::~InDetPRDtoTrackMapToolGangedPixels()
{}

StatusCode InDet::InDetPRDtoTrackMapToolGangedPixels::initialize()
{
  bool has_ambi_map = m_pixelClusterAmbiguitiesMapName.initialize(!m_pixelClusterAmbiguitiesMapName.key().empty()).isSuccess();
  if (!has_ambi_map && !m_pixelClusterAmbiguitiesMapName.key().empty()) {
    ATH_MSG_WARNING("Could not retrieve "<< m_pixelClusterAmbiguitiesMapName.key() << " this is ok if pixel is off");
    // @TODO introduce more robust method  e.g. dummy tool  if pixel is off ?
  }

  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetPRDtoTrackMapToolGangedPixels::finalize()
{
  return StatusCode::SUCCESS;
}

std::unique_ptr<Trk::PRDtoTrackMap> InDet::InDetPRDtoTrackMapToolGangedPixels::createPRDtoTrackMap() const {
  std::unique_ptr<PRDtoTrackMap> prd_to_track_map( new PRDtoTrackMap(m_pixelClusterAmbiguitiesMapName ) );

  ATH_MSG_DEBUG("Retrieved " << m_pixelClusterAmbiguitiesMapName.key()
                << ", number of entries for this event:"
                << (prd_to_track_map->m_gangedAmbis.isValid()  ? prd_to_track_map->m_gangedAmbis->size() : -1) );
  return std::unique_ptr<Trk::PRDtoTrackMap>(prd_to_track_map.release());
}

std::unique_ptr<Trk::PRDtoTrackMap> InDet::InDetPRDtoTrackMapToolGangedPixels::reduceToStorableMap(std::unique_ptr<Trk::PRDtoTrackMap> &&obj_in) const {
  if (obj_in->getType()!=typeid(Trk::PRDtoTrackMap)) {
    return std::move(obj_in);
  }
  if (obj_in->getType()!=typeid(InDet::InDetPRDtoTrackMapToolGangedPixels)) {
    ATH_MSG_FATAL( "Type mismap between tool and map. Expecting " << typeid(InDet::InDetPRDtoTrackMapToolGangedPixels).name()
                   << " but got " << obj_in->getType().name()
                   << " . The tool can only handle a map created by the same tool. ");
  }
  Trk::PRDtoTrackMap *the_obj=static_cast<PRDtoTrackMap *>(obj_in.get());
  return std::make_unique<Trk::PRDtoTrackMap>(std::move(*the_obj));
}


void InDet::InDetPRDtoTrackMapToolGangedPixels::ensureType(Trk::PRDtoTrackMap &virt_prd_to_track_map) const 
{
  if (virt_prd_to_track_map.getType() != typeid(InDetPRDtoTrackMapToolGangedPixels::PRDtoTrackMap)) {
    ATH_MSG_FATAL( "Type mismap between tool and map. Expecting " << typeid(InDetPRDtoTrackMapToolGangedPixels).name()
                   << " but got " << virt_prd_to_track_map.getType().name()
                   << " . The tool can only handle a map created by the same tool. ");
    throw std::logic_error("Type mismap between tool and map");
  }
  assert( dynamic_cast<InDetPRDtoTrackMapToolGangedPixels::PRDtoTrackMap *>(&virt_prd_to_track_map) != nullptr );
}


StatusCode InDet::InDetPRDtoTrackMapToolGangedPixels::addPRDs(Trk::PRDtoTrackMap &virt_prd_to_track_map, const Trk::Track& track ) const
{
  ensureType(virt_prd_to_track_map);
  PRDtoTrackMap &prd_to_track_map = static_cast<PRDtoTrackMap&>(virt_prd_to_track_map);
  // test caching
  Trk::PRDtoTrackMap::TrackPrepRawDataMap::const_iterator itvec = prd_to_track_map.m_trackPrepRawDataMap.find(&track);
  if (itvec!=prd_to_track_map.m_trackPrepRawDataMap.end())
  {
    ATH_MSG_ERROR("track already found in cache, should not happen");
    return StatusCode::FAILURE;
  }
  // get all prds on 'track'
  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack(prd_to_track_map, track );
  // loop over PRD
  for (const Trk::PrepRawData* prd : prds) {
     prd_to_track_map.m_prepRawDataTrackMap.insert(std::make_pair(prd, &track) );
     // test ganged ambiguity
     if (prd->type(Trk::PrepRawDataType::PixelCluster)) {
       const PixelCluster* pixel = static_cast<const PixelCluster*> (prd);
       if (pixel->gangedPixel()) {
         ATH_MSG_DEBUG( "Found ganged pixel, search for mirror" );
	       std::pair<PixelGangedClusterAmbiguities::const_iterator,
	           PixelGangedClusterAmbiguities::const_iterator> ambi = prd_to_track_map.m_gangedAmbis->equal_range(pixel);
         for (; ambi.first != ambi.second ; ++(ambi.first) ) {
           // add ambiguity as used by this track as well
           if (msgLvl(MSG::DEBUG)) msg() << "Found mirror pixel, add mirror to association map" << endmsg;
           prd_to_track_map.m_prepRawDataTrackMap.insert(std::make_pair(ambi.first->second, &track) );
         }
       }
     }
  }

  // cache this using m_trackPrepRawDataMap
  prd_to_track_map.m_trackPrepRawDataMap.insert( std::make_pair(&track, prds) );

  ATH_MSG_DEBUG("Added PRDs from Track at ("<<&track<<") - map now has size: \t"
                <<prd_to_track_map.m_prepRawDataTrackMap.size() );
  return StatusCode::SUCCESS;
}


Trk::IPRDtoTrackMapTool::TrackSet
InDet::InDetPRDtoTrackMapToolGangedPixels::findConnectedTracks(Trk::PRDtoTrackMap &virt_prd_to_track_map,
                                                              const Trk::Track& track ) const
{
  ensureType(virt_prd_to_track_map);
  PRDtoTrackMap &prd_to_track_map = static_cast<PRDtoTrackMap&>(virt_prd_to_track_map);
  
  TrackSet connectedTracks;
  
  std::vector< const Trk::PrepRawData* > prds = getPrdsOnTrack(virt_prd_to_track_map, track);
  for (const Trk::PrepRawData* prd : prds) {
    Trk::PRDtoTrackMap::ConstPrepRawDataTrackMapRange range = prd_to_track_map.onTracks(*prd);
    // add them into the list
    for ( ; range.first!=range.second; ++(range.first) )
      connectedTracks.insert((range.first)->second);

    // test ganged ambiguity
    
    if (prd->type(Trk::PrepRawDataType::PixelCluster)) {
      const PixelCluster* pixel = static_cast<const PixelCluster*> (prd);
      if (pixel->gangedPixel()) {
	      std::pair<PixelGangedClusterAmbiguities::const_iterator,
	          PixelGangedClusterAmbiguities::const_iterator> ambi = prd_to_track_map.m_gangedAmbis->equal_range(pixel);
        for (; ambi.first != ambi.second ; ++(ambi.first) ) {
          range = prd_to_track_map.onTracks( *(ambi.first->second) );
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
InDet::InDetPRDtoTrackMapToolGangedPixels::getPrdsOnTrack(Trk::PRDtoTrackMap &virt_prd_to_track_map,
                                                            const Trk::Track& track) const
{
  ensureType(virt_prd_to_track_map);
  PRDtoTrackMap &prd_to_track_map = static_cast<PRDtoTrackMap&>(virt_prd_to_track_map);
  using PRDs_t = std::vector<const Trk::PrepRawData *>;

  // test caching
  Trk::PRDtoTrackMap::TrackPrepRawDataMap::const_iterator itvec = prd_to_track_map.m_trackPrepRawDataMap.find(&track);
  if (itvec!=prd_to_track_map.m_trackPrepRawDataMap.end())
  {
    ATH_MSG_VERBOSE("found track in cache, return cached PRD vector for track");
    return itvec->second;
  }

  if (track.measurementsOnTrack()==nullptr) {
    ATH_MSG_WARNING("Track has no RoTs");
    return {}; // return vector optimization
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
  for (;it!=itEnd;++it)
    {
    const auto meas{*it};
    if (meas->type(Trk::MeasurementBaseType::RIO_OnTrack)) {
    const Trk::RIO_OnTrack* rot = static_cast<const Trk::RIO_OnTrack*>(meas);
      vec.push_back(rot->prepRawData());
    }
  }
  ATH_MSG_DEBUG(" Getting "<<vec.size()<<" PRDs from track at:"<<&track);
  // new mode, we add the outleirs in the TRT
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

InDet::InDetPRDtoTrackMapToolGangedPixels::PRDtoTrackMap::PRDtoTrackMap(const SG::ReadHandleKey<PixelGangedClusterAmbiguities> &key)   
{
  if (!key.key().empty()) {
    m_gangedAmbis=SG::makeHandle(key);
  }
}

