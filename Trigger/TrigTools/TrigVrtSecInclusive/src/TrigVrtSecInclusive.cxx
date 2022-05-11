/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

  * Trigger specific modification of VSI, that is aimed at reconstructing displaced vertex
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
  *        Takane Sano <takane.sano@cern.ch> - Kyoto University
*/
#include "TrigVrtSecInclusive/KDPoint.h"
#include "TrigVrtSecInclusive/VtxMap.h"
#include "TrigVrtSecInclusive/Coordinate.h"
#include "TrigVrtSecInclusive/Constants.h"
#include "TrigVrtSecInclusive/TrigVrtSecInclusive.h"

#include <TFile.h>
#include "PathResolver/PathResolver.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "TrkVKalVrtCore/PGraph.h"

#include <limits>

namespace TrigVSI {

TrigVrtSecInclusive::TrigVrtSecInclusive(const std::string& name,ISvcLocator* pSvcLocator)
   : AthReentrantAlgorithm(name, pSvcLocator),
     m_materialMapInnerFileName("MaterialMap_v3.2_Inner.root"),
     m_materialMapInnerHistName("FinalMap_inner"),
     m_materialMapInnerMatrixName("FoldingInfo"),
     m_materialMapOuterFileName("MaterialMap_v3_Outer.root"),
     m_materialMapOuterHistName("matmap_outer")
{}

StatusCode TrigVrtSecInclusive::initialize()
{
   const std::string header = "UTT> ";

   // read material maps
   if (m_doMaterialMapVeto) {

     std::string inFileName = PathResolver::find_file(m_materialMapInnerFileName, "DATAPATH");
     if ( inFileName.empty()) {
        ATH_MSG_ERROR(header << "Cannot find material map inner file: " << m_materialMapInnerFileName);
        return StatusCode::FAILURE;
      }
     std::unique_ptr<TFile> materialMapInnerFile = std::make_unique<TFile>(inFileName.c_str(), "READ");
     if(materialMapInnerFile->IsOpen()){
        materialMapInnerFile->GetObject(m_materialMapInnerHistName.c_str(), m_materialMapInner);
        materialMapInnerFile->GetObject(m_materialMapInnerMatrixName.c_str(), m_materialMapMatrix);
        if(m_materialMapInner) m_materialMapInner->SetDirectory(0);
     }
     materialMapInnerFile->Close();

     std::string outFileName = PathResolver::find_file(m_materialMapOuterFileName, "DATAPATH");
     if ( outFileName.empty()) {
        ATH_MSG_ERROR(header << "Cannot find material map outer file: " << m_materialMapOuterFileName);
        return StatusCode::FAILURE;
      }
     std::unique_ptr<TFile> materialMapOuterFile = std::make_unique<TFile>(outFileName.c_str(), "READ");
     if(materialMapOuterFile->IsOpen()){
        materialMapOuterFile->GetObject(m_materialMapOuterHistName.c_str(), m_materialMapOuter);
        if(m_materialMapOuter) m_materialMapOuter->SetDirectory(0);
     }
     materialMapOuterFile->Close();
     ATH_MSG_INFO( header << "read material maps" );

   }

   // monitoring tool
   if ( !m_monTool.empty() ) ATH_CHECK(m_monTool.retrieve());

   // vertex fitters
   ATH_CHECK( m_fitSvc.retrieve() );
   ATH_CHECK( m_vertexPointEstimator.retrieve() );

   // collection names
   ATH_CHECK(m_firstPassTracksName.initialize());
   ATH_CHECK(m_secondPassTracksName.initialize());
   ATH_CHECK(m_vxCandidatesOutputName.initialize());
   ATH_CHECK(m_trkPairOutputName.initialize());
   ATH_CHECK(m_PrimaryVxInputName.initialize());

   //
   ATH_MSG_INFO( header << "Initialization completed successfully" );
   return StatusCode::SUCCESS;
}

StatusCode TrigVrtSecInclusive::execute(const EventContext& ctx) const
{
   const std::string header = "UTT> ";

   // Setup output container for vertex
   SG::WriteHandle<xAOD::VertexContainer> outputVertices (m_vxCandidatesOutputName, ctx);

   std::unique_ptr<xAOD::VertexContainer>     theXAODContainer    = std::make_unique<xAOD::VertexContainer>();
   std::unique_ptr<xAOD::VertexAuxContainer>  theXAODAuxContainer = std::make_unique<xAOD::VertexAuxContainer>();
   theXAODContainer->setStore( theXAODAuxContainer.get() );
   xAODContainers theXAODContainers = std::make_pair( std::move(theXAODContainer), std::move(theXAODAuxContainer) );

   // Setup output container for track pair
   SG::WriteHandle<xAOD::VertexContainer> outputTrkPairs (m_trkPairOutputName, ctx);

   std::unique_ptr<xAOD::VertexContainer>     theXAODTrkPairContainer    = std::make_unique<xAOD::VertexContainer>();
   std::unique_ptr<xAOD::VertexAuxContainer>  theXAODTrkPairAuxContainer = std::make_unique<xAOD::VertexAuxContainer>();
   theXAODTrkPairContainer->setStore( theXAODTrkPairAuxContainer.get() );
   xAODContainers theXAODTrkPairContainers = std::make_pair( std::move(theXAODTrkPairContainer), std::move(theXAODTrkPairAuxContainer) );

   // retrieve primary vertices
   //_________________________________________________________________________
   SG::ReadHandle<xAOD::VertexContainer> vtxCont(m_PrimaryVxInputName, ctx);
   const xAOD::Vertex* privtx = nullptr;
   if( vtxCont.isValid() ){
      const xAOD::VertexContainer* vertex_container = vtxCont.get();
      privtx = vertex_container->at(0);
      if (  !( privtx->vertexType() == xAOD::VxType::PriVtx
            && privtx->nTrackParticles() >= 2 ) ){
         ATH_MSG_WARNING( header << "Illed primary vertex, keeping null privtx" );
         privtx = nullptr;
      } else {
      	 ATH_MSG_INFO( header << "primary vertex successfully retrieved" );
      }
   }
   else {
      ATH_MSG_WARNING( header << "couldn't retrieve primary vertex, keeping null privtx" );
   }

   // retrieve the tracks
   //_________________________________________________________________________
   SG::ReadHandle<xAOD::TrackParticleContainer> firstPassTrackParticleCollection(m_firstPassTracksName, ctx);
   SG::ReadHandle<xAOD::TrackParticleContainer> secondPassTrackParticleCollection(m_secondPassTracksName, ctx);
   bool isFirstTrackValid  = firstPassTrackParticleCollection.isValid();
   bool isSecondTrackValid = secondPassTrackParticleCollection.isValid();
   if( ! isFirstTrackValid )  ATH_MSG_WARNING( header << "couldn't retrieve first pass tracks: "  << m_firstPassTracksName);
   if( ! isSecondTrackValid ) ATH_MSG_WARNING( header << "couldn't retrieve second pass tracks: " << m_secondPassTracksName);

   // monitoring
   //_________________________________________________________________________
   auto timerTrackSel        = Monitored::Timer<std::chrono::nanoseconds>  ("TIME_TrackSel");
   auto timerTwoTrackVertex  = Monitored::Timer<std::chrono::milliseconds> ("TIME_TwoTrackVertex");
   auto timerMapClustering   = Monitored::Timer<std::chrono::microseconds> ("TIME_MapClustering");
   auto timerNTrackVertex    = Monitored::Timer<std::chrono::milliseconds> ("TIME_NTrackVertex");
   auto timerNTrackVtxOffVSI = Monitored::Timer<std::chrono::milliseconds> ("TIME_NTrackVtxOffVSI");
   auto timerCleanUp         = Monitored::Timer<std::chrono::nanoseconds>  ("TIME_CleanUp");
   auto timerWriteVertices   = Monitored::Timer<std::chrono::milliseconds> ("TIME_WriteVertices");
   auto timerOverall         = Monitored::Timer<std::chrono::milliseconds> ("TIME_Overall");

   auto monTime = Monitored::Group(m_monTool, timerTrackSel, timerTwoTrackVertex, timerMapClustering, timerNTrackVertex, timerNTrackVtxOffVSI, timerCleanUp, timerWriteVertices, timerOverall);

   timerOverall.start();

   // Reset incompatible pair list and selectedTracks
   //_________________________________________________________________________
   std::vector<std::pair<size_t,size_t>>    v_incomp;
   std::vector<const xAOD::TrackParticle*>  v_selectedTracks;

   // tracks selection
   //_________________________________________________________________________
   timerTrackSel.start();

   if( isFirstTrackValid ) {
     if( isSecondTrackValid ) {
       ATH_CHECK( trackSelection( firstPassTrackParticleCollection.cptr(), secondPassTrackParticleCollection.cptr(), v_selectedTracks ) );
     }
     else {
       ATH_CHECK( trackSelection( firstPassTrackParticleCollection.cptr(), nullptr, v_selectedTracks ) );
     }
   }
   timerTrackSel.stop();


   // Vertex reconstruction
   //_________________________________________________________________________

   // Find compatible track pairs and record as di-trakc vertex
   // ===============================
   WrkVrtContainer v_diwrkvrt;
   v_diwrkvrt.reserve(5000);

   if ( m_vtxAlgorithm == 0 ) {
     // TrigVSI
     timerTwoTrackVertex.start();
  	 ATH_CHECK( findDiTrackVertex( v_diwrkvrt, v_incomp, v_selectedTracks, ctx, privtx ) );
     timerTwoTrackVertex.stop();
   } else {
     // Offline VSI like
     timerTwoTrackVertex.start();
  	 ATH_CHECK( findDiTrackVertexVSI( v_diwrkvrt, v_incomp, v_selectedTracks, ctx, privtx ) );
     timerTwoTrackVertex.stop();
   }

   // Reconstruct N track vertices
   // ===============================
   // Ensure that the adress of each di-track wrkvrt can't be changed.
   const WrkVrtContainer v_diTrkVertices = std::move(v_diwrkvrt);

   // define vertex map
   namespace vsic = TrigVSI::AlgConsts;
   TrigVSI::VtxMap<WrkVrt, TrigVSI::Coordinate::Pseudo> vtxmap( std::make_unique<TH3D>("TrigVrtSecInclusive_VtxMap", "TrigVrtSecInclusive_VtxMap", vsic::binNumR, vsic::IDrInner, vsic::IDrInner+vsic::mapBinWid*vsic::binNumR, vsic::nEta, -vsic::maxEta, vsic::maxEta, vsic::nPhi, -TMath::Pi(), TMath::Pi()) );

   // Container for reconstructed N-track vertices
   WrkVrtContainer v_wrkvrt;
   v_wrkvrt.reserve(1000);

   if ( m_vtxAlgorithm == 0 ) {
     // TrigVSI
     int trkpair_cnt = 0;

     // Fill vertex map
     timerMapClustering.start();
     for (auto wrkvrt_iter = v_diTrkVertices.begin(); wrkvrt_iter != v_diTrkVertices.end(); ++wrkvrt_iter ) {
       if ( wrkvrt_iter->isGood && wrkvrt_iter->cuts.isFullPass() ) {
         vtxmap.Fill( &(*wrkvrt_iter) );
         trkpair_cnt++;
       }
     }
     vtxmap.lock();
     ATH_MSG_DEBUG( header << "filled vertex map with " << trkpair_cnt << " pairs" );
     vtxmap.ClusterizeCells(1.,1);
     timerMapClustering.stop();
     ATH_MSG_DEBUG( header << "vertex map clustering successfully completed : " << vtxmap.nClusters() << " clusters" );

     // N track vertexing
     timerNTrackVertex.start();
     ATH_CHECK( findNTrackVertex( v_wrkvrt, vtxmap, v_selectedTracks, ctx ) );
     timerNTrackVertex.stop();

     // Cleanup vertex fully included in other vertex.
     timerCleanUp.start();
     size_t n_pre_clean   = v_wrkvrt.size();
     ATH_CHECK( cleanUp(v_wrkvrt) );
     size_t n_post_clean  = v_wrkvrt.size();
     timerCleanUp.stop();
     ATH_MSG_DEBUG( header << "cleaned up " << n_pre_clean - n_post_clean << " out of " << n_pre_clean );

   } else {
     // Offline VSI like
     timerNTrackVtxOffVSI.start();
     ATH_CHECK( findNtrackVerticesVSI( v_wrkvrt, v_incomp, v_selectedTracks, ctx ) );
     timerNTrackVtxOffVSI.stop();

   }

   // Fill Vertex in the VertexContainer
   timerWriteVertices.start();

   ATH_CHECK( fillVtxContainer( theXAODContainers,        v_wrkvrt,         v_selectedTracks ) );
   ATH_CHECK( fillVtxContainer( theXAODTrkPairContainers, v_diTrkVertices,  v_selectedTracks ) );

   // record AOD object
   ATH_MSG_DEBUG( header << "Record vertices into container." );
   ATH_CHECK(outputVertices.record(std::move(theXAODContainers.first), std::move(theXAODContainers.second)));
   ATH_CHECK(outputTrkPairs.record(std::move(theXAODTrkPairContainers.first), std::move(theXAODTrkPairContainers.second)));
   ATH_MSG_DEBUG( "Recorded Vertices with key: " << m_vxCandidatesOutputName.key() );

   timerWriteVertices.stop();

   timerOverall.stop();
   ATH_MSG_DEBUG( header << "::execute() finished successfully." );

   return StatusCode::SUCCESS;
}


StatusCode TrigVrtSecInclusive::trackSelection( const xAOD::TrackParticleContainer* firstPassTrackParticles, const xAOD::TrackParticleContainer* secondPassTrackParticles, std::vector<const xAOD::TrackParticle*>& selectedTracks ) const
{
  const std::string header = "UTT> ";

  // Pack TrackParticleContainers to one vector
  std::vector<const xAOD::TrackParticleContainer*> v_containers;
  if( firstPassTrackParticles  != nullptr ) v_containers.push_back(firstPassTrackParticles);
  if( secondPassTrackParticles != nullptr ) v_containers.push_back(secondPassTrackParticles);

  typedef DataVector<xAOD::TrackParticle>::const_iterator TrackParticleDataVecIter;

  for(unsigned int i=0; i<v_containers.size(); i++) {
     const xAOD::TrackParticleContainer* trackParticles = v_containers[i];
     size_t n_trk = 0;
     for (TrackParticleDataVecIter itr  = trackParticles->begin(); itr != trackParticles->end(); ++itr) {
  if( selectTrack(*itr) ) { selectedTracks.push_back(*itr); n_trk++; }
     }
     ATH_MSG_DEBUG(header << "Of " << trackParticles->size() << " tracks " << n_trk << " survived the preselection.");
  }
  ATH_MSG_DEBUG(header << selectedTracks.size() << " tracks in total passed the selection.");
  return StatusCode::SUCCESS;
}


StatusCode TrigVrtSecInclusive::fillVtxContainer( xAODContainers& theXAODContainers, const WrkVrtContainer& workVerticesContainer, std::vector<const xAOD::TrackParticle*>& selectedTracks ) const
{
  xAOD::VertexContainer*    theVertexContainer    = theXAODContainers.first.get();

  for (const auto& wrkvrt : workVerticesContainer) {

    if ( !wrkvrt.isGood && !wrkvrt.isPair ) continue;

    std::vector<const xAOD::TrackParticle*> baseTracks;
    for ( auto i : wrkvrt.selectedTrackIndices() ) {
      baseTracks.emplace_back( selectedTracks.at(i) );
    }

    // Create a xAOD::Vertex instance
    xAOD::Vertex* vertex = new xAOD::Vertex();
    vertex->makePrivateStore();
    theVertexContainer->emplace_back( vertex );

    for( auto *trk: baseTracks ) {
             // Acquire link to the track
             ElementLink<xAOD::TrackParticleContainer>  trackElementLink( *( dynamic_cast<const xAOD::TrackParticleContainer*>( trk->container() ) ), trk->index() );
             // Register link to the vertex
             vertex->addTrackAtVertex( trackElementLink, 1. );
           }

    vertex->setVertexType( xAOD::VxType::SecVtx );
    vertex->setPosition( wrkvrt.vertex );
    vertex->setFitQuality( wrkvrt.chi2, 1 ); // Ndof is always 1

    vertex->auxdata<float>("vsi_mass")   = wrkvrt.vertexMom.M();
    vertex->auxdata<float>("vsi_pT")     = wrkvrt.vertexMom.Perp();
    vertex->auxdata<float>("vsi_charge") = wrkvrt.charge;
    vertex->auxdata<bool>("vsi_isFake")  = false;

    vertex->auxdata<float>("vsi_twoCirc_dr")                      = wrkvrt.param.twoCirc_dr;
    vertex->auxdata<float>("vsi_twoCirc_dphi")                    = wrkvrt.param.twoCirc_dphi;
    vertex->auxdata<float>("vsi_twoCirc_int_r")                   = wrkvrt.param.twoCirc_int_r;
    vertex->auxdata<float>("vsi_vrtFast_r")                       = wrkvrt.param.vrtFast_r;
    vertex->auxdata<float>("vsi_vrtFast_eta")                     = wrkvrt.param.vrtFast_eta;
    vertex->auxdata<float>("vsi_vrtFast_phi")                     = wrkvrt.param.vrtFast_phi;
    vertex->auxdata< std::vector<float> >("vsi_vrtFast_trkd0")    = wrkvrt.param.vrtFast_trkd0;
    vertex->auxdata< std::vector<float> >("vsi_vrtFast_trkz0")    = wrkvrt.param.vrtFast_trkz0;
    vertex->auxdata<float>("vsi_vrtFit_r")                        = wrkvrt.vertex.perp();
    vertex->auxdata<float>("vsi_vrtFit_chi2")                     = wrkvrt.chi2;
    vertex->auxdata<float>("vsi_vPos")                            = wrkvrt.param.vPos;
    vertex->auxdata<float>("vsi_vPosMomAngT")                     = wrkvrt.param.vPosMomAngT;
    vertex->auxdata<float>("vsi_dphi1")                           = wrkvrt.param.dphi1;
    vertex->auxdata<float>("vsi_dphi2")                           = wrkvrt.param.dphi2;
    vertex->auxdata<bool>("vsi_isPassMMV")                        = wrkvrt.param.isPassMMV;

    vertex->auxdata<bool>("vsi_trkd0cut")                         = wrkvrt.cuts.trkd0cut;
    vertex->auxdata<bool>("vsi_twoCircErrcut")                    = wrkvrt.cuts.twoCircErrcut;
    vertex->auxdata<bool>("vsi_twoCircRcut")                      = wrkvrt.cuts.twoCircRcut;
    vertex->auxdata<bool>("vsi_fastErrcut")                       = wrkvrt.cuts.fastErrcut;
    vertex->auxdata<bool>("vsi_fastRcut")                         = wrkvrt.cuts.fastRcut;
    vertex->auxdata<bool>("vsi_fitErrcut")                        = wrkvrt.cuts.fitErrcut;
    vertex->auxdata<bool>("vsi_chi2cut")                          = wrkvrt.cuts.chi2cut;

  }
  //
  return StatusCode::SUCCESS;
}


StatusCode TrigVrtSecInclusive::findDiTrackVertex
( WrkVrtContainer& workVerticesContainer, std::vector<std::pair<size_t,size_t>>& incomp, std::vector<const xAOD::TrackParticle*>& selectedTracks, const EventContext& ctx, const xAOD::Vertex *primVertex ) const
{
   const std::string header = "UTT> ";

   // monitoring
   auto timerTwoCircIntsect = Monitored::Timer<std::chrono::nanoseconds> ("TIME_TwoCircIntsect");
   auto timerVrtFitFast     = Monitored::Timer<std::chrono::nanoseconds> ("TIME_VrtFitFast");
   auto timerVrtFit         = Monitored::Timer<std::chrono::microseconds>("TIME_VrtFit");

   std::vector<float> mnt_pair_dphi;
   std::vector<float> mnt_pair_dr;
   std::vector<float> mnt_intersect_r;
   std::vector<float> mnt_init_r;
   std::vector<float> mnt_init_trkd0;
   std::vector<float> mnt_init_trkz0;
   std::vector<float> mnt_vtxfit_chi2;
   std::vector<float> mnt_vtxfit_r;
   std::vector<float> mnt_vtx_chi2;
   std::vector<float> mnt_vtx_mass;
   std::vector<float> mnt_vtx_mass_high;
   std::vector<float> mnt_vtx_pt;
   std::vector<float> mnt_vtx_charge;
   std::vector<float> mnt_vtx_r;
   auto mon_pair_dphi    = Monitored::Collection("pair_dphi",    mnt_pair_dphi);
   auto mon_pair_dr      = Monitored::Collection("pair_dr",      mnt_pair_dr);
   auto mon_intersect_r  = Monitored::Collection("intersect_r",  mnt_intersect_r);
   auto mon_init_r       = Monitored::Collection("init_r",       mnt_init_r);
   auto mon_init_trkd0   = Monitored::Collection("init_trkd0",   mnt_init_trkd0);
   auto mon_init_trkz0   = Monitored::Collection("init_trkz0",   mnt_init_trkz0);
   auto mon_vtxfit_chi2  = Monitored::Collection("vtxfit_chi2",  mnt_vtxfit_chi2);
   auto mon_vtxfit_r     = Monitored::Collection("vtxfit_r",     mnt_vtxfit_r);
   auto mon_vtx_chi2     = Monitored::Collection("vtx_chi2",     mnt_vtx_chi2);
   auto mon_vtx_mass     = Monitored::Collection("vtx_mass",     mnt_vtx_mass);
   auto mon_vtx_mass_high= Monitored::Collection("vtx_mass_high",mnt_vtx_mass);
   auto mon_vtx_pt       = Monitored::Collection("vtx_pt",       mnt_vtx_pt);
   auto mon_vtx_charge   = Monitored::Collection("vtx_charge",   mnt_vtx_charge);
   auto mon_vtx_r        = Monitored::Collection("vtx_r",        mnt_vtx_r);
   auto monVertex  = Monitored::Group(m_monTool, mon_pair_dphi, mon_pair_dr, mon_intersect_r, mon_init_r, mon_init_trkd0, mon_init_trkz0, mon_vtxfit_chi2, mon_vtxfit_r,
                      mon_vtx_chi2,mon_vtx_mass,mon_vtx_mass_high,mon_vtx_pt,mon_vtx_charge,mon_vtx_r);

   // vertexing
   unsigned int nPairsAll           = 0;
   unsigned int nPairsTrkd0         = 0;
   unsigned int nPairsIntersect     = 0;
   unsigned int nPairsIntersectPos = 0;
   unsigned int nPairsInitPos      = 0;
   unsigned int nPairsInitTrkd0z0  = 0;
   unsigned int nPairsVtxFit       = 0;
   unsigned int nPairsVtxChi2      = 0;
   unsigned int nPairsVtxComp      = 0;
   unsigned int nPairsVtxMatveto   = 0;

   for( auto itrkIter = selectedTracks.begin(); itrkIter != selectedTracks.end(); ++itrkIter ) {
      const xAOD::TrackParticle* itrk = *itrkIter;
      for( auto jtrkIter = std::next(itrkIter); jtrkIter != selectedTracks.end(); ++jtrkIter ) {  // for starts from here
         const xAOD::TrackParticle* jtrk = *jtrkIter;
         nPairsAll++;

         auto monTimeStep = Monitored::Group(m_monTool, timerTwoCircIntsect, timerVrtFitFast, timerVrtFit);

         //
         const int itrk_id = std::distance(selectedTracks.begin(), itrkIter);
         const int jtrk_id = std::distance(selectedTracks.begin(), jtrkIter);

         WrkVrt::AlgCuts  wrkcuts;
         WrkVrt::AlgParam wrkprm;
         WrkVrt           wrkvrt;

         // Set track indices
         wrkvrt.selectedTrackIndices().clear();
         wrkvrt.selectedTrackIndices().emplace_back(itrk_id);
         wrkvrt.selectedTrackIndices().emplace_back(jtrk_id);

         // trk d0
         wrkcuts.trkd0cut = std::abs( itrk->d0() ) < m_twoTrkVtxFormingD0Cut && std::abs( jtrk->d0() ) < m_twoTrkVtxFormingD0Cut;
         nPairsTrkd0++;

         // Attempt to think the combination is incompatible by default
         incomp.emplace_back( std::pair<size_t, size_t>(itrk_id, jtrk_id) );

         // two circles intersection
         const Trk::Perigee& perigee1 = itrk->perigeeParameters();
         const Trk::Perigee& perigee2 = jtrk->perigeeParameters();
         int flag = 0;
         int errorcode = 0;
         InDet::VertexPointEstimator::Values_t decors;
         timerTwoCircIntsect.start();
         Amg::Vector3D circIntersect = m_vertexPointEstimator->getCirclesIntersectionPoint(&perigee1, &perigee2, flag, errorcode, decors);
         timerTwoCircIntsect.stop();
         if (errorcode != 0) {
            ATH_MSG_VERBOSE( header << ": two circles intersect failed");
            wrkcuts.twoCircErrcut   = true;
            wrkvrt.param            = wrkprm;
            wrkvrt.cuts             = wrkcuts;
            wrkvrt.isPair           = true;

            workVerticesContainer.push_back( std::move(wrkvrt) );
            continue;
         }
         float dphi = std::abs(decors["deltaPhiTracks"]);
         float dr   = std::abs(decors["DR1R2"]);
         float intersect_r = circIntersect.perp();
         mnt_pair_dphi.push_back(dphi);
         mnt_pair_dr.push_back(dr);
         mnt_intersect_r.push_back(intersect_r);

         wrkprm.twoCirc_dphi  = dphi;
         wrkprm.twoCirc_dr    = dr;
         wrkprm.twoCirc_int_r = intersect_r;

         nPairsIntersect++;
         wrkcuts.twoCircRcut = intersect_r > m_maxR;
         if (m_doTwoCircRCut && intersect_r > m_maxR) continue;
         nPairsIntersectPos++;

         // initial fast fitting
         std::vector<const xAOD::TrackParticle*> baseTracks;
         baseTracks.emplace_back( itrk );
         baseTracks.emplace_back( jtrk );
         Amg::Vector3D   initVertex;

         timerVrtFitFast.start();
         auto fitterState = m_fitSvc->makeState(ctx);
         m_fitSvc->setApproximateVertex( circIntersect.x(), circIntersect.y(), circIntersect.z(), *fitterState );

         StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex, *fitterState );
         timerVrtFitFast.stop();
         if( sc.isFailure() ) {
            ATH_MSG_VERBOSE( header << ": fast crude estimation fails ");
            wrkcuts.fastErrcut  = true;
            wrkvrt.param        = wrkprm;
            wrkvrt.cuts         = wrkcuts;
            wrkvrt.isPair       = true;

            workVerticesContainer.push_back( std::move(wrkvrt) );
            continue;
         }

         // position of initial fast fitting
         mnt_init_r.push_back(initVertex.perp());
         wrkprm.vrtFast_r   = initVertex.perp();
         wrkprm.vrtFast_eta = initVertex.eta();
         wrkprm.vrtFast_phi = initVertex.phi();

         wrkcuts.fastRcut = initVertex.perp() > m_maxR;
         if( m_doFastRCut && initVertex.perp() > m_maxR ) continue;
         nPairsInitPos++;

         // impact parameter on initial fast fitting
         std::vector<double> impactParameters;
         std::vector<double> impactParErrors;
         if( ! m_fitSvc->VKalGetImpact(itrk, initVertex, static_cast<int>( itrk->charge() ), impactParameters, impactParErrors) ) continue;
         const auto roughD0_itrk = impactParameters.at(TrkParameter::k_d0);
         const auto roughZ0_itrk = impactParameters.at(TrkParameter::k_z0);
         mnt_init_trkd0.push_back(std::abs(roughD0_itrk));
         mnt_init_trkz0.push_back(std::abs(roughZ0_itrk));
         wrkprm.vrtFast_trkd0.push_back(std::abs(roughD0_itrk));
         wrkprm.vrtFast_trkz0.push_back(std::abs(roughZ0_itrk));

         if( ! m_fitSvc->VKalGetImpact(jtrk, initVertex, static_cast<int>( jtrk->charge() ), impactParameters, impactParErrors) ) continue;
         const auto roughD0_jtrk = impactParameters.at(TrkParameter::k_d0);
         const auto roughZ0_jtrk = impactParameters.at(TrkParameter::k_z0);
         mnt_init_trkd0.push_back(std::abs(roughD0_jtrk));
         mnt_init_trkz0.push_back(std::abs(roughZ0_jtrk));
         wrkprm.vrtFast_trkd0.push_back(std::abs(roughD0_jtrk));
         wrkprm.vrtFast_trkz0.push_back(std::abs(roughZ0_jtrk));

         if ( std::min(roughD0_itrk, roughD0_jtrk) > m_fastD0minCut || std::abs(roughD0_itrk - roughD0_jtrk) > m_fastD0deltaCut ) continue;
         if ( std::min(roughZ0_itrk, roughZ0_jtrk) > m_fastZ0minCut || std::abs(roughZ0_itrk - roughZ0_jtrk) > m_fastZ0deltaCut ) continue;

         nPairsInitTrkd0z0++;

         // Vertex VKal Fitting
         std::vector<const xAOD::NeutralParticle*>  dummyNeutrals;
         timerVrtFit.start();
         m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z(), *fitterState );
         sc = m_fitSvc->VKalVrtFit( baseTracks,
                                    dummyNeutrals,
                                    wrkvrt.vertex, wrkvrt.vertexMom, wrkvrt.charge,
                                    wrkvrt.vertexCov, wrkvrt.chi2PerTrk,
                                    wrkvrt.trkAtVrt, wrkvrt.chi2, *fitterState, false  );
         timerVrtFit.stop();
         if( sc.isFailure() ) {
            wrkcuts.fitErrcut  = true;
            wrkvrt.param       = wrkprm;
            wrkvrt.cuts        = wrkcuts;
            wrkvrt.isPair      = true;

            workVerticesContainer.push_back( std::move(wrkvrt) );
            continue;
         }
         nPairsVtxFit++;

         // Chi2 cut
         mnt_vtxfit_chi2.push_back(wrkvrt.chi2);
         if( wrkvrt.chi2 > m_selVrtChi2Cut) {
            ATH_MSG_VERBOSE( header << ": failed to pass chi2 threshold." );
            wrkcuts.chi2cut = true;
            wrkvrt.param    = wrkprm;
            wrkvrt.cuts     = wrkcuts;
            wrkvrt.isPair   = true;

            workVerticesContainer.push_back( std::move(wrkvrt) );
            continue;
         }
         nPairsVtxChi2++;
         mnt_vtxfit_r.push_back(wrkvrt.vertex.perp());

         // Compatibility to the primary vertex
         Amg::Vector3D vDist = (primVertex!=nullptr)? wrkvrt.vertex - primVertex->position() : wrkvrt.vertex;
         const double vPos = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py()+vDist.z()*wrkvrt.vertexMom.Pz() )/wrkvrt.vertexMom.Rho();
         const double vPosMomAngT = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py() ) / vDist.perp() / wrkvrt.vertexMom.Pt();

         double dphi1 = vDist.phi() - itrk->phi(); while( dphi1 > TMath::Pi() ) { dphi1 -= TMath::TwoPi(); } while( dphi1 < -TMath::Pi() ) { dphi1 += TMath::TwoPi(); }
         double dphi2 = vDist.phi() - jtrk->phi(); while( dphi2 > TMath::Pi() ) { dphi2 -= TMath::TwoPi(); } while( dphi2 < -TMath::Pi() ) { dphi2 += TMath::TwoPi(); }

         wrkprm.vPos        = vPos;
         wrkprm.vPosMomAngT = vPosMomAngT;
         wrkprm.dphi1       = dphi1;
         wrkprm.dphi2       = dphi2;

         if (m_doPVCompatibilityCut) {

            if( std::cos( dphi1 ) < m_dphiPVCut && std::cos( dphi2 ) < m_dphiPVCut ) {
               ATH_MSG_DEBUG( header << ": failed to pass the vPos cut. (both tracks are opposite against the vertex pos)" );
               continue;
            }
            if( vPosMomAngT < m_dphiPVCut ) {
               ATH_MSG_DEBUG( header << ": failed to pass the vPos cut. (pos-mom directions are opposite)" );
               continue;
            }

            if( vPos < m_pvCompatibilityCut ) {
               ATH_MSG_DEBUG( header << ": failed to pass the vPos cut." );
               continue;
            }

         }
         nPairsVtxComp++;

         // material map veto
         wrkprm.isPassMMV = true;
         if (m_doMaterialMapVeto) {
            if (wrkvrt.vertex.perp() > 150) {
               wrkprm.isPassMMV = ( m_materialMapOuter->GetBinContent(m_materialMapOuter->FindBin( wrkvrt.vertex.perp(), wrkvrt.vertex.phi(), wrkvrt.vertex.z() ) ) == 0);
            }
            else {
               for (int i=0;i<5;i++){
                  if (wrkvrt.vertex.perp() < (*m_materialMapMatrix)[i][0]) {
                     float test_x = wrkvrt.vertex.x() + (*m_materialMapMatrix)[i][1];
                     float test_y = wrkvrt.vertex.y() + (*m_materialMapMatrix)[i][2];
                     double calc_phi = std::fmod( TMath::ATan2(test_y,test_x),TMath::Pi()/(*m_materialMapMatrix)[i][3] );
                     if (calc_phi <0) calc_phi = calc_phi + TMath::Pi()/(*m_materialMapMatrix)[i][3];
                     wrkprm.isPassMMV = ( m_materialMapInner->GetBinContent(m_materialMapInner->FindBin(sqrt(test_x*test_x + test_y*test_y), calc_phi, wrkvrt.vertex.z() ) ) == 0);
                  }
               }
            }
           if( ! wrkprm.isPassMMV ) continue;
         }
         nPairsVtxMatveto++;

         // Now this vertex passed all criteria and considred to be a compatible vertices.
         // Therefore the track pair is removed from the incompatibility list.
         if (wrkcuts.isFullPass()) incomp.pop_back();

         wrkvrt.param = wrkprm;
         wrkvrt.cuts  = wrkcuts;
         wrkvrt.isGood  = true;
         wrkvrt.isPair  = true;

         workVerticesContainer.push_back( std::move(wrkvrt) );
      }
   }
   ATH_MSG_DEBUG(header << "Of " << nPairsAll  << " pairs : trkd0" << " / "  <<  "intersect"       << " / "  <<  "intersectPos"        <<  " / " <<  "initPos"       <<  " / " <<  "initD0Z0"          <<  " / " <<  "vtxFit"        <<  " / " <<  "vtxChi2"       <<  " / " <<  "vtxComp"       <<  " / " <<  "matVeto = "
                                                << nPairsTrkd0     << " / "  <<  nPairsIntersect  << " / "  <<  nPairsIntersectPos  <<  " / " <<  nPairsInitPos <<  " / " <<  nPairsInitTrkd0z0 <<  " / " <<  nPairsVtxFit  <<  " / " <<  nPairsVtxChi2 <<  " / " <<  nPairsVtxComp <<  " / " <<  nPairsVtxMatveto);
   //
   return StatusCode::SUCCESS;
}


