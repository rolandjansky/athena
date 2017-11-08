/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <numeric>
#include <string>

#include "TrigL2MuonSA/MuFastSteering.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

using namespace SG;
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MuFastSteering::MuFastSteering(const std::string& name, ISvcLocator* svc) 
  : HLT::FexAlgo(name, svc), 
    m_storeGate("StoreGateSvc", this->name()), 
    m_timerSvc(0),
    m_regionSelector(0),
    m_dataPreparator("TrigL2MuonSA::MuFastDataPreparator"),
    m_patternFinder("TrigL2MuonSA::MuFastPatternFinder"),
    m_stationFitter("TrigL2MuonSA::MuFastStationFitter"),
    m_trackFitter("TrigL2MuonSA::MuFastTrackFitter"),
    m_trackExtrapolator("TrigL2MuonSA::MuFastTrackExtrapolator"),
    m_backExtrapolatorTool("TrigMuonBackExtrapolator"),
    m_calStreamer("TrigL2MuonSA::MuCalStreamerTool"),
    m_recMuonRoIUtils(),
    m_cscsegmaker("TrigL2MuonSA::CscSegmentMaker"),
    m_rpcHits(), m_tgcHits(),
    m_mdtRegion(), m_muonRoad(),
    m_rpcFitResult(), m_tgcFitResult(),
    m_mdtHits_normal(), m_mdtHits_overlap(),
    m_cscHits(),
    m_jobOptionsSvc(0) 
    //m_trigCompositeContainer(0)
{
  declareProperty("DataPreparator",    m_dataPreparator,    "data preparator");
  declareProperty("PatternFinder",     m_patternFinder,     "pattern finder");
  declareProperty("StationFitter",     m_stationFitter,     "station fitter");
  declareProperty("TrackFitter",       m_trackFitter,       "track fitter");
  declareProperty("TrackExtrapolator", m_trackExtrapolator, "track extrapolator");

  declareProperty("BackExtrapolator", m_backExtrapolatorTool, "public tool for back extrapolating the muon tracks to the IV");
  declareProperty("CscSegmentMaker", m_cscsegmaker);
  declareProperty("Timing", m_use_timer=false);
  declareProperty("UseLUTForMC", m_use_mcLUT=true);

  // options for the calibration stream
  declareProperty("DoCalibrationStream", m_doCalStream=false);
  declareProperty("AllowOksConfig", m_allowOksConfig=true);
  declareProperty("MuonCalBufferName", m_calBufferName="/tmp/testOutput");
  declareProperty("MuonCalBufferSize", m_calBufferSize=1024*1024);
  declareProperty("MuonCalDataScouting",m_calDataScouting=false);

  declareProperty("ESD_RPC_size",m_esd_rpc_size=100);
  declareProperty("ESD_TGC_size",m_esd_tgc_size=50);
  declareProperty("ESD_MDT_size",m_esd_mdt_size=100);
  declareProperty("ESD_CSC_size",m_esd_csc_size=100);

  declareProperty("R_WIDTH_RPC_FAILED",m_rWidth_RPC_Failed=400);
  declareProperty("R_WIDTH_TGC_FAILED",m_rWidth_TGC_Failed=200);

  declareProperty("USE_RPC", m_use_rpc = true);
  declareProperty("USE_MDTCSM", m_use_mdtcsm = false);
  declareProperty("USE_ROIBASEDACCESS_MDT", m_use_RoIBasedDataAccess_MDT = true);
  declareProperty("USE_ROIBASEDACCESS_TGC", m_use_RoIBasedDataAccess_TGC = true);
  declareProperty("USE_ROIBASEDACCESS_RPC", m_use_RoIBasedDataAccess_RPC = true);
  declareProperty("USE_ROIBASEDACCESS_CSC", m_use_RoIBasedDataAccess_CSC = true);

  declareProperty("USE_NEW_SEGMENTFIT", m_use_new_segmentfit = true);

  declareProperty("Scale_Road_BarrelInner",m_scaleRoadBarrelInner=1);
  declareProperty("Scale_Road_BarrelMiddle",m_scaleRoadBarrelMiddle=1);
  declareProperty("Scale_Road_BarrelOuter",m_scaleRoadBarrelOuter=1);
  
  declareProperty("WinPt",m_winPt=4.0);

  declareProperty("RpcErrToDebugStream",m_rpcErrToDebugStream = false);

  declareProperty("UseEndcapInnerFromBarrel",m_use_endcapInnerFromBarrel = false);

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

  //adding a part of DataHandle for AthenaMT
  declareProperty("MuRoIs", m_roiCollectionKey = std::string("MURoIs"), "MuRoIs to read in"); 
  declareProperty("RecMuonRoI", m_recRoiCollectionKey = std::string("RecMuonRoI"), "RecMuonRoI to read in"); 
  declareProperty("MuFastDecisions",m_muFastContainerKey = std::string("xAOD::L2StandAloneMuonContainer"),"xAOD::L2StandAloneMuonContainer to record");	
  declareProperty("MuFastComposite",m_muCompositeContainerKey = std::string("xAOD::TrigCompositeContainer"),"xAOD::TrigCompositeContainer to record");	
  declareProperty("MuFastForID",m_muIdContainerKey = std::string("TrigRoiDescriptorCollection"),"TrigRoiDescriptor for ID to record");
  declareProperty("MuFastForMS",m_muMsContainerKey = std::string("TrigRoiDescriptorCollection"),"TrigRoiDescriptor for MS to record");
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MuFastSteering::~MuFastSteering() {
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuFastSteering::hltInitialize()
{
  ATH_MSG_DEBUG("Initializing MuFastSteering - package version " << PACKAGE_VERSION);
  
  if (m_storeGate.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve service StoreGateSvc");
    return HLT::BAD_JOB_SETUP;
  }
  
  StatusCode sc;

  m_timerSvc = 0;
  if (m_use_timer) {
    StatusCode sc = service( "TrigTimerSvc", m_timerSvc, true); 
    if( sc.isFailure() ) {
      ATH_MSG_ERROR(": Unable to locate TrigTimer Service");
    }
  
    if(m_timerSvc) {
      m_timers.push_back(m_timerSvc->addItem(name()+":DataPreparator"));
      m_timers.push_back(m_timerSvc->addItem(name()+":PatternFinder"));
      m_timers.push_back(m_timerSvc->addItem(name()+":StationFitter"));
      m_timers.push_back(m_timerSvc->addItem(name()+":TrackFitter"));
      m_timers.push_back(m_timerSvc->addItem(name()+":TrackExtrapolator"));
      m_timers.push_back(m_timerSvc->addItem(name()+":CalibrationStreamer"));    
      m_timers.push_back(m_timerSvc->addItem(name()+":TotalProcessing"));
    }
  }

  // Locate RegionSelector
  sc = service("RegSelSvc", m_regionSelector);
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve the regionselector service");
    return HLT::ERROR;
  }
  ATH_MSG_DEBUG("Retrieved the RegionSelector service ");

  // 
  if (m_dataPreparator.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve Tool DataPreparator");
    return HLT::BAD_JOB_SETUP;
  }
  
  // 
  if (m_patternFinder.retrieve().isFailure()) {
   ATH_MSG_ERROR("Cannot retrieve Tool DataPreparator");
    return HLT::BAD_JOB_SETUP;
  }

  // 
  if (m_stationFitter.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve Tool StationFitter");
    return HLT::BAD_JOB_SETUP;
  }

  // 
  if (m_trackFitter.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve Tool TrackFitter");
    return HLT::BAD_JOB_SETUP;
  }

  // 
  if (m_trackExtrapolator.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve Tool TrackExtrapolator");
    return HLT::BAD_JOB_SETUP;
  }

  // BackExtrapolator services
  if ( m_backExtrapolatorTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve " << m_backExtrapolatorTool);
    return HLT::BAD_JOB_SETUP;
  } 

  // CscSegmentMaker
  if ( m_cscsegmaker.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve " << m_cscsegmaker);
    return HLT::BAD_JOB_SETUP;
  }
  // Set service tools
  m_trackExtrapolator->setExtrapolatorTool(&m_backExtrapolatorTool);
  m_dataPreparator->setExtrapolatorTool(&m_backExtrapolatorTool);

  // set road width in case TGC/RPC readout failure 
  m_dataPreparator->setRoadWidthForFailure(m_rWidth_RPC_Failed, m_rWidth_TGC_Failed);

  //StatusCode sc;

  m_dataPreparator->setRpcGeometry(m_use_rpc);
  m_dataPreparator->setMdtDataCollection(m_use_mdtcsm);
  m_dataPreparator->setRoIBasedDataAccess(m_use_RoIBasedDataAccess_MDT,
                                          m_use_RoIBasedDataAccess_RPC,
                                          m_use_RoIBasedDataAccess_TGC,
                                          m_use_RoIBasedDataAccess_CSC);

  // set data or MC flag
  sc = m_dataPreparator->setMCFlag(m_use_mcLUT);
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Failed to set MC flag to DataPreparator");
    return HLT::ERROR;
        }
  
  ////stationfit mc flag
  sc = m_stationFitter->setMCFlag(m_use_mcLUT);
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Failed to set MC flag to StationFitter");
    return HLT::ERROR;
  }
  sc = m_trackFitter->setMCFlag(m_use_mcLUT);
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Failed to set MC flag to TrackFitter");
    return HLT::ERROR;
  }
  m_trackFitter -> setUseEIFromBarrel( m_use_endcapInnerFromBarrel );

  // initialize the joboptions service
  sc = service("JobOptionsSvc", m_jobOptionsSvc);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find JobOptionsSvc");
    return HLT::ERROR;
  } else {
    IService* svc = dynamic_cast<IService*>(m_jobOptionsSvc);
    if(svc != 0 ) {
      ATH_MSG_DEBUG(" Algorithm = " << name() << " is connected to JobOptionsSvc Service = " << svc->name());
    }  
  }

  //
  // Initialize the calibration streamer and the incident 
  // service for its initialization
  //
  if (m_doCalStream) {
    // retrieve the calibration streamer
    if (m_calStreamer.retrieve().isFailure()) {
      ATH_MSG_ERROR("Cannot retrieve Tool CalStreamer");
      return HLT::BAD_JOB_SETUP;
    }
    // set properties
    m_calStreamer->setBufferName(m_calBufferName);
    ATH_MSG_DEBUG("Initialized the Muon Calibration Streamer. Buffer name: " << m_calBufferName 
		  << ", buffer size: " << m_calBufferSize 
		  << " doDataScouting: "  << m_calDataScouting);
    
    ServiceHandle<IIncidentSvc> p_incidentSvc("IncidentSvc",name());
    sc =  p_incidentSvc.retrieve();
    if (!sc.isSuccess()) {
      ATH_MSG_ERROR("Could not find IncidentSvc");
      return HLT::ERROR;
    } else {
      long int pri=100;
      p_incidentSvc->addListener(this,"UpdateAfterFork",pri);
      p_incidentSvc.release().ignore();
    }
  } 

  //adding a part of DataHandle for AThenaMT
  if (m_roiCollectionKey.initialize().isFailure() ) { 
    ATH_MSG_ERROR("ReadHandleKey for MURoIs initialize Failure!");
    return HLT::BAD_JOB_SETUP;   
  }
  if (m_recRoiCollectionKey.initialize().isFailure() ) { 
    ATH_MSG_ERROR("ReadHandleKey for RecMuonRoI initialize Failure!");
    return HLT::BAD_JOB_SETUP;   
  }
  if (m_muFastContainerKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("WriteHandleKey for xAOD::L2StandAloneMuonContainer initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  } 
  if (m_muCompositeContainerKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("WriteHandleKey for xAOD::TrigCompositeContainer initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  } 
  if (m_muIdContainerKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("WriteHandleKey for TrigRoiDescriptorCollection for ID initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  }

  if (m_muMsContainerKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("WriteHandleKey for TrigRoiDescriptorCollection for MS initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  }

  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuFastSteering::hltBeginRun() {
  ATH_MSG_DEBUG("hltBeginRun");
  return HLT::OK;
}

HLT::ErrorCode MuFastSteering::hltEndRun() {
  ATH_MSG_DEBUG("hltEndRun");
   // close the calibration stream 
   if (m_doCalStream) { 
     if ( !m_calDataScouting ) {
       StatusCode sc = m_calStreamer->closeStream();
       if ( sc != StatusCode::SUCCESS ) {
	 ATH_MSG_ERROR("Failed to close the calibration stream");
       }
     }
   } 

   return HLT::OK;
}

HLT::ErrorCode MuFastSteering::hltFinalize() {
  ATH_MSG_DEBUG("hltFinalize()");
  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

const LVL1::RecMuonRoI* matchingRecRoI( uint32_t roiWord,
		      			const DataVector<LVL1::RecMuonRoI>& collection ) 
{
  for ( auto recRoI: collection ) {
    if ( recRoI->roiWord() == roiWord ){
      return recRoI;
    } 
  } 
  return nullptr;         
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode MuFastSteering::execute()
{
  ATH_MSG_DEBUG("StatusCode MuFastSteering::execute() start");

  // retrieve with ReadHandle
  auto roiCollectionHandle = SG::makeHandle( m_roiCollectionKey );
  const TrigRoiDescriptorCollection *roiCollection = roiCollectionHandle.cptr();
  if (!roiCollectionHandle.isValid()){
    ATH_MSG_ERROR("ReadHandle for MURoIs isn't Valid");
    return StatusCode::FAILURE;
  } 

  auto recRoiCollectionHandle = SG::makeHandle( m_recRoiCollectionKey );
  const DataVector<LVL1::RecMuonRoI> *recRoiCollection = recRoiCollectionHandle.cptr();
  if (!recRoiCollectionHandle.isValid()){
    ATH_MSG_ERROR("ReadHandle for RecMuonRoI isn't Valid");
    return StatusCode::FAILURE;
  }  

  DataVector<const TrigRoiDescriptor> *internalRoI = new DataVector<const TrigRoiDescriptor>;
  internalRoI->clear();
  TrigRoiDescriptorCollection::const_iterator p_roids = roiCollection->begin();
  TrigRoiDescriptorCollection::const_iterator p_roidsEn = roiCollection->end();
  ATH_MSG_DEBUG("REGTEST: MURoIs size = " << roiCollection->size());

  for(; p_roids != p_roidsEn; ++p_roids ) {
    internalRoI->push_back(*p_roids);
    ATH_MSG_DEBUG("REGTEST: MURoIs eta = " << "(" << (*p_roids)->etaMinus() << ")" << (*p_roids)->eta() << "(" << (*p_roids)->etaPlus() << ")");
    ATH_MSG_DEBUG("REGTEST: MURoIs phi = " << "(" << (*p_roids)->phiMinus() << ")" << (*p_roids)->phi() << "(" << (*p_roids)->phiPlus() << ")");
    ATH_MSG_DEBUG("REGTEST: MURoIs zed = " << "(" << (*p_roids)->zedMinus() << ")" << (*p_roids)->zed() << "(" << (*p_roids)->zedPlus() << ")");
  }
  ATH_MSG_DEBUG("REGTEST: MURoIs DONE");

  DataVector<LVL1::RecMuonRoI>::const_iterator p_roi = recRoiCollection->begin();
  DataVector<LVL1::RecMuonRoI>::const_iterator p_roiEn = recRoiCollection->end();
  ATH_MSG_DEBUG("REGTEST: RecMURoIs size = " << recRoiCollection->size());

  // make RecMURoIs maching with MURoIs
  const LVL1::RecMuonRoI* recRoI = matchingRecRoI( roiCollection->at(0)->roiWord(),  *recRoiCollection );
  CHECK( recRoI != nullptr );

  DataVector<const LVL1::RecMuonRoI> *recRoIVector = new DataVector<const LVL1::RecMuonRoI>;
  recRoIVector->clear();
  recRoIVector->push_back(recRoI);

  ATH_MSG_DEBUG("REGTEST: RecMURoIs eta/phi = " << (recRoI)->eta() << "/" << (recRoI)->phi());
  ATH_MSG_DEBUG("REGTEST: RecMURoIs DONE");

  // define objects to record output data
  // for xAOD::L2StandAloneMuonContainer
  xAOD::L2StandAloneMuonContainer *outputTracks = new xAOD::L2StandAloneMuonContainer();
  outputTracks->clear();
  xAOD::L2StandAloneMuonAuxContainer aux;
  outputTracks->setStore( &aux );

  // for xAOD::TrigCompositeContainer
  xAOD::TrigCompositeContainer* outputComposite = new xAOD::TrigCompositeContainer();
  outputComposite->clear();
  xAOD::TrigCompositeAuxContainer auxComp;
  outputComposite->setStore( &auxComp );

  // for TrigRoiDescriptor for ID
  TrigRoiDescriptorCollection *outputID = new TrigRoiDescriptorCollection();
  outputID->clear();

  // for TrigRoiDescriptor for MS
  TrigRoiDescriptorCollection *outputMS = new TrigRoiDescriptorCollection();
  outputMS->clear();
  
  // to StatusCode findMuonSignature()
  ATH_CHECK(findMuonSignature(*internalRoI, *recRoIVector, 
			      outputTracks, outputID, outputMS, outputComposite));	

  // record data objects with WriteHandle
  SG::WriteHandle<xAOD::L2StandAloneMuonContainer> muFastContainer (m_muFastContainerKey);
  ATH_CHECK(muFastContainer.record(std::make_unique<xAOD::L2StandAloneMuonContainer>(*outputTracks)));

  SG::WriteHandle<xAOD::TrigCompositeContainer> muCompositeContainer (m_muCompositeContainerKey);
  ATH_CHECK(muCompositeContainer.record(std::make_unique<xAOD::TrigCompositeContainer>(*outputComposite)));

  SG::WriteHandle<TrigRoiDescriptorCollection> muIdContainer (m_muIdContainerKey);
  ATH_CHECK(muIdContainer.record(std::make_unique<TrigRoiDescriptorCollection>(*outputID)));

  SG::WriteHandle<TrigRoiDescriptorCollection> muMsContainer(m_muMsContainerKey);
  ATH_CHECK(muMsContainer.record(std::make_unique<TrigRoiDescriptorCollection>(*outputMS)));

  // DEBUG TEST: Recorded data objects
  ATH_MSG_DEBUG("Recorded data objects"); 
  ATH_MSG_DEBUG("REGTEST: xAOD::L2StansAloneMuoonContainer size = " << muFastContainer->size());
  xAOD::L2StandAloneMuonContainer::const_iterator p_muon = (*muFastContainer).begin();
  xAOD::L2StandAloneMuonContainer::const_iterator p_muonEn = (*muFastContainer).end();
  for (;p_muon != p_muonEn; ++p_muon) {
    ATH_MSG_DEBUG("REGTEST: xAOD::L2StandAloneMuonContainer pt = " << (*p_muon)->pt());
    ATH_MSG_DEBUG("REGTEST: xAOD::L2StansAloneMuonContainer  eta/phi = " << (*p_muon)->eta() << "/" << (*p_muon)->phi());  
  }

  ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection for ID size = " << muIdContainer->size());
  TrigRoiDescriptorCollection::const_iterator p_muonID = muIdContainer->begin();
  TrigRoiDescriptorCollection::const_iterator p_muonIDEn = muIdContainer->end();
  for(; p_muonID != p_muonIDEn; ++p_muonID ) {
    ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection for ID eta/phi = " << (*p_muonID)->eta() << "/" << (*p_muonID)->phi());
  }  

  ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection for MS size = " << muMsContainer->size());
  TrigRoiDescriptorCollection::const_iterator p_muonMS = muMsContainer->begin();
  TrigRoiDescriptorCollection::const_iterator p_muonMSEn = muMsContainer->end();
  for(; p_muonMS != p_muonMSEn; ++p_muonMS ) {
    ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection for MS eta/phi = " << (*p_muonMS)->eta() << "/" << (*p_muonMS)->phi());
  }

  ATH_MSG_DEBUG("REGTEST: Recorded data objects DONE");

  ATH_MSG_DEBUG("StatusCode MuFastSteering::execute() success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuFastSteering::hltExecute(const HLT::TriggerElement* /*inputTE*/, 
                                          HLT::TriggerElement* outputTE)
{
  ATH_MSG_DEBUG("hltExecute called");

  std::vector< const TrigRoiDescriptor*> roids;
  std::vector< const TrigRoiDescriptor*>::const_iterator p_roids;
  HLT::ErrorCode hec2 = getFeatures(outputTE, roids);	
 
  std::vector<const LVL1::RecMuonRoI*> muonRoIs;
  std::vector<const LVL1::RecMuonRoI*>::const_iterator p_roi;
  HLT::ErrorCode hec = getFeatures(outputTE, muonRoIs);	

  if (hec != HLT::OK && hec2 != HLT::OK) {
    ATH_MSG_ERROR("Could not find input TE");
    return hec2;
  }
 
  bool ActiveState = outputTE->getActiveState();

  DataVector<const TrigRoiDescriptor> *internalRoI = new DataVector<const TrigRoiDescriptor>;
  ATH_MSG_DEBUG("REGTEST: MURoIs size: " << roids.size());

  DataVector<const LVL1::RecMuonRoI> *internalRecRoI = new DataVector<const LVL1::RecMuonRoI>;
  ATH_MSG_DEBUG("REGTEST: RecMURoIs size = " << muonRoIs.size());

  p_roids = roids.begin();
  p_roi = muonRoIs.begin();
  for (const auto& roid : roids){
    internalRoI->push_back(*p_roids);
    ATH_MSG_DEBUG("REGTEST: MURoIs eta = " << "(" << (*p_roids)->etaMinus() << ")" << (*p_roids)->eta() << "(" << (*p_roids)->etaPlus() << ")");
    ATH_MSG_DEBUG("REGTEST: MURoIs phi = " << "(" << (*p_roids)->phiMinus() << ")" << (*p_roids)->phi() << "(" << (*p_roids)->phiPlus() << ")");
    ATH_MSG_DEBUG("REGTEST: MURoIs zed = " << "(" << (*p_roids)->zedMinus() << ")" << (*p_roids)->zed() << "(" << (*p_roids)->zedPlus() << ")");

    internalRecRoI->push_back(*p_roi);
    ATH_MSG_DEBUG("REGTEST: RecMURoIs eta/phi = " << (*p_roi)->eta() << "/" << (*p_roi)->phi());

    p_roids++;
    p_roi++;
  }
  ATH_MSG_DEBUG("REGTEST: DONE");

  // define objects to record output data
  // for xAOD::L2StandAloneMuonContainer
  xAOD::L2StandAloneMuonContainer *outputTracks = new xAOD::L2StandAloneMuonContainer();
  outputTracks->clear();
  xAOD::L2StandAloneMuonAuxContainer aux;
  outputTracks->setStore( &aux );
  
  // for xAOD::TrigCompositeContainer
  xAOD::TrigCompositeContainer* outputComposite = new xAOD::TrigCompositeContainer();
  outputComposite->clear();
  xAOD::TrigCompositeAuxContainer auxComp;
  outputComposite->setStore( &auxComp );

  // for TrigRoiDescriptor for ID
  TrigRoiDescriptorCollection *outputID = new TrigRoiDescriptorCollection();
  outputID->clear();

  // for TrigRoiDescriptor for MS
  TrigRoiDescriptorCollection *outputMS = new TrigRoiDescriptorCollection();
  outputMS->clear();
  
  // to StatusCode findMuonSignature()
  StatusCode sc = findMuonSignature(*internalRoI, *internalRecRoI, 
				    outputTracks, outputID, outputMS, outputComposite);	
  
  HLT::ErrorCode code = HLT::OK;
  // in case of findMuonSignature failed
  if (sc != StatusCode::SUCCESS) {
    delete outputTracks;
    outputTE -> setActiveState(ActiveState);
    code = attachFeature(outputTE, new xAOD::L2StandAloneMuonContainer(SG::VIEW_ELEMENTS), "MuonL2SAInfo");
    if (code != HLT::OK) {
      return code;
    }
    delete outputID;
    code = attachFeature(outputTE, new TrigRoiDescriptorCollection(SG::VIEW_ELEMENTS), "forID");
    if (code != HLT::OK) {
      return code;
    }
    delete outputMS;
    code = attachFeature(outputTE, new TrigRoiDescriptorCollection(SG::VIEW_ELEMENTS), "forMS");
    if (code != HLT::OK) {
      return code;
    }   
    return HLT::ERROR;
  }

  // in case of findMuonSignature success
  if (outputTracks->empty()) {
    delete outputTracks;
    ActiveState = false;
    outputTE -> setActiveState(ActiveState);
    code = attachFeature(outputTE, new xAOD::L2StandAloneMuonContainer(SG::VIEW_ELEMENTS), "MuonL2SAInfo");
  } else {
    ActiveState = true;
    outputTE -> setActiveState(ActiveState);
    int size = 0;
    for (int size=0; size<outputTracks->size(); size++){
     outputTracks->at(size)->setTeId( outputTE->getId() );		
     ATH_MSG_DEBUG("outputTE(" << size << ") = " << outputTracks->at(size)->teId());
    }
    code = attachFeature(outputTE, outputTracks, "MuonL2SAInfo");
    if ( code != HLT::OK ) { 
      ATH_MSG_ERROR("Record of MuonL2SAInfo failed");
      ActiveState = false;
      return false;
    }
    ATH_MSG_DEBUG("Recorded a xAOD::L2StandAloneMuonContainer");
    ATH_MSG_DEBUG("REGTEST: xAOD::L2StansAloneMuoonContainer size = " << outputTracks->size());
    xAOD::L2StandAloneMuonContainer::const_iterator p_muon = (*outputTracks).begin();
    xAOD::L2StandAloneMuonContainer::const_iterator p_muonEn = (*outputTracks).end();
    for (;p_muon != p_muonEn; ++p_muon) {
      ATH_MSG_DEBUG("REGTEST: xAOD::L2StandAloneMuonContainer pt = " << (*p_muon)->pt());
      ATH_MSG_DEBUG("REGTEST: xAOD::L2StansAloneMuonContainer  eta/phi = " << (*p_muon)->eta() << "/" << (*p_muon)->phi());  
    }
  }

  if (outputID->empty()) {
    delete outputID;
    //ActiveState = false;
    //outputTE -> setActiveState(ActiveState);
    //code = attachFeature(outputTE, new TrigRoiDescriptorCollection(SG::VIEW_ELEMENTS), "forID");
  } else {
    ActiveState = true;
    outputTE -> setActiveState(ActiveState);
    code = attachFeature(outputTE, outputID, "forID");
    if ( code != HLT::OK ) { 
      ATH_MSG_ERROR("Record of TrigRoiInfo for ID failed");
      ActiveState = false;
      return false;
    }
    ATH_MSG_DEBUG("Recorded an RoiDescriptor for ID");
  }

  if (outputMS->empty()) {
    delete outputMS;
    //ActiveState = false;
    //outputTE -> setActiveState(ActiveState);
    //code = attachFeature(outputTE, new TrigRoiDescriptorCollection(SG::VIEW_ELEMENTS), "forMS");
  } else {
    ActiveState = true;
    outputTE -> setActiveState(ActiveState);
    code = attachFeature(outputTE, outputMS, "forMS");
    if ( code != HLT::OK ) { 
      ATH_MSG_ERROR("Record of TrigRoiInfo for MS failed");
      ActiveState = false;
      return false;
    }
    ATH_MSG_DEBUG("Recorded an RoiDescriptor for MS");
  }

  if (outputComposite->empty()) {
    delete outputComposite;
  } else {
    ActiveState = true;
    outputTE -> setActiveState(ActiveState);
    code = attachFeature(outputTE, outputComposite, "MuonCalibrationStream");
    ATH_MSG_DEBUG("Recorded a xAOD::TrigCompositeContainer");
    if ( code != HLT::OK ) { 
      ATH_MSG_ERROR("Record of MuonCalibrationStream in TriggerElement failed");
      ActiveState = false;
      return false;
    }
  }
  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode MuFastSteering::findMuonSignature(const DataVector<const TrigRoiDescriptor>&	roids,
                                             const DataVector<const LVL1::RecMuonRoI>& 	muonRoIs,
				             xAOD::L2StandAloneMuonContainer* 		outputTracks,
					     TrigRoiDescriptorCollection* 		outputID,
					     TrigRoiDescriptorCollection* 		outputMS,
					     xAOD::TrigCompositeContainer*            	outputComposite)

{
  ATH_MSG_DEBUG("StatusCode MuFastSteering::findMuonSignature start");

  StatusCode sc = StatusCode::SUCCESS;
  // Initialize monitored variables
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

  ATH_MSG_DEBUG("REGTEST: MURoIs size = " << roids.size() << " (argument)"); 
  ATH_MSG_DEBUG("REGTEST: RecMURoIs size = " << muonRoIs.size() << " (argument)"); 

  if (m_timerSvc) m_timers[ITIMER_TOTAL_PROCESSING]->resume();
  if (m_timerSvc) m_timers[ITIMER_DATA_PREPARATOR]->resume();

  m_currentStage = 1;

  DataVector<const TrigRoiDescriptor>::const_iterator p_roids;
  DataVector<const LVL1::RecMuonRoI>::const_iterator p_roi;

  // muonRoIs = RecMURoIs, roids = MURoIs from L1Decoderi
  p_roids = roids.begin();
  for (p_roi=(muonRoIs).begin(); p_roi!=(muonRoIs).end(); ++p_roi) {

    double roiEta = (*p_roids)->eta();
    double roiPhi = (*p_roids)->phi();
    double roiZed = (*p_roids)->zed();
    ATH_MSG_DEBUG("REGTEST: MURoIs eta = " << "(" << (*p_roids)->etaMinus() << ")" << roiEta << "(" << (*p_roids)->etaPlus() << ")" << " (argument)");
    ATH_MSG_DEBUG("REGTEST: MURoIs phi = " << "(" << (*p_roids)->phiMinus() << ")" << roiPhi << "(" << (*p_roids)->phiPlus() << ")" << " (argument)");
    ATH_MSG_DEBUG("REGTEST: MURoIs zed = " << "(" << (*p_roids)->zedMinus() << ")" << roiZed << "(" << (*p_roids)->zedPlus() << ")" << " (argument)");

    double recroiEta = (*p_roi)->eta();
    double recroiPhi = (*p_roi)->phi();
    ATH_MSG_DEBUG("REGTEST: RecMURoIs eta/phi = " << recroiEta << "/" << recroiPhi << " (argument)");
    ATH_MSG_DEBUG("REGTEST: RecMURoIs ID = " << (*p_roi)->sectorID() << " (argument)");
    ATH_MSG_DEBUG("REGTEST: DONE");

    std::vector<TrigL2MuonSA::TrackPattern> trackPatterns;
    m_mdtHits_normal.clear();
    m_mdtHits_overlap.clear();
    m_cscHits.clear();
    m_rpcFitResult.Clear();
    m_tgcFitResult.Clear();

    m_muonRoad.Clear();

    ATH_MSG_DEBUG("Start an algorithm of MuonSA");
    if ( m_recMuonRoIUtils.isBarrel(*p_roi) ) { // Barrel
      ATH_MSG_DEBUG("Barrel");
   
      m_muonRoad.setScales(m_scaleRoadBarrelInner,
                           m_scaleRoadBarrelMiddle,
                           m_scaleRoadBarrelOuter);      

      // Data preparation
      m_rpcHits.clear();
      m_tgcHits.clear();     
      sc = m_dataPreparator->prepareData(*p_roi,
                                         *p_roids,
                                         m_rpcHits,
                                         m_muonRoad,
                                         m_mdtRegion,
                                         m_rpcFitResult,
                                         m_mdtHits_normal,
                                         m_mdtHits_overlap);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Data preparation failed");
	TrigL2MuonSA::TrackPattern trackPattern;
	trackPatterns.push_back(trackPattern);
	updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
		     m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
                     trackPatterns, outputTracks, outputID, outputMS);
	return StatusCode::SUCCESS;
      }
      if (m_timerSvc) m_timers[ITIMER_DATA_PREPARATOR]->pause();

      if ( m_rpcErrToDebugStream && m_dataPreparator->isRpcFakeRoi() ) {
        ATH_MSG_ERROR("Invalid RoI in RPC data found: event to debug stream");
	TrigL2MuonSA::TrackPattern trackPattern;
	trackPatterns.push_back(trackPattern);
	updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
		     m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
                     trackPatterns, outputTracks, outputID, outputMS);
	return StatusCode::FAILURE;
      } 

      // Pattern finding
      if (m_timerSvc) m_timers[ITIMER_PATTERN_FINDER]->resume();
      sc = m_patternFinder->findPatterns(m_muonRoad,
                                         m_mdtHits_normal,
                                         trackPatterns);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Pattern finder failed");
        updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
                     m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
                     trackPatterns, outputTracks, outputID, outputMS);
	return StatusCode::SUCCESS;
      }
      if (m_timerSvc) m_timers[ITIMER_PATTERN_FINDER]->pause();

      // Superpoint fit
      if (m_timerSvc) m_timers[ITIMER_STATION_FITTER]->resume();      
      sc = m_stationFitter->findSuperPoints(*p_roi,
                                            m_rpcFitResult,
                                            trackPatterns);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Super point fitter failed");
        updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
                     m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
                     trackPatterns, outputTracks, outputID, outputMS);
 	return StatusCode::SUCCESS;
      }
      if (m_timerSvc) m_timers[ITIMER_STATION_FITTER]->pause();      

      // Track fitting
      if (m_timerSvc) m_timers[ITIMER_TRACK_FITTER]->resume();      
      sc = m_trackFitter->findTracks(*p_roi,
                                      m_rpcFitResult,
                                      trackPatterns);
                                     
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Track fitter failed");
        updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
                     m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
                     trackPatterns, outputTracks, outputID, outputMS);
	return StatusCode::SUCCESS;
      }
      if (m_timerSvc) m_timers[ITIMER_TRACK_FITTER]->pause();      

    } else { // Endcap
      ATH_MSG_DEBUG("Endcap");

      // Data preparation
      m_rpcHits.clear();
      m_tgcHits.clear();     
      sc = m_dataPreparator->prepareData(*p_roi,
                                         *p_roids,
                                         m_tgcHits,
                                         m_muonRoad,
                                         m_mdtRegion,
                                         m_tgcFitResult,
                                         m_mdtHits_normal,
                                         m_mdtHits_overlap,
                                         m_cscHits);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Data preparation failed");
 	TrigL2MuonSA::TrackPattern trackPattern;
	trackPatterns.push_back(trackPattern);
	updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
		     m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
                     trackPatterns, outputTracks, outputID, outputMS);
	return StatusCode::SUCCESS;
      }
      if (m_timerSvc) m_timers[ITIMER_DATA_PREPARATOR]->pause();

      // Pattern finding
      if (m_timerSvc) m_timers[ITIMER_PATTERN_FINDER]->resume();
      sc = m_patternFinder->findPatterns(m_muonRoad,
                                         m_mdtHits_normal,
                                         trackPatterns);


      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Pattern finder failed");
        updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
                     m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
                     trackPatterns, outputTracks, outputID, outputMS);
	return StatusCode::SUCCESS;
      }
      if (m_timerSvc) m_timers[ITIMER_PATTERN_FINDER]->pause();
      
      // Superpoint fit
      if (m_timerSvc) m_timers[ITIMER_STATION_FITTER]->resume();
      if(!m_use_new_segmentfit){
        sc = m_stationFitter->findSuperPoints(*p_roi,
                                              m_tgcFitResult,
                                              trackPatterns);
      }else{
        sc = m_stationFitter->findSuperPoints(*p_roi,
                                              m_muonRoad,
                                              m_tgcFitResult,
                                              trackPatterns);
      }
      /////csc SuperPoint
      m_cscsegmaker->FindSuperPointCsc(m_cscHits,trackPatterns,m_tgcFitResult,m_muonRoad);

      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Super point fitter failed");
        updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
                     m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
		     trackPatterns, outputTracks, outputID, outputMS);
	return StatusCode::SUCCESS;
      }

      if (m_timerSvc) m_timers[ITIMER_STATION_FITTER]->pause();      

      // Track fittingh    
      if (m_timerSvc) m_timers[ITIMER_TRACK_FITTER]->resume();     
      sc = m_trackFitter->findTracks(*p_roi,
                                     m_tgcFitResult,
                                     trackPatterns,
                                     m_muonRoad);

      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Track fitter failed");
        updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
                     m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
		     trackPatterns, outputTracks, outputID, outputMS);
  	return StatusCode::SUCCESS;
      }
      if (m_timerSvc) m_timers[ITIMER_TRACK_FITTER]->pause();      
    }
 
    // fix if eta is strange
    for (unsigned int i=0 ;i<trackPatterns.size(); i++) {
       TrigL2MuonSA::TrackPattern track = trackPatterns[i]; 
       const float ETA_LIMIT       = 2.8;
       const float DELTA_ETA_LIMIT = 1.0;
       float roiEta = (*p_roi)->eta();
       const float ZERO_LIMIT = 1.e-5;
       if (fabs(track.pt) > ZERO_LIMIT
           && ( fabs(track.etaMap) > ETA_LIMIT || fabs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
          trackPatterns[i].etaMap = roiEta;
       }
    }

    // Track extrapolation for ID combined
    if (m_timerSvc) m_timers[ITIMER_TRACK_EXTRAPOLATOR]->resume();

    sc = m_trackExtrapolator->extrapolateTrack(trackPatterns, m_winPt);

    if (sc != StatusCode::SUCCESS) {
      ATH_MSG_WARNING("Track extrapolator failed");
      updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
                   m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
                   trackPatterns, outputTracks, outputID, outputMS);
       return StatusCode::SUCCESS;
    }
    if (m_timerSvc) m_timers[ITIMER_TRACK_EXTRAPOLATOR]->pause();
    
    // Update monitoring variables
    sc = updateMonitor(*p_roi, m_mdtHits_normal, trackPatterns);
    if (sc != StatusCode::SUCCESS) {
      ATH_MSG_WARNING("Failed to update monitoring variables");
      updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
                   m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
                   trackPatterns, outputTracks, outputID, outputMS);
       return StatusCode::SUCCESS;
    }

    // Update output trigger element
    updateOutput(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
                 m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits,
                 trackPatterns, outputTracks, outputID, outputMS);
            
    // call the calibration streamer 
    if (m_doCalStream && trackPatterns.size()>0 ) { 
      TrigL2MuonSA::TrackPattern tp = trackPatterns[0];
      if (m_timerSvc) m_timers[ITIMER_CALIBRATION_STREAMER]->resume();

      //      m_calStreamer->setInstanceName(this->name());
      
      bool updateTriggerElement = false;
      sc = m_calStreamer->createRoiFragment(*p_roi,tp,m_mdtHits_normal,
					    m_rpcHits,
					    m_tgcHits,
					    m_calBufferSize,
					    m_calDataScouting,
					    updateTriggerElement); 

      if (sc != StatusCode::SUCCESS ) {  
        ATH_MSG_WARNING("Calibration streamer: create Roi Fragment failed");
      }
      if (m_timerSvc) m_timers[ITIMER_CALIBRATION_STREAMER]->pause(); 

      // if it's a data scouting chain check the buffer length
      if ( m_calDataScouting ) {
        
        if ( updateTriggerElement ) {
          
          ATH_MSG_DEBUG("Updating the trigger element");
          ATH_MSG_DEBUG(">> Retrieved the buffer, with size: " << m_calStreamer->getLocalBufferSize());
	  // create the TrigCompositeContainer to store the calibration buffer
	  // at StatusCode execute() and hltExecute().

	  // add the trigcomposite object to the container
	  xAOD::TrigComposite* tc = new xAOD::TrigComposite();
	  outputComposite->push_back(tc);

	  ATH_MSG_DEBUG("The size of the TrigCompositeContainer is: " << outputComposite->size() );
	  	  
	  // set the detail of the trigcomposite object
	  //	  xAOD::TrigComposite* tc = m_trigCompositeContainer->at(0);
	  tc->setDetail("MuonCalibrationStream", *(m_calStreamer->getLocalBuffer()) );
	  
	  m_calStreamer->clearLocalBuffer();

        }
	// test: set to false the active state if no buffer size limit reached
	else {
	}
      } 
    }
      
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

  ATH_MSG_DEBUG("StatusCode MuFastSteering::findMuonSignature success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void MuFastSteering::clearEvent()
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool MuFastSteering::updateOutput(const LVL1::RecMuonRoI*                  roi,
                                  const TrigRoiDescriptor*                 roids,
                                  const TrigL2MuonSA::MuonRoad&            muonRoad,
                                  const TrigL2MuonSA::MdtRegion&           mdtRegion,
                                  const TrigL2MuonSA::RpcHits&             rpcHits,
                                  const TrigL2MuonSA::TgcHits&             tgcHits,
                                  const TrigL2MuonSA::RpcFitResult&        rpcFitResult,
                                  const TrigL2MuonSA::TgcFitResult&        tgcFitResult,
                                  const TrigL2MuonSA::MdtHits&             mdtHits,
                                  const TrigL2MuonSA::CscHits&             cscHits,
                                  std::vector<TrigL2MuonSA::TrackPattern>& trackPatterns,
				  xAOD::L2StandAloneMuonContainer*	   outputTracks,
				  TrigRoiDescriptorCollection*	  	   outputID,
				  TrigRoiDescriptorCollection*	   	   outputMS)
{
  const float ZERO_LIMIT = 1.e-5;

  const int currentRoIId = roids->roiId();

  const double scalePhiWidthForFailure = 2;
  const double scaleRoIforZeroPt = 2;   
    
  const EventInfo* pEventInfo(0);
  StatusCode sc = m_storeGate->retrieve(pEventInfo);
  if (sc.isFailure()){
    ATH_MSG_FATAL("Can't get EventInfo object");
    return HLT::SG_ERROR;
  }
  
  const EventID* pEventId = pEventInfo->event_ID();
  if (pEventId==0) {
    ATH_MSG_ERROR("Could not find EventID object");
    return HLT::SG_ERROR;
  }
  
  const TriggerInfo* pTriggerInfo = pEventInfo->trigger_info();
  if (pTriggerInfo==0) {
    ATH_MSG_ERROR("Could not find TriggerInfo object");
    return HLT::SG_ERROR;
  }
  
  int inner  = 0;
  int middle = 1;
  int outer  = 2;
  int ee     = 6;
  int csc    = 7;
  int barrelinner = 0;
  int endcapinner = 3;
  int bee = 8;
  int bme = 9;

  if( trackPatterns.size() > 0 ) {
    
    const TrigL2MuonSA::TrackPattern& pattern = trackPatterns[0]; 

    // define inner, middle, outer
    if (pattern.s_address==-1) {
      inner  = xAOD::L2MuonParameters::Chamber::EndcapInner;
      middle = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
      outer  = xAOD::L2MuonParameters::Chamber::EndcapOuter;
      ee     = xAOD::L2MuonParameters::Chamber::EndcapExtra;
      barrelinner     = xAOD::L2MuonParameters::Chamber::BarrelInner;
      bee = xAOD::L2MuonParameters::Chamber::BEE;
    } else {
      inner  = xAOD::L2MuonParameters::Chamber::BarrelInner;
      middle = xAOD::L2MuonParameters::Chamber::BarrelMiddle;
      outer  = xAOD::L2MuonParameters::Chamber::BarrelOuter;
      bme = xAOD::L2MuonParameters::Chamber::BME;
      endcapinner  = xAOD::L2MuonParameters::Chamber::EndcapInner;
    }

    ATH_MSG_DEBUG("pattern#0: # of hits at inner  =" << pattern.mdtSegments[inner].size());
    ATH_MSG_DEBUG("pattern#0: # of hits at middle =" << pattern.mdtSegments[middle].size());
    ATH_MSG_DEBUG("pattern#0: # of hits at outer  =" << pattern.mdtSegments[outer].size());
    if (pattern.s_address==-1){
      ATH_MSG_DEBUG("pattern#0: # of hits at ee  =" << pattern.mdtSegments[ee].size());
      ATH_MSG_DEBUG("pattern#0: # of hits at endcap barrel inner  =" << pattern.mdtSegments[barrelinner].size());
      ATH_MSG_DEBUG("pattern#0: # of hits at BEE  =" << pattern.mdtSegments[bee].size());
    } else {
      ATH_MSG_DEBUG("pattern#0: # of hits at BME  =" << pattern.mdtSegments[bme].size());
      ATH_MSG_DEBUG("pattern#0: # of hits at barrel endcap inner  =" << pattern.mdtSegments[endcapinner].size());
    }
    ATH_MSG_DEBUG("pt=" << pattern.pt);

    // ---------
    // store xAOD

    xAOD::L2StandAloneMuon* muonSA = new xAOD::L2StandAloneMuon();
    muonSA->makePrivateStore();

    // add pT
    muonSA->setPt(pattern.pt*pattern.charge);
    muonSA->setPtEndcapAlpha(pattern.ptEndcapAlpha*pattern.charge);
    muonSA->setPtEndcapBeta(pattern.ptEndcapBeta*pattern.charge);
    muonSA->setPtEndcapRadius(pattern.ptEndcapRadius*pattern.charge);
    muonSA->setPtCSC(pattern.ptCSC*pattern.charge);

    muonSA->setEta(pattern.etaVtx);
    muonSA->setPhi(pattern.phiVtx);
    muonSA->setDeltaPt(pattern.deltaPt);
    muonSA->setDeltaEta(pattern.deltaEtaVtx);
    muonSA->setDeltaPhi(pattern.deltaPhiVtx);

    // add s_address
    muonSA->setSAddress(pattern.s_address);

    // add positions at MS
    muonSA->setEtaMS(pattern.etaMap);
    muonSA->setPhiMS(pattern.phiMS);
    muonSA->setDirPhiMS(pattern.phiMSDir);
    muonSA->setRMS(pattern.superPoints[inner].R);
    muonSA->setZMS(pattern.superPoints[inner].Z);
    muonSA->setDirZMS(pattern.superPoints[inner].Alin);

    // add pt variables
    // Endcap
    muonSA->setEndcapAlpha(pattern.endcapAlpha);
    muonSA->setEndcapBeta(pattern.endcapBeta);
    muonSA->setEndcapRadius(pattern.endcapRadius3P);
    // Barrel
    muonSA->setBarrelRadius(pattern.barrelRadius);
    muonSA->setBarrelSagitta(pattern.barrelSagitta);    
    
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
    if (pattern.s_address==-1){
      muonSA->setSuperPoint(ee, pattern.superPoints[ee].R, pattern.superPoints[ee].Z,
                            pattern.superPoints[ee].Alin, pattern.superPoints[ee].Blin, pattern.superPoints[ee].Chi2);
      muonSA->setSuperPoint(barrelinner, pattern.superPoints[barrelinner].R, pattern.superPoints[barrelinner].Z,
                            pattern.superPoints[barrelinner].Alin, pattern.superPoints[barrelinner].Blin, pattern.superPoints[barrelinner].Chi2);
      muonSA->setSuperPoint(csc, pattern.superPoints[csc].R, pattern.superPoints[csc].Z,
			    pattern.superPoints[csc].Alin, pattern.superPoints[csc].Blin, pattern.superPoints[csc].Chi2);
    } else {
      muonSA->setSuperPoint(endcapinner, pattern.superPoints[endcapinner].R, pattern.superPoints[endcapinner].Z,
                            pattern.superPoints[endcapinner].Alin, pattern.superPoints[endcapinner].Blin, pattern.superPoints[endcapinner].Chi2);
    }

    ///////////////////////////////
    // Below are detailed information

    uint32_t muondetmask = 0;

    /// Set L2 muon algorithm ID
    muonSA->setAlgoId( L2MuonAlgoMap(name()) );
    /// Set input TE ID
    //muonSA->setTeId( inputTE->getId() );	// move to hltExecute()	
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
    muonSA->setRpcHitsCapacity( m_esd_rpc_size );
    muonSA->setTgcHitsCapacity( m_esd_tgc_size );
    muonSA->setMdtHitsCapacity( m_esd_mdt_size );
    muonSA->setCscHitsCapacity( m_esd_csc_size );

    // MDT hits
    for (unsigned int i_hit=0; i_hit<mdtHits.size(); i_hit++) {
      if ( mdtHits[i_hit].isOutlier==0 || mdtHits[i_hit].isOutlier==1 ) {
        muonSA->setMdtHit(mdtHits[i_hit].OnlineId, mdtHits[i_hit].isOutlier, mdtHits[i_hit].Chamber,
                          mdtHits[i_hit].R, mdtHits[i_hit].Z, mdtHits[i_hit].cPhi0, mdtHits[i_hit].Residual, 
                          mdtHits[i_hit].DriftTime, mdtHits[i_hit].DriftSpace, mdtHits[i_hit].DriftSigma);  
      }
    }
    
    //CSC hits
    for(unsigned int i_hit=0; i_hit<cscHits.size(); i_hit++) {
      if ( 1/*cscHits[i_hit].MeasuresPhi==0*/ ){
        if ( cscHits[i_hit].isOutlier==0 || cscHits[i_hit].isOutlier==1 ) {
          muonSA->setCscHit(cscHits[i_hit].isOutlier, cscHits[i_hit].Chamber, cscHits[i_hit].StationName,
                            cscHits[i_hit].StationEta, cscHits[i_hit].StationPhi,
                            cscHits[i_hit].ChamberLayer, cscHits[i_hit].WireLayer, cscHits[i_hit].MeasuresPhi, cscHits[i_hit].Strip,
                            cscHits[i_hit].eta, cscHits[i_hit].phi, cscHits[i_hit].r, cscHits[i_hit].z,
                            cscHits[i_hit].charge, cscHits[i_hit].time, cscHits[i_hit].Residual);
          ATH_MSG_DEBUG("CSC Hits stored in xAOD: "
			<< "OL=" << cscHits[i_hit].isOutlier << ","
			<< "Ch=" << cscHits[i_hit].Chamber << ","
			<< "StationName=" << cscHits[i_hit].StationName << ","
			<< "StationEta=" << cscHits[i_hit].StationEta << ","
			<< "StationPhi=" << cscHits[i_hit].StationPhi << ","
			<< "ChamberLayer=" << cscHits[i_hit].ChamberLayer << ","
			<< "WireLayer=" << cscHits[i_hit].WireLayer << ","
			<< "MeasuresPhi=" << cscHits[i_hit].MeasuresPhi << ","
			<< "Strip=" << cscHits[i_hit].Strip << ","
			<< "eta="  << cscHits[i_hit].eta << ","
			<< "phi="  << cscHits[i_hit].phi << ","
			<< "r="  << cscHits[i_hit].r << ","
			<< "z="  << cscHits[i_hit].z << ","
			<< "charge=" << cscHits[i_hit].charge << ","
			<< "Rs=" << cscHits[i_hit].Residual << ","
			<< "t="  << cscHits[i_hit].time);
        }
      }
    }

    // RPC hits
    float sumbeta[8]={0};
    float nhit_layer[8]={0};
    for(unsigned int i_hit=0; i_hit<rpcHits.size(); i_hit++) {
      muonSA->setRpcHit(rpcHits[i_hit].layer, rpcHits[i_hit].measuresPhi, 
                        rpcHits[i_hit].x, rpcHits[i_hit].y, rpcHits[i_hit].z,
                        rpcHits[i_hit].time, rpcHits[i_hit].distToEtaReadout, rpcHits[i_hit].distToPhiReadout,
                        rpcHits[i_hit].stationName);
      ATH_MSG_DEBUG("RPC hits stored in xAOD: "
		    << "stationName=" << rpcHits[i_hit].stationName << ","
		    << "layer=" << rpcHits[i_hit].layer << ","
		    << "measuresPhi=" << rpcHits[i_hit].measuresPhi << ","
		    << "x=" << rpcHits[i_hit].x << ","
		    << "y=" << rpcHits[i_hit].y << ","
		    << "y=" << rpcHits[i_hit].z);
      
      float dRMS = sqrt( fabs(pattern.etaMap-rpcHits[i_hit].eta)*fabs(pattern.etaMap-rpcHits[i_hit].eta) + acos(cos(pattern.phiMS-rpcHits[i_hit].phi))*acos(cos(pattern.phiMS-rpcHits[i_hit].phi)) );
      if(dRMS>0.05) continue;
      float muToF = rpcHits[i_hit].l/1000/(CLHEP::c_light/1000);
      float Tprop = rpcHits[i_hit].distToPhiReadout/1000*4.8;
      float beta = rpcHits[i_hit].l/1000/(muToF+rpcHits[i_hit].time-Tprop+3.125/2)/(CLHEP::c_light/1000);
      sumbeta[rpcHits[i_hit].layer]=sumbeta[rpcHits[i_hit].layer]+beta;
      nhit_layer[rpcHits[i_hit].layer]=nhit_layer[rpcHits[i_hit].layer]+1;
    }

    std::vector<float> Avebeta_layer;
    for(int i_layer=0;i_layer<8;i_layer++){
      if(nhit_layer[i_layer]!=0)Avebeta_layer.push_back( sumbeta[i_layer]/nhit_layer[i_layer] );
    }
    if(Avebeta_layer.size()>0) muonSA->setBeta( std::accumulate(Avebeta_layer.begin(),Avebeta_layer.end(),0.0)/Avebeta_layer.size() );
    else muonSA->setBeta( 9999 );
    Avebeta_layer.clear();
    
    // TGC hits
    for(unsigned int i_hit=0; i_hit<tgcHits.size(); i_hit++) {
      muonSA->setTgcHit(tgcHits[i_hit].eta, tgcHits[i_hit].phi, tgcHits[i_hit].r, tgcHits[i_hit].z,
                        tgcHits[i_hit].width, tgcHits[i_hit].sta, tgcHits[i_hit].isStrip,
                        tgcHits[i_hit].bcTag, tgcHits[i_hit].inRoad);
      ATH_MSG_DEBUG("TGC hits stored in xAOD: "
		    << "eta=" << tgcHits[i_hit].eta << ","
		    << "phi=" << tgcHits[i_hit].phi << ","
		    << "r=" << tgcHits[i_hit].r << ","
		    << "z=" << tgcHits[i_hit].z << ","
		    << "width=" << tgcHits[i_hit].width << ","
		    << "stationNum=" << tgcHits[i_hit].sta << ","
		    << "isStrip=" << tgcHits[i_hit].isStrip << ","
		    << "bcTag=" << tgcHits[i_hit].bcTag << ","
		    << "inRoad=" << tgcHits[i_hit].inRoad);
    }

    // Muon road
    for (int i_station=0; i_station<8; i_station++) {
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
      if (tgcFitResult.isSuccess ) {
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
      }
    } else {
      // RPC fit results
      if (rpcFitResult.isSuccess ) {
	// Fill middle fit results for the moment

	muonSA->setRpcFitInn(rpcFitResult.phi_inner, rpcFitResult.slope_inner, rpcFitResult.offset_inner);
	muonSA->setRpcFitMid(rpcFitResult.phi_middle, rpcFitResult.slope_middle, rpcFitResult.offset_middle);
	muonSA->setRpcFitOut(rpcFitResult.phi_outer, rpcFitResult.slope_outer, rpcFitResult.offset_outer);
      }
    }

    // Store track positions if set of (R, Z, eta, phi) are all available
    if (pattern.s_address==-1) { // endcap

      // Inner
      if ( fabs(pattern.superPoints[inner].R) > ZERO_LIMIT && fabs(pattern.superPoints[inner].Z) > ZERO_LIMIT ) { // if R and Z exist   
        if ( tgcFitResult.isSuccess && fabs(tgcFitResult.tgcInn[3]) > ZERO_LIMIT ) { // if phi exist
          float theta = atan(pattern.superPoints[inner].R/fabsf(pattern.superPoints[inner].Z));
          float eta = (tan(theta/2.)!=0.)? -log(tan(theta/2.))*pattern.superPoints[inner].Z/fabsf(pattern.superPoints[inner].Z): 0.;
          muonSA->setTrackPosition( pattern.superPoints[inner].R, pattern.superPoints[inner].Z, eta, tgcFitResult.tgcInn[1] );
        }
      }

      // Middle
      if ( fabs(pattern.superPoints[middle].R) > ZERO_LIMIT && fabs(pattern.superPoints[middle].Z) > ZERO_LIMIT ) { // if R and Z exist 
        float phi = 0;
        if (tgcFitResult.isSuccess && ( fabs(tgcFitResult.tgcMid1[3]) > ZERO_LIMIT || fabs(tgcFitResult.tgcMid2[3]) > ZERO_LIMIT )) { // if phi exist
          double phi1 = tgcFitResult.tgcMid1[1];
          double phi2 = tgcFitResult.tgcMid2[1];
          if ( tgcFitResult.tgcMid1[3]==0. || tgcFitResult.tgcMid2[3]==0. ) {
            if ( fabs(tgcFitResult.tgcMid1[3]) > ZERO_LIMIT ) phi = phi1;
            if ( fabs(tgcFitResult.tgcMid2[3]) > ZERO_LIMIT ) phi = phi2;
          } else if( phi1*phi2 < 0 && std::abs(phi1)>(CLHEP::pi/2.) ) {
            double tmp1 = (phi1>0)? phi1 - CLHEP::pi : phi1 + CLHEP::pi;
            double tmp2 = (phi2>0)? phi2 - CLHEP::pi : phi2 + CLHEP::pi;
            double tmp  = (tmp1+tmp2)/2.;
            phi  = (tmp>0.)? tmp - CLHEP::pi : tmp + CLHEP::pi;
          } else {      
            phi  = (phi2+phi1)/2.;     
          }
        } else {
          phi = roi->phi();
        }
        float theta = atan(pattern.superPoints[middle].R/fabsf(pattern.superPoints[middle].Z));
        float eta = (tan(theta/2.)!=0.)? -log(tan(theta/2.))*pattern.superPoints[middle].Z/fabsf(pattern.superPoints[middle].Z): 0.;
        muonSA->setTrackPosition( pattern.superPoints[middle].R, pattern.superPoints[middle].Z, eta, phi );
      }

    } else { // barrel

      // Middle
      if ( fabs(pattern.superPoints[middle].R) > ZERO_LIMIT && fabs(pattern.superPoints[middle].Z) > ZERO_LIMIT ) { // if R and Z exist 
        float phi = 0;
        if (rpcFitResult.isSuccess) {
          phi = rpcFitResult.phi;
        } else {
          phi = roi->phi();
        }
        float theta = atan(pattern.superPoints[middle].R/fabsf(pattern.superPoints[middle].Z));
        float eta = (tan(theta/2.)!=0.)? -log(tan(theta/2.))*pattern.superPoints[middle].Z/fabsf(pattern.superPoints[middle].Z): 0.;
        muonSA->setTrackPosition( pattern.superPoints[middle].R, pattern.superPoints[middle].Z, eta, phi );
      }

      // Not stored outer position for the moment as the phi is not available

    }

    outputTracks->push_back(muonSA);
//-----------------------------------------------------------------------------------------------------------------------------------------
    // -------
    // store TrigRoiDescriptor
    if (fabs(muonSA->pt()) > ZERO_LIMIT ) {

      TrigRoiDescriptor* MSroiDescriptor = new TrigRoiDescriptor(roids->l1Id(),
                                                                 roids->roiId(),
                                                                 pattern.etaMap,
                                                                 pattern.etaMap,
                                                                 pattern.etaMap,
                                                                 pattern.phiMS,
                                                                 pattern.phiMS,
                                                                 pattern.phiMS);

      ATH_MSG_DEBUG("...TrigRoiDescriptor for MS "
		    << "pattern.etaMap/pattern.phiMS="
		    << pattern.etaMap << "/" << pattern.phiMS);

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
      phiHalfWidth = getRoiSizeForID(false,muonSA);
      etaHalfWidth = getRoiSizeForID(true, muonSA);

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
      
      ATH_MSG_DEBUG("...TrigRoiDescriptor for ID "
		    << "pattern.etaVtx/pattern.phiVtx="
		    << pattern.etaVtx << "/" << pattern.phiVtx);
      ATH_MSG_DEBUG("old RoI:        " << *roids);
      ATH_MSG_DEBUG("updated ID RoI: " << *IDroiDescriptor);      

      ATH_MSG_DEBUG("will Record an RoiDescriptor for TrigMoore:"
		      << " phi=" << MSroiDescriptor->phi()
		      << ",  eta=" << MSroiDescriptor->eta());

      ATH_MSG_DEBUG("will Record an RoiDescriptor for Inner Detector:"
		      << " phi=" << IDroiDescriptor->phi()
		      << ",  eta=" << IDroiDescriptor->eta());

      outputID->push_back(IDroiDescriptor);
      outputMS->push_back(MSroiDescriptor);
 
   } else { // pt = 0.
      
      TrigRoiDescriptor* IDroiDescriptor = new TrigRoiDescriptor(roids->l1Id(),
                                                                 roids->roiId(),
                                                                 roids->eta(),
                                                                 roids->eta() - (roids->eta() - roids->etaMinus()) * scaleRoIforZeroPt,
                                                                 roids->eta() + (roids->etaPlus() - roids->eta()) * scaleRoIforZeroPt,
                                                                 roids->phi(),
                                                                 HLT::wrapPhi(roids->phi() - HLT::wrapPhi(roids->phiPlus() - roids->phiMinus())/2. * scaleRoIforZeroPt),
                                                                 HLT::wrapPhi(roids->phi() + HLT::wrapPhi(roids->phiPlus() - roids->phiMinus())/2. * scaleRoIforZeroPt));

      ATH_MSG_DEBUG ("will Record an RoiDescriptor for Inner Detector in case with zero pT:"
		       << " phi=" << IDroiDescriptor->phi()
		       << ", phi min=" << IDroiDescriptor->phiMinus()
		       << ", phi max=" << IDroiDescriptor->phiPlus()
		       << ", eta=" << IDroiDescriptor->eta()
		       << ", eta min=" << IDroiDescriptor->etaMinus()
		       << ", eta max=" << IDroiDescriptor->etaPlus());

      outputID->push_back(IDroiDescriptor);
    }
  }

  if (outputTracks != 0 && outputTracks->size() > 0) {
    ATH_MSG_DEBUG ("will Record a xAOD::L2StandAloneMuonContainer");

    ATH_MSG_DEBUG("At findMuonSignature outputTracks size = " << outputTracks->size());
    xAOD::L2StandAloneMuonContainer::const_iterator p_muonOut = outputTracks->begin();
    xAOD::L2StandAloneMuonContainer::const_iterator p_muonOutEn = outputTracks->end();
    for (; p_muonOut != p_muonOutEn ; ++p_muonOut ){
      ATH_MSG_DEBUG("At findMuonSignature outputTracks pt = " << (*p_muonOut)->pt());
      ATH_MSG_DEBUG("At findMuonSignature outputTracks eta/phi = " << (*p_muonOut)->eta() << "/" << (*p_muonOut)->phi());
    }
  }
  return true;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

int MuFastSteering::L2MuonAlgoMap(const std::string& name)
{
  int algoId = 0;
  if (name == "MuFastSteering_Muon")  {
    algoId = xAOD::L2MuonParameters::L2MuonAlgoId::MUONID;
  } else if (name == "MuFastSteering_900GeV")  {
    algoId = xAOD::L2MuonParameters::L2MuonAlgoId::GEV900ID;
  } else {
    algoId = xAOD::L2MuonParameters::L2MuonAlgoId::NULLID;
  }

  return algoId;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

float MuFastSteering::getRoiSizeForID(bool isEta, const xAOD::L2StandAloneMuon* muonSA)
{
   bool  isBarrel = (muonSA->sAddress()==-1) ? true : false;
   float eta = muonSA->etaMS();
   float phi = muonSA->phiMS();
   float pt  = muonSA->pt();

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
  
  if( trackPatterns.size() > 0 ) {

    m_efficiency  = 1;
  
    const TrigL2MuonSA::TrackPattern& pattern = trackPatterns[0]; 
    float norm = 10.;
    
    float count_inner  = 0;
    float count_middle = 0;
    float count_outer  = 0;
    
    for(unsigned int i_hit=0; i_hit<mdtHits.size(); i_hit++) {
      
      if (fabs(mdtHits[i_hit].DriftSpace) < ZERO_LIMIT) continue;
      
      char st = mdtHits[i_hit].cType[1];
      
      if (st=='I') {
        count_inner++;
        m_res_inner.push_back(mdtHits[i_hit].Residual/norm);
        if (mdtHits[i_hit].isOutlier==0) m_fit_residuals.push_back(mdtHits[i_hit].Residual/norm);
      }
      
      if (st=='M') {
        count_middle++;
        m_res_middle.push_back(mdtHits[i_hit].Residual/norm);
        if (mdtHits[i_hit].isOutlier==0) m_fit_residuals.push_back(mdtHits[i_hit].Residual/norm);
      }
      
      if (st=='O') {
        count_outer++;
        m_res_outer.push_back(mdtHits[i_hit].Residual/norm);
        if (mdtHits[i_hit].isOutlier==0) m_fit_residuals.push_back(mdtHits[i_hit].Residual/norm);
      }
    }
    
    m_inner_mdt_hits  = count_inner;
    m_middle_mdt_hits = count_middle;
    m_outer_mdt_hits  = count_outer;
    
    m_track_pt    = (fabs(pattern.pt ) > ZERO_LIMIT)? pattern.charge*pattern.pt: 9999.;
    m_absolute_pt = fabs(m_track_pt);

    if ( fabs(pattern.etaMap) > ZERO_LIMIT || fabs(pattern.phiMS) > ZERO_LIMIT ) {
      m_track_eta.push_back(pattern.etaMap);
      m_track_phi.push_back(pattern.phiMS);
    }
    if ( fabs(pattern.pt ) < ZERO_LIMIT){
      m_failed_eta.push_back(roi->eta());
      m_failed_phi.push_back(roi->phi());
    }

    m_sagitta     = (fabs(pattern.barrelSagitta) > ZERO_LIMIT)? pattern.barrelSagitta: 9999.;
    m_sag_inverse = (fabs(pattern.barrelSagitta) > ZERO_LIMIT)? 1./pattern.barrelSagitta: 9999.;
    m_address     = pattern.s_address;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
// handler for "UpdateAfterFork")
void MuFastSteering::handle(const Incident& incident) {
  
  if (incident.type()!="UpdateAfterFork") return;
  ATH_MSG_DEBUG("+-----------------------------------+");
  ATH_MSG_DEBUG("| handle for UpdateAfterFork called |");
  ATH_MSG_DEBUG("+-----------------------------------+");
  
  // Find the Worker ID and create an individual muon buffer name for each worker
  StringProperty worker_id;
  std::string worker_name;

  worker_id.setName("worker_id");
  worker_id = std::string("");
  const std::vector<const Property*>* dataFlowProps = m_jobOptionsSvc->getProperties("DataFlowConfig");
  if ( dataFlowProps ) {
    ATH_MSG_DEBUG(" Properties available for 'DataFlowConfig': number = " << dataFlowProps->size());
    ATH_MSG_DEBUG(" --------------------------------------------------- ");
    for ( std::vector<const Property*>::const_iterator cur = dataFlowProps->begin();
          cur != dataFlowProps->end(); cur++) {
      ATH_MSG_DEBUG((*cur)->name() << " = " << (*cur)->toString());
      // the application name is found
      if ( (*cur)->name() == "DF_WorkerId" ) {
        if (worker_id.assign(**cur)) {
          ATH_MSG_DEBUG(" ---> got worker ID = " << worker_id.value());
          worker_name = worker_id.value() ;
        } else {
          ATH_MSG_WARNING(" ---> set property failed.");
        }
      }
    }
    
    if ( worker_id.value() == "" ) {
      ATH_MSG_DEBUG(" Property for DF_WorkerId not found.");
    }
  } else {
    ATH_MSG_DEBUG(" No Properties for 'DataFlowConfig' found.");
  }

  ATH_MSG_DEBUG(" MuonCalBufferSize     = " << m_calBufferSize);
  ATH_MSG_DEBUG("=================================================");
  
  // release JobOptionsSvc
  unsigned long mjcounter = m_jobOptionsSvc->release();
  ATH_MSG_DEBUG(" --> Release JobOptionsSvc Service, Counter = " << mjcounter);

  
  //
  // Create the calibration stream
  if (m_doCalStream) {
    // set a fixed name for the buffer
    m_calBufferName = "/tmp/muonCalStreamOutput";
    m_calStreamer->setBufferName(m_calBufferName);
    m_calStreamer->setInstanceName(worker_name);

    // if it's not a data scouting chain, open the circular buffer
    if (!m_calDataScouting) {
      StatusCode sc = m_calStreamer->openStream(m_calBufferSize);
      if ( sc != StatusCode::SUCCESS ) {  
	ATH_MSG_ERROR("Failed to open the connection to the circular buffer");
      }
      else {
	ATH_MSG_DEBUG("Opened the connection to the circular buffer");
      }
    }
  }

}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuFastSteering::prepareRobRequests(const HLT::TriggerElement* inputTE){

  ATH_MSG_DEBUG("prepareRobRequests called");
  
  HLT::RobRequestInfo* RRInfo = config()->robRequestInfo();

  if (!RRInfo) {
    ATH_MSG_ERROR("Null pointer to RobRequestInfo");
    return HLT::ERROR;
  }
  
  std::vector<uint32_t> MdtRobList;
  std::vector<uint32_t> RpcRobList;
  std::vector<uint32_t> TgcRobList;
  std::vector<uint32_t> CscRobList;
  
  std::vector<const TrigRoiDescriptor*> roids;
  HLT::ErrorCode hec = getFeatures(inputTE, roids);

  if (hec != HLT::OK) {
    ATH_MSG_ERROR("Could not find input TE");
    return hec;
  }
  
  // RoI base data access
  for (unsigned int i=0; i < roids.size(); i++) {
    
    if ( m_use_RoIBasedDataAccess_MDT) {

      float roi_eta = roids[i]->eta();
      float roi_phi = roids[i]->phi();
      if (roi_phi < 0) roi_phi += 2.0 * CLHEP::pi;
   
      double etaMin = roi_eta - 0.2;
      double etaMax = roi_eta + 0.2;
      double phiMin = roi_phi - 0.2;
      double phiMax = roi_phi + 0.2;
      if( phiMin < 0 ) phiMin += 2*CLHEP::pi;
      if( phiMax < 0 ) phiMax += 2*CLHEP::pi;
      if( phiMin > 2*CLHEP::pi ) phiMin -= 2*CLHEP::pi;
      if( phiMax > 2*CLHEP::pi ) phiMax -= 2*CLHEP::pi;

      TrigRoiDescriptor* roi = new TrigRoiDescriptor( roi_eta, etaMin, etaMax, roi_phi, phiMin, phiMax ); 
      const IRoiDescriptor* iroi = (IRoiDescriptor*) roi;

      MdtRobList.clear();
      if ( iroi ) m_regionSelector->DetROBIDListUint(MDT, *iroi, MdtRobList);
      RRInfo->addRequestScheduledRobIDs(MdtRobList);
      ATH_MSG_DEBUG("prepareRobRequests, find " << MdtRobList.size() << " Mdt Rob's,");

      if(roi) delete roi;
    }

    if ( m_use_RoIBasedDataAccess_RPC) {

      const IRoiDescriptor* iroi = (IRoiDescriptor*) roids[i];

      RpcRobList.clear();
      if ( iroi ) m_regionSelector->DetROBIDListUint(RPC, *iroi, RpcRobList);
      RRInfo->addRequestScheduledRobIDs(RpcRobList);
      ATH_MSG_DEBUG("prepareRobRequests, find " << RpcRobList.size() << " Rpc Rob's,");
    }

    if ( m_use_RoIBasedDataAccess_TGC) {

      float roi_eta = roids[i]->eta();
      float roi_phi = roids[i]->phi();
      if (roi_phi < 0) roi_phi += 2.0 * CLHEP::pi;
   
      double etaMin = roi_eta - 0.2;
      double etaMax = roi_eta + 0.2;
      double phiMin = roi_phi - 0.1;
      double phiMax = roi_phi + 0.1;
      if( phiMin < 0 ) phiMin += 2*CLHEP::pi;
      if( phiMax < 0 ) phiMax += 2*CLHEP::pi;
      if( phiMin > 2*CLHEP::pi ) phiMin -= 2*CLHEP::pi;
      if( phiMax > 2*CLHEP::pi ) phiMax -= 2*CLHEP::pi;

      TrigRoiDescriptor* roi = new TrigRoiDescriptor( roi_eta, etaMin, etaMax, roi_phi, phiMin, phiMax ); 
      const IRoiDescriptor* iroi = (IRoiDescriptor*) roi;

      TgcRobList.clear();
      if ( iroi ) m_regionSelector->DetROBIDListUint(TGC, *iroi, TgcRobList);
      RRInfo->addRequestScheduledRobIDs(TgcRobList);
      ATH_MSG_DEBUG("prepareRobRequests, find " << TgcRobList.size() << " Tgc Rob's,");

      if(roi) delete roi;
    }

    if ( m_use_RoIBasedDataAccess_CSC) {

      const IRoiDescriptor* iroi = (IRoiDescriptor*) roids[i];

      CscRobList.clear();
      if ( iroi ) m_regionSelector->DetROBIDListUint(CSC, *iroi, CscRobList);
      RRInfo->addRequestScheduledRobIDs(CscRobList);
      ATH_MSG_DEBUG("prepareRobRequests, find " << CscRobList.size() << " Csc Rob's,");
    }
  }
  
  // Full data access
  if ( !m_use_RoIBasedDataAccess_MDT ) {
    MdtRobList.clear();
    m_regionSelector->DetROBIDListUint(MDT, MdtRobList);
    RRInfo->addRequestScheduledRobIDs(MdtRobList);
    ATH_MSG_DEBUG("prepareRobRequests, find " << MdtRobList.size() << " Mdt Rob's,");
  }

  if ( !m_use_RoIBasedDataAccess_RPC ) {
    RpcRobList.clear();
    m_regionSelector->DetROBIDListUint(RPC, RpcRobList);
    RRInfo->addRequestScheduledRobIDs(RpcRobList);
    ATH_MSG_DEBUG("prepareRobRequests, find " << RpcRobList.size() << " Rpc Rob's,");
  }

  if ( !m_use_RoIBasedDataAccess_TGC ) {
    TgcRobList.clear();
    m_regionSelector->DetROBIDListUint(TGC, TgcRobList);
    RRInfo->addRequestScheduledRobIDs(TgcRobList);
    ATH_MSG_DEBUG("prepareRobRequests, find " << TgcRobList.size() << " Tgc Rob's,");
  }

  if ( !m_use_RoIBasedDataAccess_CSC ) {
    CscRobList.clear();
    m_regionSelector->DetROBIDListUint(CSC, CscRobList);
    RRInfo->addRequestScheduledRobIDs(CscRobList);
    ATH_MSG_DEBUG("prepareRobRequests, find " << CscRobList.size() << " Csc Rob's,");
  }
  
  return HLT::OK;
}
