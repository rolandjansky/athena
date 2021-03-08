/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigT2HistoPrmVtxAllTE.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2HistoPrmVtx
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigT2HistoPrmVtxAllTEMT.h"
#include "TrigT2HistoPrmVtxBaseMT.h"
#include "HistoVertexHelper.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "BeamSpotConditionsData/BeamSpotData.h"


//** ----------------------------------------------------------------------------------------------------------------- **//

TrigT2HistoPrmVtxAllTEMT::TrigT2HistoPrmVtxAllTEMT(const std::string& name, ISvcLocator* pSvcLocator) :
  TrigT2HistoPrmVtxBaseMT( name,pSvcLocator ),
  m_hisVtx( std::unique_ptr< HistoVertexHelper >( new HistoVertexHelper(8000, -200, 200) ) )
{
}


//** ----------------------------------------------------------------------------------------------------------------- **//

StatusCode TrigT2HistoPrmVtxAllTEMT::initialize() {
  ATH_MSG_INFO( "Inizializing " << name() <<" ... " );

  // Print properties
  CHECK( TrigT2HistoPrmVtxBaseMT::initialize() );

  // Initialize Beam Spot
  CHECK( m_beamSpotKey.initialize() );

  // Initialize input Keys
  if ( m_inputRoIContainerKey.key() == "Undefined" ) {
    ATH_MSG_ERROR( "Input RoI Container Key is Undefined!" );
    return StatusCode::FAILURE;
  }

  if ( m_inputTracksContainerKey.key() == "Undefined" ) {
    ATH_MSG_ERROR( "Input Track Container Key is Undefined!" );
    return StatusCode::FAILURE;
  }

  CHECK( m_inputRoIContainerKey.initialize() );
  CHECK( m_inputTracksContainerKey.initialize() );

  // Initialize output Keys
  ATH_MSG_DEBUG( "Output Vertex Container is set to " << m_outputVertexContainerKey.key() );
  CHECK( m_outputVertexContainerKey.initialize() );

  return StatusCode::SUCCESS;
}

//** ----------------------------------------------------------------------------------------------------------------- **//

unsigned int TrigT2HistoPrmVtxAllTEMT::getTrackNumbers(const xAOD::TrackParticleContainer* pointerToEFTrackCollections) {
  
  unsigned int nEFtracks = 0;
  
  if ( pointerToEFTrackCollections ) {
    nEFtracks = pointerToEFTrackCollections->size();
    ATH_MSG_DEBUG( "Found " << nEFtracks << " tracks in the RoI"  );
  } else {
    ATH_MSG_DEBUG( "No tracks in the RoI"  );
  }
  
  return nEFtracks;
}

//** ----------------------------------------------------------------------------------------------------------------- **//

