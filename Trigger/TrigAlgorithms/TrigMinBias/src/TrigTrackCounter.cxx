/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// executes after event accept track reconstruction
// retrieves z position of track and append it to TE
#include "TrigMinBias/TrigTrackCounter.h"
//#include "TrkSpacePoint/SpacePointContainer.h "
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrigInterfaces/FexAlgo.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODTrigMinBias/TrigTrackCounts.h"
//---------------------------------------------------------------------------------

TrigTrackCounter::TrigTrackCounter(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator), 
    m_z0_pt(0),
    m_eta_phi(0),
    m_formFeaturesTimer(0),
    m_getTracksTimer(0),
    m_recoTracks(0)
{  
  
  // Histogram dimensions
  declareProperty("Z0Bins", m_hZ0Bins = 20);
  declareProperty("Z0Min",  m_hZ0Min = 0.5);
  declareProperty("Z0Max",  m_hZ0Max = 400.5);
  declareProperty("PtBins", m_hPtBins = 20);
  declareProperty("PtMin",  m_hPtMin = 0.1);
  declareProperty("PtMax",  m_hPtMax = 2.5);
  declareProperty("EtaBins",m_hEtaBins = 10);
  declareProperty("EtaMin", m_hEtaMin = 0.);
  declareProperty("EtaMax", m_hEtaMax = 3.0);
  declareProperty("PhiBins",m_hPhiBins = 16);
  declareProperty("PhiMin", m_hPhiMin = 0.);
  declareProperty("PhiMax", m_hPhiMax = 3.2);
  declareProperty("InputTrackContainerName",  m_trkContainerName = "AmbigSolv");
  // thresholds for filling m_eta_phi histo
  declareProperty("Min_pt", m_pt_min= 0.1000 ); // GeV
  declareProperty("Max_z0", m_z0_max= 200.0 ); // GeV
  
  // Monitoring of the data stored in TrigTrackCounts
  declareMonitoredStdContainer("TrkZ0",       m_trkZ0); 
  declareMonitoredStdContainer("TrkPt",       m_trkPt); 
  declareMonitoredStdContainer("TrkEta",      m_trkEta); 
  declareMonitoredStdContainer("TrkPhi",      m_trkPhi);
 
  // Set the size of the vectors to pass the monitoring's 
  // kVecUO size check.
  m_trkZ0.resize((m_hZ0Bins+2),0);
  m_trkPt.resize((m_hPtBins+2),0);
  m_trkEta.resize((m_hEtaBins+2),0);
  m_trkPhi.resize((m_hPhiBins+2),0);

  // Other monitoring
  declareMonitoredVariable("NTrks", m_ntrks);
}

//---------------------------------------------------------------------------------

