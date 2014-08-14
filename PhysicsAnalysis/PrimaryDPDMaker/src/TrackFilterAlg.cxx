/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 *  @brief This file contains the class implementation for the TrackFilterAlg class.
 *  @author Eric Feng <Eric.Feng@cern.ch>
 *  Based on:  MuonFilterAlg by David Lopez Mateos <David.Lopez@cern.ch>
 *  $Id: TrackFilterAlg.cxx,v 1.4 2008-10-23 16:28:38 efeng Exp $
 **/

#include <string>
#include <vector>

#include "PrimaryDPDMaker/TrackFilterAlg.h"


#include "Particle/TrackParticle.h"
#include "JetEvent/JetCollection.h"
#include "Particle/TrackParticleContainer.h"
//#include "IsolationTool/IIsolationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "TrkParameters/TrackParameters.h"

#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "ITrackToVertex/ITrackToVertex.h"


/// Standard Service Constructor
TrackFilterAlg::TrackFilterAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator), 
    m_log(messageService(),name),
    m_nProcessed(0),
    m_nTracks(0),
    m_nEventTrPassEta(0),
    m_nEventTrPassPt(0),
    m_nEventTrPassSumPt(0),
    m_nEventTrPassIsolation(0),
    m_nEventTrPassChi2DOF(0),
    m_nEventTrPassBLayerHits(0),
    m_nEventTrPassHoles(0),
    m_nEventTrPassSiHits(0),
    m_jetCollection(0),

// SingleTracks
    //m_trackIsolationTool("TrackIsolationTool/TrackFilter_TrackIsolationTool"),
    m_toCalo("ExtrapolateToCaloTool"),
    m_toVertex("ExtrapolateToVertexTool"),
    m_nEventPassNumberOfPrimaryVertexes(0),
    m_nEventTrPassCaloEta(0),
    m_nEventTrPassTrackD0(0),
    m_nEventTrPassTrackZ0(0),
    m_nEventTrPassSCTHits(0),
    m_nEventTrPassPixelHits(0)
// SingleTracks end

{

  declareProperty("cutPtMinTr",         m_cutPtMinTr = 0.0);
  declareProperty("cutSumPtRatio",      m_cutSumPtRatio = 0.1);
  declareProperty("trackContainer",     m_trackContainer = "TrackParticleCandidate"); 
  declareProperty("cutEtaMax",          m_cutEtaMax = 2.5);
  declareProperty("cutChi2DOF",         m_cutChi2DOF = 1.5);
  declareProperty("cutBLayerHits",      m_cutBLayerHits = 1);
  declareProperty("cutSiHits",          m_cutSiHits = 5);
  declareProperty("cutHoles",           m_cutHoles = 1);
  declareProperty("passAll",            m_passAll = false);
  declareProperty("doNoTrackIsol",      m_doNoTrackIsol = false);
  declareProperty("noTrackIsol",        m_noTrackIsol = 1);
  declareProperty("innerNoTrackRadius", m_innerNoTrackRadius = 0.0001);
  declareProperty("outerNoTrackRadius", m_outerNoTrackRadius = 0.4);
  declareProperty("doPtTrackIsol",      m_doPtTrackIsol = false);
  declareProperty("ptTrackIsol",        m_ptTrackIsol = 8000.);
  declareProperty("innerPtTrackRadius", m_innerPtTrackRadius = 0.0001);
  declareProperty("outerPtTrackRadius", m_outerPtTrackRadius = 0.4);
  declareProperty("doCaloIsol",         m_doCaloIsol = false);
  declareProperty("caloIsol",           m_caloIsol = 6000.);
  declareProperty("innerCaloRadius",    m_innerCaloRadius = 0.0001);
  declareProperty("outerCaloRadius",    m_outerCaloRadius = 0.4);
  declareProperty("doJetIsol",          m_doJetIsol = false);
  declareProperty("jetIsolEt",          m_jetIsolEt = 15000.);
  declareProperty("jetDistance",        m_jetDistance = 0.5);
  declareProperty("jetContainerName",   m_jetContainerName = "Cone4H1TowerJets");
  //declareProperty("TrackIsolationTool",  m_trackIsolationTool);

// SingleTracks
  declareProperty("ExtrapolateToCaloTool", m_toCalo);
  declareProperty("ExtrapolateToVertexTool", m_toVertex);
  declareProperty("UseEPFilter", m_useEPFilter = false); // true in 
  declareProperty("NumberOfPrimaryVertexes", m_NumberOfPrimaryVertexes = 2);
  declareProperty("TrackAtCaloEta", m_TrackAtCaloEta = 2.4);
  declareProperty("TrackD0", m_TrackD0 = 1.5);
  declareProperty("TrackZ0", m_TrackZ0 = 1.5);
  declareProperty("TrackNumberOfSCTHits", m_TrackNumberOfSCTHits = 6);
  declareProperty("TrackNumberOfPixelHits", m_TrackNumberOfPixelHits = 1);
  declareProperty("trackIsoDR", m_trackIsoDR = 0.4);
  declareProperty("VertexContainerName", m_vertexContainerName ="VxPrimaryCandidate");
// SingleTracks end

}

