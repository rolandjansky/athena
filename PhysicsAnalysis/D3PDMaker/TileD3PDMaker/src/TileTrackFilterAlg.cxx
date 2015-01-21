/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileTrackFilterAlg.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>
 * 
 * Created in September 2012
 */

// INCLUDE HEADER
#include "TileTrackFilterAlg.h"

//===============================================================================================
TileTrackFilterAlg::TileTrackFilterAlg( const std::string& name,ISvcLocator* pSvcLocator ): 
    AthAlgorithm( name, pSvcLocator ),
    m_track_iso_tool("TrackIsolationTool"){
//==============================================================================================
    m_storeGate = 0;
    m_outputCont = 0;
    declareProperty("track_iso_tool", m_track_iso_tool );
    declareProperty("InputTracksName", m_trackContainerName = "TrackParticleCandidate"); // INPUT TRACK CONTAINER
    declareProperty("OutputTracksName", m_outputTracksName = "SelectedTracks"); // OUTPUT TRACK CONTAINER
    
    // CUTS
    declareProperty("cut_p", m_cutP = 500.); // CUT IN MOMENTUM
    declareProperty("cut_eta", m_cutEta = 2.4); // CUT IN ETA
    declareProperty("cut_sizeofcone", m_cutSizeOfCone = 0.4); // CUT IN CONE SIZE 

} // TileTrackFilterAlg::TileTrackFilterAlg

//==========================================
StatusCode TileTrackFilterAlg::initialize(){
//==========================================
    // INITIALISE THE TRACK FILTER ALGORITHM
    
    ATH_MSG_INFO("TileTrackFilterAlg::initialize() ");

    CHECK(m_track_iso_tool.retrieve());
    CHECK(service("StoreGateSvc",m_storeGate));

    return StatusCode::SUCCESS;
} // TileTrackFilterAlg::initialize()

//=======================================
StatusCode TileTrackFilterAlg::execute(){
//=======================================

    // TRACKPARTICLE OUTPUT CONTAINER
    m_outputCont = new TRACKCONTAINER( SG::VIEW_ELEMENTS );
    CHECK( m_storeGate->record( m_outputCont, m_outputTracksName ) );
    
    // NUMBER OF TRACKPARTICLECANDIDATES
    int counter = 0;
    //bool debug = false;

    // GET HANDLE ON TRACKPARTICLE CONTAINER AND RETRIEVE FROM STOREGATE
    const TRACKCONTAINER* trackcoll = 0;
    CHECK( m_storeGate->retrieve( trackcoll, m_trackContainerName ) );

    // ALLOW FOR MULTIPLE TRACKPARTICLE TYPES
    std::vector<TRACKCONTAINER::const_iterator> start;
    std::vector<TRACKCONTAINER::const_iterator> stop;
    start.push_back(trackcoll->begin());
    stop.push_back(trackcoll->end());

    // LOOP OVER TRACKPARTICLE TYPES
    for(unsigned int i=0;i<start.size();++i){
        TRACKCONTAINER::const_iterator f_track = start[i];
        TRACKCONTAINER::const_iterator l_track = stop[i];
        // LOOP OVER TRACKS 
        for( ; f_track != l_track; ++f_track ){
            const TRACK* trackPointer = ( *f_track);
            if(!trackPointer){
                ATH_MSG_INFO("NO TRACK FOUND.");
                continue;
            } // IF
            // CHECK WHETHER TRACK SHOULD BE ACCEPTED
            if(accept(trackPointer)){
                if(std::find(m_outputCont->begin(),m_outputCont->end(),trackPointer) == m_outputCont->end()){
                    m_outputCont->push_back(const_cast<TRACK*>(trackPointer));
                    ++counter;
                } // IF
            } // IF
        } // FOR (TRACKS)
    } // FOR (TRACK TYPES)

    return StatusCode::SUCCESS;
} // TileTrackFilterAlg::execute()

//=====================================================
bool TileTrackFilterAlg::accept(const TRACK* p){
//=====================================================
   int nucone     = 0; // FIXME: NEED TO USE xAOD tool m_track_iso_tool->trackIsolationNumberOfTrack(p, m_cutSizeOfCone);
   float momentum = p->p4().P(); // for xAOD::TrackParticle
   //float momentum = p->p();
   float eta      = p->eta();
   // cout<<"num de trazas "<<nucone<<endl;
   //  if( nucone != 1 || momentum < m_cutP || fabs(eta) > m_cutEta ){
   // return false;
   // } // IF
   // return true;
  
   if (nucone==0 && momentum >= m_cutP && fabs(eta) <= m_cutEta){
     // cout<<"solo ella "<<nucone<<endl;
     return true;
   }
   else{
     // cout<<"mas de ella misma "<<nucone<<endl;
     return false; 
   }
  /* 
  int tracksnum = h->tracks_n;
  for(int tracki=0; tracki<tracksnum;tracki++){
    for(int trackii=0; trackii<tracksnum; trackii++){
      if(trackii==tracki) continue;
      
      for(int layy=4; layy<7;layy++){
	float etatr=h->tracks_eta_layer->at(trackii).at(layy);
	float phitr=h->tracks_phi_layer->at(trackii).at(layy);
	float delta2=dist(phitr,phitrack,etatr,etatrack);
	if(delta2<isolation){ 
	  isolation=delta2;
	  indextrack=trackii;
	  indexlay=layy;
	  energyclosesttrack=h->tracks_pt->at(trackii);
	}
      }
    }
  }
  */
} // TileTrackFilterAlg::accept

//========================================
StatusCode TileTrackFilterAlg::finalize(){
//========================================
    return StatusCode::SUCCESS;
} // TileTrackFilterAlg::finalize()
