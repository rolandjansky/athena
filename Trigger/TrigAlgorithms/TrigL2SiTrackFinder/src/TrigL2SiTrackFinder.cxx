/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigL2SiTrackFinder.cxx
// -------------------------------
// ATLAS Collaboration
//
// package created 19/07/2010 by Dmitry Emeliyanov (see ChangeLog for more details)
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <algorithm>

#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IssueSeverity.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigSiSpacePointTool/ISpacePointProvider.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetToolInterfaces/ITrigTRT_TrackExtensionTool.h"
#include "TrigInDetToolInterfaces/TrigL2HitResidual.h"
#include "TrigInDetToolInterfaces/ITrigL2ResidualCalculator.h"

#include "TrigInDetRecoTools/ITrigL2PattRecoStrategy.h"

#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 

#include "IRegionSelector/IRegSelSvc.h"
// #include "RegionSelector/RegSelSvc.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigL2SiTrackFinder/TrigL2SiTrackFinder.h"

template <class SRC>
inline const DataVector<TrigInDetTrack>** dvec_cast(SRC** ptr) { 
  return (const DataVector<TrigInDetTrack>**)(ptr); 
} 

TrigL2SiTrackFinder::TrigL2SiTrackFinder(const std::string& name, ISvcLocator* pSvcLocator) : 

  HLT::FexAlgo(name, pSvcLocator), 

  m_spacePointProvider("OnlineSpacePointProviderTool"),
  m_trigInDetTrackFitter("TrigInDetTrackFitter"),
  m_trigTRT_TrackExtensionTool("TrigTRT_TrackExtensionTool"),
  m_trigL2ResidualCalculator("TrigL2ResidualCalculator"),
  m_recoStrategy("TrigL2PattRecoStrategyC"),

  m_useBeamSpot(true),
  m_doTrackFit(true),
  m_doShift(false), 
  //m_generateRoI(false), 
  m_useRoiDescriptorWidths(false), 
  m_pixelSpacePointsContainer(nullptr),
  m_sctSpacePointsContainer(nullptr),
  m_ntracks(0),
  m_nPixSPsInRoI(0),
  m_nSCTSPsInRoI(0),
  m_currentStage(-1),
  m_attachedFeatureName(""),
  m_attachedFeatureName2("")

{
  declareProperty( "etaHalfWidth",        m_etaHalfWidth = 0.1);
  declareProperty( "phiHalfWidth",        m_phiHalfWidth = 0.1);
  declareProperty( "FullScanMode",        m_fullScanMode = false);
  declareProperty( "VertexSeededMode",    m_vertexSeededMode = false);
  //declareProperty( "GenerateRoI",         m_generateRoI = false);
  declareProperty( "useRoiDescriptorWidths", m_useRoiDescriptorWidths = false);

  declareProperty( "PixelSP_ContainerName", m_pixelSpContName = "TrigPixelSpacePoints" );
  declareProperty( "SCT_SP_ContainerName",  m_sctSpContName = "TrigSCT_SpacePoints" );

  declareProperty( "RegionSelectorService",    m_regionSelectorName = "RegSelSvc" );
  declareProperty( "SpacePointProviderTool", m_spacePointProvider  );
  declareProperty( "TrigInDetTrackFitter",   m_trigInDetTrackFitter );
  declareProperty( "TrigTRT_TrackExtensionTool", m_trigTRT_TrackExtensionTool);
  declareProperty( "TrigL2ResidualCalculator", m_trigL2ResidualCalculator);

  declareProperty( "RecoStrategy",m_recoStrategy);

  declareProperty( "MinHits",               m_minHits = 5 );
  //  declareProperty( "TrkTrackCollName",      m_TrkTrackCollName = "L2STAR_Tracks");

  declareProperty( "OutputCollectionSuffix",m_outputCollectionSuffix = "");
 
  declareProperty( "PrintDiagnosticMessages", m_printDiagnosticMessages = false );
  
  
  declareProperty("TrackSummaryTool", m_trackSummaryTool);
  // declareProperty( "TrueVertexLocation",      m_trueVertexLocation = "TrueTrigVertexColl" );
  // declareProperty( "TrigToTrkTrackConvertorTool",m_convertorTool);
  //  declareProperty( "trackConvertorMode",    m_trackConvertorMode = 1 );

  declareProperty( "UseBeamSpot",           m_useBeamSpot = true);

  declareProperty( "doTrackFit",      m_doTrackFit = true );
  declareProperty( "doTRTpropagation",      m_doTRTpropagation = true );
  declareProperty( "doResMonitoring",       m_doResMonitoring = false);
  declareProperty( "attachVertices",        m_attachVertices = false);
  /*declareProperty( "AdjustLayerThreshold",  m_adjustLayerThreshold = false );
  declareProperty( "DetMaskCheck",          m_check_detector_mask = false);
  declareProperty( "DetMaskCheckOnEvent",   m_detector_mask_on_event = false);*/

  

  m_countTotalEvents=0;
  m_countTotalRoI=0;
  m_countRoIwithEnoughHits=0;
  m_countRoIwithTracks=0;
  m_l1Id=99999999;
  
  m_recoTracks=NULL;

  // declare monitoring histograms


  declareMonitoredCollection("trk_pt",         *dvec_cast(&m_recoTracks), &monPt);
  declareMonitoredCollection("trk_a0",         *dvec_cast(&m_recoTracks), &monA0);
  declareMonitoredCollection("trk_z0",         *dvec_cast(&m_recoTracks), &monZ0);
  declareMonitoredCollection("trk_phi0",       *dvec_cast(&m_recoTracks), &monPhi0);
  declareMonitoredCollection("trk_eta",        *dvec_cast(&m_recoTracks), &monEta);
  declareMonitoredCollection("trk_chi2dof",    *dvec_cast(&m_recoTracks), &monChi2);
  declareMonitoredCollection("trk_nSiHits",    *dvec_cast(&m_recoTracks), &monNHit_Si);
  declareMonitoredCollection("trk_nPIXHits",   *dvec_cast(&m_recoTracks), &TrigInDetTrack::NPixelSpacePoints); 
  declareMonitoredCollection("trk_nSCTHits",   *dvec_cast(&m_recoTracks), &TrigInDetTrack::NSCT_SpacePoints); 
  declareMonitoredCollection("trk_nTRTHits",   *dvec_cast(&m_recoTracks), &TrigInDetTrack::NTRHits);
  declareMonitoredCollection("trk_nTRTStraws", *dvec_cast(&m_recoTracks), &TrigInDetTrack::NStrawHits);
  declareMonitoredStdContainer("trk_a0beam",m_a0beam);
  declareMonitoredStdContainer("trk_dPhi0",m_trkdPhi0);
  declareMonitoredStdContainer("trk_dEta" ,m_trkdEta);

  declareMonitoredStdContainer("sp_x" ,m_sp_x);
  declareMonitoredStdContainer("sp_y" ,m_sp_y);
  declareMonitoredStdContainer("sp_z" ,m_sp_z);
  declareMonitoredStdContainer("sp_r" ,m_sp_r);

  declareMonitoredStdContainer("hit_PIXBarrelPhiResidual",m_pixResPhiBarrel);
  declareMonitoredStdContainer("hit_PIXBarrelEtaResidual",m_pixResEtaBarrel);
  declareMonitoredStdContainer("hit_PIXBarrelPhiPull",    m_pixPullPhiBarrel);
  declareMonitoredStdContainer("hit_PIXBarrelEtaPull",    m_pixPullEtaBarrel);
  declareMonitoredStdContainer("hit_SCTBarrelResidual",   m_sctResBarrel);
  declareMonitoredStdContainer("hit_SCTBarrelPull",       m_sctPullBarrel);
  declareMonitoredStdContainer("hit_PIXEndCapPhiResidual",m_pixResPhiEC);
  declareMonitoredStdContainer("hit_PIXEndCapEtaResidual",m_pixResEtaEC);
  declareMonitoredStdContainer("hit_PIXEndCapPhiPull",    m_pixPullPhiEC);
  declareMonitoredStdContainer("hit_PIXEndCapEtaPull",    m_pixPullEtaEC);
  declareMonitoredStdContainer("hit_SCTEndCapResidual",   m_sctResEC);
  declareMonitoredStdContainer("hit_SCTEndCapPull",       m_sctPullEC);

  declareMonitoredStdContainer("dataErrors_SCT",  m_sctDataErrors);
  declareMonitoredStdContainer("dataErrors_PIX",  m_pixelDataErrors);
  declareMonitoredStdContainer("dataErrors_TRT",  m_trtDataErrors);
  declareMonitoredStdContainer("dataErrors",      m_dataErrors);
  declareMonitoredVariable("roi_nTracks",m_ntracks);
  declareMonitoredVariable("roi_nSPsPIX",m_nPixSPsInRoI);
  declareMonitoredVariable("roi_nSPsSCT",m_nSCTSPsInRoI);
  declareMonitoredVariable("roi_lastStageExecuted",m_currentStage);
  declareMonitoredVariable("roi_eta", m_roiEta);
  declareMonitoredVariable("roi_etaWidth", m_roiEtaWidth);
  declareMonitoredVariable("roi_phi", m_roiPhi);
  declareMonitoredVariable("roi_phiWidth", m_roiPhiWidth);
  declareMonitoredVariable("roi_nSPs", m_roi_nSPs);

  // Z-vertexing 
  declareMonitoredVariable("roi_nZvertices",m_nZvertices);
  declareMonitoredStdContainer("roi_zVertices",m_zVertices);
}