/// Destructor
TrackFilterAlg::~TrackFilterAlg()
{

}

/// Gaudi Service Interface method implementations:
StatusCode TrackFilterAlg::initialize()
{

  m_log << MSG::INFO << "==> initialize ..." << name() << endreq;
  
  //retrieve StoreGateSvc
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    m_log << MSG::ERROR
	  << "Unable to retrieve pointer to StoreGateSvc"
	  << endreq;
    return sc;
  }

#if 0  
  if(m_doNoTrackIsol || m_doPtTrackIsol || m_doCaloIsol){
    if(m_trackIsolationTool.retrieve().isFailure()){
      m_log << MSG::WARNING << "Problem retrieving TrackIsolationTool pointer " << endreq;
      m_log << MSG::WARNING << "setting Track and Calo isolation to False " << endreq;
      m_doNoTrackIsol = false;
      m_doPtTrackIsol = false;
      m_doCaloIsol = false;
    }
  }
#endif

// SingleTracks
  //Retrieve the tool for track extrapolation to the calorimeter
  if (m_toCalo.retrieve().isFailure()) {
    m_log << MSG::FATAL << "Could not retrieve " << m_toCalo << endreq;
    return StatusCode::FAILURE;
  } m_log << MSG::INFO << "Successfully retrieved ExtrapolateToCalo tool" << m_toCalo << endreq;
  
  //Retrieve the tool for track extrapolation to the vertex
  if (m_toVertex.retrieve().isFailure()) {
    m_log << MSG::FATAL << "Could not retrieve " << m_toVertex << endreq;
    return StatusCode::FAILURE;
  } else m_log << MSG::INFO << "Successfully retrieved ExtrapolateToVertex tool" << m_toVertex << endreq;
// SingleTracks end  

  return sc;
}


