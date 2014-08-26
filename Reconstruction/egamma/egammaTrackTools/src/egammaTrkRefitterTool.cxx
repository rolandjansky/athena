/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaTrkRefitterTool.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "DataModel/DataVector.h"

#include "TrkTrack/TrackStateOnSurface.h"

#include "TrkVertexOnTrack/VertexOnTrack.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkCaloCluster_OnTrack/CaloCluster_OnTrack.h"
#include "egammaInterfaces/ICaloCluster_OnTrackBuilder.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"


#include <algorithm> 
#include <math.h>
#include <vector>


// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/ObjectVector.h"      
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"




egammaTrkRefitterTool::egammaTrkRefitterTool(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_refittedTrack(0),
  m_originalTrack(0),
  m_egammaObject(0),
  m_oMeasPer(0),
  m_rMeasPer(0),
  m_ITrackFitter("TrkKalmanFitter/AtlasKalmanFitter"),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_IVertexUpdator("Trk::KalmanVertexUpdator"),
  m_linFactory("Trk::FullLinearizedTrackFactory"),
  m_beamCondSvc("BeamCondSvc",name),
  m_CCOTBuilder("CaloCluster_OnTrackBuilder")
{
  // declare interface
  declareInterface< IegammaTrkRefitterTool >(this) ;

  // ToolHandle for track fitter implementation
  declareProperty("FitterTool",
                  m_ITrackFitter,
                  "ToolHandle for track fitter implementation");                                           
  // Switch if refit should be made on PRD or ROT level
  declareProperty("Fit_RIO_OnTrack",
                  m_fitRIO_OnTrack=false, 
                  "Switch if refit should be made on PRD or ROT level");

  // Switch to control outlier finding in track fit
  declareProperty("runOutlier",
                   m_runOutlier = false,
                  "Switch to control outlier finding in track fit");
                  
  // Switch to control outlier finding in track fit
  declareProperty("ReintegrateOutliers",
                   m_reintegrateOutliers = false,
                  "Switch to control addition of  outliers back for track fit");


  // Type of material interaction in extrapolation (default Electron)
  declareProperty("matEffects",
                   m_matEffects = 1,
                   "Type of material interaction in extrapolation (Default Electron)");
      
  declareProperty("minNoSiHits",
                  m_MinNoSiHits = 3,
                  "Minimum number of silicon hits on track before it is allowed to be refitted");
      
  // Switch to control use of Beam spot measurement
  declareProperty("useBeamSpot",
                   m_useBeamSpot = false, 
                   "Switch to control use of Beam Spot Measurement ");
  
  declareProperty("Extrapolator",              
                   m_extrapolator );

  declareProperty("primaryVertexCollection",          
                   m_primaryVertexCollection = "VxPrimaryCandidate");

  declareProperty("useClusterPosition",
                  m_useClusterPosition = false, 
                  "Switch to control use of Cluster position measurement");

  declareProperty("CCOTBuilder", 
                   m_CCOTBuilder);
                   
  declareProperty("RemoveTRTHits",m_RemoveTRT  = false,"RemoveTRT Hits");	  
                 

}

  
egammaTrkRefitterTool::~egammaTrkRefitterTool()
{
}

/////////////////////////////////////////////////////////////////

// INITIALIZE METHOD:
     
StatusCode egammaTrkRefitterTool::initialize()
{
  ATH_MSG_DEBUG("Initializing egammaTrackRefitter");
 
  // Retrieve fitter
  if (m_ITrackFitter.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_ITrackFitter);
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_INFO("Retrieved tool " << m_ITrackFitter);

  //linearized track factory
  if (m_linFactory.retrieve().isFailure()){
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_linFactory );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved VertexLinearizedTrackFactory tool " << m_linFactory );
  }

   // KalmanVertexUpdator
  if (m_IVertexUpdator.retrieve().isFailure()) {
     ATH_MSG_FATAL( "Can not retrieve VertexUpdator of type " << m_IVertexUpdator.typeAndName() );
     return StatusCode::FAILURE;
  } else { 
    ATH_MSG_DEBUG( "Retrieved VertexUpdator Tool " << m_IVertexUpdator.typeAndName() ); 
  }


  // configure Atlas extrapolator
  if (m_extrapolator.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_extrapolator );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved " << m_extrapolator);

  // configure Atlas extrapolator
  if (m_CCOTBuilder.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_CCOTBuilder );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved " << m_CCOTBuilder);




  // configure beam-spot conditions service
  if (m_beamCondSvc.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Failed to retrieve beamspot service "<<m_beamCondSvc );
      return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved " << m_beamCondSvc);


  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_FATAL( "Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO("Retrieved ID helpers");


  // Set the particle hypothesis to match the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_ParticleHypothesis = particleSwitch.particle[m_matEffects];
  
  ATH_MSG_INFO("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

// ===============================================================
StatusCode egammaTrkRefitterTool::finalize()
{
  resetRefitter();
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode egammaTrkRefitterTool::execute()
{
  return StatusCode::SUCCESS;
}

// =====================================================================
StatusCode  egammaTrkRefitterTool::refitElectronTrack(const xAOD::Electron* eg) 
{
  ATH_MSG_DEBUG("Refitting a track associated  with a egamma object");

  // protection against bad pointers
  if (eg==0) return StatusCode::SUCCESS;

  // Set the pointer to the egamma object. 
  m_egammaObject = eg;

  const xAOD::TrackParticle *trackParticle = eg->trackParticle();
 
  StatusCode sc = refitTrackParticle( trackParticle, eg );
  
  return sc;
}

// =======================================================================
StatusCode egammaTrkRefitterTool::refitTrackParticle(const xAOD::TrackParticle* trackParticle, const xAOD::Electron* eg) 
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
  
  if ( trackParticle->trackLink().isValid() ) {      
    // retrieve and refit original track
    StatusCode sc  = refitTrack( trackParticle->track() , eg );
    
    if (sc == StatusCode::FAILURE) return StatusCode::FAILURE;
    
  } else {
    ATH_MSG_WARNING("Could not get TrackElementLink of the TrackParticle");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

// =====================================================================
StatusCode  egammaTrkRefitterTool::refitTrack(const Trk::Track* track, const xAOD::Electron * eg) 
{
  //
  // Refit the track associated with an egamma object
  //
  
  m_refittedTrack=0;
  m_originalTrack=0;
  m_oMeasPer=0;
  m_rMeasPer=0;

  // Set the pointer to the egamma object. 
  m_egammaObject = eg;


  if (!track) return StatusCode::FAILURE;
  
  //Set the pointer to the track
  m_originalTrack = track;
  
  //Set pointer to the original perigee
  m_oMeasPer=dynamic_cast<const Trk::Perigee*>( m_originalTrack->perigeeParameters() );
  
  if (m_oMeasPer!=0){
      double od0 = m_oMeasPer->parameters()[Trk::d0];
      double oz0 = m_oMeasPer->parameters()[Trk::z0];
      double ophi0 = m_oMeasPer->parameters()[Trk::phi0];
      double otheta = m_oMeasPer->parameters()[Trk::theta];
      double oqOverP = m_oMeasPer->parameters()[Trk::qOverP];         
      ATH_MSG_DEBUG("Original parameters " << od0  << " " 
        << oz0  << " " << ophi0 << " " << otheta << " " << oqOverP << " " << 1/oqOverP) ;
  } else {
    ATH_MSG_WARNING("Could not get Trk::Perigee of original track");
  }
  
  // Refit the track with the beam spot if desired otherwise just refit the original track
  if (m_useBeamSpot || m_useClusterPosition){
    std::vector<const Trk::MeasurementBase*>  measurements = addPointsToTrack(m_originalTrack, eg );
    if(measurements.size()>4)
      m_refittedTrack = m_ITrackFitter->fit(measurements,*m_originalTrack->perigeeParameters(),m_runOutlier,m_ParticleHypothesis);
    else {
      ATH_MSG_WARNING("Could **NOT** add BeamSpot information into Vector refitting without BS");
      m_refittedTrack = m_ITrackFitter->fit(*m_originalTrack,m_runOutlier,m_ParticleHypothesis);
    }
  } else {
    std::vector<const Trk::MeasurementBase*>  measurements = getIDHits(m_originalTrack);  
    if(measurements.size()>4){
      ATH_MSG_DEBUG("Could not remove TRT hits !!!");
      m_refittedTrack = m_ITrackFitter->fit(measurements,*m_originalTrack->perigeeParameters(),m_runOutlier,m_ParticleHypothesis);
    } else {
      ATH_MSG_DEBUG("Not enough measurements on track remove TRT hits?");
      m_refittedTrack = 0; 
    }
  } 


  // Store refitted perigee pointers
  if (m_refittedTrack) {
    m_rMeasPer=
      dynamic_cast<const Trk::Perigee*>(m_refittedTrack->perigeeParameters() );
    
    if (m_rMeasPer!=0){
        double d0 = m_rMeasPer->parameters()[Trk::d0];
        double z0 = m_rMeasPer->parameters()[Trk::z0];
        double phi0 = m_rMeasPer->parameters()[Trk::phi0];
        double theta = m_rMeasPer->parameters()[Trk::theta];
        double qOverP = m_rMeasPer->parameters()[Trk::qOverP];
        ATH_MSG_DEBUG("Refitted parameters " << d0  << " " << z0  << " " << phi0 << " " << theta << " " << qOverP << "  " << 1/qOverP);
    } else {
      ATH_MSG_WARNING("Could not get refitted Trk::Perigee");     
      delete m_refittedTrack; 
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Refit Failed");
    return StatusCode::FAILURE;
  }
}

// ================================================================
Trk::Track*  egammaTrkRefitterTool::refittedTrack() 
{
  //
  // Returns the refitted track
  //

  ATH_MSG_DEBUG("Getting the refitted track"); 
  if (  m_refittedTrack != 0){
    return  m_refittedTrack;    
  } else {
    ATH_MSG_DEBUG("No Refitted Track");
    return 0;
  }
}
  
// =================================================================
/** Returns the refitted track perigee */
const Trk::Perigee*  egammaTrkRefitterTool::refittedTrackPerigee() 
{
  ATH_MSG_DEBUG("Getting the original track");  
  
  if (m_rMeasPer==0){
    ATH_MSG_DEBUG("No Measured Perigee on Track");
    return 0;
  } else {
    ATH_MSG_DEBUG("Successfully returning original perigee");
    return m_rMeasPer;
  }
}
  
// ======================================================================
/** Returns the original track */
const Trk::Track*  egammaTrkRefitterTool::originalTrack() 
{
  ATH_MSG_DEBUG("Getting the original track");  
  if (  m_originalTrack != 0){
    return  m_originalTrack;    
  } else {
    ATH_MSG_DEBUG("No refitted track");
    return 0;
  }
}
  
// =======================================================================
/** Returns the original track */
const Trk::Perigee*  egammaTrkRefitterTool::originalTrackPerigee() 
{
  ATH_MSG_DEBUG("Getting the original track");  
  
  if (m_oMeasPer==0){
    ATH_MSG_DEBUG("No Measured Perigee on Track");
    return 0;
  } else {
    ATH_MSG_DEBUG("Successfully returning original perigee");
    return m_oMeasPer;
  }
}

// =================================================================
void  egammaTrkRefitterTool::resetRefitter() 
{
  ATH_MSG_DEBUG("Resetting Refitter"); 
  m_refittedTrack=0; 
  m_originalTrack=0;
  m_egammaObject=0;
  m_oMeasPer=0;
  m_rMeasPer=0;
  for( int i(0); i< (int)m_trash.size(); ++i){
    delete m_trash[i];
  }
  m_trash.clear();
  
  return;
}

// ===================================================================
const Trk::TrackParameters* egammaTrkRefitterTool::refittedEndParameters()
{
  return lastTrackParameters( m_refittedTrack );
}

// ==================================================================
const Trk::TrackParameters* egammaTrkRefitterTool::originalEndParameters()
{
  return lastTrackParameters( const_cast<Trk::Track*>(m_originalTrack) );
}

// ======================================================================
const Trk::TrackParameters* egammaTrkRefitterTool::lastTrackParameters(Trk::Track* track)
{ 
  ATH_MSG_DEBUG("Getting the final track parameters"); 
  
  if (track == 0 ){
    ATH_MSG_DEBUG("Track == 0  returning  0"); 
    return 0;
  }
  
  const DataVector<const Trk::TrackStateOnSurface>* oldTrackStates = track->trackStateOnSurfaces();
  if (oldTrackStates == 0)
  {
    ATH_MSG_DEBUG("Track has no TSOS vector! Skipping track, returning 0.");
    return 0;
  }
  
    
  const Trk::TrackParameters* lastValidTrkParameters(0);  
  for ( DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rItTSoS = oldTrackStates->rbegin(); rItTSoS != oldTrackStates->rend(); ++rItTSoS)
  { 
    if (lastValidTrkParameters!=0){
      break;
    }
    
    if ( (*rItTSoS)->type(Trk::TrackStateOnSurface::Measurement) && (*rItTSoS)->trackParameters()!=0 && (*rItTSoS)->measurementOnTrack()!=0)
    {
      lastValidTrkParameters = (*rItTSoS)->trackParameters()->clone();
    }
  }
    
  if (lastValidTrkParameters!=0){
    ATH_MSG_DEBUG ("Last Valid Trk Q/P" << lastValidTrkParameters->parameters()[Trk::qOverP] );
    return lastValidTrkParameters;
  }
  ATH_MSG_DEBUG("Last Track Parameters");  
  return 0;

}

// =====================================================================
double egammaTrkRefitterTool::getMaterial() 
{
  return getMaterialTraversed(m_refittedTrack);
}

// ======================================================================
double egammaTrkRefitterTool::getMaterialTraversed(Trk::Track* track) 
{
  ATH_MSG_DEBUG("Calculating Material Traversed by the Track");  
  
  if (!track) return 0.;
  
  double material(0);
  //Note I am not sure if this is the correct way of accessing the information
  DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurface = track->trackStateOnSurfaces()->begin();    
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

// =========================================================================
std::vector<const Trk::MeasurementBase*> egammaTrkRefitterTool::addPointsToTrack(const Trk::Track* track, const xAOD::Electron* eg) 
{
  ATH_MSG_DEBUG("Adding a Point to the Track");  

  const Trk::VertexOnTrack* vot(0);
  if(m_useBeamSpot){
    //vot=  provideVotFromVertex(track);
    
    if(!vot){
      vot = provideVotFromBeamspot( track );
    } 
  }

  
  //  Object to return
  std::vector<const Trk::MeasurementBase*> vec;
  
  if (track && track->trackParameters() && track->trackParameters()->size() > 0) {
    // fill the beamSpot if you have it
    if (vot){
      vec.push_back(vot);
      m_trash.push_back(vot);
    }
    std::vector<const Trk::MeasurementBase*> vecIDHits  = getIDHits(track);
        
    std::vector<const Trk::MeasurementBase*>::const_iterator it    = vecIDHits.begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator itend = vecIDHits.end();
    // Fill the track
    for (;it!=itend;++it) vec.push_back(*it);
   
  } else {
    ATH_MSG_WARNING("Could not extract MeasurementBase from track");
    return vec;
  }

  if (m_useClusterPosition && eg){
    int charge(0);
    if( track->perigeeParameters() ) charge  = (int)track->perigeeParameters()->charge(); 
    const Trk::CaloCluster_OnTrack* ccot = m_CCOTBuilder->buildClusterOnTrack(eg,charge);
    if (ccot){
      vec.push_back(ccot);
      m_trash.push_back(ccot);
    }
  }

  return vec;

  
}



const Trk::VertexOnTrack* egammaTrkRefitterTool::provideVotFromVertex(const Trk::Track *originalTrack) const
{

  const Trk::VxCandidate* vtx        = 0;
  Trk::VxTrackAtVertex* vtxTrk       = 0;
  const Trk::VertexOnTrack * vot     = 0;
  Trk::VxCandidate* tmpVtx           = 0;
  Trk::VxCandidate* updatedVtx       = 0;

  std::pair<const Trk::VxCandidate*, Trk::VxTrackAtVertex*> findPair;//= findAssociatedPrimaryVertex(originalTrack);  
 
  ATH_MSG_DEBUG("findPair in provideVotFromVertex: "<<findPair);
 
  if (!( 0==findPair.first || 0==findPair.second )) {
    vtx    = findPair.first;
    vtxTrk = findPair.second;
    const Trk::RecVertex & rvtx = vtx->recVertex();
    ATH_MSG_DEBUG("the vertex position in provideVotFromVertex:  "<< rvtx);
    
    //////////////////////////////////////////////////////////////////////////     
    if(!vtxTrk->linState()) m_linFactory->linearize(*vtxTrk, rvtx);
    
    tmpVtx = vtx->clone();
    updatedVtx = m_IVertexUpdator->remove(*tmpVtx, *vtxTrk);
    
    
    if(updatedVtx){
      ATH_MSG_DEBUG(" updated Vertex by KalmanVertexUpdator: "<<*updatedVtx);
      
      
      const Amg::Vector2D localPosition(0,0);
      Trk::LocalParameters localParams(localPosition);
      
      ///vertex as perigeeSurface
      Amg::Vector3D globPos(updatedVtx->recVertex().position().x(),updatedVtx->recVertex().position().y(),updatedVtx->recVertex().position().z());
      const Trk::PerigeeSurface* surface = new Trk::PerigeeSurface(globPos);
      
      const Trk::Perigee *  perigee = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolate( *originalTrack, *surface));
      if (!perigee) {
        const Trk::Perigee * trackPerigee = originalTrack->perigeeParameters();
        if ( trackPerigee && ((trackPerigee->associatedSurface())) == *surface )
          perigee = trackPerigee->clone();
      }
      
      // create the Jacobian matrix from Cartisian to Perigee
      AmgMatrix(2,3)  Jacobian;
      Jacobian.setZero();
      double ptInv                                =  1./perigee->momentum().perp();
      Jacobian(0,0)                              = -ptInv*perigee->momentum().y();
      Jacobian(0,1)                              =  ptInv*perigee->momentum().x();
      Jacobian(1,2)                              =  1.0;

      ATH_MSG_DEBUG(" Jacobian matrix from Cartesian to Perigee: "<< Jacobian);
      Amg::MatrixX vtxCov = updatedVtx->recVertex().covariancePosition();   
      Amg::MatrixX covarianceMatrix(Jacobian*(vtxCov*Jacobian.transpose()));

      // VertexOnTrack Object
      vot = new Trk::VertexOnTrack(localParams, covarianceMatrix, *surface);

      delete perigee;
      delete tmpVtx;
      delete surface;
      ATH_MSG_DEBUG("the VertexOnTrack created from vertex: "<<*vot);
    }
  }
  return vot;
}


const Trk::VertexOnTrack* egammaTrkRefitterTool::provideVotFromBeamspot(const Trk::Track* track) const
{

  const Trk::VertexOnTrack * vot = 0;

  Amg::Vector3D bpos = m_beamCondSvc->beamPos();
  ATH_MSG_DEBUG("beam spot: "<<bpos);
  float beamSpotX = bpos.x();
  float beamSpotY = bpos.y();
  float beamSpotZ = bpos.z();
  float beamTiltX = m_beamCondSvc->beamTilt(0);
  float beamTiltY = m_beamCondSvc->beamTilt(1);
  float beamSigmaX = m_beamCondSvc->beamSigma(0);
  float beamSigmaY = m_beamCondSvc->beamSigma(1);
  //float beamSigmaZ = m_scalingFactor * m_beamCondSvc->beamSigma(2);

  ATH_MSG_DEBUG("running refit with beam-spot");

  
  float z0 = track->perigeeParameters()->parameters()[Trk::z0];
  float beamX = beamSpotX + tan(beamTiltX) * (z0-beamSpotZ);
  float beamY = beamSpotY + tan(beamTiltY) * (z0-beamSpotZ);
  Amg::Vector3D BSC(beamX, beamY, z0);
  ATH_MSG_DEBUG("constructing beam point (x,y,z) = ( "<<beamX<<" , "<<beamY<<" , "<<z0<<" )");

  const Trk::PerigeeSurface * surface = 0;
 

  
  // covariance matrix of the beam-spot
  AmgSymMatrix(2)  beamSpotCov;
  beamSpotCov.setZero();
  beamSpotCov(0,0) = beamSigmaX * beamSigmaX;
  beamSpotCov(1,1) = beamSigmaY * beamSigmaY;

  
  Amg::Vector3D globPos(BSC);
  surface = new Trk::PerigeeSurface(globPos);
  
  // create a measurement for the beamspot
  const Amg::Vector2D  Par0(0.,Trk::d0);
  Trk::LocalParameters beamSpotParameters(Par0);
  
  // calculate perigee parameters wrt. beam-spot
  const Trk::Perigee * perigee = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolate(*track, *surface));
  if (!perigee) {
    const Trk::Perigee * trackPerigee = track->perigeeParameters();
    if ( trackPerigee && ((trackPerigee->associatedSurface())) == *surface )
      perigee = trackPerigee->clone();
  }
  
  Eigen::Matrix<double,1,2> Jacobian;
  Jacobian.setZero();
  double ptInv   =  1./perigee->momentum().perp();
  Jacobian(0,0) = -ptInv * perigee->momentum().y();
  Jacobian(0,1) =  ptInv * perigee->momentum().x();
  
  Amg::MatrixX errorMatrix(Jacobian*(beamSpotCov*Jacobian.transpose()));  
  vot = new Trk::VertexOnTrack(beamSpotParameters, errorMatrix, *surface);
  ATH_MSG_DEBUG(" the VertexOnTrack objects created from BeamSpot are " << *vot);

  delete perigee;
  delete surface;
  return vot;
}
/*
std::pair<const Trk::VxCandidate*, Trk::VxTrackAtVertex*>  egammaTrkRefitterTool::findAssociatedPrimaryVertex(const Trk::Track* originalTrack)const
{
  const VxContainer* vxContainer(0);
  int npv = 0;
  StatusCode sc = evtStore()->retrieve(vxContainer, m_primaryVertexCollection);

  if (sc.isFailure()) {
    msg( MSG::WARNING ) << "Could not retrieve primary vertex info: " << m_primaryVertexCollection << endreq;
    return std::pair<const Trk::VxCandidate*,Trk::VxTrackAtVertex*>(0,0);
  } else {
    ATH_MSG_DEBUG( "Found primary vertex info: " << m_primaryVertexCollection );
    if(vxContainer) {
      ATH_MSG_DEBUG("Nb of reco primary vertex for coll " 
                        << " = " << vxContainer->size() );
      VxContainer::const_iterator vxI = vxContainer->begin();
      VxContainer::const_iterator vxE = vxContainer->end();
      for(; vxI!=vxE; ++vxI) {
        int nbtk = 0;
        const std::vector<Trk::VxTrackAtVertex*>* tracks = (*vxI)->vxTrackAtVertex();
        if (tracks) nbtk = tracks->size();
        if (nbtk) {
          for(int ivtk=0;ivtk<nbtk;ivtk++) {
            Trk::VxTrackAtVertex* trk = tracks->at(ivtk);
            if(trk) {
              const Rec::TrackParticle* tp = getTrackParticle( trk );
              if ( tp->trackElementLink()->isValid() ) {      
                const Trk::Track * trktrk = tp->originalTrack();
                if ( trktrk==originalTrack ) 
                  return std::pair<const Trk::VxCandidate*,Trk::VxTrackAtVertex*>( (*vxI),trk ); 
              }
            }
          }
          ++npv;
        } else {
          ATH_MSG_DEBUG( "Vertex " << npv << " has no Tracks assciated to it!"  );
        }   
      }
    } else {
        ATH_MSG_DEBUG( "No container in collection?? " << m_primaryVertexCollection );
    }    
  }

  return std::pair<const Trk::VxCandidate*,Trk::VxTrackAtVertex*>(0,0);

}
const Rec::TrackParticle* egammaTrkRefitterTool::getTrackParticle(Trk::VxTrackAtVertex *trkAtVx)const
{

  //find the link to the TrackParticleBase
  const Trk::ITrackLink* trkLink = trkAtVx->trackOrParticleLink();
  const Trk::TrackParticleBase* trkPB(0);
  if(0!= trkLink){
    const Trk::LinkToTrackParticleBase* linktrkPB = dynamic_cast<const Trk::LinkToTrackParticleBase *>(trkLink);
    if(0!= linktrkPB){
      if(linktrkPB->isValid()) trkPB = linktrkPB->cachedElement();
    }//end of dynamic_cast check
  }//end of ITrackLink existance check

  //cast to TrackParticle
  if(trkPB){
    const Rec::TrackParticle* trkP = dynamic_cast<const Rec::TrackParticle*>(trkPB);
    return trkP;
  }else{
    return 0;
  }
}
*/


std::vector<const Trk::MeasurementBase*> egammaTrkRefitterTool::getIDHits(const Trk::Track* track) 
{
  
  std::vector<const Trk::MeasurementBase*> measurementSet;
  //store all silicon measurements into the measurementset

  DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurface = track->trackStateOnSurfaces()->begin();
  for ( ; trackStateOnSurface != track->trackStateOnSurfaces()->end(); ++trackStateOnSurface ) {
    
    if ( !(*trackStateOnSurface) ){
      ATH_MSG_WARNING( "This track contains an empty MeasurementBase object that won't be included in the fit" );
      continue;
    }
    
    if ( (*trackStateOnSurface)->measurementOnTrack() ){
      if ( (*trackStateOnSurface)->type( Trk::TrackStateOnSurface::Measurement) ){
        const Trk::RIO_OnTrack* rio = dynamic_cast <const Trk::RIO_OnTrack*>( (*trackStateOnSurface)->measurementOnTrack() );
        if (rio != 0) {
          const Identifier& surfaceID = (rio->identify()) ;
          if( m_idHelper->is_sct(surfaceID) || m_idHelper->is_pixel(surfaceID) ) {
            measurementSet.push_back( (*trackStateOnSurface)->measurementOnTrack() );
          } else if( !m_RemoveTRT && m_idHelper->is_trt(surfaceID) ) {
            measurementSet.push_back( (*trackStateOnSurface)->measurementOnTrack() );
          }
        }        				
      }
      else if ( m_reintegrateOutliers && (*trackStateOnSurface)->type( Trk::TrackStateOnSurface::Outlier) ){
        const Trk::RIO_OnTrack* rio = dynamic_cast <const Trk::RIO_OnTrack*>( (*trackStateOnSurface)->measurementOnTrack() );
        if (rio != 0) {
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

