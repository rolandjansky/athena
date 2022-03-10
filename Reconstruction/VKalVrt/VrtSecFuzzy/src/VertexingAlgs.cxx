/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecFuzzy/VrtSecFuzzy.h"

#include "TrkTrackSummary/TrackSummary.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"


#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"
#include "TLorentzVector.h"

#include <iostream>
#include <algorithm>
#include <array>

#include "TMVA/Reader.h"
#include "TMVA/MethodBDT.h"
#include "MVAUtils/BDT.h"

//-------------------------------------------------

using namespace std;

namespace VKalVrtAthena {


  //____________________________________________________________________________________________________
  StatusCode VrtSecFuzzy::form2trackVertices( std::vector<WrkVrt>* workVerticesContainer )
  {
    try {

    // Output SVs as xAOD::Vertex
    // Needs a conversion function from WrkVrtSet to xAOD::Vertex here.
    // The supposed form of the function will be as follows:
    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_jp.TrackLocation) );
  
    xAOD::VertexContainer *twoTrksVertexContainer( nullptr );
    if( m_jp.FillIntermediateVertices ) {
      ATH_CHECK( evtStore()->retrieve( twoTrksVertexContainer, "VrtSecFuzzy_" + m_jp.all2trksVerticesContainerName ) );
    }
    
    // Work variables
    std::vector<const xAOD::TrackParticle*>    baseTracks;
    std::vector<const xAOD::NeutralParticle*>  dummyNeutrals;
 
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Selected Tracks = "<< m_selectedTracks->size());
    
    const double maxR { 563. };         // r = 563 mm is the TRT inner surface
    const double roughD0Cut { 100. };
    const double roughZ0Cut { 50.  };
    
    unsigned int nfiles = m_bdt.size();

    // first, make 2-track vertices (seeds)
    int nSeedCand = 0;
    std::vector<const xAOD::TrackParticle*>  usedTracks; 
    std::vector<int>  usedTracks_id; 
    usedTracks.clear();
    usedTracks_id.clear();

    for( auto itrk = m_selectedTracks->begin(); itrk != m_selectedTracks->end(); ++itrk ) {
      for( auto jtrk = std::next(itrk); jtrk != m_selectedTracks->end(); ++jtrk ) {

        const int itrk_id = itrk - m_selectedTracks->begin();
        const int jtrk_id = jtrk - m_selectedTracks->begin();

        std::vector<float> wgtSelect;
        BDTSelection(*itrk, *jtrk, wgtSelect);
        bool storePair = false;
        for(unsigned int ifile = 0; ifile < nfiles; ifile ++){  
          if(wgtSelect.at(ifile) > m_jp.BDTMins_1stTry.at(ifile)){
            storePair = true;
          }
        }
        if(storePair == false) continue; //reject track pairs with low BDT score
        nSeedCand++;

        WrkVrt wrkvrt;
        wrkvrt.selectedTrackIndices.emplace_back( itrk_id );
        wrkvrt.selectedTrackIndices.emplace_back( jtrk_id );
        
        baseTracks.clear();
        baseTracks.emplace_back( *itrk );
        baseTracks.emplace_back( *jtrk );
        dummyNeutrals.clear(); 

        // new code to find initial approximate vertex
        Amg::Vector3D initVertex;

        StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex ); // Fast crude estimation 
        if( sc.isFailure() ) {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");
          continue;
        }
      
        if( initVertex.perp() > maxR ) {
          continue; //r position is out of the fiducial volume
        }

        std::vector<double> impactParameters;
        std::vector<double> impactParErrors;
        
        if( !getSVImpactParameters( *itrk, initVertex, impactParameters, impactParErrors) ){
          continue; // failed to get impact parameters with respect to initVertex
        }
        if( std::abs( impactParameters.at(0)) > roughD0Cut || std::abs( impactParameters.at(1) ) > roughZ0Cut ) {
          continue; // impact parameters with respect to initVertex are too large
        }

        if( !getSVImpactParameters( *jtrk, initVertex, impactParameters, impactParErrors) ){ 
          continue; // failed to get impact parameters with respect to initVertex
        }
        if( std::abs( impactParameters.at(0) ) > roughD0Cut || std::abs( impactParameters.at(1) ) > roughZ0Cut ) {
          continue; // impact parameters with respect to initVertex are too large
        }
       
        m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z() );
      
        // Vertex VKal Fitting
        sc = m_fitSvc->VKalVrtFit( baseTracks,
                                   dummyNeutrals,
                                   wrkvrt.vertex, wrkvrt.vertexMom, wrkvrt.Charge,
                                   wrkvrt.vertexCov, wrkvrt.Chi2PerTrk,
                                   wrkvrt.TrkAtVrt, wrkvrt.Chi2  );
      
        if( sc.isFailure() ) {
          continue;          // No fit  
        }