//--------------------------------------------------------------------------

TrigL2SiTrackFinder::~TrigL2SiTrackFinder() {}

//-----------------------------------------------------------------------

HLT::ErrorCode TrigL2SiTrackFinder::hltInitialize() {

  msg() << MSG::INFO << "TrigL2SiTrackFinder::initialize() "  << PACKAGE_VERSION << endreq;

  StatusCode sc = StatusCode::FAILURE;
  
  if (m_printDiagnosticMessages) 
    msg() << MSG::WARNING << "TrigL2SiTrackFinder Diagnostic Messages enabled - this will give many messages per event to std::cout !!" 
	  << endreq;
  
  sc = m_spacePointProvider.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate SpacePointProviderTool " << m_spacePointProvider << endreq;
    return HLT::BAD_JOB_SETUP;
  } 

  sc = m_trigInDetTrackFitter.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrackFitter tool " << m_trigInDetTrackFitter << endreq;
    return HLT::BAD_JOB_SETUP;
  }      
  
  if(m_doTRTpropagation) {
    sc = m_trigTRT_TrackExtensionTool.retrieve();
    if ( sc.isFailure() ) {
      msg() << MSG::FATAL <<"Unable to locate TRT track extension tool " << m_trigTRT_TrackExtensionTool << endreq;
      return HLT::BAD_JOB_SETUP;
    }
  }

  sc = m_trigL2ResidualCalculator.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL <<"Unable to locate Residual calculator tool " << m_trigL2ResidualCalculator << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  sc = m_recoStrategy.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL <<"Unable to locate Reconstruction Tool " << m_recoStrategy << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if ( sc.isFailure() ) { 
    msg() << MSG::ERROR << "DetStore service not found" << endreq;
    return HLT::BAD_JOB_SETUP;
  }
  if (detStore->retrieve(m_pixelId, "PixelID").isFailure()) { 
    msg() << MSG::ERROR << "Could not get Pixel ID helper" << endreq;
    return HLT::BAD_JOB_SETUP;
  }
  if (detStore->retrieve(m_sctId, "SCT_ID").isFailure()) {  
    msg() << MSG::ERROR << "Could not get SCT ID helper" << endreq;
    return HLT::BAD_JOB_SETUP;
  }


  /*if ( m_adjustLayerThreshold ) { 
    msg() << MSG::INFO << "will adjust layer threshold depending on disabled modules" << endreq;
    sc = serviceLocator()->service( m_regionSelectorName, m_regionSelector);
    if ( sc.isFailure() ) {
      msg() << MSG::FATAL << "Unable to retrieve RegionSelector Service  " << m_regionSelectorName << endreq;
      return HLT::BAD_JOB_SETUP;
    };
  }

  m_detector_mask_not_checked = m_check_detector_mask;

  m_hasPixelBLayer = true;
  m_hasPixelBarrel = true; m_hasPixelDisk  = true;
  m_hasSCTBarrelA  = true; m_hasSCTEndcapA = true;
  m_hasSCTBarrelC  = true; m_hasSCTEndcapC = true;*/

  declareMonitoredObject("time_TRTDataPrep",m_trigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRTTotalTime);
  declareMonitoredObject("time_TRTBStoDC",m_trigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRTTrtCnvTime);
  declareMonitoredObject("time_TRTRobProv",m_trigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRTRobProvTime);
  declareMonitoredObject("time_TRTDataTotal",m_trigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRT_DataProviderTime);
  declareMonitoredObject("time_TRTRobPreload",m_trigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRT_RobPreloaderTime);
  declareMonitoredObject("time_TRTRoads",m_trigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRT_RoadBuilderTime);
  declareMonitoredObject("time_TRTPDAF",m_trigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRT_PDAF_TrackingTime);
  declareMonitoredObject("time_PIXBStoClusters",m_spacePointProvider.operator*(),&ISpacePointProvider::getBStoClustersTimingPixel);
  declareMonitoredObject("time_SCTBStoClusters",m_spacePointProvider.operator*(),&ISpacePointProvider::getBStoClustersTimingSCT);
  declareMonitoredObject("time_PIXSPFormation",m_spacePointProvider.operator*(),&ISpacePointProvider::getSPformationTimingPixel);
  declareMonitoredObject("time_SCTSPFormation",m_spacePointProvider.operator*(),&ISpacePointProvider::getSPformationTimingSCT);
  declareMonitoredObject("time_SiDataRegSel",m_spacePointProvider.operator*(),&ISpacePointProvider::getRegSelTiming);
  declareMonitoredObject("time_SiDataTotal",m_spacePointProvider.operator*(),&ISpacePointProvider::getTotalTiming);

  declareMonitoredObject("time_PattRecoOnly",m_recoStrategy.operator*(),&ITrigL2PattRecoStrategy::getTimingA);
  //   add some timers:
  
  if ( timerSvc() ) {
    m_SpacePointReaderTimer = addTimer("SpacePointReader"); 
    m_PatternRecoTimer      = addTimer("PattReco","PattReco_nSP");
    m_TrackFitterTimer      = addTimer("TrackFitter","TrackFitter_nTracks");
    m_TrtExtensionTimer     = addTimer("TrtExtension", "TrtExtension_nTracks");
  }

  msg() << MSG::INFO << " TrigL2SiTrackFinder : MinHits set to " << m_minHits << endreq;

  if (m_useBeamSpot) {
    StatusCode scBS = service("BeamCondSvc", m_iBeamCondSvc);
    if (scBS.isFailure() || m_iBeamCondSvc == 0) {
      m_iBeamCondSvc = 0;
      msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endreq;
    }
  }

  if ( m_outputCollectionSuffix != "" )  {
    m_attachedFeatureName = string("TrigL2SiTrackFinder_") + m_outputCollectionSuffix;  
    m_attachedFeatureName2 = string("TrigL2SiTrackFinder_TrkTrack_") + m_outputCollectionSuffix;  

  }
  else {
    string namestr = name();
    std::transform(namestr.begin(), namestr.end(),
		   namestr.begin(), (int(*)(int)) std::tolower);
    if ( namestr.find("cosmic") == string::npos ) {
      m_attachedFeatureName = name();
      m_attachedFeatureName2 = m_attachedFeatureName; 
      m_attachedFeatureName2.append("_TrkTrack"); 
    }
    else {
      m_attachedFeatureName = string("TrigL2SiTrackFinder");
      m_attachedFeatureName2 = string("TrigL2SiTrackFinder_TrkTrack");
    }
  }
  msg() << MSG::DEBUG << " Features recorded with Key " << m_attachedFeatureName << endreq;
  msg() << MSG::DEBUG << " Features recorded with Key " << m_attachedFeatureName2 << endreq;

  sc= m_trackSummaryTool.retrieve();
  if(sc.isFailure()) {
    msg() << MSG::ERROR << "unable to locate track summary tool" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

   

  msg() << MSG::DEBUG << " Initialized successfully" << endreq; 
  return HLT::OK;  
}