StatusCode TrigVrtSecInclusive::findDiTrackVertexVSI
( WrkVrtContainer& workVerticesContainer, std::vector<std::pair<size_t,size_t>>& incomp, std::vector<const xAOD::TrackParticle*>& selectedTracks, const EventContext& ctx, const xAOD::Vertex *primVertex ) const
{
   const std::string header = "UTT> ";

   // monitoring
   auto timerTwoCircIntsect = Monitored::Timer<std::chrono::nanoseconds> ("TIME_TwoCircIntsect");
   auto timerVrtFitFast     = Monitored::Timer<std::chrono::nanoseconds> ("TIME_VrtFitFast");
   auto timerVrtFit         = Monitored::Timer<std::chrono::microseconds>("TIME_VrtFit");

   std::vector<float> mnt_pair_dphi;
   std::vector<float> mnt_pair_dr;
   std::vector<float> mnt_intersect_r;
   std::vector<float> mnt_init_r;
   std::vector<float> mnt_init_trkd0;
   std::vector<float> mnt_init_trkz0;
   std::vector<float> mnt_vtxfit_chi2;
   std::vector<float> mnt_vtxfit_r;
   std::vector<float> mnt_vtx_chi2;
   std::vector<float> mnt_vtx_mass;
   std::vector<float> mnt_vtx_mass_high;
   std::vector<float> mnt_vtx_pt;
   std::vector<float> mnt_vtx_charge;
   std::vector<float> mnt_vtx_r;
   auto mon_pair_dphi    = Monitored::Collection("pair_dphi",    mnt_pair_dphi);
   auto mon_pair_dr      = Monitored::Collection("pair_dr",      mnt_pair_dr);
   auto mon_intersect_r  = Monitored::Collection("intersect_r",  mnt_intersect_r);
   auto mon_init_r       = Monitored::Collection("init_r",       mnt_init_r);
   auto mon_init_trkd0   = Monitored::Collection("init_trkd0",   mnt_init_trkd0);
   auto mon_init_trkz0   = Monitored::Collection("init_trkz0",   mnt_init_trkz0);
   auto mon_vtxfit_chi2  = Monitored::Collection("vtxfit_chi2",  mnt_vtxfit_chi2);
   auto mon_vtxfit_r     = Monitored::Collection("vtxfit_r",     mnt_vtxfit_r);
   auto mon_vtx_chi2     = Monitored::Collection("vtx_chi2",     mnt_vtx_chi2);
   auto mon_vtx_mass     = Monitored::Collection("vtx_mass",     mnt_vtx_mass);
   auto mon_vtx_mass_high= Monitored::Collection("vtx_mass_high",mnt_vtx_mass);
   auto mon_vtx_pt       = Monitored::Collection("vtx_pt",       mnt_vtx_pt);
   auto mon_vtx_charge   = Monitored::Collection("vtx_charge",   mnt_vtx_charge);
   auto mon_vtx_r        = Monitored::Collection("vtx_r",        mnt_vtx_r);
   auto monVertex  = Monitored::Group(m_monTool, mon_pair_dphi, mon_pair_dr, mon_intersect_r, mon_init_r, mon_init_trkd0, mon_init_trkz0, mon_vtxfit_chi2, mon_vtxfit_r,
        mon_vtx_chi2,mon_vtx_mass,mon_vtx_mass_high,mon_vtx_pt,mon_vtx_charge,mon_vtx_r);

   // vertexing
   const double roughD0Cut{ 100. };
   const double roughZ0Cut{  50. };

   unsigned int nPairsAll           = 0;
   unsigned int nPairsTrkd0         = 0;
   unsigned int nPairsIntersect     = 0;
   unsigned int nPairsIntersectPos  = 0;
   unsigned int nPairsInitPos       = 0;
   unsigned int nPairsInitTrkd0z0   = 0;
   unsigned int nPairsVtxFit        = 0;
   unsigned int nPairsVtxChi2       = 0;
   unsigned int nPairsVtxComp       = 0;

   for( auto itrkIter = selectedTracks.begin(); itrkIter != selectedTracks.end(); ++itrkIter ) {
      const xAOD::TrackParticle* itrk = *itrkIter;
      for( auto jtrkIter = std::next(itrkIter); jtrkIter != selectedTracks.end(); ++jtrkIter ) {  // for starts from here
         const xAOD::TrackParticle* jtrk = *jtrkIter;
         nPairsAll++;

         auto monTimeStep = Monitored::Group(m_monTool, timerTwoCircIntsect, timerVrtFitFast, timerVrtFit);

         //
         const int itrk_id = std::distance(selectedTracks.begin(), itrkIter);
         const int jtrk_id = std::distance(selectedTracks.begin(), jtrkIter);

         WrkVrt::AlgCuts  wrkcuts;
         WrkVrt::AlgParam wrkprm;
         WrkVrt           wrkvrt;

         // Set track indices
         wrkvrt.selectedTrackIndices().clear();
         wrkvrt.selectedTrackIndices().emplace_back(itrk_id);
         wrkvrt.selectedTrackIndices().emplace_back(jtrk_id);

         // trk d0
         if( std::abs( itrk->d0() ) < m_twoTrkVtxFormingD0Cut && std::abs( jtrk->d0() ) < m_twoTrkVtxFormingD0Cut ) continue;
         nPairsTrkd0++;

         // Attempt to think the combination is incompatible by default
         incomp.emplace_back( std::pair<size_t, size_t>(itrk_id, jtrk_id) );

         // two circles intersection
         const Trk::Perigee& perigee1 = itrk->perigeeParameters();
         const Trk::Perigee& perigee2 = jtrk->perigeeParameters();
         int flag = 0;
         int errorcode = 0;
         InDet::VertexPointEstimator::Values_t decors;
         timerTwoCircIntsect.start();
         Amg::Vector3D circIntersect = m_vertexPointEstimator->getCirclesIntersectionPoint(&perigee1, &perigee2, flag, errorcode, decors);
         timerTwoCircIntsect.stop();
         if (errorcode != 0) {
            ATH_MSG_VERBOSE( header << ": two circles intersect failed");
            wrkcuts.twoCircErrcut   = true;
            wrkvrt.param            = wrkprm;
            wrkvrt.cuts             = wrkcuts;
            wrkvrt.isPair           = true;

            workVerticesContainer.push_back( std::move(wrkvrt) );
            continue;
         }
         float dphi = std::abs(decors["deltaPhiTracks"]);
         float dr   = std::abs(decors["DR1R2"]);
         float intersect_r = circIntersect.perp();
         mnt_pair_dphi.push_back(dphi);
         mnt_pair_dr.push_back(dr);
         mnt_intersect_r.push_back(intersect_r);

         wrkprm.twoCirc_dphi  = dphi;
         wrkprm.twoCirc_dr    = dr;
         wrkprm.twoCirc_int_r = intersect_r;

         nPairsIntersect++;
         wrkcuts.twoCircRcut = intersect_r > m_maxR;
         nPairsIntersectPos++;

         // initial fast fitting
         std::vector<const xAOD::TrackParticle*> baseTracks;
         baseTracks.emplace_back( itrk );
         baseTracks.emplace_back( jtrk );
         Amg::Vector3D   initVertex;
         timerVrtFitFast.start();
         auto fitterState = m_fitSvc->makeState(ctx);
         m_fitSvc->setApproximateVertex( 0., 0., 0., *fitterState );
         StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex, *fitterState );
         timerVrtFitFast.stop();
         if( sc.isFailure() ) {
            ATH_MSG_VERBOSE( header << ": fast crude estimation fails ");
            wrkcuts.fastErrcut  = true;
            wrkvrt.param        = wrkprm;
            wrkvrt.cuts         = wrkcuts;
            wrkvrt.isPair       = true;

            workVerticesContainer.push_back( std::move(wrkvrt) );
            continue;
         }

         // position of initial fast fitting
         mnt_init_r.push_back(initVertex.perp());
         wrkprm.vrtFast_r   = initVertex.perp();
         wrkprm.vrtFast_eta = initVertex.eta();
         wrkprm.vrtFast_phi = initVertex.phi();
         wrkcuts.fastRcut = initVertex.perp() > m_maxR;

         nPairsInitPos++;

         // impact parameter on initial fast fitting
         std::vector<double> impactParameters;
         std::vector<double> impactParErrors;
         if( ! m_fitSvc->VKalGetImpact(itrk, initVertex, static_cast<int>( itrk->charge() ), impactParameters, impactParErrors) ) continue;
         const auto roughD0_itrk = impactParameters.at(TrkParameter::k_d0);
         const auto roughZ0_itrk = impactParameters.at(TrkParameter::k_z0);
         mnt_init_trkd0.push_back(std::abs(roughD0_itrk));
         mnt_init_trkz0.push_back(std::abs(roughZ0_itrk));
         wrkprm.vrtFast_trkd0.push_back(std::abs(roughD0_itrk));
         wrkprm.vrtFast_trkz0.push_back(std::abs(roughZ0_itrk));
         if( std::abs(roughD0_itrk) > roughD0Cut || std::abs(roughZ0_itrk) > roughZ0Cut ) continue;

         if( ! m_fitSvc->VKalGetImpact(jtrk, initVertex, static_cast<int>( jtrk->charge() ), impactParameters, impactParErrors) ) continue;
         const auto roughD0_jtrk = impactParameters.at(TrkParameter::k_d0);
         const auto roughZ0_jtrk = impactParameters.at(TrkParameter::k_z0);
         mnt_init_trkd0.push_back(std::abs(roughD0_jtrk));
         mnt_init_trkz0.push_back(std::abs(roughZ0_jtrk));
         wrkprm.vrtFast_trkd0.push_back(std::abs(roughD0_jtrk));
         wrkprm.vrtFast_trkz0.push_back(std::abs(roughZ0_jtrk));
         if( std::abs(roughD0_jtrk) > roughD0Cut || std::abs(roughZ0_jtrk) > roughZ0Cut ) continue;

         nPairsInitTrkd0z0++;

         // Vertex VKal Fitting
         std::vector<const xAOD::NeutralParticle*>  dummyNeutrals;
         timerVrtFit.start();
         m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z(), *fitterState );
         sc = m_fitSvc->VKalVrtFit( baseTracks,
            dummyNeutrals,
            wrkvrt.vertex, wrkvrt.vertexMom, wrkvrt.charge,
            wrkvrt.vertexCov, wrkvrt.chi2PerTrk,
            wrkvrt.trkAtVrt, wrkvrt.chi2, *fitterState, false  );
         timerVrtFit.stop();
         if( sc.isFailure() ) {
           wrkcuts.fitErrcut = true;
           wrkvrt.param = wrkprm;
           wrkvrt.cuts  = wrkcuts;
           wrkvrt.isPair  = true;

           workVerticesContainer.push_back( std::move(wrkvrt) );
           continue;
         }

         nPairsVtxFit++;

         // Chi2 cut
         mnt_vtxfit_chi2.push_back(wrkvrt.chi2);
         if( wrkvrt.chi2 > m_selVrtChi2Cut) {
            ATH_MSG_VERBOSE( header << ": failed to pass chi2 threshold." );
            wrkcuts.chi2cut = true;
            wrkvrt.param    = wrkprm;
            wrkvrt.cuts     = wrkcuts;
            wrkvrt.isPair   = true;

            workVerticesContainer.push_back( std::move(wrkvrt) );
            continue;
         }
         nPairsVtxChi2++;
         mnt_vtxfit_r.push_back(wrkvrt.vertex.perp());

         // Compatibility to the primary vertex
         Amg::Vector3D vDist = (primVertex!=nullptr)? wrkvrt.vertex - primVertex->position() : wrkvrt.vertex;
         const double vPos = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py()+vDist.z()*wrkvrt.vertexMom.Pz() )/wrkvrt.vertexMom.Rho();
         const double vPosMomAngT = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py() ) / vDist.perp() / wrkvrt.vertexMom.Pt();

         double dphi1 = vDist.phi() - itrk->phi(); while( dphi1 > TMath::Pi() ) { dphi1 -= TMath::TwoPi(); } while( dphi1 < -TMath::Pi() ) { dphi1 += TMath::TwoPi(); }
         double dphi2 = vDist.phi() - jtrk->phi(); while( dphi2 > TMath::Pi() ) { dphi2 -= TMath::TwoPi(); } while( dphi2 < -TMath::Pi() ) { dphi2 += TMath::TwoPi(); }

         wrkprm.vPos        = vPos;
         wrkprm.vPosMomAngT = vPosMomAngT;
         wrkprm.dphi1       = dphi1;
         wrkprm.dphi2       = dphi2;

         if( std::cos( dphi1 ) < m_dphiPVCut && std::cos( dphi2 ) < m_dphiPVCut ) {
                  ATH_MSG_DEBUG( header << ": failed to pass the vPos cut. (both tracks are opposite against the vertex pos)" );
                  continue;
         }
         if( vPosMomAngT < m_dphiPVCut ) {
                  ATH_MSG_DEBUG( header << ": failed to pass the vPos cut. (pos-mom directions are opposite)" );
                  continue;
         }
         if( vPos < m_pvCompatibilityCut ) {
                  ATH_MSG_DEBUG( header << ": failed to pass the vPos cut." );
                  continue;
         }
         nPairsVtxComp++;

         // Now this vertex passed all criteria and considred to be a compatible vertices.
         // Therefore the track pair is removed from the incompatibility list.
         if (wrkcuts.isFullPass()) incomp.pop_back();

         wrkvrt.param = wrkprm;
         wrkvrt.cuts  = wrkcuts;
         wrkvrt.isGood  = true;
         wrkvrt.isPair  = true;

         workVerticesContainer.push_back( std::move(wrkvrt) );
      }
   }
   ATH_MSG_INFO(header << "Of " <<  nPairsAll  << " pairs : trkd0" << " / "  <<  "intersect"      << " / " <<  "intersectPos"         <<  " / " <<   "initPos"         <<  " / " <<  "initD0Z0"          <<  " / " <<  "vtxFit"        <<  " / " <<  "vtxChi2"       <<  " / " <<  "vtxComp = "
                                                << nPairsTrkd0     << " / "  <<  nPairsIntersect << " / " <<  nPairsIntersectPos   <<  " / " <<   nPairsInitPos   <<  " / " <<  nPairsInitTrkd0z0 <<  " / " <<  nPairsVtxFit  <<  " / " <<  nPairsVtxChi2 <<  " / " <<  nPairsVtxComp );
   //
   return StatusCode::SUCCESS;
}