StatusCode TrigT2HistoPrmVtxAllTEMT::execute() {
  ATH_MSG_DEBUG( "Executing " << name() <<" ... " );

  const EventContext& ctx = getContext();

  // ==============================================================================================================================
  //    ** Prepare Input
  // ==============================================================================================================================

  ATH_MSG_DEBUG( "Ready to retrieve RoIs : " << m_inputRoIContainerKey.key() );
  SG::ReadHandle< TrigRoiDescriptorCollection > roiContainerHandle = SG::makeHandle( m_inputRoIContainerKey,ctx ); 
  CHECK( roiContainerHandle.isValid() );

  const TrigRoiDescriptorCollection *roiContainer = roiContainerHandle.get();
  ATH_MSG_DEBUG( "Retrieved " << roiContainer->size() <<" RoIs" );
  for ( unsigned int i(0); i<roiContainer->size(); i++ )
    ATH_MSG_DEBUG( "   * roi eta=" << roiContainer->at(i)->eta() <<
		   " phi=" << roiContainer->at(i)->phi() );


  
  ATH_MSG_DEBUG( "Ready to retrieve FTF tracks : " << m_inputTracksContainerKey.key() );
  SG::ReadHandle< xAOD::TrackParticleContainer > trackParticleContainerHandle = SG::makeHandle( m_inputTracksContainerKey,ctx );
  CHECK( trackParticleContainerHandle.isValid() );

  const xAOD::TrackParticleContainer *trackParticleContainer = trackParticleContainerHandle.get();
  ATH_MSG_DEBUG( "Retrieved " << trackParticleContainer->size() <<" FTF tracks" );
  for ( unsigned int i(0); i<trackParticleContainer->size(); i++ ) 
    ATH_MSG_DEBUG( "   * track pt=" << trackParticleContainer->at(i)->p4().Et() <<
		   " eta=" << trackParticleContainer->at(i)->eta() <<
		   " phi=" << trackParticleContainer->at(i)->phi()  );
  
  // ==============================================================================================================================
  //    ** Prepare Output
  // ==============================================================================================================================

  std::unique_ptr< xAOD::VertexContainer > primaryVertexContainer( new xAOD::VertexContainer() );
  std::unique_ptr< xAOD::VertexAuxContainer > primaryVertexAuxContainer( new xAOD::VertexAuxContainer() );
  primaryVertexContainer->setStore( primaryVertexAuxContainer.get() );

  // ==============================================================================================================================

  float zFirstTrack = 0;
  float zFirstTrackError = 0;

  m_totTracks = 0;
  m_totTracks_All = 0;
  m_totSelTracks = 0;
  m_totSelTracks_All = 0;

  m_hisVtx->reset();

  for (int i = 0; i <3; i++) {
    m_zPrmVtx.at(i) = -200;
    m_zPrmVtxSigmaAll.at(i)  = -200;
    m_zPrmVtxSigma2Trk.at(i) = -200;
    m_nTrackVtx.at(i) = -200;
  }

  // Retrieve beamspot information
  float xBeamSpot = 0.0;
  float yBeamSpot = 0.0;
  float zBeamSpot = 0.0;
  float xBeamSpotSigma = 1e-6;
  float yBeamSpotSigma = 1e-6;
  float zBeamSpotSigma = 1e-6;
  float xBeamSpotTilt = 0.0;
  float yBeamSpotTilt = 0.0;
  
  if (m_useBeamSpot) {
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
    
    if  (beamSpotHandle.isValid() == false ) {

      ATH_MSG_WARNING( "Could not retrieve Beam Conditions Service. "  );
      ATH_MSG_WARNING( "Using origin at ( " << xBeamSpot << " , " << yBeamSpot << " , " << zBeamSpot << " ) "  );
      
    } else {
      
      const Amg::Vector3D &beamSpot = beamSpotHandle->beamPos();
      
      xBeamSpot = beamSpot.x();
      yBeamSpot = beamSpot.y();
      zBeamSpot = beamSpot.z();
      xBeamSpotSigma = beamSpotHandle->beamSigma(0);
      yBeamSpotSigma = beamSpotHandle->beamSigma(1);
      zBeamSpotSigma = beamSpotHandle->beamSigma(2);
      xBeamSpotTilt = beamSpotHandle->beamTilt(0);
      yBeamSpotTilt = beamSpotHandle->beamTilt(1);

      ATH_MSG_DEBUG( "Beam spot from service: x = " << xBeamSpot << " +/- " << xBeamSpotSigma << "   "
                     << "y = " << yBeamSpot << " +/- " << yBeamSpotSigma << "   "
                     << "z = " << zBeamSpot << " +/- " << zBeamSpotSigma  );
    }
  }


  for ( const TrigRoiDescriptor* roiDescriptor : *roiContainer ) {

    zFirstTrack = 0;
    m_totTracks = 0;
    m_totSelTracks = 0;

    m_totTracks = getTrackNumbers( trackParticleContainer );
    m_totTracks_All += m_totTracks;
    m_listCutApplied.clear();
    
    
    // Retrieve z0 parameters  
    for (unsigned int i(0); i<m_totTracks; i++) {
      
      if (i == 0) 
	ATH_MSG_DEBUG( "Loop over tracks: retrieving z0"  );
      
      const xAOD::TrackParticle* track = trackParticleContainer->at(i);
      float z0 =  track->z0();
      const xAOD::ParametersCovMatrix_t covTrk = track->definingParametersCovMatrix();
      float ez0 = Amg::error(covTrk, 1);
      
      if ( trackSel(track, i, roiDescriptor->eta(), roiDescriptor->phi()) == false ) 
	continue;
      
      m_totSelTracks++;
      m_totSelTracks_All++;
      
      if ( m_totSelTracks == 1 ) {
	zFirstTrack = z0;
	zFirstTrackError = ez0;
      }
      
      ATH_MSG_DEBUG( "Track number " << i+1 << " with z0 = " << z0 << " and ez0 = " << ez0  );      
      m_hisVtx->fill( z0 );
    }
  } // End loop on rois

  ATH_MSG_DEBUG( "m_totSelTracks = " << m_totSelTracks );

  // Get primary vertex from TrigT2HistoPrmVtxAllTE::findPrmVtx
  if ( m_totSelTracks_All == 1 ) {
    m_zPrmVtx.at(0)          = zFirstTrack;
    m_zPrmVtxSigmaAll.at(0)  = zFirstTrackError;
    m_nTrackVtx.at(0)        = 1;
    m_nVtxFound = 1;
  } else {
    findPrmVtx( m_hisVtx );
  }

  for ( unsigned int i(0); i<m_zPrmVtx.size(); i++ )
    ATH_MSG_DEBUG( "Primary vertex " << i << ": z=" << m_zPrmVtx.at(i) << ", sigma=" << m_zPrmVtxSigmaAll.at(i) 
		   << ", nTracks=" << m_nTrackVtx.at(i) << ", and ntot track in all ROIs" << m_totSelTracks_All  );


  // ==============================================================================================================================

  // Create the xAOD container and its auxiliary store:
  //  xAOD::Vertex* trigVertex = new xAOD::Vertex();
  std::unique_ptr< xAOD::Vertex > trigVertex = std::make_unique< xAOD::Vertex >( xAOD::Vertex() );
  primaryVertexContainer->push_back( trigVertex.release() );

  primaryVertexContainer->back()->setZ( m_zPrmVtx.at(0) + zBeamSpot );  
  primaryVertexContainer->back()->setX( xBeamSpot + tan(xBeamSpotTilt ) * primaryVertexContainer->back()->z());  
  primaryVertexContainer->back()->setY( yBeamSpot + tan(yBeamSpotTilt ) * primaryVertexContainer->back()->z()); 
  primaryVertexContainer->back()->setVertexType( xAOD::VxType::PriVtx ); 

  AmgSymMatrix(3) cov;
  cov(0,0) = xBeamSpotSigma*xBeamSpotSigma;
  cov(1,1) = yBeamSpotSigma*yBeamSpotSigma;
  cov(2,2) = m_zPrmVtxSigmaAll.at(0)*m_zPrmVtxSigmaAll.at(0);
  cov(0,1) = cov(1,0) = 0.0; 
  cov(0,2) = cov(2,0) = 0.0;
  cov(1,2) = cov(2,1) = 0.0;
  primaryVertexContainer->back()->setCovariancePosition( cov );

  // ==============================================================================================================================
  //    ** Store Output
  // ==============================================================================================================================

  ATH_MSG_DEBUG( "Found " << primaryVertexContainer->size() <<" PVs " );
  for ( const xAOD::Vertex *primVertex : *primaryVertexContainer ) 
    ATH_MSG_DEBUG( "Prim Vtx: x=" << primVertex->x() << " "
		   << "y=" << primVertex->y() << " " 
		   << "z=" << primVertex->z() );
  
  ATH_MSG_DEBUG( "Storing vertex collection with key " << m_outputVertexContainerKey.key() );
  SG::WriteHandle< xAOD::VertexContainer > outputVertexConainerHandle = SG::makeHandle( m_outputVertexContainerKey,ctx );
  ATH_CHECK( outputVertexConainerHandle.record( std::move(primaryVertexContainer),std::move(primaryVertexAuxContainer) ) ); 

  return StatusCode::SUCCESS;
}

/** ----------------------------------------------------------------------------------------------------------------- **/