StatusCode TrackFilterAlg::execute()
{

  m_nProcessed += 1;
  // Simple status message at the beginning of each event execute
  m_log << MSG::DEBUG 
        << "==> execute " << name() << " on " << m_nProcessed << ". event..."
        << endreq;


  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  //If passAll is selected, accept all events
  if(m_passAll){
    m_log<< MSG::DEBUG << name() << " event passed because passAll is true" << endreq;
    setFilterPassed(true);
    return sc;
  }
  
  if(m_doJetIsol){
    sc = m_storeGate->retrieve(m_jetCollection, m_jetContainerName);
    if(sc.isFailure()){
      m_log << MSG::WARNING << m_jetContainerName <<" container not found in StoreGate " << endreq;
      m_log << MSG::WARNING << "Turning jet isolation off" << endreq;
      m_doJetIsol = false;
    }
  }
// SingleTracks
  //Then... here we go!

  //std::vector<std::string> containerNames;
  const Rec::TrackParticleContainer* tracks = 0;
  sc = m_storeGate->retrieve(tracks, m_trackContainer);
  if(sc.isFailure()){
    m_log << MSG::WARNING << m_trackContainer << " container not found in StoreGate." << endreq;
    sc = StatusCode::SUCCESS; // ??
    return sc;
  }
  
  /***** Primary vertex *****/    
  //get the primary vertex
  const VxContainer * Vxcontainer = 0;
  if (m_storeGate->retrieve(Vxcontainer, m_vertexContainerName).isFailure()) {
    m_log << MSG::WARNING << "No Vertex container found" << endreq;
    sc = StatusCode::SUCCESS; // ??
    return sc;
  }
  Trk::Vertex beamposition=(*(Vxcontainer->begin()))->recVertex();
  m_log << MSG::DEBUG << "PV position " << beamposition.position() << endreq;
  
  
  int nTrPassEta        = 0;
  int nTrPassPt         = 0;
  int nTrPassSumPt      = 0;
  int nTrPassIsolation  = 0;
  int nTrPassChi2DOF    = 0;
  int nTrPassBLayerHits = 0;
  int nTrPassHoles      = 0;
  int nTrPassSiHits     = 0;
  
  int nTrPassCaloEta    = 0;
  int nTrPassTrackD0    = 0;
  int nTrPassTrackZ0    = 0;
  int nTrPassSCTHits    = 0;
  int nTrPassPixelHits  = 0;  

  bool hasTrack = false;
  Rec::TrackParticleContainer::const_iterator track, trackE = tracks->end();

  double sumPtTracks=0.;

  std::vector < double > trackToCaloEta;
  std::vector < double > trackToCaloPhi; 
  const Trk::TrackParameters* param_at_calo(0);
  const double GAP_ETA = 1.5;

  if(m_useEPFilter){

    // make the extrapolation of all tracks to EM calorimeter
    for(track = tracks->begin(); track != trackE; track++){
      //m_log << MSG::DEBUG << "Extrapolating track to calo" 
      //      << " for track with eta=" << (*track)->eta() 
      //      << " and phi=" << (*track)->phi() << endreq;

      if(fabs((*track)->eta()) < GAP_ETA){
        param_at_calo = m_toCalo->extrapolate((Trk::TrackParticleBase) *(*track),   //*(*track)->originalTrack(),
                                              CaloCell_ID::EMB2,
                                              0.0,
                                              Trk::alongMomentum,
                                              Trk::undefined);	  
      }
      else{
        param_at_calo = m_toCalo->extrapolate((Trk::TrackParticleBase) *(*track), //*(*track)->originalTrack(),
                                              CaloCell_ID::EME2,
                                              0.0,
                                              Trk::alongMomentum,
                                              Trk::undefined);
      }

      if(!param_at_calo){ //if extrap. doesn't work
        //m_log << MSG::WARNING << "Problem extrapolating track to calo" 
        //     << " for track with eta=" << (*track)->eta()
        //     << " and phi=" << (*track)->phi() << endreq;
        
        //use non-extrapolated track direction.
        trackToCaloEta.push_back((*track)->eta());
        trackToCaloPhi.push_back((*track)->phi());
        
      }
      else{
        trackToCaloEta.push_back(param_at_calo->position().eta());
        trackToCaloPhi.push_back(param_at_calo->position().phi());
        //m_log << MSG::DEBUG << "Extrapolated position " 
        //     << " eta=" << param_at_calo->position().eta()
        //     << " and phi=" << param_at_calo->position().phi() << endreq;
	  
      }
      delete param_at_calo;
      param_at_calo=0;
    }
  
    std::vector<double>::iterator track_eta_itr = trackToCaloEta.begin();
    std::vector<double>::iterator track_phi_itr = trackToCaloPhi.begin();
    
    if((Vxcontainer->size())>=m_NumberOfPrimaryVertexes){
      m_nEventPassNumberOfPrimaryVertexes += 1;

      // check cuts and isolation for all tracks of the event
      for(track = tracks->begin(); track != trackE; track++, ++track_eta_itr, ++track_phi_itr){
        m_nTracks += 1;

        // extrapolate to PV
        const Trk::Perigee * mperigee(m_toVertex->perigeeAtVertex(*(*track), beamposition.position()));
        if (!mperigee) {
          m_log << MSG::WARNING << "Extrapolation to vertex is failed. Skip this track." << endreq;
          continue;	
        }
	    
        double p  = fabs(1./mperigee->parameters()[Trk::qOverP]);
        double pt = p*sin(mperigee->parameters()[Trk::theta]);      
        
        if(pt > m_cutPtMinTr){
          nTrPassPt += 1;
          if(fabs(*track_eta_itr)<m_TrackAtCaloEta){
            nTrPassCaloEta += 1;
            if(fabs(mperigee->parameters()[Trk::d0])<=m_TrackD0){
              nTrPassTrackD0 += 1;	    
              if(fabs(mperigee->parameters()[Trk::z0]*sin(mperigee->parameters()[Trk::theta]))<=m_TrackZ0){
                nTrPassTrackZ0 += 1;	  
                if(((*track)->trackSummary()->get(Trk::numberOfSCTHits))>=m_TrackNumberOfSCTHits){
                  nTrPassSCTHits += 1;
                  if(((*track)->trackSummary()->get(Trk::numberOfPixelHits))>=m_TrackNumberOfPixelHits){
                    nTrPassPixelHits += 1;
                    
                    // is this track isolated from other tracks? 
                    std::vector<double>::iterator track_eta_itr2 = trackToCaloEta.begin();
                    std::vector<double>::iterator track_phi_itr2 = trackToCaloPhi.begin();	
                    // loop over all other tracks
                    bool track_failed_cuts = false;
                    
                    for (; track_eta_itr2!= trackToCaloEta.end(); track_eta_itr2++, track_phi_itr2++) {
                      
                      //m_log << MSG::DEBUG << "Track1 eta: "<< *track_eta_itr
                      //     << ", phi: "<< *track_phi_itr << endreq;
                      
                      double deta = fabs(*track_eta_itr - *track_eta_itr2);
                      double dphi = fabs(*track_phi_itr - *track_phi_itr2);
                      
                      if (dphi > M_PI)
                        dphi = 2 * M_PI - dphi;
                      
                      // Can we check pointers here, instead of double differences?
                      
                      double dr = 1000;
                      
                      if( track_eta_itr != track_eta_itr2 ){ //account for dr of track with itself
                        
                        dr = sqrt(dphi*dphi+deta*deta);
                   
                        if (dr < m_trackIsoDR){
                          track_failed_cuts = true;
                          break; // no need to go further
                        }
                        
                      }
                      //m_log << MSG::DEBUG << "  Track2 eta: "<< *track_eta_itr2
                      //     << "         phi: " << *track_phi_itr2 << endreq;
                      //m_log << MSG::DEBUG << " DR = " << dr << " Track failed " << track_failed_cuts << endreq;
                    }
                    
                    if(!track_failed_cuts){
                      nTrPassIsolation += 1;
                      hasTrack = true;
                    }
                  }
                }
              }
            }
          }
        }
        delete mperigee;
      }
    }
  } else {
    // old code
    // calculate the sum of Pt of all tracks
    for(track = tracks->begin(); track != trackE; track++) 
      sumPtTracks+=(*track)->pt();
    
    for(track = tracks->begin(); track != trackE; track++){
      m_nTracks += 1;
      if(fabs((*track)->eta()) < m_cutEtaMax){
        nTrPassEta += 1;
        if((*track)->pt() > m_cutPtMinTr){
          nTrPassPt += 1;
          if((*track)->pt() > m_cutSumPtRatio*sumPtTracks){ 
            nTrPassSumPt += 1;
            if(passCaloIsol(*track) && passNoTracksIsol(*track) && passPtTracksIsol(*track) && passJetIsol(*track)) { 
              nTrPassIsolation += 1;
              
              // Protect against division by zero
              int nDoF    = (*track)->fitQuality()->numberDoF();
              float chiSq = (*track)->fitQuality()->chiSquared();
              float chiSqPerNDoF(0.0);
              if ( nDoF != 0 )
                {
                  chiSqPerNDoF = chiSq / float(nDoF);
                }
              
              if(m_cutChi2DOF == -1.0 || chiSqPerNDoF < m_cutChi2DOF){
                nTrPassChi2DOF += 1;
                int nBLayerHits = (*track)->trackSummary()->get(Trk::numberOfBLayerHits);
                if (nBLayerHits>=m_cutBLayerHits){
                  nTrPassBLayerHits += 1;
                  int nHoles = (*track)->trackSummary()->get(Trk::numberOfPixelHoles)+
                    (*track)->trackSummary()->get(Trk::numberOfSCTHoles);
                  if (nHoles<=m_cutHoles){
                    nTrPassHoles += 1;
                    int nSiHits = (*track)->trackSummary()->get(Trk::numberOfPixelHits)+(*track)->trackSummary()->get(Trk::numberOfSCTHits);
                    if (nSiHits>=m_cutSiHits){
                      nTrPassSiHits += 1;
                      hasTrack = true;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // Translate the track pass counters into event pass counters
  if(nTrPassEta > 0)
    m_nEventTrPassEta += 1;
  
  if(nTrPassPt > 0)
    m_nEventTrPassPt += 1;

  if(nTrPassSumPt > 0)
    m_nEventTrPassSumPt += 1;

  if(nTrPassIsolation > 0) 
    m_nEventTrPassIsolation += 1;
  
  if(nTrPassChi2DOF > 0) 
    m_nEventTrPassChi2DOF += 1;

  if(nTrPassBLayerHits > 0) 
    m_nEventTrPassBLayerHits += 1;

  if(nTrPassHoles > 0) 
    m_nEventTrPassHoles += 1;

  if(nTrPassSiHits > 0) 
    m_nEventTrPassSiHits += 1;

////    
  if(nTrPassCaloEta > 0)   m_nEventTrPassCaloEta += 1;
  if(nTrPassTrackD0 > 0)   m_nEventTrPassTrackD0 += 1;
  if(nTrPassTrackZ0 > 0)   m_nEventTrPassTrackZ0 += 1;
  if(nTrPassSCTHits > 0)   m_nEventTrPassSCTHits += 1;
  if(nTrPassPixelHits > 0) m_nEventTrPassPixelHits += 1;
////

  if(hasTrack){
    m_log << MSG::DEBUG << " Event passed." << endreq;
    setFilterPassed(true);    
  }else{
    m_log << MSG::DEBUG << " Event failed." << endreq;
    setFilterPassed(false);
  }
  
  return sc;
  // SingleTracks end
}



StatusCode TrackFilterAlg::finalize()
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // Calculate the efficiencies and their errors
  double effiTracks           = 0.0;
  double effiTrPassEta        = 0.0;
  double effiTrPassPt         = 0.0;
  double effiTrPassSumPt      = 0.0;
  double effiTrPassIsolation  = 0.0;
  double effiTrPassChi2DOF    = 0.0;
  double effiTrPassBLayerHits = 0.0;
  double effiTrPassHoles   = 0.0;
  double effiTrPassSiHits = 0.0;

// SingleTracks
  double effiTrPassCaloEta    = 0.0;
  double effiTrPassTrackD0    = 0.0;
  double effiTrPassTrackZ0    = 0.0;
  double effiTrPassSCTHits    = 0.0;
  double effiTrPassPixelHits  = 0.0;
// SingleTracks end

  double effiErrTracks        = 0.0;
  double effiErrTrPassEta    = 0.0;
  double effiErrTrPassPt     = 0.0;
  double effiErrTrPassSumPt      = 0.0;
  double effiErrTrPassIsolation  = 0.0;
  double effiErrTrPassChi2DOF    = 0.0;
  double effiErrTrPassBLayerHits = 0.0;
  double effiErrTrPassHoles      = 0.0;
  double effiErrTrPassSiHits = 0.0;

// SingleTracks
  double effiErrTrPassCaloEta    = 0.0;
  double effiErrTrPassTrackD0    = 0.0;
  double effiErrTrPassTrackZ0    = 0.0;
  double effiErrTrPassSCTHits    = 0.0;
  double effiErrTrPassPixelHits  = 0.0;
// SingleTracks end

  // Protection against division by zero
  if ( m_nProcessed != 0 )
    {
      // Efficiencies

// SingleTracks      
      effiTracks        = m_nEventPassNumberOfPrimaryVertexes != 0 ? m_nTracks / (1.0*m_nEventPassNumberOfPrimaryVertexes) : 0;
// SingleTracks end      

      effiTracks        = m_nProcessed != 0 ? m_nTracks / (1.0*m_nProcessed) : 0;
      effiTrPassEta    = m_nProcessed != 0 ? 100.0*m_nEventTrPassEta / m_nProcessed : 0;
      effiTrPassPt     = m_nProcessed != 0 ? 100.0*m_nEventTrPassPt / m_nProcessed : 0;
      effiTrPassSumPt = m_nProcessed != 0 ? 100.0*m_nEventTrPassSumPt / m_nProcessed : 0;
      effiTrPassIsolation = m_nProcessed != 0 ? 100.0*m_nEventTrPassIsolation / m_nProcessed : 0;
      effiTrPassChi2DOF   = m_nProcessed != 0 ? 100.0*m_nEventTrPassChi2DOF / m_nProcessed : 0;
      effiTrPassBLayerHits   = m_nProcessed != 0 ? 100.0*m_nEventTrPassBLayerHits / m_nProcessed : 0;
      effiTrPassHoles   = m_nProcessed != 0 ? 100.0*m_nEventTrPassHoles / m_nProcessed : 0;
      effiTrPassSiHits  = m_nProcessed != 0 ? 100.0*m_nEventTrPassSiHits / m_nProcessed : 0;

// SingleTracks
      effiTrPassCaloEta  = m_nProcessed != 0 ? 100.0*m_nEventTrPassCaloEta / m_nProcessed : 0;
      effiTrPassTrackD0  = m_nProcessed != 0 ? 100.0*m_nEventTrPassTrackD0 / m_nProcessed : 0;
      effiTrPassTrackZ0  = m_nProcessed != 0 ? 100.0*m_nEventTrPassTrackZ0 / m_nProcessed : 0;
      effiTrPassSCTHits  = m_nProcessed != 0 ? 100.0*m_nEventTrPassSCTHits / m_nProcessed : 0;
      effiTrPassPixelHits  = m_nProcessed != 0 ? 100.0*m_nEventTrPassPixelHits / m_nProcessed : 0;
// SingleTracks end
      
      // Error on efficiencies
      effiErrTracks        = m_nTracks != 0 ? sqrt(m_nTracks) / m_nTracks : 0;
      effiErrTrPassEta    = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassEta) / m_nProcessed : 0;
      effiErrTrPassPt     = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassPt) / m_nProcessed : 0;
      effiErrTrPassSumPt = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassSumPt) / m_nProcessed : 0;
      effiErrTrPassIsolation = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassIsolation) / m_nProcessed : 0;
      effiErrTrPassChi2DOF   = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassChi2DOF) / m_nProcessed : 0;
      effiErrTrPassBLayerHits   = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassBLayerHits) / m_nProcessed : 0;
      effiErrTrPassHoles   = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassHoles) / m_nProcessed : 0;
      effiErrTrPassSiHits  = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassSiHits) / m_nProcessed : 0;

// SingleTracks
      effiErrTrPassCaloEta  = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassCaloEta) / m_nProcessed : 0;
      effiErrTrPassTrackD0  = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassTrackD0) / m_nProcessed : 0;
      effiErrTrPassTrackZ0  = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassTrackZ0) / m_nProcessed : 0;
      effiErrTrPassSCTHits  = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassSCTHits) / m_nProcessed : 0;
      effiErrTrPassPixelHits  = m_nProcessed != 0 ? 100 * sqrt(m_nEventTrPassPixelHits) / m_nProcessed : 0;