//-------------------------------------------------------------------------

HLT::ErrorCode TrigL2SiTrackFinder::hltStart()
{
  // Reset warnings to make them reproducible on stop/start
  m_roiForIDWarning = false;
  m_roiWidthWarning = false;
  return HLT::OK;
}

//-------------------------------------------------------------------------

HLT::ErrorCode TrigL2SiTrackFinder::hltExecute(const HLT::TriggerElement* inputTE,
					       HLT::TriggerElement* outputTE) {

  m_ntracks = 0;
  m_recoTracks=nullptr;
  m_recoTrkTracks=nullptr;
  m_dataErrors.clear();
  m_pixelDataErrors.clear();
  m_sctDataErrors.clear();
  m_trtDataErrors.clear();
  m_a0beam.clear();
  m_trkdPhi0.clear();
  m_trkdEta.clear();

  m_sp_x.clear();m_sp_y.clear();m_sp_z.clear();m_sp_r.clear();

  m_zVertices.clear();

  m_pixResPhiBarrel.clear();
  m_pixResEtaBarrel.clear();
  m_pixPullPhiBarrel.clear();
  m_pixPullEtaBarrel.clear();
  m_sctResBarrel.clear();
  m_sctPullBarrel.clear();
  m_pixResPhiEC.clear();
  m_pixResEtaEC.clear();
  m_pixPullPhiEC.clear();
  m_pixPullEtaEC.clear();
  m_sctResEC.clear();
  m_sctPullEC.clear();

  m_nPixSPsInRoI=0;
  m_nSCTSPsInRoI=0;
  m_currentStage=0;
  m_roi_nSPs=0;
  m_nZvertices=0;

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigL2SiTrackFinder::execHLTAlgorithm()" << endreq;

  // 1. Event information, masking update
  
  // get the run and event no.
  /*const EventInfo* pEventInfo;
  unsigned int IdRun=0;
  unsigned int IdEvent=0;
  if ( store()->retrieve(pEventInfo).isFailure() ) {
    if(m_detector_mask_not_checked && m_detector_mask_on_event) {
      msg() << MSG::ERROR << "Could not find EventInfo object for detector mask info" << endreq;
      return HLT::SG_ERROR;
    }
    // if we are not interested in the detector mask, this is a minor problem.
    if (msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
  } 
  else {
    IdRun   = pEventInfo->event_ID()->run_number();
    IdEvent = pEventInfo->event_ID()->event_number();
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent << endreq;    
    if(m_detector_mask_not_checked && m_detector_mask_on_event)
      setup_detector_mask(pEventInfo->event_ID());
      }*/

  // Retrieve vertexing information if needed
  const TrigVertexCollection* vertexCollection = NULL;
  if(m_vertexSeededMode) {
    //HLT::ErrorCode status = getFeature(inputTE, vertexCollection,"");
    //
    //NOTE the inputTE vs outputTE difference - the feature is assumed to come from the same step in the sequence
    HLT::ErrorCode status = getFeature(outputTE, vertexCollection);
    if(status != HLT::OK) return status;
    if(vertexCollection==NULL) return HLT::ERROR;
  }
    

  // FullScan and RoI split here
 
  StatusCode scOSP=StatusCode::SUCCESS;
  std::vector<int> listOfPixIds;
  std::vector<int> listOfSctIds;
  m_currentStage = 1;

  m_recoTracks = new TrigInDetTrackCollection;

  if(m_fullScanMode) // full-scan strategy
    {
      // 4. Data retrieval and spacepoint acceptance test(s)

      m_filteredSpacePoints.clear();

      if ( timerSvc() ) m_SpacePointReaderTimer->start();

      scOSP=m_spacePointProvider->fillCollections(true,true,listOfPixIds,listOfSctIds);

      if(scOSP.isRecoverable()||scOSP.isSuccess()) { 

	HLT::ErrorCode code = retrieveSpacePointsContainers();
	if (code != HLT::OK) {
	  if ( timerSvc() ) m_SpacePointReaderTimer->stop();
	  delete m_recoTracks;
	  m_recoTracks=NULL;
	  return code;
	}

	FullScanFilter filter(m_filteredSpacePoints);
	SpacePointSelector<FullScanFilter> selector(filter);
	
	m_nPixSPsInRoI=selector.select(m_pixelSpacePointsContainer,listOfPixIds);
	m_nSCTSPsInRoI=selector.select(m_sctSpacePointsContainer,listOfSctIds);
	
	m_roi_nSPs = m_filteredSpacePoints.size();

	if ( timerSvc() ) m_SpacePointReaderTimer->stop();
	if ( timerSvc() ) m_PatternRecoTimer->start();

	HLT::ErrorCode hltCode;
	if(!m_vertexSeededMode)
	  hltCode = m_recoStrategy->findTracks(m_filteredSpacePoints, 
					       nullptr,
					       m_recoTracks);
	else
	  hltCode = m_recoStrategy->findTracks(m_filteredSpacePoints, 
					       vertexCollection,
					       m_recoTracks);
      
	if ( timerSvc() ) m_PatternRecoTimer->propVal( m_recoTracks->size() );
	if ( timerSvc() ) m_PatternRecoTimer->stop();

	if (hltCode != HLT::OK) {
	  delete m_recoTracks;
	  m_recoTracks=nullptr;
	  return hltCode;
	}

      }
      else if ( timerSvc() ) m_SpacePointReaderTimer->stop();
    }
  else // RoI-guided strategy
    {
      const IRoiDescriptor* internalRoI;// will be used for all pattern recognition tools in RoI-guided mode

      // 2. RoI preparation/update 
      
      const TrigRoiDescriptor* roi = nullptr;
      if( getFeature(inputTE, roi, "forID") != HLT::OK || roi == nullptr ) {
        if (!m_roiForIDWarning) {
          ATH_MSG_INFO("REGTEST / using ordinary RoI ( no forID RoI found ) ");
          m_roiForIDWarning = true;
        }
        getFeature(inputTE, roi);
      }
      if ( roi==nullptr ) {
	msg() <<  MSG::WARNING << "REGTEST / Failed to find RoiDescriptor " << endreq;
	delete m_recoTracks;
	m_recoTracks=nullptr;
	return HLT::NAV_ERROR;
      }
      internalRoI = roi;

      // Important: copy constructor of TrigRoiDescriptor does not exist - compiler 
      // generates the ctor for us, but the
      // automatically generated ctor does not check roi phi/eta/zed ranges
      // We may want to implement the copy constructor properly. Until then...
      // check validity of internalRoI
      
      m_roiEta = internalRoI->eta();
      m_roiEtaWidth = internalRoI->etaPlus() - internalRoI->etaMinus();
      m_roiPhi = internalRoI->phi();
      m_roiPhiWidth = HLT::wrapPhi(internalRoI->phiPlus() - internalRoI->phiMinus());
      
      if(msgLvl()<=MSG::DEBUG) {
        msg() <<  MSG::DEBUG << "REGTEST / RoI: " << *roi << endreq;
      }
      m_countTotalRoI++;
      if(roi->etaMinus()== roi->etaPlus() || roi->phiMinus()== roi->phiPlus()){// use values from configurable
	if (!m_roiWidthWarning) {
	  msg() << MSG::WARNING << "REGTEST / RoI width not set properly" << " " << *roi << endreq;
	  m_roiWidthWarning=true;
	}
	TrigRoiDescriptor* _roi = new TrigRoiDescriptor( m_roiEta, m_roiEta-m_etaHalfWidth, m_roiEta+m_etaHalfWidth, 
							 m_roiPhi, m_roiPhi-m_phiHalfWidth, m_roiPhi+m_phiHalfWidth ); 
	attachFeature( outputTE, _roi );
	internalRoI = _roi;
      }

     
      // 4. Data retrieval and spacepoint acceptance test(s)
      //retrieve input spacepoints using internalRoI

      m_filteredSpacePoints.clear();
      
      if ( timerSvc() ) m_SpacePointReaderTimer->start();

      scOSP=m_spacePointProvider->fillCollections(*internalRoI,true,true,listOfPixIds,listOfSctIds);

      if(scOSP.isRecoverable()||scOSP.isSuccess()) { 

	HLT::ErrorCode code = retrieveSpacePointsContainers();
	if (code != HLT::OK) {
	  if ( timerSvc() ) m_SpacePointReaderTimer->stop();
	  delete m_recoTracks;
	  m_recoTracks=NULL;
	  return code;
	}

	/// filter the spacepoints according to the RoI
	RoI_Filter filter( m_filteredSpacePoints, internalRoI );
	SpacePointSelector<RoI_Filter> selector(filter);
	m_nPixSPsInRoI=selector.select(m_pixelSpacePointsContainer,listOfPixIds);
	m_nSCTSPsInRoI=selector.select(m_sctSpacePointsContainer,listOfSctIds);
	

	m_roi_nSPs = m_filteredSpacePoints.size();

	

	/** Debug info. */
	if(msgLvl()<=MSG::DEBUG) {
	    msg() << MSG::DEBUG << "SEED DATA ACCESS: found " << m_filteredSpacePoints.size() << " space points" << endreq;
	}

	if ( timerSvc() ) m_SpacePointReaderTimer->stop();
      
	// do the rest of RoI-guided pattern recognition

	if ( timerSvc() ) m_PatternRecoTimer->start();

	HLT::ErrorCode hltCode;
	if(!m_vertexSeededMode)
	  hltCode = m_recoStrategy->findTracks(m_filteredSpacePoints,
					       *internalRoI,
					       NULL,
					       m_recoTracks);
	else
	  hltCode = m_recoStrategy->findTracks(m_filteredSpacePoints,
					       *internalRoI,
					       vertexCollection,
					       m_recoTracks);
	
	if ( timerSvc() ) m_PatternRecoTimer->propVal( m_recoTracks->size() );
	if ( timerSvc() ) m_PatternRecoTimer->stop();

	if (hltCode != HLT::OK) {
	  delete m_recoTracks;
	  m_recoTracks=NULL;
	  return hltCode;
	}
      }
      else if ( timerSvc() ) m_SpacePointReaderTimer->stop();
    }

  if(scOSP.isRecoverable()) {
    m_dataErrors.push_back(1);
    const std::vector<int>* errVect = m_spacePointProvider->fillPixelDataErrors();
    std::copy(errVect->begin(),errVect->end(),std::back_inserter(m_pixelDataErrors));
    errVect = m_spacePointProvider->fillSCT_DataErrors();
    std::copy(errVect->begin(),errVect->end(),std::back_inserter(m_sctDataErrors));
  }
  else if(scOSP.isFailure()) {
    msg() << MSG::WARNING << "SpacePoint formation failed " << endreq; 
    m_dataErrors.push_back(0);
    delete m_recoTracks;
    m_recoTracks=NULL;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::CORRUPTED_ROD); 
  }

  if(scOSP.isRecoverable()||scOSP.isSuccess()) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Modules hit in RoI: PIX = " << listOfPixIds.size()
				      << " , SCT = " << listOfSctIds.size()
				      << endreq;
  }
      
  if( m_roi_nSPs >= m_minHits ) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST / Found " << m_roi_nSPs << " space points." << endreq;
    m_countRoIwithEnoughHits++;
  }
  else {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "No tracks found - too few hits in ROI to run " << m_roi_nSPs << endreq;
    HLT::ErrorCode code = attachFeature(outputTE, new TrigInDetTrackCollection, m_attachedFeatureName);

    if ( code != HLT::OK ) {
      msg() << MSG::ERROR << "REGTEST/ Write into outputTE failed" << endreq;
      delete m_recoTracks;
      m_recoTracks=NULL;
      return code;
    }

    code = attachFeature(outputTE, new TrackCollection, m_attachedFeatureName2);
    if ( code != HLT::OK ) {
      return code;
    }
  }
      
  m_currentStage = 2;
      
  // 5. missing layers determination 
  //  int missing_layers = findMissingLayers(roiEtaMin,roiEtaMax,roiPhiMin,roiPhiMax); 
  //int missing_layers = findMissingLayers(internalRoI);
      
  // 6. Extract the beamspot shift
  double shiftx(0), shifty(0);
  if (!m_doShift && m_useBeamSpot && m_iBeamCondSvc) {
	
    Amg::Vector3D vertex = m_iBeamCondSvc->beamPos();
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Beam spot position " << vertex << endreq;
    double xVTX = vertex.x();
    double yVTX = vertex.y();
    double zVTX = vertex.z();
    double tiltXZ = m_iBeamCondSvc->beamTilt(0);
    double tiltYZ = m_iBeamCondSvc->beamTilt(1);
    shiftx = xVTX - tiltXZ*zVTX;//correction for tilt
    shifty = yVTX - tiltYZ*zVTX;//correction for tilt
	
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Center position:  " << shiftx <<"  "<< shifty << endreq;
  }
      
  // 7. actual pattern recognition, for example
  // m_recoTracks=pattRecoTool->findTracks(m_filteredSpacePoints) 
  // or
  // StatusCode pattRecSC = pattRecoTool->findTracks(m_filteredSpacePoints, m_recoTracks) 
  // or
  // if(doShift) { or maybe add a special flag doCosmic ?


  // 7a. run shifter (for cosmic reco) and 
  // 7b. pattRecoTool

  // m_recoTracks=NULL;
  // if(m_recoTracks->empty()) {
  //  delete m_recoTracks;
  //  m_recoTracks=NULL;
  // }

  /*
  if ( timerSvc() ) {
    if (m_recoTracks!=NULL) 
      m_PatternRecoTimer->propVal( m_recoTracks->size() );
    else 
      m_PatternRecoTimer->propVal(0);
    m_PatternRecoTimer->stop();
  }
  */

  if( m_recoTracks->empty() ) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST / No tracks reconstructed" << endreq;
    if (m_printDiagnosticMessages) std::cout << "TrigL2SiTrackFinder DIAGNOSTIC: No tracks reconstructed" << std::endl;
    //     StatusCode sc = m_EventData2XML->execute();
  }

  //monitor Z-vertexing

  m_recoStrategy->getZvertices(m_zVertices);
  m_nZvertices=m_zVertices.size();
	msg() << MSG::VERBOSE << "vertices.size(): " << m_zVertices << endreq;
  if(msgLvl() <= MSG::VERBOSE) {
    for (auto vertex : m_zVertices) {
      msg() << MSG::VERBOSE << "vertex " << vertex << endreq;
    }
  }

  //monitor number of tracks
  m_ntracks=m_recoTracks->size();	
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
				    << "REGTEST / Found " << m_recoTracks->size() 
				    << " tracks. Proceeding to track fitter." << endreq;
  if (m_printDiagnosticMessages) std::cout << "TrigL2SiTrackFinder DIAGNOSTIC " <<  " Found " << m_recoTracks->size() 
					   << " tracks. Proceeding to track fitter." << std::endl;
  if( !m_recoTracks->empty() )
    m_countRoIwithTracks++;
  
  if ( timerSvc() ) m_TrackFitterTimer->start();

  if(m_doTrackFit) {

    m_trigInDetTrackFitter->fit( m_recoTracks);
	
  }

  ///////////// fill vectors of quantities to be monitored
  // a0 wrt beam spot
  for(TrigInDetTrackCollection::iterator trackIt = m_recoTracks->begin();trackIt != m_recoTracks->end();++trackIt) {
    double a0 = (*trackIt)->param()->a0();
    double phi0 = (*trackIt)->param()->phi0();
    m_a0beam.push_back(a0+shiftx*sin(phi0)-shifty*cos(phi0));
    double dPhi0 = phi0 - m_roiPhi;
    if ( dPhi0> 2*M_PI ) dPhi0 -= 2*M_PI;
    if ( dPhi0<-2*M_PI ) dPhi0 += 2*M_PI;
    m_trkdPhi0.push_back(dPhi0);
    m_trkdEta.push_back((*trackIt)->param()->eta() - m_roiEta);

    for(std::vector<const TrigSiSpacePoint*>::iterator spIt = (*trackIt)->siSpacePoints()->begin();spIt!=(*trackIt)->siSpacePoints()->end();++spIt) {
      m_sp_x.push_back((*spIt)->original_x());
      m_sp_y.push_back((*spIt)->original_y());
      m_sp_z.push_back((*spIt)->z());
      m_sp_r.push_back((*spIt)->r());
    }

    //zVTX = m_zPosition; // precise calculation using vertex z, not the beamspot cond service - not needed
  }

  // hit residuals
  if (m_doResMonitoring) {
    for(TrigInDetTrackCollection::iterator trackIt = m_recoTracks->begin();trackIt != m_recoTracks->end();++trackIt)
      runResidualMonitoring(*trackIt);
  } // if doResMonitoring
      
  if ( timerSvc() ) { 
    m_TrackFitterTimer->propVal( m_recoTracks->size() );
    m_TrackFitterTimer->stop();
  }
    
  m_currentStage = 6;

  if(m_doTRTpropagation) {
    if ( timerSvc() ) m_TrtExtensionTimer->start();
    StatusCode sc=m_trigTRT_TrackExtensionTool->propagate(m_recoTracks);
    if ( timerSvc() ) { 
      m_TrtExtensionTimer->propVal(m_recoTracks->size() );
      m_TrtExtensionTimer->stop();
    }
    if(sc.isRecoverable()) {
      m_dataErrors.push_back(3);
      const std::vector<int>* errVect = m_trigTRT_TrackExtensionTool->fillTRT_DataErrors();
      std::copy(errVect->begin(),errVect->end(),std::back_inserter(m_trtDataErrors));
    }
    else if(sc.isFailure()) {
      msg() << MSG::WARNING <<"Bad TRT data"<<endreq;
      m_dataErrors.push_back(2);
      delete m_recoTracks;
      m_recoTracks=NULL;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::CORRUPTED_ROD);
    }
  }
  m_currentStage = 7; 

  if ( msgLvl() <= MSG::DEBUG || m_printDiagnosticMessages) { 
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST / Reconstructed " << m_recoTracks->size() << " tracks " << endreq;
    if (m_printDiagnosticMessages) std::cout << "TrigL2SiTrackFinder DIAGNOSTIC " << " Reconstructed " << m_recoTracks->size() << " tracks " << std::endl;
    TrigInDetTrackCollection::iterator track = m_recoTracks->begin();
    TrigInDetTrackCollection::iterator lastTrack = m_recoTracks->end();
    
    for(; track !=lastTrack; track++)	{
      
      msg() << MSG::DEBUG << "REGTEST / track nstraw/ntr/phi0/pt/eta/d0/z0/chi2: " <<
	(*track)->NStrawHits() << " / "  << 
	(*track)->NTRHits() << " / "  << 
	(*track)->param()->phi0() << " / "  << 
	(*track)->param()->pT() << " / " << 
	(*track)->param()->eta() << " / " << 
	(*track)->param()->a0() << " / " <<
	(*track)->param()->z0() << " / " <<
	(*track)->chi2() << endreq;
      
      if (m_printDiagnosticMessages) std::cout << "TrigL2SiTrackFinder DIAGNOSTIC " << " track nstraw/ntr/phi0/pt/eta/d0/z0/chi2: " <<
	(*track)->NStrawHits() << " / "  << 
	(*track)->NTRHits() << " / "  << 
	(*track)->param()->phi0() << " / "  << 
	(*track)->param()->pT() << " / " << 
	(*track)->param()->eta() << " / " << 
	(*track)->param()->a0() << " / " <<
	(*track)->param()->z0() << " / " <<
	(*track)->chi2() << std::endl;
    }
  }
  // Store RoI ID
  /*
    if (m_generateRoI) {
    m_recoTracks->RoI_ID (-2);
    } else {
    m_recoTracks->RoI_ID (roiDescr.roiId());
    }
  */
 
  HLT::ErrorCode code = attachFeature(outputTE, m_recoTracks, m_attachedFeatureName);

  if ( code != HLT::OK ) {
    msg() << MSG::ERROR << "REGTEST/ Write into outputTE failed" << endreq;
    delete m_recoTracks;
    m_recoTracks=NULL;
    return code;
  }

  m_recoTrkTracks = new TrackCollection;
  convertToTrkTrack(m_recoTracks, m_recoTrkTracks);
  //make track summary
  for (auto track = m_recoTrkTracks->begin(); track!=m_recoTrkTracks->end(); ++track) {
    ATH_MSG_VERBOSE("Updating fitted track: " << **track);
    m_trackSummaryTool->updateTrack(**track);
  }
  code = attachFeature(outputTE, m_recoTrkTracks, m_attachedFeatureName2);
  //msg() << MSG::INFO << "Number of converted tracks: " << m_recoTrkTracks->size() << endreq;

  //if (m_recoTrkTracks->size()!=m_recoTracks->size()) {
  //  msg() << MSG::INFO << "Mismatch " << endreq;
  //  msg() << MSG::INFO << "Number of original tracks: " << m_recoTracks->size() << endreq;
  //  msg() << MSG::INFO << "Number of converted tracks: " << m_recoTrkTracks->size() << endreq;
  //}
    

  if ( code != HLT::OK ) {
    msg() << MSG::ERROR << "REGTEST/ Write into outputTE failed" << endreq;
    delete m_recoTrkTracks;
    m_recoTrkTracks=NULL;
    return code;
  }

  if(m_attachVertices) {//create and attach TrigVertexCollection
    TrigVertexCollection* zVertColl = new TrigVertexCollection;
    for(std::vector<float>::iterator it=m_zVertices.begin();it!=m_zVertices.end();++it) {

      float zv = (*it);
      if(zv<-999.0) {//no vertex, in fact
	continue;
      }

      zVertColl->push_back(new TrigVertex(zv));
    }
    HLT::ErrorCode code = attachFeature(outputTE,zVertColl, m_attachedFeatureName);
    if ( code != HLT::OK ) {
      msg() << MSG::ERROR << "REGTEST/ Write into outputTE failed" << endreq;
      delete zVertColl;
      return code;
    }
  }

  return HLT::OK;
}

