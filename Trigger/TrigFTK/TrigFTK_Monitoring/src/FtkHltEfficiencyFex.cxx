/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_Monitoring/FtkHltEfficiencyFex.h"
#include "TrigFTK_Monitoring/HltTrackComparison.h"

// General ATHENA/Trigger stuff
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

//#include "iostream"

using namespace TrigFTK;
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
  FtkHltEfficiencyFex::FtkHltEfficiencyFex( const std::string& name, ISvcLocator* pSvcLocator )
: HLT::FexAlgo(name, pSvcLocator)
  , m_FTKDataProviderSvc("TrigFTK_DataProviderSvc",name)
  , m_efficiencyAnalysis(new HltTrackComparison())
  , m_purityAnalysis(new HltTrackComparison())
  , m_timer( nTimers, 0 )
  , eventCounter(0)
{
  // declare properties
  declareProperty("FTK_DataProvider", m_FTKDataProviderSvc);
  declareProperty("DeltaRMax", m_deltaR_max = 0.05);

  declareProperty("MinPt",           m_minPt          = 0);
  declareProperty("MaxEta",          m_maxEta         = 10);
  declareProperty("MaxZ0",           m_maxZ0          = 100);
  declareProperty("MaxD0",           m_maxD0          = 2);

  declareProperty("MaxHLTZ0Err",        m_maxHLTZ0err       = 100);
  declareProperty("MaxHLTD0Err",        m_maxHLTD0err       = 100);
  declareProperty("MinHLTSiHits",       m_minHLTSiHits      = 9);
  declareProperty("MinHLTPixHits",      m_minHLTPixHits     = 0);                                           
  declareProperty("MinHLTSctHits",      m_minHLTSctHits     = 0);
  declareProperty("MaxHLTPixHoles",     m_maxHLTPixHoles    = 0);                                           
  declareProperty("MaxHLTSctHoles",     m_maxHLTSctHoles    = 100);
  declareProperty("MaxHLTReducedChi2",  m_maxHLTReducedChi2 = 100);

  declareProperty("MaxFTKZ0Err",        m_maxFTKZ0err       = 100);
  declareProperty("MaxFTKD0Err",        m_maxFTKD0err       = 100);
  declareProperty("MinFTKSiHits",       m_minFTKSiHits      = 9);
  declareProperty("MinFTKPixHits",      m_minFTKPixHits     = 0);                                           
  declareProperty("MinFTKSctHits",      m_minFTKSctHits     = 0);
  declareProperty("MaxFTKPixHoles",     m_maxFTKPixHoles    = 0);                                           
  declareProperty("MaxFTKSctHoles",     m_maxFTKSctHoles    = 100);
  declareProperty("MaxFTKReducedChi2",  m_maxFTKReducedChi2 = 100);

  declareProperty("Numb2DEtaBins",  m_numb2DEtaBins = 4);
  declareProperty("Min2DEta",       m_min2DEta = -3);
  declareProperty("Max2DEta",       m_max2DEta = 3);
  declareProperty("Min2DZ0",        m_min2DZ0 = -100);
  declareProperty("Max2DZ0",        m_max2DZ0 = 100);

  // declare variables for monitoring
  // efficiency variables from m_efficiencyAnalysis
  const IMonitoredAlgo::ContainerReset reset = IMonitoredAlgo::AutoClear; 

  m_efficiencyAnalysis->monitored_ref_track.declareForMonitoring( this, "HLT_trk_");
  m_efficiencyAnalysis->monitored_test_matched_track.declareForMonitoring( this, "FTK_match_trk_");
  m_efficiencyAnalysis->monitored_track_pair.declareForMonitoring( this );
  declareMonitoredStdContainer("FTKHLT_eff_weight",         m_efficiencyAnalysis->m_eff_weight_vec,  reset );
  declareMonitoredVariable("numb_HLT_trks",                 m_efficiencyAnalysis->m_numb_ref_trks );
  declareMonitoredVariable("numb_FTK_trks",                 m_efficiencyAnalysis->m_numb_test_trks );

  // purity variables from m_purityAnalysis
  m_purityAnalysis->monitored_ref_track.declareForMonitoring( this, "FTK_trk_");
  m_purityAnalysis->monitored_ref_unmatched_track.declareForMonitoring( this, "FTK_fake_trk_");
  declareMonitoredStdContainer("FTKHLT_fake_weight",    m_purityAnalysis->m_fake_weight_vec,       reset );  
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
FtkHltEfficiencyFex::~FtkHltEfficiencyFex()
{
  delete m_efficiencyAnalysis;
  delete m_purityAnalysis;
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
HLT::ErrorCode FtkHltEfficiencyFex::hltInitialize()
{
  ATH_MSG_INFO("FtkHltEfficiencyFex::hltInitialize");

  StatusCode sc = m_FTKDataProviderSvc.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Unable to locate FTKDataProviderSvc");
    return HLT::BAD_JOB_SETUP;
  } else {
    ATH_MSG_INFO("Configured to retrieve FTK tracks from FTKDataProviderSvc");
  }

  // configure efficiency tool
  m_efficiencyAnalysis->m_deltaR_max      = m_deltaR_max;
  m_efficiencyAnalysis->m_min_pt          = m_minPt;
  m_efficiencyAnalysis->m_max_eta         = m_maxEta;
  m_efficiencyAnalysis->m_max_z0          = m_maxZ0;
  m_efficiencyAnalysis->m_max_d0          = m_maxD0;

  m_efficiencyAnalysis->m_max_z0err       = m_maxHLTZ0err;
  m_efficiencyAnalysis->m_max_d0err       = m_maxHLTD0err;
  m_efficiencyAnalysis->m_max_reducedChi2 = m_maxHLTReducedChi2;
  m_efficiencyAnalysis->m_min_siHits      = m_minHLTSiHits;
  m_efficiencyAnalysis->m_min_pixHits     = m_minHLTPixHits;
  m_efficiencyAnalysis->m_min_sctHits     = m_minHLTSctHits;
  m_efficiencyAnalysis->m_max_pixHoles    = m_maxHLTPixHoles;
  m_efficiencyAnalysis->m_max_sctHoles    = m_maxHLTSctHoles;

  m_efficiencyAnalysis->m_2d_n_eta        = m_numb2DEtaBins;
  m_efficiencyAnalysis->m_2d_eta_min      = m_min2DEta;
  m_efficiencyAnalysis->m_2d_eta_max      = m_max2DEta;
  m_efficiencyAnalysis->m_2d_z0_min       = m_min2DZ0;
  m_efficiencyAnalysis->m_2d_z0_max       = m_max2DZ0;

  // configure purity tool
  m_purityAnalysis->m_deltaR_max      = m_deltaR_max;
  m_purityAnalysis->m_min_pt          = m_minPt;
  m_purityAnalysis->m_max_eta         = m_maxEta;
  m_purityAnalysis->m_max_z0          = m_maxZ0;
  m_purityAnalysis->m_max_d0          = m_maxD0;

  m_purityAnalysis->m_max_z0err       = m_maxFTKZ0err;
  m_purityAnalysis->m_max_d0err       = m_maxFTKD0err;
  m_purityAnalysis->m_max_reducedChi2 = m_maxFTKReducedChi2;
  m_purityAnalysis->m_min_siHits      = m_minFTKSiHits;
  m_purityAnalysis->m_min_pixHits     = m_minFTKPixHits;
  m_purityAnalysis->m_min_sctHits     = m_minFTKSctHits;
  m_purityAnalysis->m_max_pixHoles    = m_maxFTKPixHoles;
  m_purityAnalysis->m_max_sctHoles    = m_maxFTKSctHoles;

  m_purityAnalysis->m_2d_n_eta        = m_numb2DEtaBins;
  m_purityAnalysis->m_2d_eta_min      = m_min2DEta;
  m_purityAnalysis->m_2d_eta_max      = m_max2DEta;
  m_purityAnalysis->m_2d_z0_min       = m_min2DZ0;
  m_purityAnalysis->m_2d_z0_max       = m_max2DZ0;

  // timers
  m_timer[AlgoTime]           =  addTimer("FtkHltEffEfficiency");
  m_timer[GetFtkTracksTime]   =  addTimer("RetrieveFtkTracks");
  m_timer[GetHltTracksTime]   =  addTimer("RetrieveHltTracks");
  m_timer[EfficiencyToolTime] =  addTimer("EfficiencyTool");
  m_timer[PurityToolTime]     =  addTimer("PurityTool");
  
  return HLT::OK;  
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
HLT::ErrorCode FtkHltEfficiencyFex::hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement*)
{
  ATH_MSG_INFO("FtkHltEfficiencyFex::hltExecute event" << eventCounter++);
  m_timer[AlgoTime]->start();
  
  //To start monitoring:  
  beforeExecMonitors().ignore();

  // clear track containers, track analyses from previous execution
  m_HLTTrkPtr_vec.clear();
  m_FTKTrkPtr_vec.clear();
  m_efficiencyAnalysis->Clear();
  m_purityAnalysis->Clear();

  // Get the ROI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode status = getFeature( input, roiDescriptor );
  if ( status != HLT::OK || !roiDescriptor )
    ATH_MSG_WARNING("No ROI descriptors for this Trigger Element! ");
  else{
    ATH_MSG_DEBUG(
        "Looking at ROI descriptor" 
        << "Using inputTE->getId(): " << input->getId()
        << "; RoI ID = "      << roiDescriptor->roiId() 
        << "; is full scan? " << roiDescriptor->isFullscan()
        << ": Eta = "         << roiDescriptor->eta()
        << ", Phi = "         << roiDescriptor->phi()
        );

    //  Get last track collection attached to this ROI (there should only be one)
    m_timer[GetHltTracksTime]->start();
    const xAOD::TrackParticleContainer* HLT_tpcPtr;
    status = getFeature(input, HLT_tpcPtr);
    if( status == HLT::OK && HLT_tpcPtr ){
      ATH_MSG_DEBUG("Got " << HLT_tpcPtr->size() << " HLT TrackParticles");
      SelectTracks(HLT_tpcPtr, m_HLTTrkPtr_vec);
    }
    else   
      ATH_MSG_DEBUG("Failed to get TrackParticleContainers ");
    m_timer[GetHltTracksTime]->stop();

    // Get FTK tracks in ROI
    m_timer[GetFtkTracksTime]->start(); 
    const xAOD::TrackParticleContainer* FTK_tpcPtr;
    FTK_tpcPtr = m_FTKDataProviderSvc->getTrackParticlesInRoi(*roiDescriptor, 0);
    if( FTK_tpcPtr ){
      ATH_MSG_DEBUG("Got " << FTK_tpcPtr->size() << " FTK TrackParticles in ROI" ); 
      SelectTracks(FTK_tpcPtr, m_FTKTrkPtr_vec);
    }
    else
      ATH_MSG_DEBUG("Failed to get FTK TrackParticleContainer ");
    m_timer[GetFtkTracksTime]->stop();    
    ATH_MSG_DEBUG("FTK retrieval time " << m_timer[GetFtkTracksTime]->elapsed());
  }
  
  // run efficiency analysis
  m_timer[EfficiencyToolTime]->start();
  m_efficiencyAnalysis->TrackAnalyisis(m_HLTTrkPtr_vec, m_FTKTrkPtr_vec);
  m_timer[EfficiencyToolTime]->stop();
  // perform regular efficiency analyisis with test and reference tracks swapped
  // works only if only FTK tracks in ROIs are selected so that there is indeed
  // 1to1 overlap between HLT and FTK tracks
  m_timer[PurityToolTime]->start();
  m_purityAnalysis->TrackAnalyisis(m_FTKTrkPtr_vec, m_HLTTrkPtr_vec, true);
  m_timer[PurityToolTime]->stop();

  //To stop monitoring:
  afterExecMonitors().ignore();

  m_timer[AlgoTime]->stop();
  ATH_MSG_DEBUG("execution time " << m_timer[AlgoTime]->lastElapsed());
  return HLT::OK;  
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
HLT::ErrorCode FtkHltEfficiencyFex::hltFinalize()
{
  ATH_MSG_INFO("FtkHltEfficiencyFex::hltFinalize");
  return HLT::OK;
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void FtkHltEfficiencyFex::SelectTracks(
    const xAOD::TrackParticleContainer* input_tpcPtr, std::vector<const xAOD::TrackParticle*>& output_trkPtr_vec)
{
  if(!input_tpcPtr) return;
  xAOD::TrackParticleContainer::const_iterator  trackItr = input_tpcPtr->begin();
  xAOD::TrackParticleContainer::const_iterator  trackEnd = input_tpcPtr->end();
  output_trkPtr_vec.reserve(input_tpcPtr->size());
  for( ; trackItr != trackEnd; ++trackItr ) {
    const xAOD::TrackParticle* track = *trackItr;
    output_trkPtr_vec.push_back(track);
  }  
}