StatusCode TrigVrtSecInclusive::findNtrackVerticesVSI
( WrkVrtContainer& workVerticesContainer, std::vector<std::pair<size_t,size_t>>& incomp, std::vector<const xAOD::TrackParticle*>& selectedTracks, const EventContext& ctx ) const
{
   ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": begin");

   const auto compSize = selectedTracks.size()*(selectedTracks.size() - 1)/2 - incomp.size();

   auto pgraph = std::make_unique<Trk::PGraph>();

   ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": compatible track pair size   = " << compSize );
   ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": incompatible track pair size = " << incomp.size() );

   ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": incompatibility graph finder mode" );

   // clear the container
   workVerticesContainer.clear();

   // Graph method: Trk::pgraphm_()
   // used in order to find compatible sub-graphs from the incompatible graph

   // List of edgeds between imcompatible nodes
   // This weit is the data model of imcompatible graph used in Trk::pgraphm_().
   std::vector<long int> weit;

   for( auto& pair : incomp ) {
      weit.emplace_back( pair.first  + 1 ); /* +1 is needed for PGRAPH due to FORTRAN-style counting */
      weit.emplace_back( pair.second + 1 ); /* +1 is needed for PGRAPH due to FORTRAN-style counting */
   }

   // Solution of the graph method routine (minimal covering of the graph)
   // The size of the solution is returned by NPTR (see below)
   std::vector<long int> solution( selectedTracks.size() );

   // Number of edges in the list is the size of incompatibility track pairs.
   long int nEdges = incomp.size();

   // input number of nodes in the graph.
   long int nTracks = static_cast<long int>( selectedTracks.size() );

   // Input variable: the threshold. Solutions shorter than nth are not returned (ignored).
   long int nth = 2;    //VK some speed up

   // NPTR: I/O variable (Destructive FORTRAN Style!!!)
   // - on input:   =0 for initialization, >0 to get next solution
   // - on output:  >0 : length of the solution stored in set; =0 : no more solutions can be found
   long int solutionSize { 0 };

   // This is just a unused strawman needed for m_fitSvc->VKalVrtFit()
   std::vector<const xAOD::TrackParticle*>    baseTracks;
   std::vector<const xAOD::NeutralParticle*>  dummyNeutrals;

   // Main iteration
   while(true) {
      // Find a solution from the given set of incompatible tracks (==weit)
      pgraph->pgraphm_( weit.data(), nEdges, nTracks, solution.data(), &solutionSize, nth);

      ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": Trk::pgraphm_() output: solutionSize = " << solutionSize );

      if(solutionSize <= 0)  break;      // No more solutions ==> Exit
      if(solutionSize == 1)  continue;   // i.e. single node  ==> Not a good solution

      baseTracks.clear();

      std::string msg = "solution = [ ";
      for( int i=0; i< solutionSize; i++) {
         msg += Form( "%ld, ", solution[i]-1 );
      }
      msg += " ]";
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": " << msg );

      // varaible of new vertex
      WrkVrt wrkvrt;

      // Try to compose a new vertex using the solution nodes
      // Here the track ID is labelled with array
      wrkvrt.isGood = true;
      wrkvrt.selectedTrackIndices().clear();

      for(long int i = 0; i<solutionSize; i++) {
         wrkvrt.selectedTrackIndices().emplace_back(solution[i]-1);
         baseTracks.emplace_back( selectedTracks.at(solution[i]-1) );
      }

      // Perform vertex fitting
      Amg::Vector3D initVertex;
      auto fitterState = m_fitSvc->makeState(ctx);

      StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex, *fitterState );/* Fast crude estimation */
      if(sc.isFailure()) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");

      m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z(), *fitterState );

      sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                wrkvrt.vertex,
                                wrkvrt.vertexMom,
                                wrkvrt.charge,
                                wrkvrt.vertexCov,
                                wrkvrt.chi2PerTrk,
                                wrkvrt.trkAtVrt,
                                wrkvrt.chi2,
                                *fitterState, false);

      ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": FoundAppVrt=" << solutionSize << ", (r, z) = " << wrkvrt.vertex.perp() << ", " << wrkvrt.vertex.z()  <<  ", chi2/ndof = "  <<  wrkvrt.fitQuality() );

      if( sc.isFailure() )  {

        if( wrkvrt.selectedTrackIndices().size() <= 2 ) {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit failed in 2-trk solution ==> give up.");
          continue;
        }

        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit failed ==> retry...");

        WrkVrt tmp;
        tmp.isGood = false;

        // Create 2-trk vertex combination and find any compatible vertex
        for( auto& itrk: wrkvrt.selectedTrackIndices() ) {
          for( auto& jtrk: wrkvrt.selectedTrackIndices() ) {
            if( itrk == jtrk ) continue;
            if( tmp.isGood ) break;

            tmp.selectedTrackIndices().clear();
            tmp.selectedTrackIndices().emplace_back( itrk );
            tmp.selectedTrackIndices().emplace_back( jtrk );

            baseTracks.clear();
            baseTracks.emplace_back( selectedTracks.at( itrk ) );
            baseTracks.emplace_back( selectedTracks.at( jtrk ) );

            // Perform vertex fitting
            Amg::Vector3D initVertex;

            sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex, *fitterState );
            if( sc.isFailure() ) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");

            m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z(), *fitterState );

            sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                      tmp.vertex,
                                      tmp.vertexMom,
                                      tmp.charge,
                                      tmp.vertexCov,
                                      tmp.chi2PerTrk,
                                      tmp.trkAtVrt,
                                      tmp.chi2,
                                      *fitterState, false);

            if( sc.isFailure() ) continue;

            tmp.isGood = true;

          }
        }

        if( !tmp.isGood ) {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Did not find any viable vertex in all 2-trk combinations. Give up.");
          continue;
        }

        // Now, found at least one seed 2-track vertex. ==> attempt to attach other tracks
        for( auto& itrk: wrkvrt.selectedTrackIndices() ) {

          if( std::find( tmp.selectedTrackIndices().begin(), tmp.selectedTrackIndices().end(), itrk ) != tmp.selectedTrackIndices().end() ) continue;

          auto backup = tmp;

          tmp.selectedTrackIndices().emplace_back( itrk );
          baseTracks.clear();
          for( auto& jtrk : tmp.selectedTrackIndices() ) { baseTracks.emplace_back( selectedTracks.at(jtrk) ); }

          // Perform vertex fitting
          Amg::Vector3D initVertex;

          sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex, *fitterState );/* Fast crude estimation */
          if(sc.isFailure()) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");

          m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z(), *fitterState );
          sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                    tmp.vertex,
                                    tmp.vertexMom,
                                    tmp.charge,
                                    tmp.vertexCov,
                                    tmp.chi2PerTrk,
                                    tmp.trkAtVrt,
                                    tmp.chi2,
                                    *fitterState, false);

          if( sc.isFailure() ) {
            tmp = backup;
            continue;
          }

        }

        wrkvrt = tmp;
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit succeeded; register the vertex to the list.");
        wrkvrt.isGood                = true;
        wrkvrt.closestWrkVrtIndex    = std::numeric_limits<unsigned>::max();
        wrkvrt.closestWrkVrtValue    = std::numeric_limits<double>::max();
        workVerticesContainer.emplace_back( wrkvrt );

      } else {

        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit succeeded; register the vertex to the list.");
        wrkvrt.isGood                = true;
        wrkvrt.closestWrkVrtIndex    = std::numeric_limits<unsigned>::max();
        wrkvrt.closestWrkVrtValue    = std::numeric_limits<double>::max();
        workVerticesContainer.emplace_back( wrkvrt );

      }

    }




  if (m_truncateWrkVertices){
    if (workVerticesContainer.size() > m_maxWrkVertices){
      ATH_MSG_INFO("WrkVertex truncated. Too many vertices");
      workVerticesContainer.resize(m_maxWrkVertices);
    }
  }

  ATH_CHECK( cleanUp(workVerticesContainer) );

  //-Identify remaining 2-track vertices with very bad Chi2 and mass (b-tagging)
  ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": Identify remaining 2-track vertices with very bad Chi2 and mass (b-tagging).");
  for( auto& wrkvrt : workVerticesContainer ) {

    if( TMath::Prob( wrkvrt.chi2, wrkvrt.ndof() ) < m_improveChi2ProbThreshold ) wrkvrt.isGood = false;
    if( wrkvrt.selectedTrackIndices().size() != 2 ) continue;
  }

  return StatusCode::SUCCESS;

}