//---------------------------------------------------------------------------

HLT::ErrorCode TrigL2SiTrackFinder::hltFinalize()
{
#ifdef IDSCANDUMP
  // finish up ntpl  
  if ( mWriteFlag ) {
    // write out last event
    if ( mWriteCounter ) mTree->Fill(); 
    mFile->Write();
    mFile->Close();
  }
#endif
  
  msg() << MSG::INFO << "=========================================================" << endreq;
  msg() << MSG::INFO << "TrigL2SiTrackFinder::finalize() - TrigL2SiTrackFinder Statistics: " << endreq;
  msg() << MSG::INFO << "Events processed: " <<  m_countTotalEvents << endreq;
  msg() << MSG::INFO << "RoI processed: " <<  m_countTotalRoI << endreq;
  msg() << MSG::INFO << "RoI with enough SPs : " <<  m_countRoIwithEnoughHits << endreq;
  msg() << MSG::INFO << "RoI with Track(s)  Total/goodZvertex/badZvertex: " << m_countRoIwithTracks << endreq;
  msg() << MSG::INFO << "=========================================================" << endreq;

  return HLT::OK;
}

//-------------------------------------------------------------------------
/*
  void TrigL2SiTrackFinder::storeInfo(double zPosition, HLT::TriggerElement* outputTE)
  {
  TrigVertexCollection* zinfo = new TrigVertexCollection();
  TrigVertex* zvert = new TrigVertex( zPosition );
  zinfo->push_back(zvert);

  HLT::ErrorCode code = attachFeature(outputTE, zinfo, m_attachedFeatureName);
  if (code != HLT::OK) msg() << MSG::ERROR << "zinfo Write into outputTE failed" << endreq;
  }
*/

