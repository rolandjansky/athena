/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaTrkRefitterTool.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "AthContainers/DataVector.h"

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkVertexOnTrack/VertexOnTrack.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkCaloCluster_OnTrack/CaloCluster_OnTrack.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"


#include <algorithm>
#include <cmath>
#include <vector>


// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ListItem.h"

egammaTrkRefitterTool::egammaTrkRefitterTool(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_ParticleHypothesis(Trk::electron),
  m_idHelper(nullptr)
{
  declareInterface< IegammaTrkRefitterTool >(this) ;
}

egammaTrkRefitterTool::~egammaTrkRefitterTool()
{
}


StatusCode egammaTrkRefitterTool::initialize()
{
  ATH_MSG_DEBUG("Initializing egammaTrackRefitter");

  // Retrieve fitter
  ATH_CHECK(m_ITrackFitter.retrieve());

  // configure Atlas extrapolator
  ATH_CHECK(m_extrapolator.retrieve());

  ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));

  ATH_CHECK(m_beamSpotKey.initialize(m_useBeamSpot));

  // configure calo cluster on track builder (only if used)
  if (m_useClusterPosition) {
    ATH_CHECK(m_CCOTBuilder.retrieve());
  } else {
    m_CCOTBuilder.disable();
  }

  // Set the particle hypothesis to match the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_ParticleHypothesis = particleSwitch.particle[m_matEffects];

  ATH_MSG_INFO("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

StatusCode egammaTrkRefitterTool::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode  egammaTrkRefitterTool::refitElectronTrack(const EventContext& ctx,
                                                      const xAOD::Electron* eg,
                                                      Cache& cache)  const
{
  ATH_MSG_DEBUG("Refitting a track associated  with an electron");
  // protection against bad pointers
  if (eg==nullptr) return StatusCode::SUCCESS;
  // Set the pointer to the egamma object.
  cache.electron = eg;
  const xAOD::TrackParticle *trackParticle = eg->trackParticle();
  return refitTrackParticle(ctx,trackParticle,cache);
}

StatusCode egammaTrkRefitterTool::refitTrackParticle(const EventContext& ctx,
                                                     const xAOD::TrackParticle* trackParticle,
                                                     Cache& cache) const
{
  ATH_MSG_DEBUG("Refitting a track associated  with a TrackParticle");
  // protection against bad pointers
  if ( !trackParticle ) {
    ATH_MSG_DEBUG("No TrackParticle");
    return StatusCode::FAILURE;
  }
  int nSiliconHits_trk(0);
  uint8_t dummy(0);
  if(trackParticle->summaryValue(dummy,xAOD::numberOfSCTHits)){
    nSiliconHits_trk += dummy;
  }
  if(trackParticle->summaryValue(dummy,xAOD::numberOfPixelHits)){
    nSiliconHits_trk += dummy;
  }

  if( nSiliconHits_trk <= m_MinNoSiHits ) {
    ATH_MSG_DEBUG( "Not enough Si hits on track particle.  The current min is set at " << m_MinNoSiHits );
    return StatusCode::FAILURE;
  }

  if ( trackParticle->trackLink().isValid()) {
    // retrieve and refit original track
    return refitTrack( ctx,trackParticle->track() , cache);
  }
    ATH_MSG_WARNING("Could not get TrackElementLink of the TrackParticle");
    return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

StatusCode  egammaTrkRefitterTool::refitTrack(const EventContext& ctx,
                                              const Trk::Track* track,
                                              Cache& cache)  const
{
  cache.refittedTrack=nullptr;
  cache.refittedTrackPerigee=nullptr;
  cache.originalTrack=nullptr;
  cache.originalTrackPerigee=nullptr;
  if (!track) {
    return StatusCode::FAILURE;
  }
  //Set the pointer to the track
  cache.originalTrack=track;

  //Set pointer to the original perigee
  cache.originalTrackPerigee=cache.originalTrack->perigeeParameters();

  if (cache.originalTrackPerigee!=nullptr){
      double od0 = cache.originalTrackPerigee->parameters()[Trk::d0];
      double oz0 = cache.originalTrackPerigee->parameters()[Trk::z0];
      double ophi0 = cache.originalTrackPerigee->parameters()[Trk::phi0];
      double otheta = cache.originalTrackPerigee->parameters()[Trk::theta];
      double oqOverP = cache.originalTrackPerigee->parameters()[Trk::qOverP];
      ATH_MSG_DEBUG("Original parameters " << od0  << " "
        << oz0  << " " << ophi0 << " " << otheta << " " << oqOverP << " " << 1/oqOverP) ;
  } else {
    ATH_MSG_WARNING("Could not get Trk::Perigee of original track");
  }

  // Refit the track with the beam spot if desired otherwise just refit the original track
  if (m_useBeamSpot || m_useClusterPosition){
    egammaTrkRefitterTool::MeasurementsAndTrash collect= addPointsToTrack(ctx,
                                                                          cache.originalTrack,
                                                                          cache.electron);
    if(collect.m_measurements.size()>4){
      cache.refittedTrack =
        m_ITrackFitter->fit(ctx,
                            collect.m_measurements,
                            *cache.originalTrack->perigeeParameters(),
                            m_runOutlier,
                            m_ParticleHypothesis);
    }else {
      ATH_MSG_WARNING("Could **NOT** add BeamSpot information into Vector, refitting without BS");
      cache.refittedTrack = m_ITrackFitter->fit(
        ctx, *cache.originalTrack, m_runOutlier, m_ParticleHypothesis);
    }
  } else {
    std::vector<const Trk::MeasurementBase*>  measurements = getIDHits(cache.originalTrack);
    if(measurements.size()>4){
      cache.refittedTrack =
        m_ITrackFitter->fit(ctx,
                            measurements,
                            *cache.originalTrack->perigeeParameters(),
                            m_runOutlier,
                            m_ParticleHypothesis);
    } else {
      ATH_MSG_DEBUG("Not enough measurements on tracks");
      cache.refittedTrack=nullptr;
    }
  }

  // Store refitted perigee pointers
  if (cache.refittedTrack) {
    cache.refittedTrackPerigee=cache.refittedTrack->perigeeParameters();
    if (cache.refittedTrackPerigee==nullptr){
      ATH_MSG_WARNING("Could not get refitted Trk::Perigee");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
    ATH_MSG_DEBUG("Refit Failed");
    return StatusCode::FAILURE;

}

const Trk::TrackParameters* egammaTrkRefitterTool::lastTrackParameters(const Trk::Track* track) const
{
  ATH_MSG_DEBUG("Getting the final track parameters");

  if (track == nullptr ){
    ATH_MSG_DEBUG("Track == 0  returning  0");
    return nullptr;
  }

  const DataVector<const Trk::TrackStateOnSurface>* oldTrackStates = track->trackStateOnSurfaces();
  if (oldTrackStates == nullptr)
  {
    ATH_MSG_DEBUG("Track has no TSOS vector! Skipping track, returning 0.");
    return nullptr;
  }

  const Trk::TrackParameters* lastValidTrkParameters(nullptr);
  for ( DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rItTSoS = oldTrackStates->rbegin();
        rItTSoS != oldTrackStates->rend(); ++rItTSoS)
  {
    if (lastValidTrkParameters!=nullptr){
      break;
    }
    if ( (*rItTSoS)->type(Trk::TrackStateOnSurface::Measurement) &&
         (*rItTSoS)->trackParameters()!=nullptr && (*rItTSoS)->measurementOnTrack()!=nullptr)
    {
      lastValidTrkParameters = (*rItTSoS)->trackParameters()->clone();
    }
  }

  if (lastValidTrkParameters!=nullptr){
    ATH_MSG_DEBUG ("Last Valid Trk Q/P" << lastValidTrkParameters->parameters()[Trk::qOverP] );
    return lastValidTrkParameters;
  }
  ATH_MSG_DEBUG("Last Track Parameters");
  return nullptr;
}

double egammaTrkRefitterTool::getMaterialTraversed(Trk::Track* track) const {
  ATH_MSG_DEBUG("Calculating Material Traversed by the Track");

  if (!track) return 0.;

  double material(0);
  //Note I am not sure if this is the correct way of accessing the information
  DataVector<const Trk::TrackStateOnSurface>::const_iterator
    trackStateOnSurface = track->trackStateOnSurfaces()->begin();
  for (;trackStateOnSurface < track->trackStateOnSurfaces()->end(); ++ trackStateOnSurface){
    // This could be a problebm
    const Trk:: MaterialEffectsBase* materialEffects = (*trackStateOnSurface)->materialEffectsOnTrack();
    if (materialEffects)
      material += materialEffects->thicknessInX0();
  }
  if (material<=0){
    ATH_MSG_DEBUG(" Material Traversed by the Track >=0");
    return 0.;
  }
  return material;
}



egammaTrkRefitterTool::MeasurementsAndTrash egammaTrkRefitterTool::addPointsToTrack(const EventContext& ctx,
                                                                                    const Trk::Track* track,
                                                                                    const xAOD::Electron* eg) const {
  ATH_MSG_DEBUG("Adding Points to the Track");
  egammaTrkRefitterTool::MeasurementsAndTrash collect {};
   /* The issue here is that some of the returned measurements are owned by storegate
    * some not. For the ones that are not put them in a vector of unique_ptr which
    * we will also return to the caller*/

  if (track && track->trackParameters() && !track->trackParameters()->empty()) {
    std::unique_ptr<const Trk::VertexOnTrack> vot (provideVotFromBeamspot(ctx,track));
    // fill the beamSpot if you have it
    if (vot!=nullptr){
      collect.m_trash.push_back(std::move(vot));
      collect.m_measurements.push_back(collect.m_trash.back().get());
    }
    std::vector<const Trk::MeasurementBase*> vecIDHits  = getIDHits(track);
    std::vector<const Trk::MeasurementBase*>::const_iterator it    = vecIDHits.begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator itend = vecIDHits.end();
    // Fill the track , these are not trash
    for (;it!=itend;++it) {
      collect.m_measurements.push_back(*it);
    }
  } else {
    ATH_MSG_WARNING("Could not extract MeasurementBase from track");
    return collect;
  }
  if (m_useClusterPosition && eg->caloCluster()){
    int charge(0);
    if( track->perigeeParameters() ) charge  = (int)track->perigeeParameters()->charge();
    std::unique_ptr<const Trk::CaloCluster_OnTrack> ccot (m_CCOTBuilder->buildClusterOnTrack(eg->caloCluster(),charge));
    if (ccot!=nullptr){
      collect.m_trash.push_back(std::move(ccot));
      collect.m_measurements.push_back(collect.m_trash.back().get());
    }
  }
  return collect;
}

const Trk::VertexOnTrack* egammaTrkRefitterTool::provideVotFromBeamspot(const EventContext& ctx,
                                                                        const Trk::Track* track) const{

  const Trk::VertexOnTrack* vot = nullptr;

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotData{m_beamSpotKey, ctx};
  /*
   * Perhaps usefull in serial Athena
   */
  if(!(beamSpotData.isValid())){
      return nullptr;
  }

  Amg::Vector3D bpos =beamSpotData->beamPos();
  float beamSpotX = bpos.x();
  float beamSpotY = bpos.y();
  float beamSpotZ = bpos.z();
  float beamTiltX = beamSpotData->beamTilt(0);
  float beamTiltY = beamSpotData->beamTilt(1);
  float beamSigmaX = beamSpotData->beamSigma(0);
  float beamSigmaY = beamSpotData->beamSigma(1);

  ATH_MSG_DEBUG("running refit with beam-spot");

  float z0 = track->perigeeParameters()->parameters()[Trk::z0];
  float beamX = beamSpotX + tan(beamTiltX) * (z0-beamSpotZ);
  float beamY = beamSpotY + tan(beamTiltY) * (z0-beamSpotZ);
  Amg::Vector3D BSC(beamX, beamY, z0);
  ATH_MSG_DEBUG("constructing beam point (x,y,z) = ( "<<beamX<<" , "<<beamY<<" , "<<z0<<" )");

  const Trk::PerigeeSurface * surface = nullptr;

  // covariance matrix of the beam-spot
  AmgSymMatrix(2)  beamSpotCov;
  beamSpotCov.setZero();
  beamSpotCov(0,0) = beamSigmaX * beamSigmaX;
  beamSpotCov(1,1) = beamSigmaY * beamSigmaY;
  const Amg::Vector3D& globPos(BSC);
  surface = new Trk::PerigeeSurface(globPos);

  // create a measurement for the beamspot
  const Amg::Vector2D  Par0(0.,Trk::d0);
  Trk::LocalParameters beamSpotParameters(Par0);

  // calculate perigee parameters wrt. beam-spot
  const Trk::Perigee * perigee = nullptr;
  const Trk::TrackParameters* tmp =m_extrapolator->extrapolate(*track, *surface);
  if(tmp->associatedSurface().type()==Trk::Surface::Perigee){
    perigee= static_cast<const Trk::Perigee*> (tmp);
  }
  if (!perigee) {// if failure
    const Trk::Perigee * trackPerigee = track->perigeeParameters();
    if ( trackPerigee && ((trackPerigee->associatedSurface())) == *surface )
      perigee = trackPerigee->clone();
  }

  Eigen::Matrix<double,1,2> Jacobian;
  Jacobian.setZero();
  if(perigee){
    double ptInv   =  1./perigee->momentum().perp();
    Jacobian(0,0) = -ptInv * perigee->momentum().y();
    Jacobian(0,1) =  ptInv * perigee->momentum().x();
  }

  Amg::MatrixX errorMatrix(Jacobian*(beamSpotCov*Jacobian.transpose()));
  vot = new Trk::VertexOnTrack(beamSpotParameters, errorMatrix, *surface);
  ATH_MSG_DEBUG(" the VertexOnTrack objects created from BeamSpot are " << *vot);

  delete perigee;
  delete surface;
  return vot;
}


std::vector<const Trk::MeasurementBase*> egammaTrkRefitterTool::getIDHits(const Trk::Track* track)  const
{
  //store all silicon measurements into the measurementset
  DataVector<const Trk::TrackStateOnSurface>::const_iterator
    trackStateOnSurface = track->trackStateOnSurfaces()->begin();

  std::vector<const Trk::MeasurementBase*> measurementSet;
  measurementSet.reserve(track->trackStateOnSurfaces()->size());

  for ( ; trackStateOnSurface != track->trackStateOnSurfaces()->end(); ++trackStateOnSurface ) {
    if ( !(*trackStateOnSurface) ){
      ATH_MSG_WARNING( "This track contains an empty MeasurementBase object that won't be included in the fit" );
      continue;
    }
    if ( (*trackStateOnSurface)->measurementOnTrack() ){
      if ( (*trackStateOnSurface)->type( Trk::TrackStateOnSurface::Measurement) ){
        const Trk::RIO_OnTrack* rio = nullptr;
        const Trk::MeasurementBase* tmp = (*trackStateOnSurface)->measurementOnTrack() ;
        if(tmp->type(Trk::MeasurementBaseType::RIO_OnTrack)){
           rio =static_cast <const Trk::RIO_OnTrack*>( tmp);
        }
        if (rio != nullptr) {
          const Identifier& surfaceID = (rio->identify()) ;
          if( m_idHelper->is_sct(surfaceID) || m_idHelper->is_pixel(surfaceID) ) {
            measurementSet.push_back( (*trackStateOnSurface)->measurementOnTrack() );
          } else if( !m_RemoveTRT && m_idHelper->is_trt(surfaceID) ) {
            measurementSet.push_back( (*trackStateOnSurface)->measurementOnTrack() );
          }
        }
      }
      else if ( m_reintegrateOutliers && (*trackStateOnSurface)->type( Trk::TrackStateOnSurface::Outlier) ){
        const Trk::RIO_OnTrack* rio = nullptr;
        const Trk::MeasurementBase* tmp = (*trackStateOnSurface)->measurementOnTrack() ;
        if(tmp->type(Trk::MeasurementBaseType::RIO_OnTrack)){
           rio =static_cast <const Trk::RIO_OnTrack*>( tmp);
        }
        if (rio != nullptr) {
          const Identifier& surfaceID = (rio->identify()) ;
          if( m_idHelper->is_sct(surfaceID) || m_idHelper->is_pixel(surfaceID) ) {
            measurementSet.push_back( (*trackStateOnSurface)->measurementOnTrack() );
          } else if( !m_RemoveTRT && m_idHelper->is_trt(surfaceID) ) {
            measurementSet.push_back( (*trackStateOnSurface)->measurementOnTrack() );
          }
        }
      }
    }
  }
  return measurementSet;
}