template<typename VrtType, typename Coord>
StatusCode TrigVrtSecInclusive::findNTrackVertex
( TrigVrtSecInclusive::WrkVrtContainer& workVerticesContainer, TrigVSI::VtxMap<VrtType,Coord>& vtxmap, const std::vector<const xAOD::TrackParticle*>& selectedTracks, const EventContext& ctx ) const
{
  const std::string header = "UTT> TrigVrtSecInclusive findNTrackVertex : ";

  ATH_MSG_DEBUG( header << "start" );

  // monitoring
  auto timerRetrvFromMap   = Monitored::Timer<std::chrono::nanoseconds>  ("TIME_RetrvFromMap");
  auto timerMergeParGraph  = Monitored::Timer<std::chrono::microseconds> ("TIME_MergeParGraph");
  auto timerMergeSimple    = Monitored::Timer<std::chrono::microseconds> ("TIME_MergeSimple");

  size_t n_clst = vtxmap.nClusters();
  size_t n_vtx_pargraph = 0;
  size_t n_vtx_simple   = 0;

  for (size_t i_clst = 0; i_clst < n_clst; i_clst++) {
    auto monTimeClust = Monitored::Group(m_monTool, timerRetrvFromMap, timerMergeParGraph, timerMergeSimple);
    timerRetrvFromMap.start();

    ATH_MSG_DEBUG( header << "Retrieve cluster, track indices and incompatible pairs" );

    auto clst = vtxmap.getCluster(i_clst);
    auto selected_track_indices = clst.getSelectedTrackIndices();
    auto incomp                 = clst.getIncompIndices();

    ATH_MSG_DEBUG( header << "Convert set to vector" );

    std::vector<size_t> v_track_indices( selected_track_indices.begin(), selected_track_indices.end() );

    timerRetrvFromMap.stop();

    ATH_MSG_DEBUG( header << "Cluster number : " <<  i_clst << " | " << incomp.size() << " incompatible pairs | " << v_track_indices.size() << " tracks" );
    ATH_MSG_DEBUG( header << "Pos avr : ( R: " << clst.PosCoord().at(0) << ", eta: " << clst.PosCoord().at(1) << ", phi: " << clst.PosCoord().at(2) << " )" );
    ATH_MSG_DEBUG( header << "Number of cells : " << clst.nPoints() );

    if ( v_track_indices.size() > m_maxTrks ) {
      ATH_MSG_DEBUG( header << "Skipped the cluster. Too many tracks" );
      continue;
    }

    if ( clst.nVtx() >= m_minTrkPairsMerge || selected_track_indices.size() >= m_minTrksMerge ) {

      timerMergeParGraph.start();
      ATH_CHECK( mergeVertexFromDiTrkVrt( workVerticesContainer, incomp, v_track_indices, selectedTracks, ctx ) );
      timerMergeParGraph.stop();

      n_vtx_pargraph++;

    } else {

      timerMergeSimple.start();

      WrkVrt wrkvrt;
      wrkvrt.isGood = true;
      wrkvrt.selectedTrackIndices().clear();

      for (size_t i_trk = 0; i_trk < v_track_indices.size(); i_trk++) {
        wrkvrt.selectedTrackIndices().emplace_back( v_track_indices.at(i_trk) );
      }

      if ( fitVertexFromTracks(wrkvrt, selectedTracks, ctx) == StatusCode::SUCCESS ) {
        workVerticesContainer.emplace_back( std::move(wrkvrt) );
      }

      timerMergeSimple.stop();

      n_vtx_simple++;

    }

  }

  ATH_MSG_DEBUG( header <<  "Partial graph method / Simple method / All vertex = " << n_vtx_pargraph << " / " << n_vtx_simple << " / "  << n_vtx_pargraph+n_vtx_simple );
  return StatusCode::SUCCESS;

}

