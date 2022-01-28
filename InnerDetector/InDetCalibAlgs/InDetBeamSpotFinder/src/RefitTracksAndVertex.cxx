/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// @file RefitTracksAndVertex.cxx
//  Anthony Morley
//

#include "RefitTracksAndVertex.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "VxVertex/VxCandidate.h"



RefitTracksAndVertex::RefitTracksAndVertex( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAlgorithm   (name, pSvcLocator)
  , m_idHelper(nullptr)
  , m_vertexListInput("PrimaryVertices")
  , m_trackListOutput("SiTracksWithRefitTracksAndVertex")
  , m_outputVertexContainerName("RefittedPrimaryVertex")
  , m_trackFitter("Trk::KalmanFitter/TrkKalmanFitter")
  , m_vertexFitter("Trk::FullVertexFitter")
  , m_applyTrkSel(false)
  , m_refitTracks(true)
  , m_addPM(false)
  , m_selPtMin(0)
  , m_selEtaMin(-2.5)
  , m_selEtaMax(2.5)
  , m_selNHitPIXMin(0)
  , m_selNHitSCTMin(6)
  , m_nTracksProcessed(0)
  , m_nTracksPresel(0)
  , m_nTracksAccepted(0)
  , m_nRejectPIX(0)
  , m_nRejectSCT(0)
  , m_nRejectTRT(0)
  , m_nRejectPM(0)
{
  // declare algorithm parameters   
  declareProperty("VertexListInput"     , m_vertexListInput) ;
  declareProperty("TrackListOutput"    , m_trackListOutput) ;
  declareProperty("OutputVertexContainer"        , m_outputVertexContainerName) ;
  declareProperty("TrackFitter"        , m_trackFitter) ;
  declareProperty("VertexFitterTool"   , m_vertexFitter);
  declareProperty("ApplyTrkSel"        , m_applyTrkSel) ;
  declareProperty("RefitTracks"        , m_refitTracks) ;
  declareProperty("AddPseudoMeasurement", m_addPM);
  declareProperty("SelPtMin"           , m_selPtMin) ;
  declareProperty("SelEtaMin"          , m_selEtaMin) ;
  declareProperty("SelEtaMax"          , m_selEtaMax) ;
  declareProperty("SelNHitPIXMin"      , m_selNHitPIXMin) ;
  declareProperty("SelNHitSCTMin"      , m_selNHitSCTMin) ;

}

RefitTracksAndVertex::~RefitTracksAndVertex() {}

StatusCode RefitTracksAndVertex::initialize() {
  //retrieve the DetectorStore service
  StatusCode status=detStore().retrieve() ;
  if( status.isFailure() ) {
    ATH_MSG_FATAL(  "DetectorStore service not found !" );
    return status;
  }else{ 
    ATH_MSG_DEBUG( "DetectorStore retrieved!" );
  }

  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_FATAL(  "Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }

  // Set up fitter
  status = m_trackFitter.retrieve();
  if (status.isFailure()) {
    ATH_MSG_FATAL( "Could not find tool " << m_trackFitter << ". Exiting.");
    return status ;
  } else {
    ATH_MSG_DEBUG( " Got " << m_trackFitter << " as TrackFitter. " ) ;
  }

 // Set up fitter
  status = m_vertexFitter.retrieve();
  if (status.isFailure()) {
    ATH_MSG_FATAL( "Could not find tool " << m_vertexFitter << ". Exiting.");
    return status ;
  } else {
    ATH_MSG_DEBUG( " Got " << m_vertexFitter << " as VertexFitter. " ) ;
  }

  // Print input properties
  if( m_applyTrkSel ) {
    ATH_MSG_DEBUG(
             "Track selection will be applied:"
          << "\n " << m_selEtaMin << " < eta < " << m_selEtaMax
          << "\n pT                            >  " << m_selPtMin/Gaudi::Units::GeV << " GeV"
          << "\n number_of_PIXEL_hits          >= " << m_selNHitPIXMin
          << "\n number_of_SCT_hits            >= " << m_selNHitSCTMin
                ) ;
  } else {
    ATH_MSG_DEBUG( "NO selection will be applied to tracks" ) ;
  }
  return StatusCode::SUCCESS;
} // initialize(.)

StatusCode RefitTracksAndVertex::finalize() {
  ATH_MSG_DEBUG( (*this) ) ;
  return StatusCode::SUCCESS;
}

StatusCode RefitTracksAndVertex::execute() {

  TrackCollection* outputtracks = new TrackCollection() ;

  //---------------------------
  // Primary Vertex
  //---------------------------
  const xAOD::VertexContainer* vertices = 0;
  if ( !evtStore()->retrieve( vertices, m_vertexListInput ).isSuccess() ){ // retrieve arguments: container type, container key
    ATH_MSG_WARNING("execute() Failed to retrieve Reconstructed vertex container. " << m_vertexListInput );
    //ATH_MSG_ERROR(evtStore()->dump());
    delete outputtracks;
    return StatusCode::SUCCESS; //?? really
  }
  
  if( evtStore()->record( outputtracks, m_trackListOutput ).isFailure() ) {
    ATH_MSG_ERROR( "Failed to record trackcollection with name " << m_trackListOutput );
  }

  xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer;
  xAOD::VertexAuxContainer* theVertexAuxContainer = new xAOD::VertexAuxContainer;
  theVertexContainer->setStore( theVertexAuxContainer );

  CHECK( evtStore()->record(theVertexContainer,  m_outputVertexContainerName) );
  CHECK( evtStore()->record(theVertexAuxContainer, m_outputVertexContainerName + "Aux.") );

  const xAOD::Vertex* primaryVertex = 0;  
  for( const xAOD::Vertex* vertex: *vertices ) {
    if( vertex->vertexType() == xAOD::VxType::PriVtx ) {
      primaryVertex = vertex;
      break;
    }
  }
  
  std::vector< const Trk::TrackParameters*  > trackParametersToFit;
  
  // Check we have found a PV
  if( primaryVertex ){
    if(primaryVertex->nTrackParticles() < 2)
      return StatusCode::SUCCESS;
    
    //Loop over all tracks in the PV
    for(unsigned int i =0; i< primaryVertex->nTrackParticles(); ++i ){
      auto trackParticle  = primaryVertex->trackParticle( i );
      auto track =   trackParticle->track();
      // Check that there is Trk::Track
      if(track){
        //Refit Track using on the SCT hits
        if(m_refitTracks){
          auto newTrack = fitSCTOnlyTrack( track ) ;
          if(newTrack){
            outputtracks->push_back( newTrack ) ;
            if(newTrack->perigeeParameters())
              trackParametersToFit.push_back( newTrack->perigeeParameters() ); 
          } 
        } else {
          if(track->perigeeParameters())
            trackParametersToFit.push_back( track->perigeeParameters() );
        }
      }
    }
    m_nTracksProcessed += primaryVertex->nTrackParticles() ;
    m_nTracksAccepted  += outputtracks->size() ;
    // Make sure there are at least 2 tracks to fit in the vertex fit
    
    ATH_MSG_DEBUG(" From: " << primaryVertex->nTrackParticles()  << " tracks " << trackParametersToFit.size() << " will be used" );
    
    if(trackParametersToFit.size() > 1){
      
      Amg::Vector3D position = primaryVertex->position();
      // Fit and store vertex 
      auto vertex =  m_vertexFitter->fit(trackParametersToFit, position );
      if(vertex){
        theVertexContainer->push_back (vertex);
        vertex->setVertexType(xAOD::VxType::PriVtx);

        ATH_MSG_DEBUG("Old Vtx " << primaryVertex->position());
        ATH_MSG_DEBUG("New Vtx " << vertex->position());
      }
    }
  }	

  
  


  return StatusCode::SUCCESS;
} // execute(.)


const Trk::PseudoMeasurementOnTrack* RefitTracksAndVertex::createPMfromSi ( const Trk::Perigee* mp ) {
  Trk::DefinedParameter z0   ( mp->parameters()[Trk::z0],    Trk::z0    ) ;
  Trk::DefinedParameter theta( mp->parameters()[Trk::theta], Trk::theta ) ;
  std::vector<Trk::DefinedParameter> defPar ;
  defPar.push_back( z0    ) ;
  defPar.push_back( theta ) ;
  if( !mp->covariance() )  return 0;
  Trk::LocalParameters  parFromSi( defPar ) ;
  AmgSymMatrix(2) covFromSi;
  
  covFromSi( 0, 0 ) = (*mp->covariance())( Trk::z0,Trk::z0 ) ;
  covFromSi( 1, 1 ) = (*mp->covariance())( Trk::theta,Trk::theta ) ;
  covFromSi( 1, 0 ) = (*mp->covariance())( Trk::z0, Trk::theta ) ;
  covFromSi( 0, 1 ) = (*mp->covariance())( Trk::z0, Trk::theta ) ;
 
  const Trk::Surface& mpSurf = mp->associatedSurface() ;
  
  Trk::PseudoMeasurementOnTrack *pm = new Trk::PseudoMeasurementOnTrack( parFromSi
                                                                       , covFromSi
                                                                       , mpSurf
                                                                       ) ;
  return pm ;
}

const Trk::MeasurementSet RefitTracksAndVertex::addPM( Trk::MeasurementSet& ms, const Trk::PseudoMeasurementOnTrack* pm ) {
  Trk::MeasurementSet sortedMS ;
  sortedMS.push_back( pm ) ;
  for( int i=0, i_max=ms.size() ; i!=i_max ; ++i ) {
    sortedMS.push_back( ms[i] ) ;
  }
  return sortedMS ;
}

Trk::Track* RefitTracksAndVertex::fitSCTOnlyTrack(const Trk::Track* track) {
  Trk::MeasurementSet setSCT ;
  Trk::MeasurementSet setPix ;
  Trk::MeasurementSet setTRT ;

  const Trk::Perigee* perTrk = track->perigeeParameters();
  if( !perTrk ) {
    ATH_MSG_WARNING("RefitTracksAndVertex() : No Perigee parameter on track!");
    return NULL ;
  }
 
  if( perTrk->eta() < m_selEtaMin || perTrk->eta() > m_selEtaMax)
    return NULL ;

  if( perTrk->pT() < m_selPtMin)
    return NULL ;
    
  
  //store all silicon measurements into the measurementset
  DataVector<const Trk::MeasurementBase>::const_iterator it      = track->measurementsOnTrack()->begin();
  DataVector<const Trk::MeasurementBase>::const_iterator itEnd   = track->measurementsOnTrack()->end(); 
  for ( ; it!=itEnd; ++it){ 
    if( !(*it) ) {
      //      log (MSG::WARNING) << "The MeasurementBase set has a void"
      //    << "  member! Skip it.." << endmsg;
    } else {
      const Trk::RIO_OnTrack* rio = dynamic_cast <const Trk::RIO_OnTrack*>(*it);
      if (rio != 0) {
        const Identifier& surfaceID = (rio->identify()) ;
        if( m_idHelper->is_sct(surfaceID) ) {
          setSCT.push_back ( *it ) ;
        } else if( m_idHelper->is_trt(surfaceID) ) {
          setTRT.push_back ( *it ) ;
        } else if( m_idHelper->is_pixel(surfaceID) ){
          setPix.push_back ( *it ) ; 
        }
      }
    }
  }
  
  if( (int)setSCT.size() <  m_selNHitSCTMin ) 
    return NULL; 

  ATH_MSG_DEBUG("RefitTracksAndVertex() : Found " << setSCT.size()  << " SCT measurm's!" ) ;
  ATH_MSG_DEBUG("RefitTracksAndVertex() : Found " << setPix.size()  << " Pix measurm's!" ) ;
  ATH_MSG_DEBUG("RefitTracksAndVertex() : Found " << setTRT.size()  << " TRT measurm's!") ;


  ATH_MSG_DEBUG( std::setiosflags( std::ios::scientific )) ;
  ATH_MSG_DEBUG (std::setprecision( 7 )) ;
  ATH_MSG_VERBOSE( std::setiosflags( std::ios::scientific )) ;
  ATH_MSG_VERBOSE( std::setprecision( 7 )) ;

  // now add z_0 and theta from original track as PseudoMeas to the TRT MeasurementSet
  if(m_addPM){
    const Trk::PseudoMeasurementOnTrack *pmFromSi = createPMfromSi( perTrk ) ;
    if( !pmFromSi ) {
      ATH_MSG_ERROR("RefitTracksAndVertex() : PseudoMeasurementOnTrack creation failed! " );
      return NULL ;
    }
    ATH_MSG_DEBUG( "RefitTracksAndVertex() : pmFromSi " << *pmFromSi) ;
    Trk::MeasurementSet setSCT = addPM( setSCT, pmFromSi ) ;
  }
  
  // Add TRT hits as they do no harm
  for( int i=0, i_max=setTRT.size() ; i!=i_max ; ++i ) {
    setSCT.push_back( setTRT[i] ) ;
  }
  
  ATH_MSG_VERBOSE ( "RefitTracksAndVertex() : Si+PM MeasurementSet : " );
  for( int i=0, i_max=setSCT.size() ; i!=i_max ; ++i ) {
    ATH_MSG_VERBOSE ("============== i=" << i << " =============");
    ATH_MSG_VERBOSE ( *(setSCT[i]));
  }
   ATH_MSG_VERBOSE ("==========================================");
  
  // fit TRT part of the track with PseudoMeas on z_0, theta
  Trk::Track* trkSCT=(m_trackFitter->fit(Gaudi::Hive::currentContext()
                                       ,setSCT
                                       , *perTrk
                                       , true
                                       , Trk::pion
                                       //, Trk::muon
                                       //, Trk::nonInteracting
                                       )).release() ;
  if( !trkSCT ) {
    ATH_MSG_DEBUG( "RefitTracksAndVertex() : Fit of SCT part of the track failed! " ) ;
    return NULL ;
  }
  const Trk::Perigee* perSCT = trkSCT->perigeeParameters();
  ATH_MSG_VERBOSE( "RefitTracksAndVertex() : perSCT " << *perSCT) ;

  return trkSCT;
} 

MsgStream& operator<<( MsgStream& outst, const RefitTracksAndVertex& alg ) {
  return alg.dump( outst ) ;
}

MsgStream& RefitTracksAndVertex::dump( MsgStream& outst ) const {
  outst << std::endl ;
  outst << "|-------------------------------------------------------------------";
  outst << "-----------------------------|" << std::endl ;
  outst << "|  processed                      : " 
        << std::setw(7) << m_nTracksProcessed 
        << " tracks                                               |"
        << std::endl ;
  outst << "|  accepted by track presel.      : " 
        << std::setw(7) << m_nTracksPresel 
        << " tracks                                               |"
        << std::endl ;
  outst << "|  accepted by track presel. + PM : " 
        << std::setw(7) << m_nTracksAccepted 
        << " tracks                                               |"
        << std::endl ;
  outst << "| ------------------------------------------------------------------";
  outst << "---------------------------- |" << std::endl ;
  outst << "|  reject by # PIX hits           : " 
        << std::setw(7) << m_nRejectPIX 
        << " tracks                                               |"
        << std::endl ;
  outst << "|  reject by # SCT hits           : " 
        << std::setw(7) << m_nRejectSCT 
        << " tracks                                               |"
        << std::endl ;
  outst << "|  reject by # TRT hits           : " 
        << std::setw(7) << m_nRejectTRT 
        << " tracks                                               |"
        << std::endl ;
  outst << "| ------------------------------------------------------------------";
  outst << "---------------------------- |" << std::endl ;
  outst << "|  reject by exist. PM(TRT)       : " 
        << std::setw(7) << m_nRejectPM 
        << " tracks                                               |"
        << std::endl ;
  outst << "|-------------------------------------------------------------------";
  outst << "-----------------------------|" << std::endl ;
  return outst ;
}
