/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
     
///////////////////////////////////////////////////////////////////
//   Implementation file for class VrtSecDecay
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "VrtSecDecay/VrtSecDecay.h"
#include "GaudiKernel/MsgStream.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkMeasurementBase/MeasurementBase.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

VKalVrtAthena::VrtSecDecay::VrtSecDecay (const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_iVertexFitter                ( "Trk::TrkVKalVrtFitter"  ),
  m_trackToVertexTool            ( "Reco::TrackToVertex"    )



{

  //
  m_outputVtxContainer        = "VrtSecDecay";
  m_parentxAODTrkContainer    = "InDetPixelThreeLayerTrackParticles";
  m_childxAODTrkContainer     = "InDetTrackParticles";
  
  //
  m_doHitPatternMatching      = false;
  m_doAngularMatching         = true;
  m_doRadiusSorting           = false;
  m_maxVtxPerEvent            = 100;
  m_vtxQuality                = 10.0;
  m_minVtxRadius              = 85.0;
  m_maxVtxRadius              = 300.0;
  m_parentPt                  = 20000.0;
  m_parentMinEta              = 0.1;
  m_parentMaxEta              = 2.0;
  m_childMinPt                = 100.0;
  m_childMaxPt                = 1500.0;
  m_childBLayerHits           = 0;
  m_childNextToInPixLayerHits = 0;
  m_childPixHits              = 1;
  m_childSCTHits              = 6;

  //
  declareProperty("VertexFitterTool",m_iVertexFitter);
  declareProperty("OutputVtxContainer",m_outputVtxContainer);
  declareProperty("ParentxAODContainer",m_parentxAODTrkContainer);
  declareProperty("ChildxAODContainer",m_childxAODTrkContainer);
  declareProperty("DoHitPatternMatching",m_doHitPatternMatching);
  declareProperty("DoAngularMatching",m_doAngularMatching);
  declareProperty("DoRadiusSorting",m_doRadiusSorting);
  declareProperty("MaxVtxPerEvent",m_maxVtxPerEvent);
  declareProperty("VtxQuality",m_vtxQuality);
  declareProperty("VtxMinRadius",m_minVtxRadius);
  declareProperty("VtxMaxRadius",m_maxVtxRadius);
  declareProperty("ParentPt",m_parentPt);
  declareProperty("ParentMinEta",m_parentMinEta);
  declareProperty("ParentMaxEta",m_parentMaxEta);
  declareProperty("ChildMinPt",m_childMinPt);
  declareProperty("ChildMaxPt",m_childMaxPt);
  declareProperty("ChildBLayerHits",m_childBLayerHits);
  declareProperty("ChildNextToInPixLayerHits",m_childNextToInPixLayerHits);
  declareProperty("ChildPixHits",m_childPixHits);
  declareProperty("ChildSCTHits",m_childSCTHits);

}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

VKalVrtAthena::VrtSecDecay::~VrtSecDecay()
{

}

///////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////

StatusCode VKalVrtAthena::VrtSecDecay::initialize()
{

  /* Get the right vertex fitting tool from ToolSvc */
  if ( m_iVertexFitter.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVertexFitter << endreq;
    return StatusCode::FAILURE;
  }
   else {
    msg(MSG::INFO) << "Retrieved tool " << m_iVertexFitter << endreq;
  }

  /* Get the TrackToVertex extrapolator tool*/
  if ( m_trackToVertexTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("initialize: failed to retrieve trackToVertex tool ");
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_INFO("initialize: Retrieved Reco::TrackToVertex Tool" << m_trackToVertexTool);
  }


  
  // Return gracefully 
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode VKalVrtAthena::VrtSecDecay::execute()
{

  //
  return doTrackParticleMethod();
  //

}
///////////////////////////////////////////////////////////////////
// Execute strategy using xAOD::TrackParticles
///////////////////////////////////////////////////////////////////
StatusCode VKalVrtAthena::VrtSecDecay::doTrackParticleMethod()
{

  //
  // Tracklet container: i.e. parent particle that "decays"
  //
  const xAOD::TrackParticleContainer* tracklets(0);
  if( evtStore()->retrieve ( tracklets, m_parentxAODTrkContainer ).isFailure() ){
    ATH_MSG_WARNING( "Could not find xAOD::TrackParticleContainer " << m_parentxAODTrkContainer << " in StoreGate!");
    return StatusCode::FAILURE;
  }
  
  //
  // Daughter particle
  //
  const xAOD::TrackParticleContainer* tracks(0);
  if( evtStore()->retrieve ( tracks, m_childxAODTrkContainer ).isFailure() ){
    ATH_MSG_ERROR( "Could not find xAOD::TrackParticleContainer " << m_childxAODTrkContainer << " in StoreGate!");
    return StatusCode::FAILURE;
  }
  
  // Pre-select soft-tracks so we don't have to 
  // apply this pre-selection for N tracklets in the loop below
  std::vector<const xAOD::TrackParticle*> preselected_tracks = preselectTracks(tracks);

  //
  // ----- Track selection -------
  //
  std::vector<const xAOD::TrackParticle*> selectedTracks; 
  selectedTracks.clear();

  // 
  // ----- Output vertex containers -------
  ///
  xAOD::VertexContainer* VtxContainer         = new xAOD::VertexContainer();
  xAOD::VertexAuxContainer* VtxContainerAux   = new xAOD::VertexAuxContainer();
  VtxContainer->setStore( VtxContainerAux );

  //
  std::vector<VtxObj> initialVertices;

  // Select the tracks we want
  for( const auto& tracklet : *tracklets ){

    /*
      Parent pT and eta cuts
    */ 
    if( tracklet->pt() < m_parentPt             ) continue;
    if( fabs(tracklet->eta()) > m_parentMaxEta  ) continue;
    if( fabs(tracklet->eta()) < m_parentMinEta  ) continue;

    //
    initialVertices.clear();

    //
    for( const auto& pion : preselected_tracks ){

      /*
        Consistency checks before doing any vertexing 
      */
      if( m_doAngularMatching && tracklet->p4().DeltaR( pion->p4() ) > 1.5 ) continue;
      if( m_doHitPatternMatching &&  !consistentHitPattern(tracklet,pion)  ) continue;

      selectedTracks.clear();
      selectedTracks.push_back(tracklet);
      selectedTracks.push_back(pion);

      /*
        Inital position estimation
      */
      Amg::Vector3D initPos(0.,0.,0.);
      StatusCode sc = m_iVertexFitter->VKalVrtFitFast( selectedTracks, initPos );
      if( sc.isFailure() ){
        ATH_MSG_INFO("FAILED TO ESTIMATE VTX INITIAL POSITION ");
        continue;
      }
 
      xAOD::Vertex* myVertex = 0;
      myVertex = m_iVertexFitter->fit(selectedTracks, initPos);
      
      /*
         Vertex selections
          1) Check if vertex exists
          2) Checked chi2/nDOF is satisified
          3) Checked position is satisified
           
         If any criteria fails, clean up the vertex pointer
      */
      if( !passVtxSelection(myVertex) ) continue;

      // Vertex type
      myVertex->setVertexType(xAOD::VxType::SecVtx);

      // Add in track parameters with respect to vertex
      decorateTrkWithVertexParameters(pion,myVertex);
      decorateTrkWithVertexParameters(tracklet,myVertex);

      // Links to the track particles in the vertex fit
      std::vector<ElementLink<xAOD::TrackParticleContainer>> trackLinks;
      trackLinks.push_back( ElementLink<xAOD::TrackParticleContainer>(pion, *tracks));
      trackLinks.push_back( ElementLink<xAOD::TrackParticleContainer>(tracklet, *tracklets));
      myVertex->setTrackParticleLinks( trackLinks );

      // Save for now..
      VtxContainer->push_back(myVertex);

      // Cache
      //initialVertices.push_back( VtxObj(myVertex,selectedTracks) );

    } // First iteration of vertex fits

    // Radius sorting method 
    // Eventually two track vertices will be clustered
    //if( m_doRadiusSorting ){
    //  std::sort( initialVertices.begin(), initialVertices.end(), rank_radius );
    //}

  } //  Loop over tracklets 

  // Write them to store gate  
  if( evtStore()->record(VtxContainer, m_outputVtxContainer).isFailure() ){
    ATH_MSG_WARNING( "Could not write " << m_outputVtxContainer << " to StoreGate.");
    return StatusCode::FAILURE;
  }
  
  if( evtStore()->record(VtxContainerAux, m_outputVtxContainer+"Aux.").isFailure() ){
    ATH_MSG_WARNING( "Could not write " << m_outputVtxContainer << "Aux. to StoreGate.");
    return StatusCode::SUCCESS;
  }

  // Return gracefully
  return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////
//// Vertex 
/////////////////////////////////////////////////////////////////////

void VKalVrtAthena::VrtSecDecay::decorateTrkWithVertexParameters(const xAOD::TrackParticle* trk,xAOD::Vertex* myVertex)
{

  static SG::AuxElement::Decorator< float > dec_pt_wrtSV( "pt_wrtSV" );
  static SG::AuxElement::Decorator< float > dec_eta_wrtSV( "eta_wrtSV" );
  static SG::AuxElement::Decorator< float > dec_phi_wrtSV( "phi_wrtSV" );
  static SG::AuxElement::Decorator< float > dec_d0_wrtSV( "d0_wrtSV" );
  static SG::AuxElement::Decorator< float > dec_z0_wrtSV( "z0_wrtSV" );
  static SG::AuxElement::Decorator< float > dec_errD0_wrtSV( "errD0_wrtSV" );
  static SG::AuxElement::Decorator< float > dec_errZ0_wrtSV( "errZ0_wrtSV" );
  static SG::AuxElement::Decorator< float > dec_errP_wrtSV( "errP_wrtSV" );

  const Trk::Perigee* sv_perigee = m_trackToVertexTool->perigeeAtVertex( *trk, myVertex->position() );
  if( sv_perigee ){
    double qOverP_wrtSV    = sv_perigee->parameters() [Trk::qOverP];
    double theta_wrtSV     = sv_perigee->parameters() [Trk::theta];
    double p_wrtSV         = 1.0 / fabs( qOverP_wrtSV );
    double pt_wrtSV        = p_wrtSV * sin( theta_wrtSV );
    double eta_wrtSV       = -log( tan( theta_wrtSV/2. ) );
    double phi_wrtSV       = sv_perigee->parameters() [Trk::phi];
    double d0_wrtSV        = sv_perigee->parameters() [Trk::d0];
    double z0_wrtSV        = sv_perigee->parameters() [Trk::z0];
    double errd0_wrtSV     = (*sv_perigee->covariance())( Trk::d0, Trk::d0 );
    double errz0_wrtSV     = (*sv_perigee->covariance())( Trk::z0, Trk::z0 );
    double errP_wrtSV      = (*sv_perigee->covariance())( Trk::qOverP, Trk::qOverP );

    dec_pt_wrtSV(*trk)    = pt_wrtSV;
    dec_eta_wrtSV(*trk)   = eta_wrtSV;
    dec_phi_wrtSV(*trk)   = phi_wrtSV;
    dec_d0_wrtSV(*trk)    = d0_wrtSV;
    dec_z0_wrtSV(*trk)    = z0_wrtSV;
    dec_errD0_wrtSV(*trk) = errd0_wrtSV;
    dec_errZ0_wrtSV(*trk) = errz0_wrtSV;
    dec_errP_wrtSV(*trk)  = errP_wrtSV;

  } 



}

///////////////////////////////////////////////////////////////////
// Basic vertex requirements 
///////////////////////////////////////////////////////////////////
bool VKalVrtAthena::VrtSecDecay::passVtxSelection(xAOD::Vertex*& vtx)
{

  // Check vertex exists 
  if( !vtx ){
    return false;
  }

  // Chi-squared cut over nDOF cut
  // Intentionally very loose right now
  if( (vtx->chiSquared() / vtx->numberDoF() ) > m_vtxQuality ){
    delete vtx;
    return false;
  }
  
  // Transverse displacement cut,
  double VtxR = vtx->position().perp();
  if( VtxR<m_minVtxRadius || VtxR>m_maxVtxRadius ){
    delete vtx;
    return false;
  }
  
  //
  return true;

}
///////////////////////////////////////////////////////////////////
// Check hit patterns
///////////////////////////////////////////////////////////////////

bool VKalVrtAthena::VrtSecDecay::consistentHitPattern(const xAOD::TrackParticle* tracklet, const xAOD::TrackParticle* track)
{

  // Three or four later tracklets, with no hits other than in the 
  // pixl barrel layers
  if( tracklet->hitPattern()!=7 && tracklet->hitPattern()!=15 ) return false;

  (void)track;

  return true;

}

///////////////////////////////////////////////////////////////////
// Pre-selection of tracks
///////////////////////////////////////////////////////////////////

std::vector<const xAOD::TrackParticle*> VKalVrtAthena::VrtSecDecay::preselectTracks(const xAOD::TrackParticleContainer* tracks)
{

  std::vector<const xAOD::TrackParticle*> preselected_tracks;
  preselected_tracks.clear();

  for( const auto& track : * tracks ){

    // pT threshold
    if( track->pt()<m_childMinPt || track->pt() > m_childMaxPt ) continue;

    // Ignore tracks with B-layer hits
    uint8_t nBLayerHits;
    track->summaryValue(nBLayerHits, xAOD::numberOfBLayerHits);
    if( nBLayerHits>m_childBLayerHits ) continue;

    // Ignore tracks with hits on the second pixel layer
    uint8_t numberOfNextToInnermostPixelLayerHits;
    track->summaryValue(numberOfNextToInnermostPixelLayerHits, xAOD::numberOfNextToInnermostPixelLayerHits);
    if( numberOfNextToInnermostPixelLayerHits>m_childNextToInPixLayerHits) continue;

    uint8_t numberOfPixelHits;
    track->summaryValue(numberOfPixelHits, xAOD::numberOfPixelHits);
    if( numberOfPixelHits>m_childPixHits ) continue;

    // Require hits in the SCT
    uint8_t nSCTHits;
    track->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
    if( nSCTHits<m_childSCTHits ) continue;

    // GOOD TRACK!
    preselected_tracks.push_back(track);

  } // Loop over track container

  return preselected_tracks;

}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode VKalVrtAthena::VrtSecDecay::finalize()
{
 
  // Return gracefully
  return StatusCode::SUCCESS;
}
