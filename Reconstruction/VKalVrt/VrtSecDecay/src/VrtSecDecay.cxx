/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
//#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include <map>
#include <iostream>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

VKalVrtAthena::VrtSecDecay::VrtSecDecay (const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_iVertexFitter                ( "Trk::TrkVKalVrtFitter"  ),
  m_trackToVertexTool            ( "Reco::TrackToVertex"    )

{

  declareProperty("VertexFitterTool",m_iVertexFitter,"Vertexing fitting tool, required");
  declareProperty("OutputVtxContainer",m_outputVtxContainer="VrtSecDecay","Name of the output vertex container");
  declareProperty("ParentxAODContainer",m_parentxAODTrkContainer="InDetPixelThreeLayerTrackParticles","Name of the primary track container from which tracks are attached");
  declareProperty("ChildxAODContainer",m_childxAODTrkContainer="InDetTrackParticles","Name of the secondary track container, to get attached to the parent track");
  declareProperty("DecorationPreFix",m_decorationPreFix="VSD","Name prefix for decorations added for track parameters with respect to secondary decorations");
  declareProperty("DoHitPatternMatching",m_doHitPatternMatching=false,"Require (good) hit patterns on parent particle" );
  declareProperty("ParentChildMaxDr",m_parentChild_maxdr=-1,"dR threshold between parent and child particles");
  declareProperty("ExpandToMultiTrkVtx",m_expandToMultiTrackVtx=false,"Expand two track vertices into n-track vertices");
  declareProperty("MaxVtxPerEvent",m_maxVtxPerEvent=100,"Maximum number of vertices per event");
  declareProperty("VtxQuality",m_vtxQuality=10,"Minimum chi2/nDOF for vertex to be accepted");
  declareProperty("VtxMinRadius",m_minVtxRadius=85.0,"Minimum radial displacement on vertex");
  declareProperty("VtxMaxRadius",m_maxVtxRadius=300.0,"Maximum radial displacement on vertex");
  declareProperty("ParentPt",m_parentPt=20000.0,"Minimum pT for parent track");
  declareProperty("ParentMinEta",m_parentMinEta=0.1,"Minimum eta for parent track");
  declareProperty("ParentMaxEta",m_parentMaxEta=2.0,"Maximum eta for parent track");
  declareProperty("ParentZ0SinTheta",m_parentZ0PVSinTheta=10.0,"Maximum z0PVxsinTheta of parent track");
  declareProperty("ParentIBLHits",m_parentIBLHits=-1,"Minimum number of IBL hits for parent track");
  declareProperty("ParentPixelLayers",m_parentPixLayers=-1,"Minumum number of pixel layers for parent track");
  declareProperty("ParentSCTHIts",m_parentSCTHits=-1,"Maximum number of SCT hits for parent track");
  declareProperty("ChildMinPt",m_childMinPt=100.0,"Minimum pT for outgoing child tracks");
  declareProperty("ChildMaxPt",m_childMaxPt=1500.0,"Maximum pT for outgoing child tracks");
  declareProperty("ChildBLayerHits",m_childBLayerHits=0,"Maximum number of b-layer hits on child track ");
  declareProperty("ChildNextToInPixLayerHits",m_childNextToInPixLayerHits=0,"Maximum number of Next to inner pixel layer hits");
  declareProperty("ChildPixHits",m_childPixHits=1,"Maximum number of pixel hits on child track");
  declareProperty("ChildSCTHits",m_childSCTHits=6,"Minumum number of SCT hits on child track");

}

///////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////

StatusCode VKalVrtAthena::VrtSecDecay::initialize()
{

  /* Get the right vertex fitting tool from ToolSvc */
  ATH_CHECK( m_iVertexFitter.retrieve() );

  /* Get the TrackToVertex extrapolator tool*/
  ATH_CHECK( m_trackToVertexTool.retrieve() );  
  
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode VKalVrtAthena::VrtSecDecay::execute()
{

  //
  // Run vertex fitting with xAOD::TrackParticle inputs
  // Note this is self contained for ease to expanding to other track inputs, 
  // e.g. one might run on Trk::Track inputs for more detailed description 
  // of the track hit information. The Trk::Track method is not implemenentd here
  //
  return doTrackParticleMethod();

}
///////////////////////////////////////////////////////////////////
// Execute strategy using xAOD::TrackParticles
///////////////////////////////////////////////////////////////////
StatusCode VKalVrtAthena::VrtSecDecay::doTrackParticleMethod()
{

  //
  // Tracklet container: i.e. parent particle that "decays"
  //
  const xAOD::TrackParticleContainer* tracklets(nullptr);
  ATH_CHECK( evtStore()->retrieve ( tracklets, m_parentxAODTrkContainer ) );
  
  //
  // Daughter particle
  //
  const xAOD::TrackParticleContainer* tracks(nullptr);
  ATH_CHECK( evtStore()->retrieve ( tracks, m_childxAODTrkContainer ) );

  ///
  /// Primary vertex
  ///
  const xAOD::VertexContainer* primVertex   = nullptr;
  ATH_CHECK( evtStore()->retrieve( primVertex, "PrimaryVertices" ) );


  //
  // ----- Output vertex containers -------
  //
  auto vtxContainer      = std::make_unique<xAOD::VertexContainer>();
  auto vtxContainerAux   = std::make_unique<xAOD::VertexAuxContainer>();
  vtxContainer->setStore( vtxContainerAux.get() );

  const xAOD::Vertex* pV = nullptr;
  for( const auto& vx : *primVertex ) {
    if(vx->vertexType() == xAOD::VxType::PriVtx){
      pV = vx;
      break;
    }
  }

  // Event doesn't have a PV
  // Write empty containers
  if( !pV ){
    ATH_CHECK( evtStore()->record(std::move(vtxContainer), m_outputVtxContainer) );
    ATH_CHECK( evtStore()->record(std::move(vtxContainerAux), m_outputVtxContainer+"Aux.") );
    return StatusCode::SUCCESS;
  }
  
  // Pre-select soft-tracks so we don't have to 
  // apply this pre-selection for N tracklets in the loop below
  std::vector<const xAOD::TrackParticle*> preselected_tracks = preselectTracks(tracks,pV);

  std::unordered_set<const xAOD::TrackParticle*> usedTracks;

  //
  // ----- Track selection -------
  //
  std::vector<const xAOD::TrackParticle*> twoTrkVtx_tracks; 
  std::vector<const xAOD::TrackParticle*> multiTrkVtx_tracks;

  //
  std::vector<VtxObj> initialVertices;

  // Select the tracks we want
  for( const auto& tracklet : *tracklets ){

    /*
      Parent selection cuts
    */ 
    if( !passParentSelection(tracklet,pV) ) continue;

    //
    initialVertices.clear();

    for( const auto& pion : preselected_tracks ){

      /*
        Consistency checks before doing any vertexing 
      */
      if( m_parentChild_maxdr>0.0 && tracklet->p4().DeltaR( pion->p4() ) > m_parentChild_maxdr ) continue;
      if( m_doHitPatternMatching &&  !consistentHitPattern(tracklet,pion)  ) continue;

      twoTrkVtx_tracks.clear();
      twoTrkVtx_tracks.push_back(tracklet);
      twoTrkVtx_tracks.push_back(pion);

      /*
        Inital position estimation
      */
      Amg::Vector3D initPos(0.,0.,0.);
      StatusCode sc = m_iVertexFitter->VKalVrtFitFast( twoTrkVtx_tracks, initPos );
      // VKalVrtFitFast reports no solution found as StatusCode::FAILURE
      if( sc.isFailure() ){
        ATH_MSG_DEBUG("FAILED TO ESTIMATE VTX INITIAL POSITION ");
        continue;
      }

      std::unique_ptr<xAOD::Vertex> myVertex (m_iVertexFitter->fit(twoTrkVtx_tracks, initPos));

      if( !passVtxSelection(myVertex.get()) ) continue;

      // User only wants to write out two-track vertices 
      // and not attempt to expand them into 1 incoming --> >=2 outgoing tracks
      if( !m_expandToMultiTrackVtx ){

        std::vector<ElementLink<xAOD::TrackParticleContainer>> trackLinks;
        trackLinks.push_back( ElementLink<xAOD::TrackParticleContainer>(pion, *tracks));
        trackLinks.push_back( ElementLink<xAOD::TrackParticleContainer>(tracklet, *tracklets));
        myVertex->setTrackParticleLinks( trackLinks );
        myVertex->setVertexType(xAOD::VxType::SecVtx);
        decorateTrkWithVertexParameters(pion,myVertex.get());
        decorateTrkWithVertexParameters(tracklet,myVertex.get());

        vtxContainer->push_back(myVertex.release());
      }
      // Cache as input for multi-track expansion
      else{
        initialVertices.emplace_back(std::move(myVertex),std::move(twoTrkVtx_tracks));
      }

    } // First iteration of vertex fits --> all two track vertex candidates formed

    if( m_expandToMultiTrackVtx ){
  
      // Sort based upon quality
      std::sort( initialVertices.begin(), initialVertices.end(), rank_quality );

      // Attempt to pair tracks that were associated to this parent particle 
      for( auto& v1 : initialVertices ){

        multiTrkVtx_tracks.clear();
        multiTrkVtx_tracks.push_back(v1.inputTrks[0]);
        multiTrkVtx_tracks.push_back(v1.inputTrks[1]);

        for( auto& trk : preselected_tracks ){
          if( trk==v1.inputTrks[1] ) continue;
          auto used = usedTracks.find(trk);
          if( used!=usedTracks.end() ) continue;
          // Extrapolate v2 soft-track to the v1 vertex; check IPs
          std::unique_ptr<const Trk::Perigee> sv_perigee (m_trackToVertexTool->perigeeAtVertex(*trk, v1.vtx->position()) );
          if( !sv_perigee ) continue;
          double d0_wrtSV = sv_perigee->parameters() [Trk::d0];
          double z0_wrtSV = sv_perigee->parameters() [Trk::z0];
          // Note: Deemed sufficently loose that analyses can study the effects of tightening
          // this cut offline (decorations are added for final vertices)
          if( std::abs(d0_wrtSV)>15.0 ) continue;
          if( std::abs(z0_wrtSV)>15.0 ) continue;
          multiTrkVtx_tracks.push_back(trk);
        }

        if( multiTrkVtx_tracks.size()==2 ) continue;

        Amg::Vector3D initPos(0.,0.,0.);
        StatusCode sc = m_iVertexFitter->VKalVrtFitFast( multiTrkVtx_tracks,initPos);
        if( sc.isFailure() ){
          ATH_MSG_INFO("FAILED TO ESTIMATE VTX INITIAL POSITION ");
          continue;
        }

        std::unique_ptr<xAOD::Vertex> finalVertex (m_iVertexFitter->fit(multiTrkVtx_tracks,initPos));
        if( !passVtxSelection(finalVertex.get()) ) continue;

        std::vector<ElementLink<xAOD::TrackParticleContainer>> trackLinks;
        trackLinks.push_back( ElementLink<xAOD::TrackParticleContainer>(tracklet, *tracklets));
        decorateTrkWithVertexParameters(tracklet,finalVertex.get());

        for(auto& trk : multiTrkVtx_tracks ){
          if( trk==tracklet ) continue;
          usedTracks.insert(trk);
          trackLinks.push_back( ElementLink<xAOD::TrackParticleContainer>(trk, *tracks));
          decorateTrkWithVertexParameters(trk,finalVertex.get());
        }

        finalVertex->setTrackParticleLinks( trackLinks );
        finalVertex->setVertexType(xAOD::VxType::SecVtx);

        // Save
        vtxContainer->push_back(finalVertex.release());

      }

    } //  Loop over tracklets 

  }


  // Write them to store gate  
  ATH_CHECK( evtStore()->record(std::move(vtxContainer), m_outputVtxContainer) );
  ATH_CHECK( evtStore()->record(std::move(vtxContainerAux), m_outputVtxContainer+"Aux.") );

  // Return gracefully
  return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////
//// Vertex 
/////////////////////////////////////////////////////////////////////

void VKalVrtAthena::VrtSecDecay::decorateTrkWithVertexParameters(const xAOD::TrackParticle* trk,xAOD::Vertex* myVertex)
{

  const SG::AuxElement::Decorator< float > dec_pt_wrtSV( m_decorationPreFix+"_pt_wrtSV" );
  const  SG::AuxElement::Decorator< float > dec_eta_wrtSV( m_decorationPreFix+"_eta_wrtSV" );
  const  SG::AuxElement::Decorator< float > dec_phi_wrtSV( m_decorationPreFix+"_phi_wrtSV" );
  const SG::AuxElement::Decorator< float > dec_d0_wrtSV( m_decorationPreFix+"_d0_wrtSV" );
  const  SG::AuxElement::Decorator< float > dec_z0_wrtSV( m_decorationPreFix+"_z0_wrtSV" );
  const SG::AuxElement::Decorator< float > dec_errD0_wrtSV( m_decorationPreFix+"_errD0_wrtSV" );
  const SG::AuxElement::Decorator< float > dec_errZ0_wrtSV( m_decorationPreFix+"_errZ0_wrtSV" );
  const SG::AuxElement::Decorator< float > dec_errP_wrtSV( m_decorationPreFix+"_errP_wrtSV" );

  std::unique_ptr<const Trk::Perigee> sv_perigee (m_trackToVertexTool->perigeeAtVertex(*trk, myVertex->position()));
  if( sv_perigee ){
    double qOverP_wrtSV    = sv_perigee->parameters() [Trk::qOverP];
    double theta_wrtSV     = sv_perigee->parameters() [Trk::theta];
    double p_wrtSV         = 1.0 / std::abs( qOverP_wrtSV );
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
bool VKalVrtAthena::VrtSecDecay::passVtxSelection(const xAOD::Vertex* vtx)
{

  // Check vertex exists 
  if( !vtx ){
    return false;
  }

  // Chi-squared cut over nDOF cut
  // Intentionally very loose right now
  if( (vtx->chiSquared() / vtx->numberDoF() ) > m_vtxQuality ){
    return false;
  }
  
  // Transverse displacement cut,
  double VtxR = vtx->position().perp();
  if( VtxR<m_minVtxRadius || VtxR>m_maxVtxRadius ){
    return false;
  }

  //
  return true;

}
///////////////////////////////////////////////////////////////////
// Check hit patterns
///////////////////////////////////////////////////////////////////

bool VKalVrtAthena::VrtSecDecay::consistentHitPattern(const xAOD::TrackParticle* tracklet, const xAOD::TrackParticle* /*track*/)
{

  // Three or four later tracklets, with no hits other than in the 
  // pixl barrel layers
  if( tracklet->hitPattern()!=7 && tracklet->hitPattern()!=15 ) return false;

  return true;

}

///////////////////////////////////////////////////////////////////
// Selection of tracks
///////////////////////////////////////////////////////////////////


bool VKalVrtAthena::VrtSecDecay::passParentSelection(const xAOD::TrackParticle* tracklet,const xAOD::Vertex* pV)
{


  if( tracklet->pt() < m_parentPt             ) return false;
  if( std::abs(tracklet->eta()) > m_parentMaxEta  ) return false;
  if( std::abs(tracklet->eta()) < m_parentMinEta  ) return false;

  uint8_t nSCTHits = 0;
  tracklet->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
  if( m_parentSCTHits>0 && nSCTHits>m_parentSCTHits) return false;

  uint8_t nPixLayers = 0;
  tracklet->summaryValue(nPixLayers, xAOD::numberOfContribPixelLayers);
  if( m_parentPixLayers>0 && nPixLayers<m_parentPixLayers ) return false;

  uint8_t nIBLHits = 0;
  tracklet->summaryValue(nIBLHits, xAOD::numberOfInnermostPixelLayerHits);
  if( m_parentIBLHits>0 && nIBLHits<m_parentIBLHits ) return false;

  if( !pV ) return false;
  double z0_exPV    = (tracklet->z0() + tracklet->vz() - pV->z()) * std::sin(tracklet->p4().Theta());
  if( m_parentZ0PVSinTheta>0 && std::abs(z0_exPV)>m_parentZ0PVSinTheta ) return false;

  return true;

}


std::vector<const xAOD::TrackParticle*> VKalVrtAthena::VrtSecDecay::preselectTracks(const xAOD::TrackParticleContainer* tracks,const xAOD::Vertex* pV)
{

  std::vector<const xAOD::TrackParticle*> preselected_tracks;

  // Cache tracks associated to PV
  std::unordered_set<const xAOD::TrackParticle*> pv_trks;
  for(unsigned int t=0; t<pV->nTrackParticles(); t++){
    pv_trks.insert(pV->trackParticle(t));
  }

  for( const auto& track : * tracks ){

    // Exclude tracks already vertexed to PV
    auto pv_trk = pv_trks.find(track);
    if( pv_trk!=pv_trks.end() ) continue;

    // pT threshold
    if( track->pt()<m_childMinPt || track->pt() > m_childMaxPt ) continue;

    // Ignore tracks with B-layer hits
    uint8_t nBLayerHits = 0;
    track->summaryValue(nBLayerHits, xAOD::numberOfBLayerHits);
    if( nBLayerHits>m_childBLayerHits ) continue;

    // Ignore tracks with hits on the second pixel layer
    uint8_t numberOfNextToInnermostPixelLayerHits = 0;
    track->summaryValue(numberOfNextToInnermostPixelLayerHits, xAOD::numberOfNextToInnermostPixelLayerHits);
    if( numberOfNextToInnermostPixelLayerHits>m_childNextToInPixLayerHits) continue;

    uint8_t numberOfPixelHits = 0;
    track->summaryValue(numberOfPixelHits, xAOD::numberOfPixelHits);
    if( numberOfPixelHits>m_childPixHits ) continue;

    // Require hits in the SCT
    uint8_t nSCTHits = 0;
    track->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
    if( nSCTHits<m_childSCTHits ) continue;

    // GOOD TRACK!
    preselected_tracks.push_back(track);

  } // Loop over track container

  return preselected_tracks;

}