// SingleTracks end

    }
  else
    {
      m_log << MSG::INFO << "No events processed! Efficiency calculation is not done!!!" << endreq;
    }


  m_log << MSG::INFO << "==> finalize " << name() << endreq;
  m_log << MSG::INFO << "***************************************************************" << endreq;
  m_log << MSG::INFO << "Cut-flow table of " << name() << " skimming algorithm:" << endreq;
  m_log << MSG::INFO << "-------------" << endreq;
  m_log << MSG::INFO << " Number of processed events: " << m_nProcessed << endreq;
  m_log << MSG::INFO << " Number of all tracks:  " << m_nTracks << " and number of tracks per event: " << effiTracks << "+/-" << effiErrTracks << endreq;
  m_log << MSG::INFO << " Events after eta cut:     " << m_nEventTrPassEta << " and resulting efficiency = (" << effiTrPassEta << "+/-" << effiErrTrPassEta << ")" << endreq;
  m_log << MSG::INFO << " Events after p_T cut:     " << m_nEventTrPassPt << " and resulting efficiency = (" << effiTrPassPt << "+/-" << effiErrTrPassPt << ")" << endreq;
  m_log << MSG::INFO << " Events after sum pt cut:  " << m_nEventTrPassSumPt << " and resulting efficiency = (" << effiTrPassSumPt << "+/-"  << effiErrTrPassSumPt << ")" << endreq;
  m_log << MSG::INFO << " Events after isolation cut:  " << m_nEventTrPassIsolation << " and resulting efficiency = (" << effiTrPassIsolation << "+/-"  << effiErrTrPassIsolation << ")" << endreq;
  m_log << MSG::INFO << " Events after chi2/DOF cut:  "   << m_nEventTrPassChi2DOF   << " and resulting efficiency = (" << effiTrPassChi2DOF   << "+/-"  << effiErrTrPassChi2DOF   << ")" << endreq;
  m_log << MSG::INFO << " Events after b-layer cut:  "   << m_nEventTrPassBLayerHits   << " and resulting efficiency = (" << effiTrPassBLayerHits   << "+/-"  << effiErrTrPassBLayerHits   << ")" << endreq;
  m_log << MSG::INFO << " Events after holes cut:  "   << m_nEventTrPassHoles   << " and resulting efficiency = (" << effiTrPassHoles   << "+/-"  << effiErrTrPassHoles   << ")" << endreq;
  m_log << MSG::INFO << " Events after SCT+Pixel cut:  "   << m_nEventTrPassSiHits   << " and resulting efficiency = (" << effiTrPassSiHits   << "+/-"  << effiErrTrPassSiHits   << ")" << endreq;

  m_log << MSG::INFO << "***************************************************************" << endreq;

