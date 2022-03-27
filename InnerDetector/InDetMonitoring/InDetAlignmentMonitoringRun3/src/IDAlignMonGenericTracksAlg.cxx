/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// AlignmentMonAlg.cxx
// AUTHORS: Beate Heinemann, Tobias Golling
// Adapted to AthenaMT by Per Johansson 2021
// **********************************************************************

//main header
#include "IDAlignMonGenericTracksAlg.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TrkTrack/TrackCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"

#include "Particle/TrackParticle.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

#include "InDetAlignGenTools/IInDetAlignHitQualSelTool.h"

#include <cmath>

// *********************************************************************
// Public Methods
// *********************************************************************

IDAlignMonGenericTracksAlg::IDAlignMonGenericTracksAlg( const std::string & name, ISvcLocator* pSvcLocator ) :
   AthMonitorAlgorithm(name, pSvcLocator),
   m_idHelper(nullptr),
   m_pixelID(nullptr),
   m_sctID(nullptr),
   m_trtID(nullptr),
   m_d0Range(2.0),
   m_d0BsRange(0.5),
   m_z0Range(250.0),
   m_etaRange(3.0),
   m_NTracksRange(200),
   m_trackSelection( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this)
{
  m_hitQualityTool = ToolHandle<IInDetAlignHitQualSelTool>("");
  declareProperty("Pixel_Manager"        , m_Pixel_Manager);
  declareProperty("SCT_Manager"          , m_SCT_Manager);
  declareProperty("TRT_Manager"          , m_TRT_Manager);
  declareProperty("TrackSelectionTool"   , m_trackSelection);
  declareProperty("HitQualityTool"       , m_hitQualityTool);
  declareProperty("useExtendedPlots"     , m_extendedPlots = false);
  declareProperty("d0Range"              , m_d0Range);
  declareProperty("d0BsRange"            , m_d0BsRange);
  declareProperty("z0Range"              , m_z0Range);
  declareProperty("etaRange"             , m_etaRange);
  declareProperty("pTRange"              , m_pTRange);
  declareProperty("NTracksRange"         , m_NTracksRange);
  declareProperty("doIP"                 , m_doIP = false);
}


IDAlignMonGenericTracksAlg::~IDAlignMonGenericTracksAlg() { }


StatusCode IDAlignMonGenericTracksAlg::initialize()
{
  StatusCode sc;  

  //ID Helper
  ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));
  
  m_pixelID = nullptr;
  ATH_CHECK(detStore()->retrieve(m_pixelID, "PixelID"));
  ATH_MSG_DEBUG("Initialized PixelIDHelper");

  m_sctID = nullptr;
  ATH_CHECK(detStore()->retrieve(m_sctID, "SCT_ID"));
  ATH_MSG_DEBUG("Initialized SCTIDHelper");

  m_trtID = nullptr;
  ATH_CHECK(detStore()->retrieve(m_trtID, "TRT_ID"));
  ATH_MSG_DEBUG("Initialized TRTIDHelper");

  ATH_CHECK(m_trackSelection.retrieve());
  ATH_MSG_DEBUG("Retrieved tool " << m_trackSelection);

  if (m_hitQualityTool.empty()) {
    ATH_MSG_DEBUG("No hit quality tool configured - not hit quality cuts will be imposed");
    m_doHitQuality = false;
  } else if (m_hitQualityTool.retrieve().isFailure()) {
    ATH_MSG_WARNING("Could not retrieve " << m_hitQualityTool << " (to apply hit quality cuts to Si hits) ");
    m_doHitQuality = false;
  } else {
    ATH_MSG_DEBUG("Hit quality tool setup - hit quality cuts will be applied to Si hits");
    m_doHitQuality = true;
  }
  
  if (m_doIP) {
    ATH_CHECK (m_trackToVertexIPEstimator.retrieve());
  }else {
    m_trackToVertexIPEstimator.disable();
  }
  
  if ( m_beamSpotKey.initialize().isFailure() ) {
    ATH_MSG_WARNING("Failed to retrieve beamspot service " << m_beamSpotKey << " - will use nominal beamspot at (0,0,0)");
    m_hasBeamCondSvc = false;
  } 
  else {
    m_hasBeamCondSvc = true;
    ATH_MSG_DEBUG("Retrieved service " << m_beamSpotKey);
  }
  
  ATH_CHECK(m_VxPrimContainerName.initialize(not m_VxPrimContainerName.key().empty()));
  ATH_CHECK(m_tracksName.initialize());
  ATH_CHECK(m_tracksKey.initialize());

  return AthMonitorAlgorithm::initialize();
}