template StatusCode TrigVrtSecInclusive::findNTrackVertex ( WrkVrtContainer&, TrigVSI::VtxMap<TrigVrtSecInclusive::WrkVrt,TrigVSI::Coordinate::Pseudo>&, const std::vector<const xAOD::TrackParticle*>&, const EventContext& ) const;



/**
 *  @brief      Reconstruct multi-track vertices from incompatible track pair lists.
 *
 *  @param[out] workVerticesContainer   Vertex container to add reconstructed vertices.
 *
 *  @param[in]  incomp                  Incompatible track pair lists. Tracks are written in indices in the full selected track lists.
 *
 *  @param[in]  trkIdx                  The partial list of indices of tracks to be used in reconstruction.
 *
 *  @param[in]  selectedTracks          Full list of tracks that passed the track selection.
 *
 *  @param[in]  ctx                     The event context. Needed for fitting vertex and extraporate tracks.
 */
StatusCode TrigVrtSecInclusive::mergeVertexFromDiTrkVrt
( TrigVrtSecInclusive::WrkVrtContainer& workVerticesContainer, const std::vector<std::pair<size_t,size_t>>& incomp, const std::vector<size_t>& trkIdx, const std::vector<const xAOD::TrackParticle*>& selectedTracks, const EventContext& ctx ) const
{

  ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": begin");
  ATH_MSG_INFO ("UTT>    TrigVrtSecInclusive::mergeVertexFromDiTrkVrt : start");

  // Graph method: Trk::pgraphm_()
  // used in order to find compatible sub-graphs from the incompatible graph
  auto pgraph = std::make_unique<Trk::PGraph>();

  std::unordered_map<size_t,size_t> dict_trk_idx;
  size_t n_trk = trkIdx.size(); // Dictionary: global trkId -> local trkId

  for (size_t i = 0; i < n_trk; i++) {
    dict_trk_idx.emplace( trkIdx.at(i), i );
  }

  const auto compSize = (n_trk*(n_trk - 1))/2 - incomp.size();

  ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": compatible track pair size   = " << compSize );
  ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": incompatible track pair size = " << incomp.size() );

  // List of edgeds between imcompatible nodes
  // This weit is the data model of imcompatible graph used in Trk::pgraphm_().
  std::vector<long int> weit;

  for( auto& pair : incomp ) {
    weit.emplace_back( dict_trk_idx[pair.first]  + 1 ); /* +1 is needed for PGRAPH due to FORTRAN-style counting */
    weit.emplace_back( dict_trk_idx[pair.second] + 1 ); /* +1 is needed for PGRAPH due to FORTRAN-style counting */
  }

  // Solution of the graph method routine (minimal covering of the graph)
  // The size of the solution is returned by NPTR (see below)
  std::vector<long int> solution( n_trk );

  // Number of edges in the list is the size of incompatibility track pairs.
  long int nEdges = incomp.size();

  // input number of nodes in the graph.
  long int nTracks = static_cast<long int>( n_trk );

  // Input variable; the threshold. Solutions shorter than nth are not returned (ignored).
  long int nth = 2;    //VK some speed up

  // NPTR: I/O variable (Destructive FORTRAN Style!!!)
  // - on input:   =0 for initialization, >0 to get next solution
  // - on output:  >0 : length of the solution stored in set; =0 : no more solutions can be found
  long int solutionSize { 0 };

  ATH_MSG_INFO( "UTT>    TrigVrtSecInclusive::mergeVertexFromDiTrkVrt : while loop start" );

  // Main iteration
  while(true) {

    // Find a solution from the given set of incompatible tracks (==weit)
    pgraph->pgraphm_( weit.data(), nEdges, nTracks, solution.data(), &solutionSize, nth);

    ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": Trk::pgraphm_() output: solutionSize = " << solutionSize );
    ATH_MSG_INFO( "UTT>    TrigVrtSecInclusive::mergeVertexFromDiTrkVrt : Trk::pgraphm_() output: solutionSize = " << solutionSize );

    if(solutionSize <= 0)  break;      // No more solutions ==> Exit
    if(solutionSize == 1)  continue;   // i.e. single node  ==> Not a good solution

    // Print solution
    std::string msg = "solution = [ ";
    for( int i=0; i< solutionSize; i++) {
      msg += Form( "%ld, ", trkIdx[ solution[i]-1 ] );
    }
    msg += " ]";
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": " << msg );

    // varaible of new vertex
    WrkVrt wrkvrt;

    // Try to compose a new vertex using the solution nodes
    // Here the track ID is labelled with array
    wrkvrt.isGood = true;
    wrkvrt.selectedTrackIndices().clear();

    for(long int i = 0; i<solutionSize; i++) {
      wrkvrt.selectedTrackIndices().emplace_back( trkIdx[ solution[i]-1 ] );
    }

    if ( fitVertexFromTracks(wrkvrt, selectedTracks, ctx) == StatusCode::SUCCESS ) {
      workVerticesContainer.emplace_back( std::move(wrkvrt) );
    }

  }

  return StatusCode::SUCCESS;
}