TrigTrackCounter::~TrigTrackCounter(void) {
  if(m_z0_pt) delete m_z0_pt;
  if(m_eta_phi) delete m_eta_phi;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTrackCounter::hltInitialize() {
  ATH_MSG_INFO( "Initialising this TrigTrackCounter: " << name()  );
  
  // Create timers
  if( timerSvc() ){
    m_formFeaturesTimer = addTimer("formFeatures");
    m_getTracksTimer    = addTimer("getTracks");
  }

  // Create empty histograms.
  m_z0_pt = new xAOD::TrigHisto2D();
  m_z0_pt->makePrivateStore();
  m_z0_pt->initialize(m_hZ0Bins, m_hZ0Min, m_hZ0Max,
		      m_hPtBins, m_hPtMin, m_hPtMax);
  
  m_eta_phi = new xAOD::TrigHisto2D();
  m_eta_phi->makePrivateStore();
  m_eta_phi->initialize(m_hEtaBins, m_hEtaMin, m_hEtaMax,
			m_hPhiBins, m_hPhiMin, m_hPhiMax);
  
  ATH_MSG_INFO( "TrigTrackCounter initialized successfully"  );
  return HLT::OK;  
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTrackCounter::hltExecute( const HLT::TriggerElement *, HLT::TriggerElement* outputTE ){
  
  ATH_MSG_DEBUG( "Executing this TrigTrackCounter " << name()  );
  
  if( timerSvc() ){ 
    m_formFeaturesTimer->start();
    m_getTracksTimer->start();
  }  
  
  // retrieve output from TrigAmbiguitySolver
  HLT::ErrorCode ec = getFeature( outputTE, m_recoTracks, m_trkContainerName );
  if( ec != HLT::OK ){
    ATH_MSG_ERROR( "Input track collection " << m_trkContainerName << "could not be found "  );
    return HLT::NAV_ERROR;
  }
  else{
    ATH_MSG_DEBUG( "Retrieved successfully track collection " << m_trkContainerName  );
  }

  if( timerSvc() ){ 
    m_getTracksTimer->stop();
  }  

  // Clear the histograms
  m_z0_pt->clear();
  m_eta_phi->clear();

  m_ntrks = -999;
  if( !m_recoTracks || m_recoTracks->size() == 0 ){
    ATH_MSG_DEBUG( "REGTEST: TrackCollection contains is empty or contains 0 tracks."  );
  }
  else{
    m_ntrks = m_recoTracks->size();
    ATH_MSG_DEBUG( "REGTEST: TrackCollection contains " << m_ntrks << " tracks."  );
    
    TrackCollection::const_iterator itr = m_recoTracks->begin();
    TrackCollection::const_iterator itrEnd = m_recoTracks->end();
    
    const Trk::Perigee* aMeasPer = 0;
    float z0, phi0, eta, pT;

    for( ; itr != itrEnd; itr++){
      aMeasPer = (*itr)->perigeeParameters();
      if( aMeasPer == 0 ) continue;

      z0 = aMeasPer->parameters()[Trk::z0];
      phi0 = aMeasPer->parameters()[Trk::phi0];
      eta = aMeasPer->eta();
      pT = aMeasPer->pT();
      
      m_z0_pt->fill(fabs(z0), pT/1000.0, 1.);
      if(pT/1000.>m_pt_min && fabs(z0)<m_z0_max){
	m_eta_phi->fill(fabs(eta), fabs(phi0), 1.);
      }
      ATH_MSG_DEBUG( "REGTEST: Found track with: "
                     << "z0 = " << z0 << " mm, "
                     << "phi0 = " << phi0 << ", "
                     << "eta = " << eta << ", "
                     << "pt = " << pT/1000. << " GeV"  );
    }
  }

  // Fill monitoring variables.
  // Project the TrigHisto2D histograms into 1D histograms and dump
  // their contents such that it can be integrated by the online
  // monitoring.
  m_trkZ0 = m_z0_pt->profileX();
  m_trkPt = m_z0_pt->profileY();
  m_trkEta = m_eta_phi->profileX();
  m_trkPhi = m_eta_phi->profileY();

  if( timerSvc() ){ 
    m_formFeaturesTimer->stop();
  }  
  
  // save feature to output TE:
  xAOD::TrigTrackCounts* tk = new xAOD::TrigTrackCounts;
  
  tk->makePrivateStore();
  
  tk->setZ0Bins(m_hZ0Bins);
  tk->setZ0Min(m_hZ0Min);
  tk->setZ0Max(m_hZ0Max);
  tk->setPtBins(m_hPtBins);
  tk->setPtMin(m_hPtMin);
  tk->setPtMax(m_hPtMax);
  
  tk->setZ0_pt(m_z0_pt->contents());
  
  tk->setEtaBins(m_hEtaBins);
  tk->setEtaMin(m_hEtaMin);
  tk->setEtaMax(m_hEtaMax);
  tk->setPhiBins(m_hPhiBins);
  tk->setPhiMin(m_hPhiMin);
  tk->setPhiMax(m_hPhiMax);
  
  tk->setEta_phi(m_eta_phi->contents());
  
  HLT::ErrorCode hltStatus = attachFeature( outputTE, tk, "trackcounts" );
  if(hltStatus != HLT::OK) {
    ATH_MSG_ERROR( "Write of TrigTrackCounts into outputTE failed"  );
    return hltStatus;
  }

  return HLT::OK;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTrackCounter::hltFinalize() {
  
  ATH_MSG_INFO( "Finalizing this TrigTrackCounter" << name()  );
  return HLT::OK;  
}