        Amg::Vector3D vDist = wrkvrt.vertex - m_thePV->position();
        const double vPos = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py()+vDist.z()*wrkvrt.vertexMom.Pz() )/wrkvrt.vertexMom.Rho();
        
        wrkvrt.BDTScores = wgtSelect;
        int maxBDTindex= std::max_element(wgtSelect.begin(), wgtSelect.end()) - wgtSelect.begin();
        wrkvrt.maxBDTScore = wgtSelect.at(maxBDTindex);
        TLorentzVector vi, vj;
        vi.SetPtEtaPhiM( (*itrk)->pt(), (*itrk)->eta(), (*itrk)->phi(), PhysConsts::mass_chargedPion);
        vj.SetPtEtaPhiM( (*jtrk)->pt(), (*jtrk)->eta(), (*jtrk)->phi(), PhysConsts::mass_chargedPion);
        wrkvrt.trackLorentzVectors.emplace_back( vi );
        wrkvrt.trackLorentzVectors.emplace_back( vj );
         
        /////////////////////////////

        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": attempting form vertex from ( " << itrk_id << ", " << jtrk_id << " )." );

        if(vertexSelection( wrkvrt, *itrk, *jtrk ) != 6) continue; //reject bad vertices

        if(m_jp.FillIntermediateVertices){
          xAOD::Vertex *vertex = new xAOD::Vertex;
          twoTrksVertexContainer->emplace_back( vertex );

          for( auto *trk: baseTracks ) {

            // Acquire link to the track
            ElementLink<xAOD::TrackParticleContainer>  trackElementLink( *( dynamic_cast<const xAOD::TrackParticleContainer*>( trk->container() ) ), trk->index() );

            // Register link to the vertex
            vertex->addTrackAtVertex( trackElementLink, 1. );
          }

          vertex->setVertexType( xAOD::VxType::SecVtx );
          vertex->setPosition( wrkvrt.vertex );
          vertex->setFitQuality( wrkvrt.Chi2, 1 ); // Ndof is always 1
          vertex->auxdata<float>("mass")   = wrkvrt.vertexMom.M();
          vertex->auxdata<float>("pT")     = wrkvrt.vertexMom.Perp();
          vertex->auxdata<float>("charge") = wrkvrt.Charge;
          vertex->auxdata<float>("vPos")   = vPos;
          vertex->auxdata<std::vector<float> >("BDTs") = wgtSelect;
          vertex->auxdata<int>("trk1_id") = wrkvrt.selectedTrackIndices.at(0); 
          vertex->auxdata<int>("trk2_id") = wrkvrt.selectedTrackIndices.at(1); 
        }
        
        // Now this vertex passed all criteria and considred to be a compatible vertices.
        wrkvrt.isGood = true;
        
        workVerticesContainer->emplace_back( wrkvrt );

        // Store the tracks which consist of seeds
        if(std::find( usedTracks.begin(), usedTracks.end(), *itrk ) == usedTracks.end()){
          usedTracks.emplace_back( *itrk ); 
          usedTracks_id.emplace_back( itrk_id ); 
        }
        if(std::find( usedTracks.begin(), usedTracks.end(), *jtrk ) == usedTracks.end()){
          usedTracks.emplace_back( *jtrk ); 
          usedTracks_id.emplace_back( jtrk_id ); 
        }
      }
    }
    ATH_MSG_INFO("execute(): # seed candidates after first trial = " << nSeedCand);
    
    ATH_MSG_INFO("execute(): # used tracks = " << usedTracks.size());
    std::vector<const xAOD::TrackParticle*>  unusedTracks; 
    std::vector<int>  unusedTracks_id; 
    unusedTracks.clear();
    unusedTracks_id.clear();

    for( auto& targetVertex : *workVerticesContainer ) {
      for( auto itrk = m_selectedTracks->begin(); itrk != m_selectedTracks->end(); ++itrk ) {
        const int itrk_id = itrk - m_selectedTracks->begin();
 
        if(std::find( usedTracks.begin(), usedTracks.end(), *itrk ) != usedTracks.end()) continue; // this track is already used for seeds
 
        // Get the closest approach
        std::vector<double> impactParameters;
        std::vector<double> impactParErrors;
        if( !getSVImpactParameters( *itrk, targetVertex.vertex, impactParameters, impactParErrors) ) continue;
        if( std::abs( impactParameters.at(0) ) > 10.0 || std::abs( impactParameters.at(1) ) > 10.0 ) continue; //select the tracks around seeds 
        if(std::find( unusedTracks.begin(), unusedTracks.end(), *itrk ) == unusedTracks.end()){
          unusedTracks.emplace_back( *itrk ); 
          unusedTracks_id.emplace_back( itrk_id );
        }
      }      
    }

    ATH_MSG_INFO("execute(): # selected tracks around seeds = " << unusedTracks.size());
    
    // second trial, 1 used track + 1 unused track around seeds
    for( auto itrk = usedTracks.begin(); itrk != usedTracks.end(); ++itrk ) {
      for( auto jtrk = unusedTracks.begin(); jtrk != unusedTracks.end(); ++jtrk ) {
        const int itrk_id = itrk - usedTracks.begin();
        const int jtrk_id = jtrk - unusedTracks.begin();

        std::vector<float> wgtSelect;
        BDTSelection(*itrk, *jtrk, wgtSelect);
        bool storePair = false;
        for(unsigned int ifile = 0; ifile < nfiles; ifile ++){  
          if(wgtSelect.at(ifile) > m_jp.BDTMins_2ndTry.at(ifile)){ //lower threshold than 1st trial
            storePair = true;
          }
        }
        if(storePair == false) continue;
        nSeedCand++;
        
        WrkVrt wrkvrt;
        wrkvrt.selectedTrackIndices.emplace_back( usedTracks_id.at(itrk_id) );
        wrkvrt.selectedTrackIndices.emplace_back( unusedTracks_id.at(jtrk_id) );
        std::sort(wrkvrt.selectedTrackIndices.begin(), wrkvrt.selectedTrackIndices.end()); // track1's ID is always smaller than track2's

        baseTracks.clear();
        baseTracks.emplace_back( *itrk );
        baseTracks.emplace_back( *jtrk );
        dummyNeutrals.clear(); 
      
        // new code to find initial approximate vertex
        Amg::Vector3D initVertex;

        StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex ); // Fast crude estimation 
        if( sc.isFailure() ) {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");
          continue;
        }
      
        if( initVertex.perp() > maxR ) {
          continue; //r position is out of the fiducial volume
        }

        std::vector<double> impactParameters;
        std::vector<double> impactParErrors;
        
        if( !getSVImpactParameters( *itrk, initVertex, impactParameters, impactParErrors) ){
          continue; // failed to get impact parameters with respect to initVertex
        }
        if( std::abs( impactParameters.at(0)) > roughD0Cut || std::abs( impactParameters.at(1) ) > roughZ0Cut ) {
          continue; // impact parameters with respect to initVertex are too large
        }

        if( !getSVImpactParameters( *jtrk, initVertex, impactParameters, impactParErrors) ){ 
          continue; // failed to get impact parameters with respect to initVertex
        }
        if( std::abs( impactParameters.at(0) ) > roughD0Cut || std::abs( impactParameters.at(1) ) > roughZ0Cut ) {
          continue; // impact parameters with respect to initVertex are too large
        }
       
        m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z() );
      
        // Vertex VKal Fitting
        sc = m_fitSvc->VKalVrtFit( baseTracks,
                                   dummyNeutrals,
                                   wrkvrt.vertex, wrkvrt.vertexMom, wrkvrt.Charge,
                                   wrkvrt.vertexCov, wrkvrt.Chi2PerTrk,
                                   wrkvrt.TrkAtVrt, wrkvrt.Chi2  );
      
        if( sc.isFailure() ) {
          continue;          // No fit  
        }

        Amg::Vector3D vDist = wrkvrt.vertex - m_thePV->position();
        const double vPos = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py()+vDist.z()*wrkvrt.vertexMom.Pz() )/wrkvrt.vertexMom.Rho();
      
        wrkvrt.BDTScores = wgtSelect;
        int maxBDTindex= std::max_element(wgtSelect.begin(), wgtSelect.end()) - wgtSelect.begin();
        wrkvrt.maxBDTScore = wgtSelect.at(maxBDTindex);
        TLorentzVector vi, vj;
        vi.SetPtEtaPhiM( (*itrk)->pt(), (*itrk)->eta(), (*itrk)->phi(), PhysConsts::mass_chargedPion);
        vj.SetPtEtaPhiM( (*jtrk)->pt(), (*jtrk)->eta(), (*jtrk)->phi(), PhysConsts::mass_chargedPion);
        wrkvrt.trackLorentzVectors.emplace_back( vi );
        wrkvrt.trackLorentzVectors.emplace_back( vj );
 
        /////////////////////////////
      
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": attempting form vertex from ( " << itrk_id << ", " << jtrk_id << " )." );

        if(vertexSelection( wrkvrt, *itrk, *jtrk ) != 6) continue; //bad vertex
        
        if(m_jp.FillIntermediateVertices){
          xAOD::Vertex *vertex = new xAOD::Vertex;  
          twoTrksVertexContainer->emplace_back( vertex );

          for( auto *trk: baseTracks ) {

            // Acquire link to the track
            ElementLink<xAOD::TrackParticleContainer>  trackElementLink( *( dynamic_cast<const xAOD::TrackParticleContainer*>( trk->container() ) ), trk->index() );

            // Register link to the vertex
            vertex->addTrackAtVertex( trackElementLink, 1. );
          }
          vertex->setVertexType( xAOD::VxType::SecVtx );
          vertex->setPosition( wrkvrt.vertex );
          vertex->setFitQuality( wrkvrt.Chi2, 1 ); // Ndof is always 1
          vertex->auxdata<float>("mass")   = wrkvrt.vertexMom.M();
          vertex->auxdata<float>("pT")     = wrkvrt.vertexMom.Perp();
          vertex->auxdata<float>("charge") = wrkvrt.Charge;
          vertex->auxdata<float>("vPos")   = vPos;
          vertex->auxdata<std::vector<float> >("BDTs") = wgtSelect;
          vertex->auxdata<int>("trk1_id") = wrkvrt.selectedTrackIndices.at(0); 
          vertex->auxdata<int>("trk2_id") = wrkvrt.selectedTrackIndices.at(1); 
        }

        // Now this vertex passed all criteria and considred to be a compatible vertices.
        wrkvrt.isGood = true;
        workVerticesContainer->emplace_back( wrkvrt );

      }
    }
    ATH_MSG_INFO("execute(): # seed candidates after second trial = " << nSeedCand);
    
    // third trial, 2 unused tracks around seeds
    for( auto itrk = unusedTracks.begin(); itrk != unusedTracks.end(); ++itrk ) {
      for( auto jtrk = std::next(itrk); jtrk != unusedTracks.end(); ++jtrk ) {
        
        const int itrk_id = itrk - unusedTracks.begin();
        const int jtrk_id = jtrk - unusedTracks.begin();

        std::vector<float> wgtSelect;
        BDTSelection(*itrk, *jtrk, wgtSelect);
        bool storePair = false;
        for(unsigned int ifile = 0; ifile < nfiles; ifile ++){  
          if(wgtSelect.at(ifile) > m_jp.BDTMins_2ndTry.at(ifile)){ //same threshold as 2nd trial
            storePair = true;
          }
        }
        if(storePair == false) continue;
        nSeedCand++;
     
        WrkVrt wrkvrt;
        wrkvrt.selectedTrackIndices.emplace_back( unusedTracks_id.at(itrk_id) );
        wrkvrt.selectedTrackIndices.emplace_back( unusedTracks_id.at(jtrk_id) );
        std::sort(wrkvrt.selectedTrackIndices.begin(), wrkvrt.selectedTrackIndices.end());
        
        baseTracks.clear();
        baseTracks.emplace_back( *itrk );
        baseTracks.emplace_back( *jtrk );
        dummyNeutrals.clear(); 
      
        // new code to find initial approximate vertex
        Amg::Vector3D initVertex;

        StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex ); // Fast crude estimation 
        if( sc.isFailure() ) {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");
          continue;
        }
      
        if( initVertex.perp() > maxR ) {
          continue; //r position is out of the fiducial volume
        }

        std::vector<double> impactParameters;
        std::vector<double> impactParErrors;
        
        if( !getSVImpactParameters( *itrk, initVertex, impactParameters, impactParErrors) ){
          continue; // failed to get impact parameters with respect to initVertex
        }
        if( std::abs( impactParameters.at(0)) > roughD0Cut || std::abs( impactParameters.at(1) ) > roughZ0Cut ) {
          continue; // impact parameters with respect to initVertex are too large
        }

        if( !getSVImpactParameters( *jtrk, initVertex, impactParameters, impactParErrors) ){ 
          continue; // failed to get impact parameters with respect to initVertex
        }
        if( std::abs( impactParameters.at(0) ) > roughD0Cut || std::abs( impactParameters.at(1) ) > roughZ0Cut ) {
          continue; // impact parameters with respect to initVertex are too large
        }
       
        m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z() );
      
        // Vertex VKal Fitting
        sc = m_fitSvc->VKalVrtFit( baseTracks,
                                   dummyNeutrals,
                                   wrkvrt.vertex, wrkvrt.vertexMom, wrkvrt.Charge,
                                   wrkvrt.vertexCov, wrkvrt.Chi2PerTrk,
                                   wrkvrt.TrkAtVrt, wrkvrt.Chi2  );
      
        if( sc.isFailure() ) {
          continue;          // No fit  
        }

        Amg::Vector3D vDist = wrkvrt.vertex - m_thePV->position();
        const double vPos = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py()+vDist.z()*wrkvrt.vertexMom.Pz() )/wrkvrt.vertexMom.Rho();
      
        wrkvrt.BDTScores = wgtSelect;
        int maxBDTindex= std::max_element(wgtSelect.begin(), wgtSelect.end()) - wgtSelect.begin();
        wrkvrt.maxBDTScore = wgtSelect.at(maxBDTindex);
        TLorentzVector vi, vj;
        vi.SetPtEtaPhiM( (*itrk)->pt(), (*itrk)->eta(), (*itrk)->phi(), PhysConsts::mass_chargedPion);
        vj.SetPtEtaPhiM( (*jtrk)->pt(), (*jtrk)->eta(), (*jtrk)->phi(), PhysConsts::mass_chargedPion);
        wrkvrt.trackLorentzVectors.emplace_back( vi );
        wrkvrt.trackLorentzVectors.emplace_back( vj );
 
        /////////////////////////////
     
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": attempting form vertex from ( " << itrk_id << ", " << jtrk_id << " )." );

        if(vertexSelection( wrkvrt, *itrk, *jtrk ) != 6) continue; //bad vertex
        
        if(m_jp.FillIntermediateVertices){
          xAOD::Vertex *vertex = new xAOD::Vertex;  
          twoTrksVertexContainer->emplace_back( vertex );

          for( auto *trk: baseTracks ) {

            // Acquire link to the track
            ElementLink<xAOD::TrackParticleContainer>  trackElementLink( *( dynamic_cast<const xAOD::TrackParticleContainer*>( trk->container() ) ), trk->index() );

            // Register link to the vertex
            vertex->addTrackAtVertex( trackElementLink, 1. );
          }

          vertex->setVertexType( xAOD::VxType::SecVtx );
          vertex->setPosition( wrkvrt.vertex );
          vertex->setFitQuality( wrkvrt.Chi2, 1 ); // Ndof is always 1
          vertex->auxdata<float>("mass")   = wrkvrt.vertexMom.M();
          vertex->auxdata<float>("pT")     = wrkvrt.vertexMom.Perp();
          vertex->auxdata<float>("charge") = wrkvrt.Charge;
          vertex->auxdata<float>("vPos")   = vPos;
          vertex->auxdata<std::vector<float> >("BDTs") = wgtSelect;
          vertex->auxdata<int>("trk1_id") = wrkvrt.selectedTrackIndices.at(0); 
          vertex->auxdata<int>("trk2_id") = wrkvrt.selectedTrackIndices.at(1); 
        }

        // Now this vertex passed all criteria and considred to be a compatible vertices.
        wrkvrt.isGood = true;
        workVerticesContainer->emplace_back( wrkvrt );
  
      }
    }
    ATH_MSG_INFO("execute(): # seed candidates after third trial = " << nSeedCand);

    }catch ( bad_alloc ) {
      ATH_MSG_WARNING( " bad_alloc error is detected in form2trackVertices " );
      return StatusCode::FAILURE; 
    }

    return StatusCode::SUCCESS;
  }

  StatusCode VrtSecFuzzy::merge2trackVertices( std::vector<WrkVrt>* workVerticesContainer )
  {
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": begin");
    ATH_MSG_INFO("execute(): # wrk vertices = " << workVerticesContainer->size());
    
    xAOD::VertexContainer *secondaryVertexContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( secondaryVertexContainer, "VrtSecFuzzy_" + m_jp.secondaryVerticesContainerName ) ); // DV's container
    xAOD::VertexContainer *twoTrksVertexInDVContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( twoTrksVertexInDVContainer, "VrtSecFuzzy_" + m_jp.twoTrksVerticesInDVContainerName ) ); // 2-track seed vertices in DVs
    
    enum { kPt, kEta, kPhi, kD0, kZ0, kErrP, kErrD0, kErrZ0 };
    if( m_trkDecors.empty() ) {
      m_trkDecors.emplace( kPt,     SG::AuxElement::Decorator<float>("pt_wrtSV_fuzzy"    ) );
      m_trkDecors.emplace( kEta,    SG::AuxElement::Decorator<float>("eta_wrtSV_fuzzy"   ) ); 
      m_trkDecors.emplace( kPhi,    SG::AuxElement::Decorator<float>("phi_wrtSV_fuzzy"   ) );
      m_trkDecors.emplace( kD0,     SG::AuxElement::Decorator<float>("d0_wrtSV_fuzzy"    ) );
      m_trkDecors.emplace( kZ0,     SG::AuxElement::Decorator<float>("z0_wrtSV_fuzzy"    ) );
      m_trkDecors.emplace( kErrP,   SG::AuxElement::Decorator<float>("errP_wrtSV_fuzzy"  ) );
      m_trkDecors.emplace( kErrD0,  SG::AuxElement::Decorator<float>("errd0_wrtSV_fuzzy" ) );
      m_trkDecors.emplace( kErrZ0,  SG::AuxElement::Decorator<float>("errz0_wrtSV_fuzzy" ) );
    }
    if( !m_decor_is_svtrk_final ) m_decor_is_svtrk_final = std::make_unique< SG::AuxElement::Decorator<char> >( "is_svtrk_final_fuzzy" );

    unsigned int nfiles = m_bdt.size();
    const double rMin = 4.0; 

    std::vector<WrkVrt> primarySeeds;
    primarySeeds.clear();
    std::vector<WrkVrt> allSeeds;
    allSeeds.clear();
    std::vector<std::deque<long int> > allSeedTrackIDs;
    allSeedTrackIDs.clear();

    for( auto& wrkvrt : *workVerticesContainer ) {
      if(wrkvrt.isGood == false) continue;
      if(wrkvrt.vertex.perp() < rMin) continue; // too close to the beam line
      if(std::find(allSeedTrackIDs.begin(), allSeedTrackIDs.end(), wrkvrt.selectedTrackIndices) != allSeedTrackIDs.end()){
        ATH_MSG_INFO("multiple seeds by the same pair ?, trk pair: " << wrkvrt.selectedTrackIndices.at(0) << ", " << wrkvrt.selectedTrackIndices.at(1));
        continue;
      }
      allSeeds.emplace_back(wrkvrt);
      allSeedTrackIDs.emplace_back(wrkvrt.selectedTrackIndices);
      
      // select primary seeds
      bool isPrimary = false;
      for(unsigned int ifile = 0; ifile < nfiles; ifile ++){  
        if(wrkvrt.BDTScores.at(ifile) > m_jp.BDTMins_primary.at(ifile)) isPrimary = true;
      }
      if(isPrimary == true) primarySeeds.emplace_back(wrkvrt);
    }
    int nprimary = primarySeeds.size();
    ATH_MSG_INFO("# primary seeds = " << nprimary);
    std::sort(primarySeeds.begin(), primarySeeds.end(), sortSeed_wBDT{});
      
    workVerticesContainer->clear();

    std::vector<std::vector<WrkVrt> > clusters;
    clusters.clear();
    std::vector<std::vector<std::deque<long int> > > clusters_trackIDs;
    clusters_trackIDs.clear();

    // merge the primary seeds and their sorrounding seeds for all primary seeds
    for(auto primseed : primarySeeds) {
      
      // four categories based on the primary seed position, different merging condition for different categories
      int regionID = -999;
      if(primseed.vertex.perp() < 23.5){ // inBP
        regionID = 0;
      }else if(primseed.vertex.perp() < 33.5){ // inIBL
        regionID = 1;
      }else if(primseed.vertex.perp() < 122.5){ // inPixel
        regionID = 2;
      }else{ // outPixel
        regionID = 3;
      }
      double sumPt_i = primseed.trackLorentzVectors.at(0).Pt() + primseed.trackLorentzVectors.at(1).Pt();
      std::vector<WrkVrt> mergedSeeds;
      mergedSeeds.clear();
      mergedSeeds.emplace_back(primseed);
      std::vector<std::deque<long int> > trackIDs;
      trackIDs.clear();
      trackIDs.emplace_back(primseed.selectedTrackIndices);

      for(auto seed : allSeeds ){
        
        if(seed.selectedTrackIndices == primseed.selectedTrackIndices) continue; //itself
        
        double sumPt_j = seed.trackLorentzVectors.at(0).Pt() + seed.trackLorentzVectors.at(1).Pt();
        double dx = std::abs(primseed.vertex.x() - seed.vertex.x());
        double dy = std::abs(primseed.vertex.y() - seed.vertex.y());
        double dz = std::abs(primseed.vertex.z() - seed.vertex.z());
        std::vector<double> angles;
        angles.clear();
        angles.emplace_back(primseed.trackLorentzVectors.at(0).Angle(seed.trackLorentzVectors.at(0).Vect()));
        angles.emplace_back(primseed.trackLorentzVectors.at(0).Angle(seed.trackLorentzVectors.at(1).Vect()));
        angles.emplace_back(primseed.trackLorentzVectors.at(1).Angle(seed.trackLorentzVectors.at(0).Vect()));
        angles.emplace_back(primseed.trackLorentzVectors.at(1).Angle(seed.trackLorentzVectors.at(1).Vect()));
        int maxAngle_id = std::max_element(angles.begin(), angles.end()) - angles.begin();
        double maxAngle = angles.at(maxAngle_id);

        // merge if the sorrounding seed satisfies the conditions
        if(dx < m_jp.SeedDist.at(regionID) && dy < m_jp.SeedDist.at(regionID) && dz < m_jp.SeedDist.at(regionID)){  
          // additional cut
          if((sumPt_i + sumPt_j)*0.001 < 3.0*(maxAngle - M_PI/2) + 4.0) continue;
     
          mergedSeeds.emplace_back(seed);
          trackIDs.emplace_back(seed.selectedTrackIndices);
        }
      }
      if(mergedSeeds.size() < 2) continue; // reject single primary seeds (not merged ones)
      clusters.emplace_back( mergedSeeds );
      clusters_trackIDs.emplace_back( trackIDs );
    }

    unsigned int nclus_beforeGroupMerge = clusters.size(); 
    ATH_MSG_DEBUG("# seed groups before seed group merging = " << nclus_beforeGroupMerge);

    // merge the clusters close to each other
    for(unsigned int iclus = 0; iclus < nclus_beforeGroupMerge; ++iclus){     
      
      if(clusters.at(iclus).empty()) continue; // have cleared
      ATH_MSG_DEBUG("# seeds in clus " << iclus << " = " << clusters.at(iclus).size());
      for(unsigned int jclus = iclus + 1; jclus < nclus_beforeGroupMerge; ++jclus){
      
        if(clusters.at(jclus).empty()) continue; // have cleared
        bool has_overlapping = false;

        for(auto seed_trackIDs : clusters_trackIDs.at(jclus)){
          if(std::find(clusters_trackIDs.at(iclus).begin(), clusters_trackIDs.at(iclus).end(), seed_trackIDs) != clusters_trackIDs.at(iclus).end()){
            has_overlapping = true; // there is the same seed which is shared by multiple clusters -> merge
            break;
          }
        }
       
        // merge the clusters
        if(has_overlapping == true){
          ATH_MSG_DEBUG("merge clus " << iclus << " and " << jclus);
          
          for(auto seed_trackIDs : clusters_trackIDs.at(jclus)){
            if(std::find(clusters_trackIDs.at(iclus).begin(), clusters_trackIDs.at(iclus).end(), seed_trackIDs) == clusters_trackIDs.at(iclus).end()){ 
              auto itr = std::find(clusters_trackIDs.at(jclus).begin(), clusters_trackIDs.at(jclus).end(), seed_trackIDs);
              int seedID = itr - clusters_trackIDs.at(jclus).begin();
              clusters.at(iclus).emplace_back(clusters.at(jclus).at(seedID)); //add clus_j's seed into clus_i
              clusters_trackIDs.at(iclus).emplace_back(seed_trackIDs);
            }
          }

          ATH_MSG_DEBUG("after merging, # seeds = " << clusters.at(iclus).size()); //cotaining multiple same seeds
          clusters.at(jclus).clear(); //clear clus_j
        }
      }
    }

    unsigned int nclus_afterGroupMerge = 0;
    for(auto icluster : clusters){
      int n_seeds = icluster.size();
      if(n_seeds < 2) continue; //cleared cluster
      nclus_afterGroupMerge++;

      // store the seed information in the DV
      std::vector<const xAOD::Vertex*> baseSeeds;
      baseSeeds.clear();
      for(auto jseed : icluster){
        xAOD::Vertex *seed_inDV = new xAOD::Vertex;  
        twoTrksVertexInDVContainer->emplace_back( seed_inDV );
        seed_inDV->setVertexType( xAOD::VxType::SecVtx );
        seed_inDV->setPosition( jseed.vertex );
        seed_inDV->setFitQuality( jseed.Chi2, 1 ); // Ndof is always 1
        seed_inDV->auxdata<float>("mass")   = jseed.vertexMom.M();
        seed_inDV->auxdata<float>("pT")     = jseed.vertexMom.Perp();
        seed_inDV->auxdata<float>("charge") = jseed.Charge;
        seed_inDV->auxdata<int>("trk1_id") = jseed.selectedTrackIndices.at(0); 
        seed_inDV->auxdata<int>("trk2_id") = jseed.selectedTrackIndices.at(1); 
        seed_inDV->auxdata<std::vector<float> >("BDTs") = jseed.BDTScores;
        baseSeeds.emplace_back(seed_inDV); 
        
        // Acquire link to the track
        ElementLink<xAOD::TrackParticleContainer>  trackElementLink1( *( dynamic_cast<const xAOD::TrackParticleContainer*>( m_selectedTracks->at(jseed.selectedTrackIndices.at(0))->container() ) ), m_selectedTracks->at(jseed.selectedTrackIndices.at(0))->index() );
        ElementLink<xAOD::TrackParticleContainer>  trackElementLink2( *( dynamic_cast<const xAOD::TrackParticleContainer*>( m_selectedTracks->at(jseed.selectedTrackIndices.at(1))->container() ) ), m_selectedTracks->at(jseed.selectedTrackIndices.at(1))->index() );
        // Register link to the vertex
        seed_inDV->addTrackAtVertex( trackElementLink1, 1. );
        seed_inDV->addTrackAtVertex( trackElementLink2, 1. );
      }

      // store DV
      xAOD::Vertex *vertex = new xAOD::Vertex;  
      secondaryVertexContainer->emplace_back( vertex );
      double aveX = 0.0;
      double aveY = 0.0;
      double aveZ = 0.0;
      double varX = 0.0;
      double varY = 0.0;
      double varZ = 0.0;
      double covXY = 0.0;
      double covYZ = 0.0;
      double covZX = 0.0;
      float maxBDT_inClus = -999;
      int n_seeds_highScore = 0;
      std::vector<const xAOD::TrackParticle*> baseTracks;
      baseTracks.clear();
      for(auto jseed : icluster){
        
        // store associated tracks in baseTracks
        if(std::find( baseTracks.begin(), baseTracks.end(), m_selectedTracks->at(jseed.selectedTrackIndices.at(0))) == baseTracks.end()){ 
          baseTracks.emplace_back( m_selectedTracks->at(jseed.selectedTrackIndices.at(0)) ); 
        }
        if(std::find( baseTracks.begin(), baseTracks.end(), m_selectedTracks->at(jseed.selectedTrackIndices.at(1))) == baseTracks.end()){ 
          baseTracks.emplace_back( m_selectedTracks->at(jseed.selectedTrackIndices.at(1)) ); 
        }
        aveX = aveX + jseed.vertex.x();
        aveY = aveY + jseed.vertex.y();
        aveZ = aveZ + jseed.vertex.z();
        if(maxBDT_inClus < jseed.maxBDTScore) maxBDT_inClus = jseed.maxBDTScore;
        if(jseed.maxBDTScore > 0.15) n_seeds_highScore = n_seeds_highScore + 1; 
      }

      // average x, y, z -> vertex position
      aveX = aveX/n_seeds;
      aveY = aveY/n_seeds;
      aveZ = aveZ/n_seeds;

      for(auto jseed : icluster){
        varX = varX +  pow(jseed.vertex.x() - aveX, 2);
        varY = varY +  pow(jseed.vertex.y() - aveY, 2);
        varZ = varZ +  pow(jseed.vertex.z() - aveZ, 2);
        covXY = covXY + (jseed.vertex.x() - aveX)*(jseed.vertex.y() - aveY);
        covYZ = covYZ + (jseed.vertex.y() - aveY)*(jseed.vertex.z() - aveZ);
        covZX = covZX + (jseed.vertex.z() - aveZ)*(jseed.vertex.x() - aveX);
      }

      // veriance x, y, z 
      varX = varX/(n_seeds - 1);
      varY = varY/(n_seeds - 1);
      varZ = varZ/(n_seeds - 1);
      covXY = covXY/(n_seeds - 1);
      covYZ = covYZ/(n_seeds - 1);
      covZX = covZX/(n_seeds - 1);

      WrkVrt wrkvrt;
      wrkvrt.isGood = true;
      wrkvrt.selectedTrackIndices.clear();
      wrkvrt.vertex.x() = aveX;
      wrkvrt.vertex.y() = aveY;
      wrkvrt.vertex.z() = aveZ;
      wrkvrt.ntrks = baseTracks.size();
      
      vertex->setVertexType( xAOD::VxType::SecVtx );
      vertex->setPosition( wrkvrt.vertex );
      vertex->auxdata<int>("ntrks") = baseTracks.size();
      vertex->auxdata<double>("aveX") = aveX;
      vertex->auxdata<double>("aveY") = aveY;
      vertex->auxdata<double>("aveZ") = aveZ;
      vertex->auxdata<double>("varX") = varX;
      vertex->auxdata<double>("varY") = varY;
      vertex->auxdata<double>("varZ") = varZ;
      vertex->auxdata<double>("covXY") = covXY;
      vertex->auxdata<double>("covYZ") = covYZ;
      vertex->auxdata<double>("covZX") = covZX;
      vertex->auxdata<int>("nseeds") = n_seeds;
      vertex->auxdata<float>("maxBDT") = maxBDT_inClus;
      vertex->auxdata<int>("nseeds_highScore") = n_seeds_highScore;

      std::vector<ElementLink<xAOD::VertexContainer> > vtxElementLinks;
      vtxElementLinks.clear();
      for( auto *seedvtx: baseSeeds ){
        
        // Acquire link to the seed vertices
        ElementLink<xAOD::VertexContainer>  vtxElementLink( *( dynamic_cast<const xAOD::VertexContainer*>( seedvtx->container() ) ), seedvtx->index() );
        vtxElementLinks.emplace_back(vtxElementLink);

      }
      // Register link to the vertex
      vertex->auxdata<std::vector<ElementLink<xAOD::VertexContainer>> >("link_vtxVertices" ) = vtxElementLinks;

      for( auto *trk: baseTracks ) {

        // Acquire link to the track
        ElementLink<xAOD::TrackParticleContainer>  trackElementLink( *( dynamic_cast<const xAOD::TrackParticleContainer*>( trk->container() ) ), trk->index() );

        // Register link to the vertex
        vertex->addTrackAtVertex( trackElementLink, 1. );
      
        // obtain track parameters wrt SV
        double qOverP_wrtSV;
        double theta_wrtSV; 
        double p_wrtSV;     
        double pt_wrtSV;    
        double eta_wrtSV;   
        double phi_wrtSV;   
        double d0_wrtSV;    
        double z0_wrtSV;    
        double errd0_wrtSV; 
        double errz0_wrtSV; 
        double errP_wrtSV;  
        const auto* sv_perigee = m_trackToVertexTool->perigeeAtVertex( *trk, wrkvrt.vertex );
        if( !sv_perigee ) {
          ATH_MSG_INFO(" > " << __FUNCTION__ << ": > Track index " << trk->index() << ": Failed in obtaining the SV perigee!" );
          qOverP_wrtSV = AlgConsts::invalidFloat;
          theta_wrtSV = AlgConsts::invalidFloat; 
          p_wrtSV = AlgConsts::invalidFloat;     
          pt_wrtSV = AlgConsts::invalidFloat;    
          eta_wrtSV = AlgConsts::invalidFloat;   
          phi_wrtSV = AlgConsts::invalidFloat;   
          d0_wrtSV = AlgConsts::invalidFloat;    
          z0_wrtSV = AlgConsts::invalidFloat;    
          errd0_wrtSV = AlgConsts::invalidFloat; 
          errz0_wrtSV = AlgConsts::invalidFloat; 
          errP_wrtSV = AlgConsts::invalidFloat;  
          (*m_decor_is_svtrk_final)( *trk ) = true;
          continue;
        }
        qOverP_wrtSV    = sv_perigee->parameters() [Trk::qOverP];
        theta_wrtSV     = sv_perigee->parameters() [Trk::theta];
        p_wrtSV         = 1.0 / std::abs( qOverP_wrtSV );
        pt_wrtSV        = p_wrtSV * sin( theta_wrtSV );
        eta_wrtSV       = -log( tan( theta_wrtSV/2. ) );
        phi_wrtSV       = sv_perigee->parameters() [Trk::phi];
        d0_wrtSV        = sv_perigee->parameters() [Trk::d0];
        z0_wrtSV        = sv_perigee->parameters() [Trk::z0];
        errd0_wrtSV     = (*sv_perigee->covariance())( Trk::d0, Trk::d0 );
        errz0_wrtSV     = (*sv_perigee->covariance())( Trk::z0, Trk::z0 );
        errP_wrtSV      = (*sv_perigee->covariance())( Trk::qOverP, Trk::qOverP );
        
        // xAOD::Track augmentation
        ( m_trkDecors.at(kPt)    )( *trk ) = pt_wrtSV;
        ( m_trkDecors.at(kEta)   )( *trk ) = eta_wrtSV;
        ( m_trkDecors.at(kPhi)   )( *trk ) = phi_wrtSV;
        ( m_trkDecors.at(kD0)    )( *trk ) = d0_wrtSV;
        ( m_trkDecors.at(kZ0)    )( *trk ) = z0_wrtSV;
        ( m_trkDecors.at(kErrP)  )( *trk ) = errP_wrtSV;
        ( m_trkDecors.at(kErrD0) )( *trk ) = errd0_wrtSV;
        ( m_trkDecors.at(kErrZ0) )( *trk ) = errz0_wrtSV;
        
        (*m_decor_is_svtrk_final)( *trk ) = true;
 
        delete sv_perigee;
 
      }

      if( m_jp.doTruth ) {
        ATH_CHECK( categorizeVertexTruthTopology( vertex ) );
      }
 
      ATH_MSG_DEBUG("# tracks in DV = " << baseTracks.size());
      ATH_MSG_DEBUG("# seeds in DV = " << n_seeds);
    }
    ATH_MSG_INFO("# seed groups after seed group merging = " << nclus_afterGroupMerge);
    
    return StatusCode::SUCCESS;
  }
 
  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::getSVImpactParameters(const xAOD::TrackParticle* trk, Amg::Vector3D vertex,
                                              std::vector<double>& impactParameters,
                                              std::vector<double>& impactParErrors){

    impactParameters.clear();
    impactParErrors.clear();
    
    if( m_jp.trkExtrapolator==1 ){
      m_fitSvc->VKalGetImpact(trk, vertex, static_cast<int>( trk->charge() ), impactParameters, impactParErrors);
    }
    else if( m_jp.trkExtrapolator==2 ){
      const Trk::Perigee* sv_perigee = m_trackToVertexTool->perigeeAtVertex( *trk, vertex );
      if( !sv_perigee ) return false;
      impactParameters.push_back(sv_perigee->parameters() [Trk::d0]);
      impactParameters.push_back(sv_perigee->parameters() [Trk::z0]);
      impactParErrors.push_back((*sv_perigee->covariance())( Trk::d0, Trk::d0 ));
      impactParErrors.push_back((*sv_perigee->covariance())( Trk::z0, Trk::z0 ));
      delete sv_perigee;
    }
    else{
      ATH_MSG_WARNING( " > " << __FUNCTION__ << ": Unknown track extrapolator " << m_jp.trkExtrapolator   );
      return false;
    }

    return true;

  } // getSVImpactParameters

  float VrtSecFuzzy::getFirstHitCategory(const xAOD::TrackParticle* trk){
    uint8_t IBLHits, BLayHits, PixelHits, SCTHits;
    trk->summaryValue( IBLHits, xAOD::numberOfInnermostPixelLayerHits );
    trk->summaryValue( BLayHits, xAOD::numberOfNextToInnermostPixelLayerHits );
    trk->summaryValue( PixelHits, xAOD::numberOfPixelHits );
    trk->summaryValue( SCTHits, xAOD::numberOfSCTHits );
    if(IBLHits > 0){
      return 1;
    }else if(BLayHits > 0){
      return 2;
    }else if(PixelHits > 0){
      return 3;
    }else if(SCTHits > 0){
      return 4;
    }else if(PixelHits == 0 && SCTHits == 0){
      return 0;
    }else{
      return -999;
    }
  }
 
  void VrtSecFuzzy::BDTSelection(const xAOD::TrackParticle* itrk, const xAOD::TrackParticle* jtrk, std::vector<float>& wgtSelect){
    wgtSelect.clear();
    float eta_i = itrk->eta();
    float eta_j = jtrk->eta();
    float dEta = std::abs(eta_i - eta_j);
    float dPhi1 = std::abs(itrk->phi() - jtrk->phi());
    float dPhi2 = std::abs(2.0*M_PI - dPhi1);
    float dPhi = min(dPhi1, dPhi2);
    float AsymPt = (itrk->pt() - jtrk->pt())/(itrk->pt() + jtrk->pt());
    float d0_i = itrk->d0();
    float d0_j = jtrk->d0();
    float d0sig_i = d0_i/sqrt(itrk->definingParametersCovMatrix()(0,0));
    float d0sig_j = d0_j/sqrt(jtrk->definingParametersCovMatrix()(0,0));
    float z0_i = itrk->z0();
    float z0_j = jtrk->z0();
    float dz0_i = z0_i - m_thePV->z();
    float dz0_j = z0_j - m_thePV->z();
    float z0sig_i = dz0_i/sqrt(itrk->definingParametersCovMatrix()(1,1));
    float z0sig_j = dz0_j/sqrt(jtrk->definingParametersCovMatrix()(1,1));
    float FirstHit_i = getFirstHitCategory(itrk);
    float FirstHit_j = getFirstHitCategory(jtrk);
    float diffHits = abs(FirstHit_i - FirstHit_j);

    std::vector<float*> VARS({&eta_i, &eta_j, &dEta, &dPhi, &AsymPt, &d0_i, &d0_j, &d0sig_i, &d0sig_j, &z0_i, &z0_j, &dz0_i, &dz0_j, &z0sig_i, &z0sig_j, &FirstHit_i, &FirstHit_j, &diffHits});
 
    unsigned int nfiles = m_bdt.size();
    for(unsigned int ifile = 0; ifile < nfiles; ifile ++){
      m_bdt[ifile]->SetPointers(VARS);
      wgtSelect.emplace_back( m_bdt[ifile]->GetClassification() );
    }
  }

  int VrtSecFuzzy::vertexSelection(WrkVrt& wrkvrt, const xAOD::TrackParticle* itrk, const xAOD::TrackParticle* jtrk){

    // fitting chi2 cut
    if( wrkvrt.fitQuality() > m_jp.SelVrtChi2Cut) {
      ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": failed to pass chi2 threshold." );
      return 1;
    }
 
    // Compatibility to the primary vertex.
    Amg::Vector3D vDist = wrkvrt.vertex - m_thePV->position();
    const double vPos = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py()+vDist.z()*wrkvrt.vertexMom.Pz() )/wrkvrt.vertexMom.Rho();
    const double vPosMomAngT = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py() ) / vDist.perp() / wrkvrt.vertexMom.Pt();
    
    double dphi1 = vDist.phi() - itrk->phi(); 
    while( dphi1 > TMath::Pi() ) { dphi1 -= TMath::TwoPi(); } 
    while( dphi1 < -TMath::Pi() ) { dphi1 += TMath::TwoPi(); }
    double dphi2 = vDist.phi() - itrk->phi(); 
    while( dphi2 > TMath::Pi() ) { dphi2 -= TMath::TwoPi(); } 
    while( dphi2 < -TMath::Pi() ) { dphi2 += TMath::TwoPi(); }
 
    if( m_jp.doPVcompatibilityCut ) {
      if( cos( dphi1 ) < -0.8 && cos( dphi2 ) < -0.8 ) {
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": failed to pass the vPos cut. (both tracks are opposite against the vertex pos)" );
        return 2;
      }
      if( vPosMomAngT < -0.8 ) {
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": failed to pass the vPos cut. (pos-mom directions are opposite)" );
        return 3;
      }
      if( vPos < m_jp.pvCompatibilityCut ) {
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": failed to pass the vPos cut." );
        return 4;
      }
    }

    // fake rejection cuts with track hit pattern consistencies
    if( m_jp.removeFakeVrt ) {
      if( !this->passedFakeReject( wrkvrt.vertex, itrk, jtrk ) ) {
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": failed to pass fake rejection algorithm." );
        return 5;
      }
    }
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": passed fake rejection." );
    return 6;
  }
 
} // end of namespace VKalVrtAthena