/**
 *  @brief                              Reconstruct vertex from given tracks.
 *
 *  @param[in,out]  wrkvrt              Base object to be fitted. This algorithm runs fitting using tracks in wrkvrt.selectedTrackIndices().
 *                                      Information gained from fitting will be stored in this object.
 *
 *  @param[in]      selectedTracks      Full list of tracks that passed the track selection.
 *
 *  @param[in]      ctx                 The event context. Needed for fitting vertex and extraporate tracks.
 *
 *  @return                             Returns SUCCESS and set wrkvrt.isGood true when a vertex is successfully reconstructed from all of given tracks or some of them.
 *                                      In the case that even a seed di-track vertex can't be reconstructed, returns FAILURE and set wrkvrt.isGood false.
 */
StatusCode TrigVrtSecInclusive::fitVertexFromTracks
( WrkVrt& wrkvrt, const std::vector<const xAOD::TrackParticle*>& selectedTracks, const EventContext& ctx ) const
{

  std::vector<const xAOD::TrackParticle*>    baseTracks;
  std::vector<const xAOD::NeutralParticle*>  dummyNeutrals; // This is just a unused strawman needed for m_fitSvc->VKalVrtFit()

  baseTracks.clear();

  // Try to compose a new vertex using the solution nodes
  // Here the track ID is labelled with array
  wrkvrt.isGood = true;

  size_t n_trk = wrkvrt.selectedTrackIndices().size();

  for(size_t i = 0; i<n_trk; i++) {
    baseTracks.emplace_back( selectedTracks.at( wrkvrt.selectedTrackIndices().at(i) ) );
  }

  // Perform vertex fitting
  Amg::Vector3D initVertex;
  auto fitterState = m_fitSvc->makeState(ctx);

  StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex, *fitterState );/* Fast crude estimation */
  if(sc.isFailure()) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");

  m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z(), *fitterState );

  sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                            wrkvrt.vertex,
                            wrkvrt.vertexMom,
                            wrkvrt.charge,
                            wrkvrt.vertexCov,
                            wrkvrt.chi2PerTrk,
                            wrkvrt.trkAtVrt,
                            wrkvrt.chi2,
                            *fitterState, false);

  ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": FoundAppVrt=" << n_trk << ", (r, z) = " << wrkvrt.vertex.perp() << ", " << wrkvrt.vertex.z()  <<  ", chi2/ndof = "  <<  wrkvrt.fitQuality() );

  if( sc.isFailure() )  {

    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit failed ==> retry...");

    WrkVrt tmp;
    tmp.isGood = false;

    // Create 2-trk vertex combination and find any compatible vertex
    for( auto& itrk: wrkvrt.selectedTrackIndices() ) {
      for( auto& jtrk: wrkvrt.selectedTrackIndices() ) {
        if( itrk == jtrk ) continue;
        if( tmp.isGood ) break;

        tmp.selectedTrackIndices().clear();
        tmp.selectedTrackIndices().emplace_back( itrk );
        tmp.selectedTrackIndices().emplace_back( jtrk );

        baseTracks.clear();
        baseTracks.emplace_back( selectedTracks.at( itrk ) );
        baseTracks.emplace_back( selectedTracks.at( jtrk ) );

        // Perform vertex fitting
        Amg::Vector3D initVertex;

        sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex, *fitterState );
        if( sc.isFailure() ) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");

        m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z(), *fitterState );

        sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                  tmp.vertex,
                                  tmp.vertexMom,
                                  tmp.charge,
                                  tmp.vertexCov,
                                  tmp.chi2PerTrk,
                                  tmp.trkAtVrt,
                                  tmp.chi2,
                                  *fitterState, false);

        if( sc.isFailure() ) continue;
        if( tmp.chi2 > m_selVrtChi2Cut ) continue;

        tmp.isGood = true;

      }
    }

    if( !tmp.isGood ) {
      ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Did not find any viable vertex in all 2-trk combinations. Give up.");
      wrkvrt = std::move(tmp);
      return StatusCode::FAILURE;
    }

    // Now, found at least one seed 2-track vertex. ==> attempt to attach other tracks
    for( auto& itrk: wrkvrt.selectedTrackIndices() ) {

      if( std::find( tmp.selectedTrackIndices().begin(), tmp.selectedTrackIndices().end(), itrk ) != tmp.selectedTrackIndices().end() ) continue;

      auto backup = tmp;

      tmp.selectedTrackIndices().emplace_back( itrk );
      baseTracks.clear();
      for( auto& jtrk : tmp.selectedTrackIndices() ) { baseTracks.emplace_back( selectedTracks.at(jtrk) ); }

      // Perform vertex fitting
      Amg::Vector3D initVertex;

      sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex, *fitterState );/* Fast crude estimation */
      if(sc.isFailure()) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");

      m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z(), *fitterState );
      sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                tmp.vertex,
                                tmp.vertexMom,
                                tmp.charge,
                                tmp.vertexCov,
                                tmp.chi2PerTrk,
                                tmp.trkAtVrt,
                                tmp.chi2,
                                *fitterState, false);

      if( sc.isFailure() ) {
        tmp = backup;
        continue;
      }

    }

    wrkvrt = std::move(tmp);

  }

  ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit succeeded; register the vertex to the list.");
  wrkvrt.isGood                = true;
  wrkvrt.closestWrkVrtIndex    = std::numeric_limits<unsigned>::max();
  wrkvrt.closestWrkVrtValue    = std::numeric_limits<double>::max();

  return StatusCode::SUCCESS;
}