StatusCode IDAlignMonGenericTracksAlg::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;
 
  // For histogram naming
  auto genericTrackGroup = getGroup("Tracks");

  //counters
  int ntrkMax=0;
  float xv=-999;
  float yv=-999;
  float zv=-999;
  int nTracks=0;
  int ngTracks=0;
  
  ATH_MSG_DEBUG ("IDAlignMonGenericTracksAlg::fillHistograms ** START ** call for track collection: " << m_tracksName.key());

  //get tracks
  auto trks = SG::makeHandle(m_tracksName, ctx);
  // check for tracks
  if (not trks.isValid()) {
    ATH_MSG_DEBUG ("IDAlignMonGenericTracksAlg::fillHistograms() --" << m_tracksName.key() << " could not be retrieved");
    return StatusCode::RECOVERABLE;
  }else {
    ATH_MSG_DEBUG("IDAlignMonGenericTracksAlg: Track container " << trks.name() <<" is found.");
  }

  //retrieving vertices
  auto handle_vxContainer = SG::makeHandle(m_VxPrimContainerName, ctx);
  // if m_doIP
  const xAOD::Vertex* pvtx = nullptr;
    
  if (!handle_vxContainer.isPresent()) {
    ATH_MSG_DEBUG ("InDetGlobalPrimaryVertexMonAlg: StoreGate doesn't contain primary vertex container with key "+m_VxPrimContainerName.key());
    return StatusCode::SUCCESS;
  }
  if (!handle_vxContainer.isValid()) {
    ATH_MSG_ERROR ("InDetGlobalPrimaryVertexMonAlg: Could not retrieve primary vertex container with key "+m_VxPrimContainerName.key());
    return StatusCode::RECOVERABLE;
  }

  const auto *vertexContainer = handle_vxContainer.cptr();
  for(const auto & vtx : *vertexContainer) {
    if ( !vtx ) continue;
    if ( !vtx->vxTrackAtVertexAvailable() ) continue;
    
    const std::vector< Trk::VxTrackAtVertex >& theTrackAtVertex = vtx->vxTrackAtVertex();
    int numTracksPerVertex = theTrackAtVertex.size();
    ATH_MSG_DEBUG("Size of TrackAtVertex: " << numTracksPerVertex);
    if (numTracksPerVertex>ntrkMax) {
      ntrkMax=numTracksPerVertex;
      xv=vtx->position()[0];
      yv=vtx->position()[1];
      zv=vtx->position()[2];
    }
  }

  if (xv==-999 || yv==-999 || zv==-999) {
    ATH_MSG_DEBUG("No vertex found => setting it to 0");
    xv=0;yv=0;zv=0;
  }

 
  std::map<const xAOD::TrackParticle*, const xAOD::Vertex*> trackVertexMapTP;
  if (m_doIP) fillVertexInformation(trackVertexMapTP, ctx);
  
  float beamSpotX = 0.;
  float beamSpotY = 0.;
  float beamSpotZ = 0.;
  float beamTiltX = 0.;
  float beamTiltY = 0.;
 
  if (m_hasBeamCondSvc) {
    auto beamSpotHandle = SG::ReadCondHandle(m_beamSpotKey, ctx);
    Amg::Vector3D bpos = beamSpotHandle->beamPos();
    beamSpotX = bpos.x();
    beamSpotY = bpos.y();
    beamSpotZ = bpos.z();
    beamTiltX = beamSpotHandle->beamTilt(0);
    beamTiltY = beamSpotHandle->beamTilt(1);
    ATH_MSG_DEBUG ("Beamspot from" << beamSpotHandle.retrieve() << ": x0 = " << beamSpotX << ", y0 = " << beamSpotY << ", z0 = " << beamSpotZ << ", tiltX = " << beamTiltX << ", tiltY = " << beamTiltY);
  }
  
  // Get EventInfo
  int lb       = GetEventInfo(ctx)->lumiBlock();
  auto lb_m    = Monitored::Scalar<int>( "m_lb", lb );
  int run      = GetEventInfo(ctx)->runNumber();
  auto run_m   = Monitored::Scalar<int>( "m_run", run );
  int event    = GetEventInfo(ctx)->eventNumber();
  auto event_m = Monitored::Scalar<int>( "m_event", event );

  fill(genericTrackGroup, lb_m);
	
  if (m_extendedPlots) {
    //Fill BeamSpot Position histos
    auto beamSpotX_m = Monitored::Scalar<float>( "m_beamSpotX", beamSpotX );
    auto beamSpotY_m = Monitored::Scalar<float>( "m_beamSpotY", beamSpotY );
    auto beamSpotZ_m = Monitored::Scalar<float>( "m_beamSpotZ", beamSpotZ );
    auto beamTiltX_m = Monitored::Scalar<float>( "m_beamTiltX", beamTiltX );
    auto beamTiltY_m = Monitored::Scalar<float>( "m_beamTiltY", beamTiltY );

    fill(genericTrackGroup, beamSpotX_m, beamSpotY_m);
    fill(genericTrackGroup, beamSpotZ_m, beamSpotY_m);
    fill(genericTrackGroup, beamSpotZ_m, beamSpotX_m);
  }
  

  if (m_doIP) {
    auto handle_vxContainer = SG::makeHandle(m_VxPrimContainerName, ctx);
    
    if (!handle_vxContainer.isPresent()) {
      ATH_MSG_DEBUG ("InDetGlobalPrimaryVertexMonAlg: StoreGate doesn't contain primary vertex container with key "+m_VxPrimContainerName.key());
      return StatusCode::SUCCESS;
    }
    if (!handle_vxContainer.isValid()) {
      ATH_MSG_ERROR ("InDetGlobalPrimaryVertexMonAlg: Could not retrieve primary vertex container with key "+m_VxPrimContainerName.key());
      return StatusCode::FAILURE;
    }
    
    const auto *vertexContainer = handle_vxContainer.cptr();
    
    xAOD::VertexContainer::const_iterator vxI = vertexContainer->begin();
    xAOD::VertexContainer::const_iterator vxE = vertexContainer->end();
    for (; vxI != vxE; ++vxI) {
      if ((*vxI)->type() == 1) {
	pvtx = (*vxI);
      }
    }
  }
  
  ATH_MSG_DEBUG ("IDAlignGenericTracks: Start loop on tracks. Number of tracks " << trks->size());
  for (const Trk::Track* trksItr: *trks) {

    // Found track?!
    if ( !trksItr || trksItr->perigeeParameters() == nullptr )
      {
	ATH_MSG_DEBUG( "InDetAlignmentMonitoringRun3: NULL track pointer in collection" );
	continue;
      }

    // Select tracks
    if ( !m_trackSelection->accept( *trksItr) )
      continue;

    nTracks++;  
    
    float chisquared     = 0.;
    int DoF              = 0;
    float chi2oDoF       = -999;
    float trkd0          = -999;
    float trkz0          = -999;
    float trkphi         = -999;
    float trktheta       = -999;
    float trketa         = -999;
    float qOverP         = -999;
    float trkpt          = -999;
    float trkP           = -999;
    float charge         = 0;
    float trkd0c         = -999;
    float beamX          = 0;
    float beamY          = 0;
    float d0bscorr   = -999;
 
    // get fit quality and chi2 probability of track
    const Trk::FitQuality* fitQual = trksItr->fitQuality();
    
    const Trk::Perigee* measPer = trksItr->perigeeParameters();
    const AmgSymMatrix(5)* covariance = measPer ? measPer->covariance() : nullptr;

    const Trk::ImpactParametersAndSigma* myIPandSigma=nullptr;
    

    if (m_doIP){

      //Get unbiased impact parameter
      if (pvtx) myIPandSigma = m_trackToVertexIPEstimator->estimate(trksItr->perigeeParameters(), pvtx, true);
    } 
    
    if (covariance == nullptr) {
      ATH_MSG_WARNING("No measured perigee parameters assigned to the track"); 
    }
    else{  
      AmgVector(5) perigeeParams = measPer->parameters(); 
      trkd0        = perigeeParams[Trk::d0];  
      trkz0        = perigeeParams[Trk::z0];    
      trkphi       = perigeeParams[Trk::phi0];  
      trktheta     = perigeeParams[Trk::theta];
      trketa       = measPer->eta(); 
      qOverP       = perigeeParams[Trk::qOverP]*1000.;  
      if(qOverP) trkP = 1/qOverP;
      trkpt        = measPer->pT()/1000.;  
      if (qOverP < 0) charge = -1;
      else charge=+1; 

      // correct the track d0 for the vertex position
      // would rather corrected for the beamline but could not find beamline
      trkd0c=trkd0-(yv*cos(trkphi)-xv*sin(trkphi));
      ATH_MSG_DEBUG("trkd0, trkd0c: " << trkd0 << ", " << trkd0c);

      // correct the track parameters for the beamspot position
      beamX = beamSpotX + tan(beamTiltX) * (trkz0-beamSpotZ);
      beamY = beamSpotY + tan(beamTiltY) * (trkz0-beamSpotZ);
      d0bscorr = trkd0 - ( -sin(trkphi)*beamX + cos(trkphi)*beamY );
    }    

    if (fitQual==nullptr) {
      ATH_MSG_WARNING("No fit quality assigned to the track"); 
    }       
    
    chisquared = (fitQual) ? fitQual->chiSquared() : -1.;
    DoF        = (fitQual) ? fitQual->numberDoF() : -1;
    if(DoF>0) chi2oDoF = chisquared/(float)DoF;

    if (trkphi<0) trkphi+=2*M_PI;
    
    ngTracks++;    
    ATH_MSG_DEBUG(nTracks << " is a good track!");  

   

    int nhpixB=0, nhpixECA=0, nhpixECC=0, nhsctB=0, nhsctECA=0, nhsctECC=0, nhtrtB=0, nhtrtECA=0, nhtrtECC=0;

    // loop over all hits on track
    ATH_MSG_VERBOSE ("  starting to loop over TSOS: " << trksItr->trackStateOnSurfaces()->size());
    for (const Trk::TrackStateOnSurface* tsos : *trksItr->trackStateOnSurfaces()) {
      //check that we have track parameters defined for the surface (pointer is not null)
      if(!(tsos->trackParameters())) {
	ATH_MSG_DEBUG(" hit skipped because no associated track parameters");
        continue;
      }
      
      Identifier surfaceID;
      const Trk::MeasurementBase* mesb=tsos->measurementOnTrack();
      // hits, outliers
      if (mesb != nullptr && mesb->associatedSurface().associatedDetectorElement()!=nullptr) surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();

      // holes, perigee 
      else continue; 

      if ( tsos->type(Trk::TrackStateOnSurface::Measurement) ){   
	//hit quality cuts for Si hits if tool is configured - default is NO CUTS
        if (m_idHelper->is_pixel(surfaceID) || m_idHelper->is_sct(surfaceID)) {
          if (m_doHitQuality) {
            ATH_MSG_DEBUG("applying hit quality cuts to Silicon hit...");
	    
            const Trk::RIO_OnTrack* hit = m_hitQualityTool->getGoodHit(tsos);
            if (hit == nullptr) {
              ATH_MSG_DEBUG("hit failed quality cuts and is rejected.");
              continue;
            } else {
              ATH_MSG_DEBUG("hit passed quality cuts");
            }
          } else {
            ATH_MSG_DEBUG("hit quality cuts NOT APPLIED to Silicon hit.");
          }
        } // hit is Pixel or SCT
	
        // --- pixel
        if (m_idHelper->is_pixel(surfaceID)){
          if(m_pixelID->barrel_ec(surfaceID)      ==  0){
            nhpixB++;
          }
          else if(m_pixelID->barrel_ec(surfaceID) ==  2)  nhpixECA++;
          else if(m_pixelID->barrel_ec(surfaceID) == -2) nhpixECC++;
        }
        // --- sct
        else if (m_idHelper->is_sct(surfaceID)){
          if(m_sctID->barrel_ec(surfaceID)      ==  0){
            nhsctB++;
          }
          else if(m_sctID->barrel_ec(surfaceID) ==  2) nhsctECA++;
          else if(m_sctID->barrel_ec(surfaceID) == -2) nhsctECC++;
        }
        // --- trt
        if (m_idHelper->is_trt(surfaceID)){
          int barrel_ec      = m_trtID->barrel_ec(surfaceID);
          if(barrel_ec == 1 || barrel_ec == -1 ) {
            nhtrtB++;
          }
          else if(barrel_ec ==  2){
            nhtrtECA++;
          }else if(barrel_ec == -2){
            nhtrtECC++;
          }
        }
      }
    }
    int nhpix= nhpixB +nhpixECA + nhpixECC;
    int nhsct= nhsctB +nhsctECA + nhsctECC;
    int nhtrt= nhtrtB +nhtrtECA + nhtrtECC;
    int nhits= nhpix+ nhsct+ nhtrt;

    auto nhits_per_track_m = Monitored::Scalar<float>( "m_nhits_per_track", nhits );
    fill(genericTrackGroup, nhits_per_track_m);
    auto npixelhits_per_track_m = Monitored::Scalar<float>( "m_npixelhits_per_track", nhpix );
    fill(genericTrackGroup, npixelhits_per_track_m);
    auto nscthits_per_track_m = Monitored::Scalar<float>( "m_nscthits_per_track", nhsct );
    fill(genericTrackGroup, nscthits_per_track_m);
    auto ntrthits_per_track_m = Monitored::Scalar<float>( "m_ntrthits_per_track", nhtrt );
    fill(genericTrackGroup, ntrthits_per_track_m);

    auto chi2oDoF_m = Monitored::Scalar<float>( "m_chi2oDoF", chi2oDoF );
    fill(genericTrackGroup, chi2oDoF_m);
    auto eta_m = Monitored::Scalar<float>( "m_eta", trketa );
    fill(genericTrackGroup, eta_m);
    if (charge>0){
    auto eta_pos_m = Monitored::Scalar<float>( "m_eta_pos", trketa );
    fill(genericTrackGroup, eta_pos_m);
    }
    else{
      auto eta_neg_m = Monitored::Scalar<float>( "m_eta_neg", trketa );
      fill(genericTrackGroup, eta_neg_m);
    }

    auto phi_m = Monitored::Scalar<float>( "m_phi", trkphi );
    fill(genericTrackGroup, phi_m);
    auto z0_m = Monitored::Scalar<float>( "m_z0", trkz0 );
    fill(genericTrackGroup, z0_m);
    float z0sintheta = trkz0*(sin(trktheta));
    auto z0sintheta_m = Monitored::Scalar<float>( "m_z0sintheta", z0sintheta );
    fill(genericTrackGroup, z0sintheta_m);
    auto d0_m = Monitored::Scalar<float>( "m_d0", trkd0 );
    fill(genericTrackGroup, d0_m);
    auto d0_bscorr_m = Monitored::Scalar<float>( "m_d0_bscorr", d0bscorr );
    fill(genericTrackGroup, d0_bscorr_m);

    float pT = charge*trkpt;
    auto pT_m = Monitored::Scalar<float>( "m_pT", pT );
    fill(genericTrackGroup, pT_m);
    auto p_m = Monitored::Scalar<float>( "m_p", trkP );
    fill(genericTrackGroup, p_m);

    delete myIPandSigma;
    myIPandSigma=nullptr;
  } // end of loop on trks

  ATH_MSG_DEBUG("Number of good tracks from TrackCollection: " << ngTracks);

  return StatusCode::SUCCESS;
}