// SingleTracks : E over P report
  m_log << MSG::INFO << " E over P filter report: " << endreq;
  m_log << MSG::INFO << " Number of processed events:   " << m_nProcessed << endreq;
  m_log << MSG::INFO << " Events after NumberOfPrimaryVertexes cut: " << m_nEventPassNumberOfPrimaryVertexes << endreq;
  m_log << MSG::INFO << " Number of all tracks:         " << m_nTracks << " and number of tracks per event: " << effiTracks << "+/-" << effiErrTracks << endreq;
  m_log << MSG::INFO << " Events after p_T cut:         " << m_nEventTrPassPt << " and resulting efficiency = (" << effiTrPassPt << "+/-" << effiErrTrPassPt << ")" << endreq;
  m_log << MSG::INFO << " Events after CaloEta cut:     " << m_nEventTrPassCaloEta << " and resulting efficiency = (" << effiTrPassCaloEta << "+/-" << effiErrTrPassCaloEta << ")" << endreq;
  m_log << MSG::INFO << " Events after TrackD0 cut:     " << m_nEventTrPassTrackD0 << " and resulting efficiency = (" << effiTrPassTrackD0 << "+/-" << effiErrTrPassTrackD0 << ")" << endreq;
  m_log << MSG::INFO << " Events after TrackZ0 cut:     " << m_nEventTrPassTrackZ0 << " and resulting efficiency = (" << effiTrPassTrackZ0 << "+/-" << effiErrTrPassTrackZ0 << ")" << endreq;
  m_log << MSG::INFO << " Events after SCTHits cut:     " << m_nEventTrPassSCTHits << " and resulting efficiency = (" << effiTrPassSCTHits << "+/-" << effiErrTrPassSCTHits << ")" << endreq;
  m_log << MSG::INFO << " Events after PixelHits cut:   " << m_nEventTrPassPixelHits << " and resulting efficiency = (" << effiTrPassPixelHits << "+/-" << effiErrTrPassPixelHits << ")" << endreq;
  m_log << MSG::INFO << " Events after isolation cut:   " << m_nEventTrPassIsolation << " and resulting efficiency = (" << effiTrPassIsolation << "+/-"  << effiErrTrPassIsolation << ")" << endreq;
  m_log << MSG::INFO << "***************************************************************" << endreq;