bool TrigVrtSecInclusive::selectTrack( const xAOD::TrackParticle* trk ) const
{
   if( ! selectTrack_pTCut(trk) )      return false;
   if( ! selectTrack_d0Cut(trk) )      return false;
   if( ! selectTrack_z0Cut(trk) )      return false;
   if( ! selectTrack_chi2Cut(trk) )    return false;
   if( ! selectTrack_hitPattern(trk) ) return false;
   return true;
}


bool TrigVrtSecInclusive::selectTrack_d0Cut       ( const xAOD::TrackParticle* trk ) const { return ( std::abs( trk->d0() ) > m_d0TrkPVDstMinCut && std::abs( trk->d0() ) < m_d0TrkPVDstMaxCut ); }
bool TrigVrtSecInclusive::selectTrack_z0Cut       ( const xAOD::TrackParticle* trk ) const { return ( std::abs( trk->z0() ) > m_z0TrkPVDstMinCut && std::abs( trk->z0() ) < m_z0TrkPVDstMaxCut ); }
bool TrigVrtSecInclusive::selectTrack_pTCut       ( const xAOD::TrackParticle* trk ) const { return trk->pt() > m_trkPtCut; }
bool TrigVrtSecInclusive::selectTrack_chi2Cut     ( const xAOD::TrackParticle* trk ) const { return trk->chiSquared() / (trk->numberDoF()+1e-9) < m_trkChi2Cut; }