const xAOD::Vertex* IDAlignMonGenericTracksAlg::findAssociatedVertexTP(const std::map<const xAOD::TrackParticle*, const xAOD::Vertex*>& trackVertexMapTP, const xAOD::TrackParticle *track) const
{

  std::map<const xAOD::TrackParticle*, const xAOD::Vertex* >::const_iterator tpVx = trackVertexMapTP.find( track);

  if (tpVx == trackVertexMapTP.end() ){
    ATH_MSG_VERBOSE("Did not find the vertex. Returning 0");
    return nullptr;
  } 
  return (*tpVx).second;

}


const Trk::Track* IDAlignMonGenericTracksAlg::getTrkTrack(const Trk::VxTrackAtVertex *trkAtVx)
{

  //find the link to the TrackParticleBase
  const Trk::ITrackLink* trkLink = trkAtVx->trackOrParticleLink();
  const Trk::TrackParticleBase* trkPB(nullptr);
  if(nullptr!= trkLink){
    const Trk::LinkToTrackParticleBase* linktrkPB = dynamic_cast<const Trk::LinkToTrackParticleBase *>(trkLink);
    if(nullptr!= linktrkPB){
      if(linktrkPB->isValid()) trkPB = linktrkPB->cachedElement();
    }//end of dynamic_cast check
  }//end of ITrackLink existance check

  //cast to TrackParticle
  if(trkPB){
    if ( trkPB->trackElementLink()->isValid() ) {      
      // retrieve and refit original track
      const Trk::Track* trktrk = trkPB->originalTrack();
      return trktrk;
    } 
  } 
  return nullptr;
}