// SingleTracks end

  return sc;

}

bool TrackFilterAlg::passCaloIsol(const Rec::TrackParticle* /*track*/)
{
  if(!m_doCaloIsol)
    return true;
#if 0
  double outerConeEt = m_trackIsolationTool->caloIsolationEnergy(track, m_outerCaloRadius,false);
  double innerConeEt = m_trackIsolationTool->caloIsolationEnergy(track, m_innerCaloRadius,false);
  if((outerConeEt-innerConeEt) < m_caloIsol)
    return true;
#endif
  return false;

}

bool TrackFilterAlg::passNoTracksIsol(const Rec::TrackParticle* /*track*/)
{
  if(!m_doNoTrackIsol)
    return true;
#if 0
  int outerConeNoTracks = m_trackIsolationTool->trackIsolationNumberOfTrack(track,m_outerNoTrackRadius,false);
  int innerConeNoTracks = m_trackIsolationTool->trackIsolationNumberOfTrack(track,m_innerNoTrackRadius,false);
  if((outerConeNoTracks-innerConeNoTracks) < m_noTrackIsol)
    return true;
#endif
  return false;
}

bool TrackFilterAlg::passPtTracksIsol(const Rec::TrackParticle* /*track*/)
{
  if(!m_doPtTrackIsol)
    return true;
#if 0
  double outerConePtTracks = m_trackIsolationTool->trackIsolationEnergy(track,m_outerNoTrackRadius, false);
  double innerConePtTracks = m_trackIsolationTool->trackIsolationEnergy(track,m_innerNoTrackRadius, false);
  if((outerConePtTracks-innerConePtTracks) < m_ptTrackIsol)
    return true;
#endif
  return false;

}

bool TrackFilterAlg::passJetIsol(const Rec::TrackParticle* track)
{

  if(!m_doJetIsol)
    return true;
  JetCollection::const_iterator jet, jetE = m_jetCollection->end();
  for(jet = m_jetCollection->begin(); jet != jetE; jet++){
    if(deltaR(*jet,track) < m_jetDistance && (*jet)->et() > m_jetIsolEt)
      return false;
  }
  return true;

}

double TrackFilterAlg::deltaR(const INavigable4Momentum* part1, const Rec::TrackParticle* part2)
{
  double deltaEta = part1->eta()-part2->eta();
  double deltaPhi = part1->phi()-part2->phi();
  deltaPhi = deltaPhi < -3.141592 ? deltaPhi+2*3.141592 : deltaPhi;
  deltaPhi = deltaPhi > 3.141592 ? deltaPhi-2*3.141592 : deltaPhi;
  return sqrt(deltaPhi*deltaPhi+deltaEta*deltaEta);
}