HLT::ErrorCode TrigL2SiTrackFinder::retrieveSpacePointsContainers() {

	
  StatusCode sc=store()->retrieve(m_pixelSpacePointsContainer,m_pixelSpContName);
  if(sc.isFailure()) {
    msg() << MSG::WARNING << "Trig SP Pixel container " <<m_pixelSpContName <<" not found"<<endreq; 
    return HLT::TOOL_FAILURE;
  }
  sc=store()->retrieve(m_sctSpacePointsContainer,m_sctSpContName);
  if(sc.isFailure()) {
    msg() << MSG::WARNING << "Trig SP SCT container " <<m_sctSpContName <<" not found"<<endreq; 
    return HLT::TOOL_FAILURE;
  }

  return HLT::OK;

}


void TrigL2SiTrackFinder::runResidualMonitoring(const TrigInDetTrack* pT) {
  // <---- add track preselection here : pT, nHits, etc.
  //bool goodTrack = (*trackIt)->param()->pT()>1000. && ((*trackIt)->NPixelSpacePoints()+(*trackIt)->NSCT_SpacePoints())>2;
  // tighter selection for unbiased residuals
  bool goodTrack = std::fabs(pT->param()->pT())>1000. && (pT->NPixelSpacePoints()+pT->NSCT_SpacePoints())>3 && pT->NSCT_SpacePoints()>0 ;
  if(!goodTrack) return;

  std::vector<TrigL2HitResidual> vResid;
  vResid.clear();
  StatusCode scRes = m_trigL2ResidualCalculator->getUnbiassedResiduals(pT,vResid);
  if(!scRes.isSuccess()) return;

  for(std::vector<TrigL2HitResidual>::iterator it=vResid.begin();it!=vResid.end();++it) {
    //msg() << MSG::INFO << (*it) << endreq;
    switch(it->regionId()) {
    case Region::PixBarrel :
      m_pixResPhiBarrel.push_back(it->phiResidual());
      m_pixPullPhiBarrel.push_back(it->phiPull());
      m_pixResEtaBarrel.push_back(it->etaResidual());
      m_pixPullEtaBarrel.push_back(it->etaPull());
      break;
    case Region::PixEndcap :
      m_pixResPhiEC.push_back(it->phiResidual());
      m_pixPullPhiEC.push_back(it->phiPull());
      m_pixResEtaEC.push_back(it->etaResidual());
      m_pixPullEtaEC.push_back(it->etaPull());
      break;
    case Region::SctBarrel :
      m_sctResBarrel.push_back(it->phiResidual());
      m_sctPullBarrel.push_back(it->phiPull());
      break;
    case Region::SctEndcap :
      m_sctResEC.push_back(it->phiResidual());
      m_sctPullEC.push_back(it->phiPull());
      break;
    case Region::IBL :
      break;
    case Region::Undefined :
      break;
    default :
      break;
    }
  }
}