bool TrigVrtSecInclusive::selectTrack_hitPattern( const xAOD::TrackParticle* trk ) const
{
   uint8_t PixelHits = 0;
   uint8_t SCTHits   = 0;
   uint8_t BLayHits  = 0;
   uint8_t PixShare  = 0;
   uint8_t SCTShare  = 0;
   uint8_t TRTHits   = 0;

   if( !(trk->summaryValue( PixelHits, xAOD::numberOfPixelHits               ) ) ) PixelHits =0;
   if( !(trk->summaryValue( SCTHits,   xAOD::numberOfSCTHits                 ) ) ) SCTHits   =0;
   if( !(trk->summaryValue( BLayHits,  xAOD::numberOfInnermostPixelLayerHits ) ) ) BLayHits  =0;
   if( !(trk->summaryValue( PixShare,  xAOD::numberOfPixelSharedHits         ) ) ) PixShare  =0;
   if( !(trk->summaryValue( SCTShare,  xAOD::numberOfSCTSharedHits           ) ) ) SCTShare  =0;
   if( !(trk->summaryValue( TRTHits,   xAOD::numberOfTRTHits                 ) ) ) TRTHits   =0;

   uint8_t SharedHits = PixShare + SCTShare;

   // do Pixel/SCT/SiHits only if we exclude StandAlone TRT hits
   if(PixelHits           < m_cutPixelHits)  return false;
   if(SCTHits             < m_cutSctHits)    return false;
   if((PixelHits+SCTHits) < m_cutSiHits)     return false;
   if(BLayHits            < m_cutBLayHits)   return false;
   if(SharedHits          > m_cutSharedHits) return false;

   // passed
   return true;
}


size_t TrigVrtSecInclusive::nTrkCommon( WrkVrtContainer& workVerticesContainer, const std::pair<unsigned, unsigned>& pairIndex) const
{
  //
  //  Number of common tracks for 2 vertices
  //

  auto& trackIndices1 = workVerticesContainer.at( pairIndex.first ).selectedTrackIndices();
  auto& trackIndices2 = workVerticesContainer.at( pairIndex.second ).selectedTrackIndices();

  if( trackIndices1.size() < 2 ) return 0;
  if( trackIndices2.size() < 2 ) return 0;

  size_t nTrkCom = 0;

  for( auto& index : trackIndices1 ) {
    if( std::find(trackIndices2.begin(),trackIndices2.end(), index) != trackIndices2.end()) nTrkCom++;
  }

  return nTrkCom;
}


StatusCode TrigVrtSecInclusive::cleanUp
( WrkVrtContainer& workVerticesContainer ) const
{
  //-Remove vertices fully contained in other vertices
  ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": Remove vertices fully contained in other vertices .");
  size_t n_vtx = workVerticesContainer.size();

  for (size_t iv = 0; iv < n_vtx; iv++) {
    for (size_t jv = iv+1; jv < n_vtx; jv++) {

      if ( !workVerticesContainer.at(iv).isGood ) continue;
      if ( !workVerticesContainer.at(jv).isGood ) continue;

      const auto nTCom = nTrkCommon( workVerticesContainer, {iv, jv} );

      if(      nTCom == workVerticesContainer.at(iv).selectedTrackIndices().size() ) {  workVerticesContainer.at(iv).isGood = false; continue; }
      else if( nTCom == workVerticesContainer.at(jv).selectedTrackIndices().size() ) {  workVerticesContainer.at(jv).isGood = false; continue; }

    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigVrtSecInclusive::finalize()
{
   return StatusCode::SUCCESS;
}

TrigVrtSecInclusive::~TrigVrtSecInclusive() {}

} // end of namespace TrigVSI