bool IDAlignMonGenericTracksAlg::fillVertexInformation(std::map<const xAOD::TrackParticle*, const xAOD::Vertex*>& trackVertexMapTP, const EventContext& ctx ) const
{
  ATH_MSG_DEBUG("Generic Tracks: fillVertexInformation(): Checking ");
  trackVertexMapTP.clear();

  // retrieving vertices
  auto handle_vxContainer = SG::makeHandle(m_VxPrimContainerName, ctx);
  
  if (!handle_vxContainer.isPresent()) {
    ATH_MSG_DEBUG ("InDetGlobalPrimaryVertexMonAlg: StoreGate doesn't contain primary vertex container with key "+m_VxPrimContainerName.key());
    return false;
  }
  if (!handle_vxContainer.isValid()) {
    ATH_MSG_ERROR ("InDetGlobalPrimaryVertexMonAlg: Could not retrieve primary vertex container with key "+m_VxPrimContainerName.key());
    return false;
  }

  const auto *vertexContainer = handle_vxContainer.cptr();
    
  for(const auto & vtx : *vertexContainer) {
      auto tpLinks = vtx->trackParticleLinks();
      ATH_MSG_DEBUG("tpLinks size " << tpLinks.size());

      if (tpLinks.size() > 4 ) {
	for(const auto& link: tpLinks) {
	  const xAOD::TrackParticle *TP = *link;
          if(TP) {
            trackVertexMapTP.insert( std::make_pair( TP, vtx )  );
          }
        }
      }
    }

    return true;
}