void TrigL2SiTrackFinder::convertToTrkTrack(const TrigInDetTrackCollection* oldTracks,
    TrackCollection* newTracks) const {

  newTracks->reserve(oldTracks->size());
  double Gk[5][5];
  int nTracks=0;

  TrigInDetTrackCollection::const_iterator trIt = oldTracks->begin();
  TrigInDetTrackCollection::const_iterator lastIt = oldTracks->end();


  for(; trIt !=lastIt; trIt++) 
  {
    nTracks++;
    float phi0=HLT::wrapPhi((*trIt)->param()->phi0());
    float theta=2.0*atan(exp(-(*trIt)->param()->eta())); 
    float pT = (*trIt)->param()->pT();

    if((*trIt)->param()->cov()==nullptr) {
      //Track failed fit, skip it
      continue;
    }
    int idx=0;
    for(int i=0;i<5;i++) {
      for(int j=i;j<5;j++) {
        Gk[j][i]=Gk[i][j]=(*(*trIt)->param()->cov())[idx];
        idx++;
      }
    }
    float A = -2*sin(0.5*theta)*cos(0.5*theta); //== d(theta) / d(eta)
    float B = tan(theta)/pT;                    //== d(theta) / d(pT)
    float C = -sin(theta)/pT;                   //== d(qOverp / d(pT)
    AmgSymMatrix(5)* pCV = new AmgSymMatrix(5);
    (*pCV).fillSymmetric(0, 0, Gk[0][0]);
    (*pCV).fillSymmetric(0, 1, Gk[2][0]);
    (*pCV).fillSymmetric(0, 2, Gk[1][0]);
    (*pCV).fillSymmetric(0, 3, A*Gk[3][0] + B*Gk[4][0]);
    (*pCV).fillSymmetric(0, 4, C*Gk[4][0]);
    (*pCV).fillSymmetric(1, 1, Gk[2][2]);
    (*pCV).fillSymmetric(1, 2, Gk[2][1]);
    (*pCV).fillSymmetric(1, 3, A*Gk[3][2] + B*Gk[4][2]);
    (*pCV).fillSymmetric(1, 4, C*Gk[4][2]);
    (*pCV).fillSymmetric(2, 2, Gk[1][1]);
    (*pCV).fillSymmetric(2, 3, A*Gk[3][1] + B*Gk[4][1]);
    (*pCV).fillSymmetric(2, 4, C*Gk[4][1]);
    (*pCV).fillSymmetric(3, 3, A*(A*Gk[3][3] + 2.0*B*Gk[4][3]) + B*B*Gk[4][4]);
    (*pCV).fillSymmetric(3, 4, C*(A*Gk[4][3] + B*Gk[4][4]));
    (*pCV).fillSymmetric(4, 4, C*C*Gk[4][4]);

    Trk::PerigeeSurface dummySurface;
    const Trk::Perigee* pMP = new Trk::Perigee((*trIt)->param()->a0(),
        (*trIt)->param()->z0(),
        phi0,theta,
        sin(theta)/(*trIt)->param()->pT(),
        dummySurface,
        pCV);

    DataVector<const Trk::TrackStateOnSurface>* pParVec = new DataVector<const Trk::TrackStateOnSurface>; 

    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
    typePattern.set(Trk::TrackStateOnSurface::Perigee);

    const Trk::TrackStateOnSurface* trackStateOnSurface = new Trk::TrackStateOnSurface(0,
        pMP,
        0,0,
        typePattern); 
    pParVec->push_back(trackStateOnSurface); 
    Trk::FitQuality* pFQ=new Trk::FitQuality((*trIt)->chi2(),1);

    if ((*trIt)->siSpacePoints()) {
      std::vector<const TrigSiSpacePoint*>::iterator pSP=(*trIt)->siSpacePoints()->begin();
      std::vector<const TrigSiSpacePoint*>::iterator lastSP=(*trIt)->siSpacePoints()->end();
      for(int iSP=0; pSP != lastSP; pSP++, iSP++) {
        if (m_sctId->is_sct((*pSP)->identify())) {
          for (int iClusInSP=0; iClusInSP<2; iClusInSP++) {
            const InDet::SCT_Cluster* pCL;
            if (iClusInSP == 1) {
              pCL = dynamic_cast<const InDet::SCT_Cluster*>((*pSP)->clusters().first);
            }
            else {
              pCL = dynamic_cast<const InDet::SCT_Cluster*>((*pSP)->clusters().second);
            }
            if(!pCL) continue;
            const IdentifierHash idHash = m_sctId->wafer_hash(m_sctId->wafer_id(pCL->identify()));
            InDet::SCT_ClusterOnTrack* pSC=new InDet::SCT_ClusterOnTrack(pCL,pCL->localPosition(),
                pCL->localCovariance().inverse(),
                idHash,pCL->globalPosition(),
                pCL->gangedPixel());

            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
            typePattern.set(Trk::TrackStateOnSurface::Measurement);

            trackStateOnSurface=new Trk::TrackStateOnSurface(pSC, 
                0, 
                0,  
                0, typePattern);
            pParVec->push_back(trackStateOnSurface);
          }
        } 
        else {  // Pixel spacepoint
          const InDet::PixelCluster* pCL = dynamic_cast<const InDet::PixelCluster*>((*pSP)->clusters().first);
          if(!pCL) continue;
          const Amg::Vector3D glo(pCL->globalPosition());
          const IdentifierHash idHash = m_pixelId->wafer_hash(m_pixelId->wafer_id(pCL->identify()));
          InDet::PixelClusterOnTrack* pSC=new InDet::PixelClusterOnTrack(pCL,pCL->localPosition(),
              pCL->localCovariance().inverse(),
              idHash,pCL->globalPosition(),
              pCL->gangedPixel());
          std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
          typePattern.set(Trk::TrackStateOnSurface::Measurement);
          trackStateOnSurface=new Trk::TrackStateOnSurface(pSC, 
              0, 
              0,  
              0, typePattern);
          pParVec->push_back(trackStateOnSurface);
        }
      }
    }
    Trk::TrackInfo::TrackPatternRecoInfo pattRecInfo;      
    TrigInDetTrack::AlgoId onlineAuthor=(*trIt)->algorithmId();
    switch(onlineAuthor)
    {
      case TrigInDetTrack::STRATEGY_A_ID: {
          pattRecInfo=Trk::TrackInfo::strategyA;break;
      }
      case TrigInDetTrack::STRATEGY_B_ID: {
          pattRecInfo=Trk::TrackInfo::strategyB;break;
      }
      case TrigInDetTrack::STRATEGY_C_ID: {
          pattRecInfo=Trk::TrackInfo::strategyC;break;
      }
      case TrigInDetTrack::STRATEGY_F_ID: {
          pattRecInfo=Trk::TrackInfo::FTK;break;
      }
      default: {
        pattRecInfo=Trk::TrackInfo::NumberOfTrackRecoInfo;
        ATH_MSG_INFO("Default reached");
      }
    };

    Trk::TrackInfo info(Trk::TrackInfo::KalmanFitter,Trk::pion);
    info.setPatternRecognitionInfo(pattRecInfo);
    newTracks->push_back(new Trk::Track(info,pParVec,pFQ));
  }
  return;
}

