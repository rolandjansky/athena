/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <string>

#include "TrigL2MuonSA/MuFastSteering.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigT1Interfaces/RecMuonRoI.h"


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MuFastSteering::MuFastSteering(const std::string& name, ISvcLocator* svc) 
  : HLT::FexAlgo(name, svc), 
    m_storeGate("StoreGateSvc", this->name()), 
    m_dataPreparator("TrigL2MuonSA::MuFastDataPreparator"),
    m_patternFinder("TrigL2MuonSA::MuFastPatternFinder"),
    m_stationFitter("TrigL2MuonSA::MuFastStationFitter"),
    m_trackFitter("TrigL2MuonSA::MuFastTrackFitter"),
    m_trackExtrapolator("TrigL2MuonSA::MuFastTrackExtrapolator"),
    m_backExtrapolatorTool("TrigMuonBackExtrapolator"),
    m_recMuonRoIUtils(),
    m_rpcHits(), m_tgcHits(),
    m_mdtRegion(), m_muonRoad(),
    m_rpcFitResult(), m_tgcFitResult(),
    m_mdtHits_normal(), m_mdtHits_overlap(),
    m_cscHits()
{
  declareProperty("DataPreparator",    m_dataPreparator,    "data preparator");
  declareProperty("PatternFinder",     m_patternFinder,     "pattern finder");
  declareProperty("StationFitter",     m_stationFitter,     "station fitter");
  declareProperty("TrackFitter",       m_trackFitter,       "track fitter");
  declareProperty("TrackExtrapolator", m_trackExtrapolator, "track extrapolator");

  declareProperty("BackExtrapolator", m_backExtrapolatorTool, "public tool for back extrapolating the muon tracks to the IV");

  declareProperty("Timing", m_use_timer=false);
  declareProperty("UseLUTForMC", m_use_mcLUT=true);
  
  declareProperty("ESD_EXT_size",m_esd_ext_size=100);
  declareProperty("ESD_ROB_size",m_esd_rob_size=10);
  declareProperty("ESD_CSM_size",m_esd_csm_size=30);
  declareProperty("ESD_LV1_size",m_esd_lv1_size=20);
  declareProperty("ESD_RPC_size",m_esd_rpc_size=100);
  declareProperty("ESD_TGC_size",m_esd_tgc_size=50);
  declareProperty("ESD_MDT_size",m_esd_mdt_size=100);

  declareProperty("R_WIDTH_RPC_FAILED",m_rWidth_RPC_Failed=30);
  declareProperty("R_WIDTH_TGC_FAILED",m_rWidth_TGC_Failed=200);

  declareProperty("Scale_Road_BarrelInner",m_scaleRoadBarrelInner=1);
  declareProperty("Scale_Road_BarrelMiddle",m_scaleRoadBarrelMiddle=1);
  declareProperty("Scale_Road_BarrelOuter",m_scaleRoadBarrelOuter=1);
  
  declareProperty("WinPt",m_winPt=4.0);

  declareMonitoredVariable("InnMdtHits", m_inner_mdt_hits);
  declareMonitoredVariable("MidMdtHits", m_middle_mdt_hits);
  declareMonitoredVariable("OutMdtHits", m_outer_mdt_hits);
  declareMonitoredStdContainer("FitResiduals", m_fit_residuals);
  declareMonitoredVariable("Efficiency", m_efficiency);
  declareMonitoredVariable("SagInv", m_sag_inverse);
  declareMonitoredVariable("Address", m_address);
  declareMonitoredVariable("AbsPt", m_absolute_pt);
  declareMonitoredVariable("Sagitta", m_sagitta);
  declareMonitoredVariable("TrackPt", m_track_pt);
  declareMonitoredStdContainer("ResInner", m_res_inner);
  declareMonitoredStdContainer("ResMiddle", m_res_middle);
  declareMonitoredStdContainer("ResOuter", m_res_outer);
  declareMonitoredStdContainer("TrackEta", m_track_eta);
  declareMonitoredStdContainer("TrackPhi", m_track_phi);
  declareMonitoredStdContainer("FailedRoIEta", m_failed_eta);
  declareMonitoredStdContainer("FailedRoIPhi", m_failed_phi);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MuFastSteering::~MuFastSteering() {
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuFastSteering::hltInitialize()
{
  msg() << MSG::DEBUG << "Initializing MuFastSteering - package version " << PACKAGE_VERSION << endreq ;
  
  if (m_storeGate.retrieve().isFailure()) {
    msg() << MSG::ERROR << "Cannot retrieve service StoreGateSvc" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  m_timerSvc = 0;
  if (m_use_timer) {
    StatusCode sc = service( "TrigTimerSvc", m_timerSvc, true); 
    if( sc.isFailure() ) {
      msg() << MSG::ERROR << ": Unable to locate TrigTimer Service" << endreq;
    }
  
    if(m_timerSvc) {
      m_timers.push_back(m_timerSvc->addItem("MuFastSteering_DataPreparator"));
      m_timers[ITIMER_DATA_PREPARATOR]->propName("DataPreparator_Time");
            
      m_timers.push_back(m_timerSvc->addItem("MuFastSteering_PatternFinder"));
      m_timers[ITIMER_PATTERN_FINDER]->propName("PatternFinder_Time");
      
      m_timers.push_back(m_timerSvc->addItem("MuFastSteering_StationFitter"));
      m_timers[ITIMER_STATION_FITTER]->propName("StationFitter_Time");
      
      m_timers.push_back(m_timerSvc->addItem("MuFastSteering_TrackFitter"));
      m_timers[ITIMER_TRACK_FITTER]->propName("TrackFitter_Time");
    
      m_timers.push_back(m_timerSvc->addItem("MuFastSteering_TrackExtrapolator"));
      m_timers[ITIMER_TRACK_EXTRAPOLATOR]->propName("TrackExtrapolator_Time");
    
      m_timers.push_back(m_timerSvc->addItem("MuFastSteering_TotalProcessing"));
      m_timers[ITIMER_TOTAL_PROCESSING]->propName("TotalProcessing_Time");
    }
  }

  // 
  if (m_dataPreparator.retrieve().isFailure()) {
    msg() << "Cannot retrieve Tool DataPreparator" << endreq;
    return HLT::BAD_JOB_SETUP;
  }
  
  // 
  if (m_patternFinder.retrieve().isFailure()) {
    msg() << "Cannot retrieve Tool DataPreparator" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  // 
  if (m_stationFitter.retrieve().isFailure()) {
    msg() << "Cannot retrieve Tool StationFitter" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  // 
  if (m_trackFitter.retrieve().isFailure()) {
    msg() << "Cannot retrieve Tool TrackFitter" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  // 
  if (m_trackExtrapolator.retrieve().isFailure()) {
    msg() << "Cannot retrieve Tool TrackExtrapolator" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  // BackExtrapolator services
  if ( m_backExtrapolatorTool.retrieve().isFailure() ) {
    msg() << MSG::ERROR << "Could not retrieve " << m_backExtrapolatorTool << endreq;
    return HLT::BAD_JOB_SETUP;
  } 

  // Set service tools
  m_trackExtrapolator->setExtrapolatorTool(&m_backExtrapolatorTool);
  m_dataPreparator->setExtrapolatorTool(&m_backExtrapolatorTool);

  // set road width in case TGC/RPC readout failure 
  m_dataPreparator->setRoadWidthForFailure(m_rWidth_RPC_Failed, m_rWidth_TGC_Failed);

  StatusCode sc;

  // set data or MC flag
  sc = m_dataPreparator->setMCFlag(m_use_mcLUT);
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to set MC flag to DataPreparator" << endreq;
    return HLT::ERROR;
  }

  sc = m_trackFitter->setMCFlag(m_use_mcLUT);
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to set MC flag to TrackFitter" << endreq;
    return HLT::ERROR;
  }

  msg() << MSG::DEBUG << "initialize success" << endreq;
  
  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuFastSteering::hltBeginRun() {

   msg() << MSG::DEBUG << "hltBeginRun"<<  endreq;
   return HLT::OK;
}

HLT::ErrorCode MuFastSteering::hltEndRun() {
   msg() << MSG::DEBUG << "hltEndRun"<<  endreq;
   return HLT::OK;
}

HLT::ErrorCode MuFastSteering::hltFinalize() {
   msg() << MSG::DEBUG << "hltFinalize()" << endreq;
   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuFastSteering::hltExecute(const HLT::TriggerElement* inputTE, 
                                          HLT::TriggerElement* outputTE)
{
  StatusCode sc = StatusCode::SUCCESS;
  
  // Initialize monitored variables;
  m_inner_mdt_hits  = -1;
  m_middle_mdt_hits = -1;
  m_outer_mdt_hits  = -1;
  
  m_fit_residuals.clear();
  m_res_inner.clear();
  m_res_middle.clear();
  m_res_outer.clear();
  
  m_efficiency  = 0;
  m_sag_inverse = 9999.;
  m_sagitta     = 9999.;
  m_address     = 9999;
  m_absolute_pt = 9999.;

  m_track_pt    = 9999.;  
  m_track_eta.clear();
  m_track_phi.clear();
  m_failed_eta.clear();
  m_failed_phi.clear();

  if (m_timerSvc) {
    for (unsigned int i_timer=0; i_timer<m_timers.size(); i_timer++) {
      m_timers[i_timer]->start();
      m_timers[i_timer]->pause();
    }
  }
  clearEvent();
  
  if (m_timerSvc) m_timers[ITIMER_TOTAL_PROCESSING]->resume();
  if (m_timerSvc) m_timers[ITIMER_DATA_PREPARATOR]->resume();
  
  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "hltExecute called" << endreq;
  
  std::vector<const TrigRoiDescriptor*> roids;
  std::vector<const TrigRoiDescriptor*>::const_iterator p_roids;
  HLT::ErrorCode hec2 = getFeatures(inputTE, roids);
  
  std::vector<const LVL1::RecMuonRoI*> muonRoIs;
  std::vector<const LVL1::RecMuonRoI*>::const_iterator p_roi;
  HLT::ErrorCode hec = getFeatures(inputTE, muonRoIs);
  if (hec != HLT::OK && hec2 != HLT::OK) {
    return hec2;
  }
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "RecMuonRoI size: " << muonRoIs.size() << endreq;
    msg() << MSG::DEBUG << "RoIdescriptor size: " << roids.size() << endreq;
  }

  p_roids = roids.begin();
  for (p_roi=muonRoIs.begin(); p_roi!=muonRoIs.end(); ++p_roi) {

    double roiEta = (*p_roi)->eta();
    double roiPhi = (*p_roi)->phi();
    
    msg() << MSG::INFO << "RoI eta/phi=" << roiEta << "/" << roiPhi << endreq;
    
    std::vector<TrigL2MuonSA::TrackPattern> m_trackPatterns;
    m_mdtHits_normal.clear();
    m_mdtHits_overlap.clear();
    m_cscHits.clear();
    
    m_rpcFitResult.Clear();
    m_tgcFitResult.Clear();

    m_muonRoad.Clear();
    

    if ( m_recMuonRoIUtils.isBarrel(*p_roi) ) { // Barrel
      msg() << MSG::INFO << "Barrel" << endreq;
      
      m_muonRoad.setScales(m_scaleRoadBarrelInner,
                           m_scaleRoadBarrelMiddle,
                           m_scaleRoadBarrelOuter);      

      // Data preparation
      m_rpcHits.clear();
      sc = m_dataPreparator->prepareData(*p_roi,
                                         m_rpcHits,
                                         m_muonRoad,
                                         m_mdtRegion,
                                         m_rpcFitResult,
                                         m_mdtHits_normal,
                                         m_mdtHits_overlap);
      if (!sc.isSuccess()) {
         msg() << MSG::WARNING << "Data preparation failed" << endreq;
         updateOutputTE(outputTE, inputTE, *p_roi, *p_roids, m_muonRoad, m_mdtRegion,
			m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, m_trackPatterns);
         return HLT::OK;
      }
      if (m_timerSvc) m_timers[ITIMER_DATA_PREPARATOR]->pause();

      // Pattern finding
      if (m_timerSvc) m_timers[ITIMER_PATTERN_FINDER]->resume();
      sc = m_patternFinder->findPatterns(m_muonRoad,
                                         m_mdtHits_normal,
                                         m_trackPatterns);
      if (!sc.isSuccess()) {
         msg() << MSG::WARNING << "Pattern finder failed" << endreq;
         updateOutputTE(outputTE, inputTE, *p_roi, *p_roids, m_muonRoad, m_mdtRegion,
                        m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, m_trackPatterns);
         return HLT::OK;
      }
      if (m_timerSvc) m_timers[ITIMER_PATTERN_FINDER]->pause();

      // Superpoint fit
      if (m_timerSvc) m_timers[ITIMER_STATION_FITTER]->resume();      
      sc = m_stationFitter->findSuperPoints(*p_roi,
                                            m_rpcFitResult,
                                            m_trackPatterns);
      if (!sc.isSuccess()) {
         msg() << MSG::WARNING << "Super point fitter failed" << endreq;
         updateOutputTE(outputTE, inputTE, *p_roi, *p_roids, m_muonRoad, m_mdtRegion,
                        m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, m_trackPatterns);
         return HLT::OK;
      }
      if (m_timerSvc) m_timers[ITIMER_STATION_FITTER]->pause();      

      // Track fitting
      if (m_timerSvc) m_timers[ITIMER_TRACK_FITTER]->resume();      
      sc = m_trackFitter->findTracks(*p_roi,
                                     m_rpcFitResult,
                                     m_trackPatterns);
      if (!sc.isSuccess()) {
         msg() << MSG::WARNING << "Track fitter failed" << endreq;
         updateOutputTE(outputTE, inputTE, *p_roi, *p_roids, m_muonRoad, m_mdtRegion,
                        m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, m_trackPatterns);
         return HLT::OK;
      }
      if (m_timerSvc) m_timers[ITIMER_TRACK_FITTER]->pause();      

    } else { // Endcap
      msg() << MSG::INFO << "Endcap" << endreq;
      
      // Data preparation
      m_tgcHits.clear();     
      sc = m_dataPreparator->prepareData(*p_roi,
                                         m_tgcHits,
                                         m_muonRoad,
                                         m_mdtRegion,
                                         m_tgcFitResult,
                                         m_mdtHits_normal,
                                         m_mdtHits_overlap,
					 m_cscHits);
      if (!sc.isSuccess()) {
         msg() << MSG::WARNING << "Data preparation failed" << endreq;
         updateOutputTE(outputTE, inputTE, *p_roi, *p_roids, m_muonRoad, m_mdtRegion,
                        m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, m_trackPatterns);
         return HLT::OK;
      }
      if (m_timerSvc) m_timers[ITIMER_DATA_PREPARATOR]->pause();

      // Pattern finding
      if (m_timerSvc) m_timers[ITIMER_PATTERN_FINDER]->resume();
      sc = m_patternFinder->findPatterns(m_muonRoad,
                                         m_mdtHits_normal,
                                         m_trackPatterns);
      if (!sc.isSuccess()) {
         msg() << MSG::WARNING << "Pattern finder failed" << endreq;
         updateOutputTE(outputTE, inputTE, *p_roi, *p_roids, m_muonRoad, m_mdtRegion,
                        m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, m_trackPatterns);
         return HLT::OK;
      }
      if (m_timerSvc) m_timers[ITIMER_PATTERN_FINDER]->pause();
      
      // Superpoint fit
      if (m_timerSvc) m_timers[ITIMER_STATION_FITTER]->resume();      
      sc = m_stationFitter->findSuperPoints(*p_roi,
                                             m_tgcFitResult,
                                             m_trackPatterns);
      if (!sc.isSuccess()) {
         msg() << MSG::WARNING << "Super point fitter failed" << endreq;
         updateOutputTE(outputTE, inputTE, *p_roi, *p_roids, m_muonRoad, m_mdtRegion,
                        m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, m_trackPatterns);
         return HLT::OK;
      }
      if (m_timerSvc) m_timers[ITIMER_STATION_FITTER]->pause();      

      // Track fittingh    
      if (m_timerSvc) m_timers[ITIMER_TRACK_FITTER]->resume();     
      sc = m_trackFitter->findTracks(*p_roi,
                                     m_tgcFitResult,
                                     m_trackPatterns);
      if (!sc.isSuccess()) {
         msg() << MSG::WARNING << "Track fitter failed" << endreq;
         updateOutputTE(outputTE, inputTE, *p_roi, *p_roids, m_muonRoad, m_mdtRegion,
                        m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, m_trackPatterns);
         return HLT::OK;
      }
      if (m_timerSvc) m_timers[ITIMER_TRACK_FITTER]->pause();      
    }
    
    // fix if eta is strange
    for (unsigned int i=0 ;i<m_trackPatterns.size(); i++) {
       TrigL2MuonSA::TrackPattern track = m_trackPatterns[i]; 
       const float ETA_LIMIT       = 2.8;
       const float DELTA_ETA_LIMIT = 1.0;
       float roiEta = (*p_roi)->eta();
       const float ZERO_LIMIT = 1.e-5;

       if (fabs(track.pt) > ZERO_LIMIT
           && ( fabs(track.etaMap) > ETA_LIMIT || fabs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
          m_trackPatterns[i].etaMap = roiEta;
       }
    }

    // Track extrapolation for ID combined
    if (m_timerSvc) m_timers[ITIMER_TRACK_EXTRAPOLATOR]->resume();

    sc = m_trackExtrapolator->extrapolateTrack(m_trackPatterns, m_winPt);

    if (sc != StatusCode::SUCCESS) {
       msg() << MSG::WARNING << "Track extrapolator failed" << endreq;
       updateOutputTE(outputTE, inputTE, *p_roi, *p_roids, m_muonRoad, m_mdtRegion,
                      m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, m_trackPatterns);
       return HLT::OK;
    }
    if (m_timerSvc) m_timers[ITIMER_TRACK_EXTRAPOLATOR]->pause();
    
    // Update monitoring variables
    sc = updateMonitor(*p_roi, m_mdtHits_normal, m_trackPatterns);
    if (sc != StatusCode::SUCCESS) {
       msg() << MSG::WARNING << "Failed to update monitoring variables" << endreq;
       updateOutputTE(outputTE, inputTE, *p_roi, *p_roids, m_muonRoad, m_mdtRegion,
                      m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, m_trackPatterns);
       return HLT::OK;
    }

    // Update output trigger element
    updateOutputTE(outputTE, inputTE, *p_roi, *p_roids, m_muonRoad, m_mdtRegion,
                   m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, m_trackPatterns);
                   
    p_roids++;    
    if (p_roids==roids.end()) break; 
  }
  
  if (m_timerSvc) m_timers[ITIMER_TOTAL_PROCESSING]->pause();
  
  int nRoI = muonRoIs.size();

  if (m_timerSvc) {
     for (unsigned int i_timer=0; i_timer<m_timers.size(); i_timer++) {
         m_timers[i_timer]->propVal(nRoI);
         m_timers[i_timer]->stop();
     }
  }

  //  m_timerSvc->print();

  return HLT::OK;
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void MuFastSteering::clearEvent()
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool MuFastSteering::updateOutputTE(HLT::TriggerElement*                     outputTE,
                                    const HLT::TriggerElement*               inputTE, 
                                    const LVL1::RecMuonRoI*                  roi,
                                    const TrigRoiDescriptor*                 roids,
                                    const TrigL2MuonSA::MuonRoad&            muonRoad,
                                    const TrigL2MuonSA::MdtRegion&           mdtRegion,
                                    const TrigL2MuonSA::RpcFitResult&        rpcFitResult,
                                    const TrigL2MuonSA::TgcFitResult&        tgcFitResult,
                                    const TrigL2MuonSA::MdtHits&             mdtHits,
                                    const TrigL2MuonSA::CscHits&             cscHits,
                                    std::vector<TrigL2MuonSA::TrackPattern>& trackPatterns)
{
  const int currentRoIId = roids->roiId();

  const double scalePhiWidthForFailure = 2;
    
  const EventInfo* pEventInfo(0);
  StatusCode sc = m_storeGate->retrieve(pEventInfo);
  if (sc.isFailure()){
     msg() << MSG::FATAL << "Can't get EventInfo object" << endreq;
     return HLT::SG_ERROR;
  }
  
  const EventID* pEventId = pEventInfo->event_ID();
  if (pEventId==0) {
     msg() << MSG::ERROR << "Could not find EventID object" << endreq;
     return HLT::SG_ERROR;
  }
  
  const TriggerInfo* pTriggerInfo = pEventInfo->trigger_info();
  if (pTriggerInfo==0) {
     msg() << MSG::ERROR << "Could not find TriggerInfo object" << endreq;
     return HLT::SG_ERROR;
  }
  
  int inner  = 0;
  int middle = 1;
  int outer  = 2;

  MuonFeature* muon_feature;

  std::string muonCollKey = "MuonL2SAInfo";
  
  xAOD::L2StandAloneMuonContainer *muonColl = new xAOD::L2StandAloneMuonContainer();
  xAOD::L2StandAloneMuonAuxContainer aux;
  muonColl->setStore( &aux );

  if ( trackPatterns.size() > 0 ) {
    
    const TrigL2MuonSA::TrackPattern& pattern = trackPatterns[0]; 
    
    if (pattern.s_address==-1) {
      inner  = xAOD::L2MuonParameters::Chamber::EndcapInner;
      middle = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
      outer  = xAOD::L2MuonParameters::Chamber::EndcapOuter;
    } else {
      inner  = xAOD::L2MuonParameters::Chamber::BarrelInner;
      middle = xAOD::L2MuonParameters::Chamber::BarrelMiddle;
      outer  = xAOD::L2MuonParameters::Chamber::BarrelOuter;
    }

    muon_feature = new MuonFeature(pattern.s_address,
                                   pattern.pt*pattern.charge,
                                   pattern.superPoints[inner].R,
                                   pattern.etaMap,
                                   pattern.phi,
                                   pattern.phiDir,
                                   pattern.superPoints[inner].Z,
                                   pattern.superPoints[inner].Alin,
                                   1.0);
    
    // add superpoints
    muon_feature->set_sp1(pattern.superPoints[inner].R, pattern.superPoints[inner].Z, pattern.superPoints[inner].Alin);
    muon_feature->set_sp2(pattern.superPoints[middle].R, pattern.superPoints[middle].Z, pattern.superPoints[middle].Alin);
    muon_feature->set_sp3(pattern.superPoints[outer].R, pattern.superPoints[outer].Z, pattern.superPoints[outer].Alin);

    // add pt variables
    muon_feature->set_br(pattern.radius, pattern.sagitta);    
    muon_feature->set_ec(pattern.alpha, pattern.beta);
    
    // store eta and phi in dq_flag1
    double flag1 = double(int(fabs(pattern.etaMap)*1e3))*1e3 + double(pattern.etaBin);
    flag1 += double(int(fabs(pattern.phiMap)*1e3))*1e-4 + double(pattern.phiBin)*1e-7;
    if( pattern.etaMap < 0 ) flag1 += 1e9;
    if( pattern.phiMap < 0 ) flag1 += 1e8;

    // store TGC/RPC readout failure flags in dq_flag1
    // -1: TGC or RPC data normaly read
    //  1: TGC readout failure => recovered using RoI
    //  2: RPC readout failure => recovered using RoI
    double flag2 = -1;
    if (pattern.isTgcFailure) flag2 = 1;
    if (pattern.isRpcFailure) flag2 = 2;
    
    muon_feature->set_dq(flag1,flag2);

  } else {
    
    muon_feature = new MuonFeature(0,0.,0.,0.,0.,0.,0.,0.,1.0);
    
  }
  
  uint32_t muondetmask = 0;
  
  MuonFeatureDetails* muon_details =
    new MuonFeatureDetails(AlgoMap(name()),
                           inputTE->getId(),
                           pTriggerInfo->extendedLevel1ID(),
                           pEventId->lumi_block(),
                           muondetmask,
                           currentRoIId,
                           roi->sysID(),
                           roi->subsysID(),
                           roi->sectorID(),
                           roi->getRoINumber(),
                           roi->getThresholdNumber(),
                           roi->eta(),
                           roi->phi(),
                           m_esd_ext_size,
                           m_esd_rob_size,
                           m_esd_csm_size,
                           m_esd_lv1_size,
                           m_esd_rpc_size,
                           m_esd_tgc_size,
                           m_esd_mdt_size );
  
  msg() << MSG::DEBUG << "size of trackPattern=" << trackPatterns.size() << endreq;
  
  if( trackPatterns.size() > 0 ) {
    
    const TrigL2MuonSA::TrackPattern& pattern = trackPatterns[0]; 
    msg() << MSG::DEBUG << "pattern#0: # of hits at inner  =" << pattern.mdtSegments[inner].size() << endreq;
    msg() << MSG::DEBUG << "pattern#0: # of hits at middle =" << pattern.mdtSegments[middle].size() << endreq;
    msg() << MSG::DEBUG << "pattern#0: # of hits at outer  =" << pattern.mdtSegments[outer].size() << endreq;
    msg() << MSG::INFO << "pt=" << pattern.pt << endreq;

    muon_details->setCharge(pattern.charge);
    muon_details->setPt(pattern.pt*pattern.charge);

    muon_details->setAddress(pattern.s_address);

    muon_details->setAlpha(pattern.alpha);
    muon_details->setBeta(pattern.beta);

    muon_details->setSagitta(pattern.sagitta);
    muon_details->setRadius(pattern.radius);

    muon_details->setSlope(pattern.superPoints[inner].Alin);
    muon_details->setIntercept(pattern.superPoints[inner].Z);
    muon_details->setDeltaR(pattern.deltaR);

    muon_details->setEta(pattern.etaMap);
    muon_details->setPhiMap(pattern.phiMap);
    muon_details->setPhi(pattern.phi);
    muon_details->setPhiDir(pattern.phiDir);

    muon_details->setSP1(pattern.superPoints[inner].Alin,
                         pattern.superPoints[inner].Blin,
                         pattern.superPoints[inner].Z,
                         pattern.superPoints[inner].R,
                         pattern.superPoints[inner].Chi2);
    muon_details->setSP2(pattern.superPoints[middle].Alin,
                         pattern.superPoints[middle].Blin,
                         pattern.superPoints[middle].Z,
                         pattern.superPoints[middle].R,
                         pattern.superPoints[middle].Chi2);
    muon_details->setSP3(pattern.superPoints[outer].Alin,
                         pattern.superPoints[outer].Blin,
                         pattern.superPoints[outer].Z,
                         pattern.superPoints[outer].R,
                         pattern.superPoints[outer].Chi2);

    for(unsigned int i_hit=0; i_hit<mdtHits.size(); i_hit++) {
      if ( mdtHits[i_hit].isOutlier==0 || mdtHits[i_hit].isOutlier==1 ) {
        muon_details->setMdtHit(mdtHits[i_hit].OnlineId, (uint32_t)mdtHits[i_hit].isOutlier,
                                mdtHits[i_hit].R, mdtHits[i_hit].Z, mdtHits[i_hit].Residual, 
                                mdtHits[i_hit].DriftTime, mdtHits[i_hit].DriftSpace, mdtHits[i_hit].DriftSigma);  
      }
    }

    for(unsigned int i_hit=0; i_hit<cscHits.size(); i_hit++) {
      if ( cscHits[i_hit].MeasuresPhi==0 ){
        if ( cscHits[i_hit].isOutlier==0 || cscHits[i_hit].isOutlier==1 ) {
          // store CSC hits into the MDT region for the moment.
          muon_details->setMdtHit(0, (uint32_t)cscHits[i_hit].isOutlier,
                                  cscHits[i_hit].r, cscHits[i_hit].z, cscHits[i_hit].Residual, 
                                  0., 0., 0.);
          msg() << MSG::DEBUG << "Stored CSC Hits: "
                << "OL=" << cscHits[i_hit].isOutlier << ","
                << "r="  << cscHits[i_hit].r << ","
                << "z="  << cscHits[i_hit].z << ","
                << "Rs=" << cscHits[i_hit].Residual << endreq;
        }
      }
    }

    for (int i_station=0; i_station<3; i_station++) {
      for (int i_sector=0; i_sector<2; i_sector++) {
        muon_details->setAw(i_sector, i_station, muonRoad.aw[i_station][i_sector]);
        muon_details->setBw(i_sector, i_station, muonRoad.bw[i_station][i_sector]);

        muon_details->setZetaMin(i_sector, i_station, mdtRegion.zMin[i_station][i_sector]);
        muon_details->setZetaMax(i_sector, i_station, mdtRegion.zMax[i_station][i_sector]);

        muon_details->setRadMin(i_sector, i_station, mdtRegion.rMin[i_station][i_sector]);
        muon_details->setRadMax(i_sector, i_station, mdtRegion.rMax[i_station][i_sector]);

        muon_details->setEtaMin(i_sector, i_station, mdtRegion.etaMin[i_station][i_sector]);
        muon_details->setEtaMax(i_sector, i_station, mdtRegion.etaMax[i_station][i_sector]);

        muon_details->setChamber_type_1(i_sector, i_station, mdtRegion.chamberType[i_station][i_sector][0]);
        muon_details->setChamber_type_2(i_sector, i_station, mdtRegion.chamberType[i_station][i_sector][1]);
      }
    }

    if ( muonRoad.isEndcap ) {
      muon_details->setTgcPt(tgcFitResult.tgcPT);

      muon_details->setTgcInn(tgcFitResult.tgcInn[0], tgcFitResult.tgcInn[1],
                              tgcFitResult.tgcInn[2], tgcFitResult.tgcInn[3]);
      muon_details->setTgcInnF(tgcFitResult.tgcInnRhoStd, tgcFitResult.tgcInnRhoNin,
                               tgcFitResult.tgcInnPhiStd, tgcFitResult.tgcInnPhiNin);

      muon_details->setTgcMid1(tgcFitResult.tgcMid1[0], tgcFitResult.tgcMid1[1],
                               tgcFitResult.tgcMid1[2], tgcFitResult.tgcMid1[3]);
      muon_details->setTgcMid2(tgcFitResult.tgcMid2[0], tgcFitResult.tgcMid2[1],
                               tgcFitResult.tgcMid2[2], tgcFitResult.tgcMid2[3]);
      muon_details->setTgcMidF(tgcFitResult.tgcMidRhoChi2, tgcFitResult.tgcMidRhoNin,
                               tgcFitResult.tgcMidPhiChi2, tgcFitResult.tgcMidPhiNin);

    } else {
      muon_details->setRpc1(rpcFitResult.rpc1[0], rpcFitResult.rpc1[1], rpcFitResult.rpc1[2]);
      muon_details->setRpc2(rpcFitResult.rpc2[0], rpcFitResult.rpc2[1], rpcFitResult.rpc2[2]);
      muon_details->setRpc3(rpcFitResult.rpc3[0], rpcFitResult.rpc3[1], rpcFitResult.rpc3[2]);
    }

    // define inner, middle, outer
    if (pattern.s_address==-1) {
      inner  = xAOD::L2MuonParameters::Chamber::EndcapInner;
      middle = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
      outer  = xAOD::L2MuonParameters::Chamber::EndcapOuter;
    } else {
      inner  = xAOD::L2MuonParameters::Chamber::BarrelInner;
      middle = xAOD::L2MuonParameters::Chamber::BarrelMiddle;
      outer  = xAOD::L2MuonParameters::Chamber::BarrelOuter;
    }

    // ---------
    // store xAOD

    xAOD::L2StandAloneMuon* muonSA = new xAOD::L2StandAloneMuon();
    muonSA->makePrivateStore();

    // add pT
    muonSA->setPt(pattern.pt*pattern.charge);

    // add s_address
    muonSA->setSAddress(pattern.s_address);

    // add positions at MS
    muonSA->setEtaMS(pattern.etaMap);
    muonSA->setPhiMS(pattern.phi);
    muonSA->setDirPhiMS(pattern.phiDir);
    muonSA->setRMS(pattern.superPoints[inner].R);
    muonSA->setZMS(pattern.superPoints[inner].Z);
    muonSA->setDirZMS(pattern.superPoints[inner].Alin);
    muonSA->setBeta(1.0);

    // add pt variables
    // Endcap
    muonSA->setEndcapAlpha(pattern.alpha);
    muonSA->setEndcapBeta(pattern.beta);
    muonSA->setEndcapRadius(pattern.endcapRadius);
    // Barrel
    muonSA->setBarrelRadius(pattern.radius);
    muonSA->setBarrelSagitta(pattern.sagitta);    
    
    muonSA->setSlopeInner(pattern.superPoints[inner].Alin);
    muonSA->setInterceptInner(pattern.superPoints[inner].Z);
    muonSA->setDeltaR(pattern.deltaR);

    // store eta and phi used as argument to pT LUT
    muonSA->setEtaMap(pattern.etaMap);
    muonSA->setPhiMap(pattern.phiMap);
    muonSA->setEtaBin(pattern.etaBin);
    muonSA->setPhiBin(pattern.phiBin);
    
    // store TGC/RPC readout failure flags
    muonSA->setIsTgcFailure((int)pattern.isTgcFailure);
    muonSA->setIsRpcFailure((int)pattern.isRpcFailure);

    // add superpoints
    muonSA->setSuperPoint(inner, pattern.superPoints[inner].R, pattern.superPoints[inner].Z,
                          pattern.superPoints[inner].Alin, pattern.superPoints[inner].Blin, pattern.superPoints[inner].Chi2);
    muonSA->setSuperPoint(middle, pattern.superPoints[middle].R, pattern.superPoints[middle].Z,
                          pattern.superPoints[middle].Alin, pattern.superPoints[middle].Blin, pattern.superPoints[middle].Chi2);
    muonSA->setSuperPoint(outer, pattern.superPoints[outer].R, pattern.superPoints[outer].Z,
                          pattern.superPoints[outer].Alin, pattern.superPoints[outer].Blin, pattern.superPoints[outer].Chi2);

    ///////////////////////////////
    // Below are detailed information

    /// Set L2 muon algorithm ID
    muonSA->setAlgoId( L2MuonAlgoMap(name()) );
    /// Set input TE ID
    muonSA->setTeId( inputTE->getId() );
    /// Set level-1 ID
    muonSA->setLvl1Id( pTriggerInfo->extendedLevel1ID() );
    /// Set lumi block
    muonSA->setLumiBlock( pEventId->lumi_block() );
    /// Set muon detector mask
    muonSA->setMuonDetMask( muondetmask );
    /// Set RoI ID
    muonSA->setRoiId( currentRoIId );
    /// Set RoI system ID
    muonSA->setRoiSystem( roi->sysID() );
    /// Set RoI subsystem ID
    muonSA->setRoiSubsystem( roi->subsysID() );
    /// Set RoI sector ID
    muonSA->setRoiSector( roi->sectorID() );
    /// Set RoI number
    muonSA->setRoiNumber( roi->getRoINumber() );
    /// Set RoI threshold number
    muonSA->setRoiThreshold( roi->getThresholdNumber() );
    /// Set RoI eta
    muonSA->setRoiEta( roi->eta() );
    /// Set RoIp phi
    muonSA->setRoiPhi( roi->phi() );

    /// Set size of storages to be reserved
    muonSA->setExtensionCapacity( m_esd_ext_size );
    muonSA->setRobCapacity( m_esd_rob_size );
    muonSA->setCsmCapacity( m_esd_csm_size );
    muonSA->setLvl1EmuCapacity( m_esd_lv1_size );
    muonSA->setRpcHitsCapacity( m_esd_rpc_size );
    muonSA->setTgcHitsCapacity( m_esd_tgc_size );
    muonSA->setMdtHitsCapacity( m_esd_mdt_size );

    // MDT hits
    for (unsigned int i_hit=0; i_hit<mdtHits.size(); i_hit++) {
      if ( mdtHits[i_hit].isOutlier==0 || mdtHits[i_hit].isOutlier==1 ) {
        muonSA->setMdtHit(mdtHits[i_hit].OnlineId, (uint32_t)mdtHits[i_hit].isOutlier, mdtHits[i_hit].Chamber,
                          mdtHits[i_hit].R, mdtHits[i_hit].Z, mdtHits[i_hit].Residual, 
                          mdtHits[i_hit].DriftTime, mdtHits[i_hit].DriftSpace, mdtHits[i_hit].DriftSigma);  
      }
    }

    // Road information
    for (int i_station=0; i_station<3; i_station++) {
      for (int i_sector=0; i_sector<2; i_sector++) {
        muonSA->setRoad(i_station, i_sector, muonRoad.aw[i_station][i_sector], muonRoad.bw[i_station][i_sector]);
        muonSA->setRegionZ(i_station, i_sector, mdtRegion.zMin[i_station][i_sector], mdtRegion.zMax[i_station][i_sector]);
        muonSA->setRegionR(i_station, i_sector, mdtRegion.rMin[i_station][i_sector], mdtRegion.rMax[i_station][i_sector]);
        muonSA->setRegionEta(i_station, i_sector, mdtRegion.etaMin[i_station][i_sector], mdtRegion.etaMax[i_station][i_sector]);
        muonSA->setChamberType1(i_station, i_sector, mdtRegion.chamberType[i_station][i_sector][0]);
        muonSA->setChamberType2(i_station, i_sector, mdtRegion.chamberType[i_station][i_sector][1]);
      }
    }

    if ( muonRoad.isEndcap ) {
    // TGC fit results
      muonSA->setTgcPt(tgcFitResult.tgcPT);

      muonSA->setTgcInn(tgcFitResult.tgcInn[0], tgcFitResult.tgcInn[1],
                        tgcFitResult.tgcInn[2], tgcFitResult.tgcInn[3]);
      muonSA->setTgcInnF(tgcFitResult.tgcInnRhoStd, tgcFitResult.tgcInnRhoNin,
                         tgcFitResult.tgcInnPhiStd, tgcFitResult.tgcInnPhiNin);

      muonSA->setTgcMid1(tgcFitResult.tgcMid1[0], tgcFitResult.tgcMid1[1],
                         tgcFitResult.tgcMid1[2], tgcFitResult.tgcMid1[3]);
      muonSA->setTgcMid2(tgcFitResult.tgcMid2[0], tgcFitResult.tgcMid2[1],
                         tgcFitResult.tgcMid2[2], tgcFitResult.tgcMid2[3]);
      muonSA->setTgcMidF(tgcFitResult.tgcMidRhoChi2, tgcFitResult.tgcMidRhoNin,
                         tgcFitResult.tgcMidPhiChi2, tgcFitResult.tgcMidPhiNin);

    } else {
    // TGC RPC results
      muonSA->setRpc1(rpcFitResult.rpc1[0], rpcFitResult.rpc1[1], rpcFitResult.rpc1[2]);
      muonSA->setRpc2(rpcFitResult.rpc2[0], rpcFitResult.rpc2[1], rpcFitResult.rpc2[2]);
      muonSA->setRpc3(rpcFitResult.rpc3[0], rpcFitResult.rpc3[1], rpcFitResult.rpc3[2]);
    }

    muonColl->push_back(muonSA);

    // -------
    // store TrigRoiDescriptor
    const float ZERO_LIMIT = 1.e-5;
    if (fabs(muon_feature->pt()) > ZERO_LIMIT ) {
      TrigRoiDescriptor* MSroiDescriptor = new TrigRoiDescriptor(roids->l1Id(),
                                                                 roids->roiId(),
                                                                 pattern.etaMap,
                                                                 pattern.etaMap,
                                                                 pattern.etaMap,
                                                                 pattern.phi,
                                                                 pattern.phi,
                                                                 pattern.phi);
      msg() << MSG::DEBUG << "...TrigRoiDescriptor for MS "
            << "pattern.etaMap/pattern.phi="
            << pattern.etaMap << "/" << pattern.phi << endreq;

      // patch for the ID RoI descriptor
      float phiHalfWidth = 0.1;
      float etaHalfWidth = 0.1;

      // 2010 runs
      //      if ( std::fabs(pattern.etaVtx)>1 && std::fabs(pattern.etaVtx)<1.5 ) {
      //        phiHalfWidth = 0.25;
      //        etaHalfWidth = 0.4;
      //      } else {
      //        phiHalfWidth = 0.1;
      //        etaHalfWidth = 0.15;
      //      }
      
      // 2011a tuning
      phiHalfWidth = getRoiSizeForID(false,muon_feature);
      etaHalfWidth = getRoiSizeForID(true, muon_feature);

      if (pattern.isTgcFailure || pattern.isRpcFailure) 
        phiHalfWidth *= scalePhiWidthForFailure;

      TrigRoiDescriptor* IDroiDescriptor = new TrigRoiDescriptor(roids->l1Id(),
                                                                 roids->roiId(),
                                                                 pattern.etaVtx,
                                                                 pattern.etaVtx - etaHalfWidth,
                                                                 pattern.etaVtx + etaHalfWidth,
                                                                 pattern.phiVtx,
                                                                 pattern.phiVtx - phiHalfWidth,
                                                                 pattern.phiVtx + phiHalfWidth);
      msg() << MSG::DEBUG << "...TrigRoiDescriptor for ID "
            << "pattern.etaVtx/pattern.phiVtx="
            << pattern.etaVtx << "/" << pattern.phiVtx << endreq;
      msg() << MSG::DEBUG << "old RoI:        " << *roids << endreq;
      msg() << MSG::DEBUG << "updated ID RoI: " << *IDroiDescriptor << endreq;      

      HLT::ErrorCode attached;

      // attach roi descriptor for TrigMoore
      attached = attachFeature(outputTE, MSroiDescriptor, "forMS");

      if ( attached!=HLT::OK) {

        msg() << MSG::WARNING  << "Could not attach the roi descriptor for TrigMoore." << endreq;

      } else {

        if (msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG  << "Recorded an RoiDescriptor for TrigMoore:"
                << " phi=" << MSroiDescriptor->phi()
                << ",  eta=" << MSroiDescriptor->eta()
                << endreq;
        }
      }

      // attach roi descriptor for ID
      attached = attachFeature(outputTE, IDroiDescriptor, "forID");

      if ( attached!=HLT::OK) {

        msg() << MSG::WARNING  << "Could not attach the roi descriptor for Inner Detector." << endreq;

      } else {

        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG  << "Recorded an RoiDescriptor for Inner Detector:"
                << " phi=" << IDroiDescriptor->phi()
                << ",  eta=" << IDroiDescriptor->eta()
                << endreq;
      }
    }

  }

  HLT::ErrorCode status = attachFeature( outputTE, muon_feature );
  if( status != HLT::OK ) {
    msg() << MSG::ERROR << "Record of Muon Feature in TriggerElement failed" << endreq;
    outputTE->setActiveState(false);
    return false;
  }
  
  if (muonColl != 0 && muonColl->size() > 0) {
    outputTE->setActiveState(true);
    status = attachFeature( outputTE, muonColl, muonCollKey );
    if( status != HLT::OK ) {
      msg() << MSG::ERROR << "Record of L2StandAloneMuon in TriggerElement failed" << endreq;
      outputTE->setActiveState(false);
      delete muonColl;
      return false;
    }
  }

  outputTE->setActiveState(true);
  
  if (muon_details) {
    status = attachFeature( outputTE, muon_details);
    if(status!=HLT::OK){
      msg() << MSG::ERROR 
            << "Record of Muon Feature Details failed" << endreq;
      return status;
    } 
  }  
    
  return true;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MuonFeatureDetails::AlgoId MuFastSteering::AlgoMap(const std::string& name)
{
  if (name == "MuFastSteering_Muon")  {
    return MuonFeatureDetails::MUONID;
  }

  return MuonFeatureDetails::NULLID;

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

int MuFastSteering::L2MuonAlgoMap(const std::string& name)
{
  int algoId = 0;
  if (name == "MuFastSteering_Muon")  {
    algoId = xAOD::L2MuonParameters::L2MuonAlgoId::MUONID;
  }

  algoId = xAOD::L2MuonParameters::L2MuonAlgoId::NULLID;

  return algoId;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

float MuFastSteering::getRoiSizeForID(bool isEta, MuonFeature* muon_feature)
{
   bool  isBarrel = (muon_feature->saddress()==-1) ? true : false;
   float eta = muon_feature->eta();
   float phi = muon_feature->phi();
   float pt  = muon_feature->pt();

   //
   const int N_PARAMS = 2;

   //
   const float etaMinWin_brl = 0.10;
   const float etaMinWin_ec1 = 0.10;
   const float etaMinWin_ec2 = 0.10;
   const float etaMinWin_ec3 = 0.10;
   const float etaMinWin_ecA = 0.10;
   const float etaMinWin_ecB = 0.10;

   const float etaMaxWin_brl = 0.20;
   const float etaMaxWin_ec1 = 0.20;
   const float etaMaxWin_ec2 = 0.20;
   const float etaMaxWin_ec3 = 0.20;
   const float etaMaxWin_ecA = 0.20;
   const float etaMaxWin_ecB = 0.20;

   const float etaParams_brl[N_PARAMS] = { 0.038,  0.284};
   const float etaParams_ec1[N_PARAMS] = { 0.011,  0.519};
   const float etaParams_ec2[N_PARAMS] = { 0.023,  0.253};
   const float etaParams_ec3[N_PARAMS] = { 0.018,  0.519};
   const float etaParams_ecA[N_PARAMS] = { 0.010,  0.431};
   const float etaParams_ecB[N_PARAMS] = { 0.023,  0.236};

   //
   const float phiMinWin_brl = 0.125;
   const float phiMinWin_ec1 = 0.125;
   const float phiMinWin_ec2 = 0.125;
   const float phiMinWin_ec3 = 0.10;
   const float phiMinWin_ecA = 0.15;
   const float phiMinWin_ecB = 0.15;

   const float phiMaxWin_brl = 0.20;
   const float phiMaxWin_ec1 = 0.20;
   const float phiMaxWin_ec2 = 0.20;
   const float phiMaxWin_ec3 = 0.20;
   const float phiMaxWin_ecA = 0.25;
   const float phiMaxWin_ecB = 0.20;

   const float phiParams_brl[N_PARAMS] = { 0.000,  0.831};
   const float phiParams_ec1[N_PARAMS] = { 0.000,  0.885};
   const float phiParams_ec2[N_PARAMS] = { 0.015,  0.552};
   const float phiParams_ec3[N_PARAMS] = { 0.008,  0.576};
   const float phiParams_ecA[N_PARAMS] = { 0.000,  0.830};
   const float phiParams_ecB[N_PARAMS] = { 0.006,  1.331};

   //
   float minWin;
   float maxWin;
   float params[N_PARAMS];
   if( isBarrel ) {
      if( isEta ) {
         memcpy(params,etaParams_brl,sizeof(params));
         minWin = etaMinWin_brl;
         maxWin = etaMaxWin_brl;
      }
      else {
         memcpy(params,phiParams_brl,sizeof(params));
         minWin = phiMinWin_brl;
         maxWin = phiMaxWin_brl;
      }
   }
   else { // endcap
      ECRegions reg = whichECRegion(eta,phi);
      if( reg == WeakBFieldA ) {
         if( isEta ) {
            memcpy(params,etaParams_ecA,sizeof(params));
            minWin = etaMinWin_ecA;
            maxWin = etaMaxWin_ecA;
         }
         else {
            memcpy(params,phiParams_ecA,sizeof(params));
            minWin = phiMinWin_ecA;
            maxWin = phiMaxWin_ecA;
         }
      }
      else if( reg == WeakBFieldB ) {
         if( isEta ) {
            memcpy(params,etaParams_ecB,sizeof(params));
            minWin = etaMinWin_ecB;
            maxWin = etaMaxWin_ecB;
         }
         else {
            memcpy(params,phiParams_ecB,sizeof(params));
            minWin = phiMinWin_ecB;
            maxWin = phiMaxWin_ecB;
         }
      }
      else {
         if( fabs(eta) < 1.5 ) {
            if( isEta ) {
               memcpy(params,etaParams_ec1,sizeof(params));
               minWin = etaMinWin_ec1;
               maxWin = etaMaxWin_ec1;
            }
            else {
               memcpy(params,phiParams_ec1,sizeof(params));
               minWin = phiMinWin_ec1;
               maxWin = phiMaxWin_ec1;
            }
         }
         else if( fabs(eta) < 2.0 ) {
            if( isEta ) {
               memcpy(params,etaParams_ec2,sizeof(params));
               minWin = etaMinWin_ec2;
               maxWin = etaMaxWin_ec2;
            }
            else {
               memcpy(params,phiParams_ec2,sizeof(params));
               minWin = phiMinWin_ec2;
               maxWin = phiMaxWin_ec2;
            }
         }
         else {
            if( isEta ) {
               memcpy(params,etaParams_ec3,sizeof(params));
               minWin = etaMinWin_ec3;
               maxWin = etaMaxWin_ec3;
            }
            else {
               memcpy(params,phiParams_ec3,sizeof(params));
               minWin = phiMinWin_ec3;
               maxWin = phiMaxWin_ec3;
            }
         }
      }
   }
   
   //
   float x = params[0] + params[1] / pt;
   float retval = x;
   if( x < minWin ) retval = minWin;
   if( x > maxWin ) retval = maxWin;
   
   return retval;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

ECRegions MuFastSteering::whichECRegion( const float eta, const float phi ) const
{
   float absEta = fabs(eta);
   
   if(      ( 1.3 <= absEta && absEta < 1.45) &&
            ( (0                 <= fabs(phi) && fabs(phi) < CLHEP::pi/48. )     ||
              (CLHEP::pi*11./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*13./48. ) ||
              (CLHEP::pi*23./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*25./48. ) ||
              (CLHEP::pi*35./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*37./48. ) ||
              (CLHEP::pi*47./48. <= fabs(phi) && fabs(phi) < CLHEP::pi )
               )
      ) return WeakBFieldA;
   
   else if( ( 1.5 <= absEta && absEta < 1.65 ) &&
            ( (CLHEP::pi*3./32.  <= fabs(phi) && fabs(phi) < CLHEP::pi*5./32. ) ||
              (CLHEP::pi*11./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*13./32.) ||
              (CLHEP::pi*19./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*21./32.) ||
              (CLHEP::pi*27./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*29./32.)
               )
      ) return WeakBFieldB;
   
   else return Bulk;
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode MuFastSteering::updateMonitor(const LVL1::RecMuonRoI*                  roi,
                                         const TrigL2MuonSA::MdtHits&             mdtHits,
                                         std::vector<TrigL2MuonSA::TrackPattern>& trackPatterns)
{
  const float ZERO_LIMIT = 1e-5;
  const int MAX_STATION = 3;
  
  if( trackPatterns.size() > 0 ) {

    m_efficiency  = 1;
  
    const TrigL2MuonSA::TrackPattern& pattern = trackPatterns[0]; 
    float norm = (trackPatterns[0].s_address==-1)? 10.: 1.;
    
    float count_inner  = 0;
    float count_middle = 0;
    float count_outer  = 0;
    
    for(unsigned int i_hit=0; i_hit<mdtHits.size(); i_hit++) {
      
      if (fabs(mdtHits[i_hit].DriftSpace) < ZERO_LIMIT) continue;
      
      char st = mdtHits[i_hit].cType[1];
      
      if (st=='I') {
        count_inner++;
        m_res_inner.push_back(mdtHits[i_hit].Residual/norm);
      }
      
      if (st=='M') {
        count_middle++;
        m_res_middle.push_back(mdtHits[i_hit].Residual/norm);
      }
      
      if (st=='O') {
        count_outer++;
        m_res_outer.push_back(mdtHits[i_hit].Residual/norm);
      }
    }
    
    m_inner_mdt_hits  = count_inner;
    m_middle_mdt_hits = count_middle;
    m_outer_mdt_hits  = count_outer;
    
    for (int i_station=0; i_station<MAX_STATION; i_station++) {
      for (int i_layer=0; i_layer<8; i_layer++) {

        if ( fabs(pattern.superPoints[i_station].Residual[i_layer]) > ZERO_LIMIT ) 
           m_fit_residuals.push_back(pattern.superPoints[i_station].Residual[i_layer]/norm);

      }
    }

    m_track_pt    = (fabs(pattern.pt ) > ZERO_LIMIT)? pattern.charge*pattern.pt: 9999.;
    m_absolute_pt = fabs(m_track_pt);

    if ( fabs(pattern.etaMap) > ZERO_LIMIT || fabs(pattern.phi) > ZERO_LIMIT ) {
      m_track_eta.push_back(pattern.etaMap);
      m_track_phi.push_back(pattern.phi);
    }
    if ( fabs(pattern.pt ) < ZERO_LIMIT){
      m_failed_eta.push_back(roi->eta());
      m_failed_phi.push_back(roi->phi());
    }

    m_sagitta     = (fabs(pattern.sagitta) > ZERO_LIMIT)? pattern.sagitta: 9999.;
    m_sag_inverse = (fabs(pattern.sagitta) > ZERO_LIMIT)? 1./pattern.sagitta: 9999.;
    m_address     = pattern.s_address;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
