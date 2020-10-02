/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <numeric>
#include <string>

#include "MuFastSteering.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

#include "CxxUtils/phihelper.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaInterprocess/Incidents.h"
#include "AthenaMonitoringKernel/Monitored.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MuFastSteering::MuFastSteering(const std::string& name, ISvcLocator* svc) 
  : HLT::FexAlgo(name, svc), 
    m_timerSvc("TrigTimerSvc", name),
    m_regionSelector("RegSelSvc", name),
    m_recMuonRoIUtils(),
    m_rpcHits(), m_tgcHits(),
    m_mdtRegion(), m_muonRoad(),
    m_rpcFitResult(), m_tgcFitResult(),
    m_mdtHits_normal(), m_mdtHits_overlap(),
    m_cscHits(),
    m_stgcHits(), m_mmHits()
{
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuFastSteering::hltInitialize()
{
  ATH_MSG_DEBUG("Initializing MuFastSteering - package version " << PACKAGE_VERSION);
  
  StatusCode sc;

  if (m_use_timer) {
    if (m_timerSvc.retrieve().isFailure()) {
      ATH_MSG_ERROR("Unable to locate TrigTimer Service");
    } else {
      ATH_MSG_DEBUG("Retrieved TrigTimer Service");
      m_timingTimers.push_back(m_timerSvc->addItem(name()+":DataPreparator"));
      m_timingTimers.push_back(m_timerSvc->addItem(name()+":PatternFinder"));
      m_timingTimers.push_back(m_timerSvc->addItem(name()+":StationFitter"));
      m_timingTimers.push_back(m_timerSvc->addItem(name()+":TrackFitter"));
      m_timingTimers.push_back(m_timerSvc->addItem(name()+":TrackExtrapolator"));
      m_timingTimers.push_back(m_timerSvc->addItem(name()+":CalibrationStreamer"));
      m_timingTimers.push_back(m_timerSvc->addItem(name()+":TotalProcessing"));
    }
  }

  // Locate RegionSelector
  if (m_regionSelector.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not retrieve the regionselector service");
    return HLT::ERROR;
  }
  ATH_MSG_DEBUG("Retrieved the RegionSelector service ");

  // Locate DataPreparator
  if (m_dataPreparator.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve Tool DataPreparator");
    return HLT::BAD_JOB_SETUP;
  }
  
  // Locate PatternFinder
  if (m_patternFinder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve Tool DataPreparator");
    return HLT::BAD_JOB_SETUP;
  }

  // Locate StationFitter
  if (m_stationFitter.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve Tool StationFitter");
    return HLT::BAD_JOB_SETUP;
  }

  // Locate TrackFitter
  if (m_trackFitter.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve Tool TrackFitter");
    return HLT::BAD_JOB_SETUP;
  }

  // Locate TrackExtrapolator
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

  // FtfRoadDefiner
  if ( m_ftfRoadDefiner.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve " << m_ftfRoadDefiner);
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
                                          m_use_RoIBasedDataAccess_CSC,
					  m_use_RoIBasedDataAccess_STGC,
					  m_use_RoIBasedDataAccess_MM);

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
      p_incidentSvc->addListener(this,AthenaInterprocess::UpdateAfterFork::type(),pri);
      p_incidentSvc.release().ignore();
    }
  }

  // DataHandles for AthenaMT
  if (m_eventInfoKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("ReadHandleKey for xAOD::EventInfo key:" << m_eventInfoKey.key()  << " initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  }
  if (m_roiCollectionKey.initialize().isFailure() ) { 
    ATH_MSG_ERROR("ReadHandleKey for TrigRoiDescriptorCollection key:" << m_roiCollectionKey.key()  << " initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  }
  if (m_recRoiCollectionKey.initialize().isFailure() ) { 
    ATH_MSG_ERROR("ReadHandleKey for DataVector<LVL1::RecMuonRoI> key:" << m_recRoiCollectionKey.key() << " initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  }
  // for Inside-Out mode ---
  if (m_FTFtrackKey.initialize(m_insideOut).isFailure() ) {
    ATH_MSG_ERROR("ReadHandleKey for xAOD::TrackParticleContainer key:" << m_FTFtrackKey.key() << " initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  }
  if (m_outputCBmuonCollKey.initialize(m_insideOut).isFailure() ) {
    ATH_MSG_ERROR("ReadHandleKey for xAOD::L2CombinedMuonContainer key:" << m_outputCBmuonCollKey.key() << " initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  }
  // ----
  if (m_muFastContainerKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("WriteHandleKey for xAOD::L2StandAloneMuonContainer key:" << m_muFastContainerKey.key() << " initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  }
  if (m_muCompositeContainerKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("WriteHandleKey for xAOD::TrigCompositeContainer key:" << m_muCompositeContainerKey.key() << " initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  }
  if (m_muIdContainerKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("WriteHandleKey for TrigRoiDescriptorCollection key:" << m_muIdContainerKey.key() << " initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  }

  if (m_muMsContainerKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("WriteHandleKey for TrigRoiDescriptorCollection key:" << m_muMsContainerKey.key() << " initialize Failure!");
    return HLT::BAD_JOB_SETUP;
  }
  if (not m_monTool.name().empty()) {
    if ( !m_monTool.retrieve() ) {
      ATH_MSG_ERROR("Cannot retrieve MonitoredTool");
      return HLT::BAD_JOB_SETUP;
    }
  }
  ATH_MSG_DEBUG( "topoRoad = " << m_topoRoad);

  if (m_fill_FSIDRoI) {
    ATH_MSG_INFO("will fill " << m_muIdContainerKey.key() << " in Full Scan mode. Please check if it's correct.");
  }

  return HLT::OK;
}

// --------------------------------------------------------------------------------

HLT::ErrorCode MuFastSteering::hltStop() {
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
  ATH_MSG_DEBUG("StatusCode MuFastSteering::execute start");
  ATH_MSG_DEBUG("InsideOutMode: " << m_insideOut);

  auto totalTimer = Monitored::Timer( "TIME_Total" );
  auto monitorIt	= Monitored::Group(m_monTool, totalTimer );
  totalTimer.start();

  auto ctx = getContext();
  ATH_MSG_DEBUG("Get event context << " << ctx );

  // retrieve with ReadHandle
  auto roiCollectionHandle = SG::makeHandle( m_roiCollectionKey, ctx );
  const TrigRoiDescriptorCollection *roiCollection = roiCollectionHandle.cptr();
  if (!roiCollectionHandle.isValid()){
    ATH_MSG_ERROR("ReadHandle for TrigRoiDescriptorCollection key:" << m_roiCollectionKey.key() << " isn't Valid");
    return StatusCode::FAILURE;
  }

  auto recRoiCollectionHandle = SG::makeHandle( m_recRoiCollectionKey, ctx );
  const DataVector<LVL1::RecMuonRoI> *recRoiCollection = recRoiCollectionHandle.cptr();
  if (!recRoiCollectionHandle.isValid()){
    ATH_MSG_ERROR("ReadHandle for DataVector<LVL1::RecMuonRoI> key:" << m_recRoiCollectionKey.key() << " isn't Valid");
    return StatusCode::FAILURE;
  }

  std::vector< const TrigRoiDescriptor* > internalRoI;
  TrigRoiDescriptorCollection::const_iterator p_roids = roiCollection->begin();
  TrigRoiDescriptorCollection::const_iterator p_roidsEn = roiCollection->end();

  for(; p_roids != p_roidsEn; ++p_roids ) {
    internalRoI.push_back(*p_roids);
    ATH_MSG_DEBUG("REGTEST: " << m_roiCollectionKey.key() << " eta = " << "(" << (*p_roids)->etaMinus() << ")" << (*p_roids)->eta() << "(" << (*p_roids)->etaPlus() << ")");
    ATH_MSG_DEBUG("REGTEST: " << m_roiCollectionKey.key() << " phi = " << "(" << (*p_roids)->phiMinus() << ")" << (*p_roids)->phi() << "(" << (*p_roids)->phiPlus() << ")");
    ATH_MSG_DEBUG("REGTEST: " << m_roiCollectionKey.key() << " zed = " << "(" << (*p_roids)->zedMinus() << ")" << (*p_roids)->zed() << "(" << (*p_roids)->zedPlus() << ")");
  }
  ATH_MSG_DEBUG("REGTEST: " << m_roiCollectionKey.key() << " size = " << internalRoI.size());

  // make RecMURoIs maching with MURoIs
  std::vector< const LVL1::RecMuonRoI* > recRoIVector;
  std::vector< const LVL1::RecMuonRoI* > surrRoIs;

  for (size_t size=0; size<roiCollection->size(); size++){
    const LVL1::RecMuonRoI* recRoI = matchingRecRoI( roiCollection->at(size)->roiWord(),  *recRoiCollection );
    CHECK( recRoI != nullptr );
    recRoIVector.push_back(recRoI);
    ATH_MSG_DEBUG("REGTEST: " << m_recRoiCollectionKey.key() << " eta/phi = " << (recRoI)->eta() << "/" << (recRoI)->phi());
    ATH_MSG_DEBUG("REGTEST: " << m_recRoiCollectionKey.key() << " size = " << recRoIVector.size());
  }

  int nPassedBarrelSurrRoi = 0;
  if(m_topoRoad ){
    for( const auto recRoI: *recRoiCollection ){
      if(std::find(recRoIVector.begin(), recRoIVector.end(), recRoI) != recRoIVector.end()) continue;

      bool surrounding = false;
      for( const auto matchedRoI: recRoIVector ){
        float deta = fabs(recRoI->eta() - matchedRoI->eta());
        float dphi = fabs(recRoI->phi() - matchedRoI->phi());
        if( dphi > M_PI )dphi = 2.*M_PI - dphi;
        if( deta < m_dEtasurrRoI && dphi < m_dPhisurrRoI)
          surrounding = true;
      }

      if(surrounding)
        surrRoIs.push_back(recRoI);
    }

    ATH_MSG_DEBUG("surrRoI: " << " size = " << surrRoIs.size());
    for( const auto recRoI: surrRoIs ){
      ATH_MSG_DEBUG("surrRoI: " << " eta/phi = " << (recRoI)->eta() << "/" << (recRoI)->phi() );
      if( fabs((recRoI)->eta()) <= 1.05 && (recRoI)->getThresholdNumber() >= 1 )nPassedBarrelSurrRoi++;
    }
    ATH_MSG_DEBUG( "nPassedBarrelSurrRoi = " << nPassedBarrelSurrRoi);
    //dynamicDeltaRpcMode
    if( m_topoRoad )m_dataPreparator->setMultiMuonTrigger( (nPassedBarrelSurrRoi >= 1)? true : false );
  }

  // record data objects with WriteHandle
  auto muFastContainer = SG::makeHandle(m_muFastContainerKey, ctx);
  ATH_CHECK(muFastContainer.record(std::make_unique<xAOD::L2StandAloneMuonContainer>(), std::make_unique<xAOD::L2StandAloneMuonAuxContainer>()));

  auto muCompositeContainer = SG::makeHandle(m_muCompositeContainerKey, ctx);
  ATH_CHECK(muCompositeContainer.record(std::make_unique<xAOD::TrigCompositeContainer>(), std::make_unique<xAOD::TrigCompositeAuxContainer>()));

  auto muIdContainer = SG::makeHandle(m_muIdContainerKey, ctx);
  ATH_CHECK(muIdContainer.record(std::make_unique<TrigRoiDescriptorCollection>()));

  auto muMsContainer = SG::makeHandle(m_muMsContainerKey, ctx);
  ATH_CHECK(muMsContainer.record(std::make_unique<TrigRoiDescriptorCollection>()));


  // Inside-out L2Muon mode
  if(m_insideOut) {
    ATH_MSG_DEBUG("start inside-out mode...");

    auto muonCBColl = SG::makeHandle (m_outputCBmuonCollKey, ctx);
    ATH_CHECK( muonCBColl.record (std::make_unique<xAOD::L2CombinedMuonContainer>(),
				  std::make_unique<xAOD::L2CombinedMuonAuxContainer>()) );

    auto trackHandle = SG::makeHandle( m_FTFtrackKey, ctx );
    const xAOD::TrackParticleContainer *tracks = trackHandle.cptr();
    if (!trackHandle.isValid()){
      ATH_MSG_ERROR("ReadHandle for TrackParticleContainer key:" << m_FTFtrackKey.key() << " isn't Valid");
      return StatusCode::FAILURE;
    }

    ATH_CHECK(findMuonSignatureIO(*tracks, internalRoI, recRoIVector,
				  *muonCBColl, *muFastContainer ));

    ATH_MSG_DEBUG("REGTEST: xAOD::L2CombinedMuonContainer key:" << m_outputCBmuonCollKey.key() << " size = " << muonCBColl->size());
    xAOD::L2CombinedMuonContainer::const_iterator p_CBmuon = (*muonCBColl).begin();
    xAOD::L2CombinedMuonContainer::const_iterator p_CBmuonEn = (*muonCBColl).end();
    for (;p_CBmuon != p_CBmuonEn; ++p_CBmuon) {
      ATH_MSG_DEBUG("REGTEST: xAOD::L2CombinedMuonContainer key:" << m_outputCBmuonCollKey.key() << " pt = " << (*p_CBmuon)->pt() << " GeV");
      ATH_MSG_DEBUG("REGTEST: xAOD::L2CombinedMuonContainer key:" << m_outputCBmuonCollKey.key() << " eta/phi = " << (*p_CBmuon)->eta() << "/" << (*p_CBmuon)->phi());
    }

  }
  else {
    // to StatusCode findMuonSignature()
    ATH_CHECK(findMuonSignature(internalRoI, recRoIVector,
				*muFastContainer, *muIdContainer, *muMsContainer, *muCompositeContainer ));
  }

  // DEBUG TEST: Recorded data objects
  ATH_MSG_DEBUG("Recorded data objects");
  ATH_MSG_DEBUG("REGTEST: xAOD::L2StandAloneMuonContainer key:" << m_muFastContainerKey.key() << " size = " << muFastContainer->size());

  for (auto  p_muon : *muFastContainer) {
    ATH_MSG_DEBUG("REGTEST: xAOD::L2StandAloneMuonContainer key:" << m_muFastContainerKey.key() << " pt = " << (*p_muon).pt() << " GeV");
    ATH_MSG_DEBUG("REGTEST: xAOD::L2StandAloneMuonContainer key:" << m_muFastContainerKey.key() << " eta/phi = " << (*p_muon).eta() << "/" << (*p_muon).phi());
  }

  ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection key:" << m_muIdContainerKey.key() << " size = " << muIdContainer->size());
  for (auto  p_muonID : *muIdContainer) {
    ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection key:" << m_muIdContainerKey.key() << " eta/phi = " << (*p_muonID).eta() << "/" << (*p_muonID).phi());
  }

  ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection key:" << m_muMsContainerKey.key() << " size = " << muMsContainer->size());
  for (auto  p_muonMS : *muMsContainer) {
    ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection key:" << m_muMsContainerKey.key() << " eta/phi = " << (*p_muonMS).eta() << "/" << (*p_muonMS).phi());
  }

  totalTimer.stop();

  ATH_MSG_DEBUG("StatusCode MuFastSteering::execute() success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode MuFastSteering::hltExecute(const HLT::TriggerElement* /*inputTE*/, 
                                          HLT::TriggerElement* outputTE)
{
  ATH_MSG_DEBUG("hltExecute called");

  auto totalTimer = Monitored::Timer( "TIME_Total" );
  auto monitorIt	= Monitored::Group(m_monTool, totalTimer );
  totalTimer.start();

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

  std::vector<const TrigRoiDescriptor*> internalRoI;
  ATH_MSG_DEBUG("REGTEST: " << m_roiCollectionKey.key() << " size: " << roids.size());

  std::vector<const LVL1::RecMuonRoI*> internalRecRoI;
  ATH_MSG_DEBUG("REGTEST: " << m_recRoiCollectionKey.key() << " size = " << muonRoIs.size());

  p_roi = muonRoIs.begin();
  for ( p_roids=roids.begin(); p_roids!=roids.end(); ++p_roids) {
    internalRoI.push_back(*p_roids);
    ATH_MSG_DEBUG("REGTEST: " << m_roiCollectionKey.key() << " eta = " << "(" << (*p_roids)->etaMinus() << ")" << (*p_roids)->eta() << "(" << (*p_roids)->etaPlus() << ")");
    ATH_MSG_DEBUG("REGTEST: " << m_roiCollectionKey.key() << " phi = " << "(" << (*p_roids)->phiMinus() << ")" << (*p_roids)->phi() << "(" << (*p_roids)->phiPlus() << ")");
    ATH_MSG_DEBUG("REGTEST: " << m_roiCollectionKey.key() << " zed = " << "(" << (*p_roids)->zedMinus() << ")" << (*p_roids)->zed() << "(" << (*p_roids)->zedPlus() << ")");
    internalRecRoI.push_back(*p_roi);
    ATH_MSG_DEBUG("REGTEST: " << m_recRoiCollectionKey.key() << " eta/phi = " << (*p_roi)->eta() << "/" << (*p_roi)->phi());

    p_roi++;
  }

  // define objects to record output data
  // for xAOD::L2StandAloneMuonContainer
  DataVector<xAOD::L2StandAloneMuon> *outputTracks = new DataVector<xAOD::L2StandAloneMuon>();
  outputTracks->clear();
  xAOD::L2StandAloneMuonAuxContainer aux;
  outputTracks->setStore( &aux );
  
  // for xAOD::TrigCompositeContainer
  DataVector<xAOD::TrigComposite> *outputComposite = new DataVector<xAOD::TrigComposite>();
  outputComposite->clear();
  xAOD::TrigCompositeAuxContainer auxComp;
  outputComposite->setStore( &auxComp );

  // for TrigRoiDescriptor for ID
  TrigRoiDescriptorCollection *outputID = new TrigRoiDescriptorCollection();
  outputID->clear();

  // for TrigRoiDescriptor for MS
  TrigRoiDescriptorCollection *outputMS = new TrigRoiDescriptorCollection();
  outputMS->clear();
  
  // set DynamiDeltaRPCMode
  m_dataPreparator->setMultiMuonTrigger(m_topoRoad);

  // to StatusCode findMuonSignature()
  StatusCode sc = findMuonSignature(internalRoI, internalRecRoI,
				    *outputTracks, *outputID, *outputMS, *outputComposite );
  
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
    code = attachFeature(outputTE, new TrigRoiDescriptor(SG::VIEW_ELEMENTS), "forID");
    if (code != HLT::OK) {
      return code;
    }
    delete outputMS;
    code = attachFeature(outputTE, new TrigRoiDescriptor(SG::VIEW_ELEMENTS), "forMS");
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
    for (size_t size=0; size<outputTracks->size(); size++){
     outputTracks->at(size)->setTeId( outputTE->getId() );
     ATH_MSG_DEBUG("outputTE(" << size << ") = " << outputTracks->at(size)->teId());
    }
    code = attachFeature(outputTE, outputTracks, "MuonL2SAInfo");
    if ( code != HLT::OK ) { 
      ATH_MSG_ERROR("Record of MuonL2SAInfo failed");
      ActiveState = false;
      return false;
    }
    ATH_MSG_DEBUG("Recorded xAOD::L2StandAloneMuonContainer key:" << m_muFastContainerKey.key());
    ATH_MSG_DEBUG("REGTEST: xAOD::L2StandAloneMuoonContainer key:" << m_muFastContainerKey.key() << " size = " << outputTracks->size());
    xAOD::L2StandAloneMuonContainer::const_iterator p_muon = (*outputTracks).begin();
    xAOD::L2StandAloneMuonContainer::const_iterator p_muonEn = (*outputTracks).end();
    for (;p_muon != p_muonEn; ++p_muon) {
      ATH_MSG_DEBUG("REGTEST: xAOD::L2StandAloneMuonContainer key:" << m_muFastContainerKey.key() << " pt = " << (*p_muon)->pt() << " GeV");
      ATH_MSG_DEBUG("REGTEST: xAOD::L2StandAloneMuonContainer key:" << m_muFastContainerKey.key() << " eta/phi = " << (*p_muon)->eta() << "/" << (*p_muon)->phi());
    }
  }

  if (outputID->empty()) {
    delete outputID;
  } else {
    ActiveState = true;
    outputTE -> setActiveState(ActiveState);
    code = attachFeature(outputTE, (TrigRoiDescriptor*)(outputID->at(0)), "forID");
    if ( code != HLT::OK ) { 
      ATH_MSG_ERROR("Record of TrigRoiInfo for ID failed");
      ActiveState = false;
      return false;
    }
    ATH_MSG_DEBUG("Recorded TrigRoiDescriptorCollection key:" << m_muIdContainerKey.key());
    ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection key:" << m_muIdContainerKey.key() << " size = " << outputID->size());
    TrigRoiDescriptorCollection::const_iterator p_muonID = outputID->begin();
    TrigRoiDescriptorCollection::const_iterator p_muonIDEn = outputID->end();
    for(; p_muonID != p_muonIDEn; ++p_muonID ) {
      ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection key:" << m_muIdContainerKey.key() << " eta/phi = " << (*p_muonID)->eta() << "/" << (*p_muonID)->phi());
    }
  }

  if (outputMS->empty()) {
    delete outputMS;
  } else {
    ActiveState = true;
    outputTE -> setActiveState(ActiveState);
    code = attachFeature(outputTE, (TrigRoiDescriptor*)(outputMS->at(0)), "forMS");
    if ( code != HLT::OK ) { 
      ATH_MSG_ERROR("Record of TrigRoiInfo for MS failed");
      ActiveState = false;
      return false;
    }
    ATH_MSG_DEBUG("Recorded TrigRoiDescriptorCollection key:" << m_muMsContainerKey.key());
    ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection key:" << m_muMsContainerKey.key() << " size = " << outputMS->size());
    TrigRoiDescriptorCollection::const_iterator p_muonMS = outputMS->begin();
    TrigRoiDescriptorCollection::const_iterator p_muonMSEn = outputMS->end();
    for(; p_muonMS != p_muonMSEn; ++p_muonMS ) {
      ATH_MSG_DEBUG("REGTEST: TrigRoiDescriptorCollection key:" << m_muMsContainerKey.key() << " eta/phi = " << (*p_muonMS)->eta() << "/" << (*p_muonMS)->phi());
    }
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

  totalTimer.stop();
  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode MuFastSteering::findMuonSignature(const std::vector<const TrigRoiDescriptor*>& roids,
                                             const std::vector<const LVL1::RecMuonRoI*>&  muonRoIs,
				             DataVector<xAOD::L2StandAloneMuon>& 	outputTracks,
					     TrigRoiDescriptorCollection& 		outputID,
					     TrigRoiDescriptorCollection&		outputMS,
					     DataVector<xAOD::TrigComposite>&          	outputComposite )
{
  ATH_MSG_DEBUG("StatusCode MuFastSteering::findMuonSignature start");
  StatusCode sc = StatusCode::SUCCESS;

  auto prepTimer           = Monitored::Timer( "TIME_Data_Preparator" );
  auto patternTimer        = Monitored::Timer( "TIME_Pattern_Finder" );
  auto stationFitterTimer  = Monitored::Timer( "TIME_Station_Fitter" );
  auto trackFitterTimer    = Monitored::Timer( "TIME_Track_Fitter" );
  auto trackExtraTimer     = Monitored::Timer( "TIME_Track_Extrapolator" );
  auto calibrationTimer    = Monitored::Timer( "TIME_Calibration_Streamer" );

  auto monitorIt	= Monitored::Group(m_monTool, prepTimer, patternTimer, stationFitterTimer,
                                                trackFitterTimer, trackExtraTimer, calibrationTimer );
 
  if (m_use_timer) {
    for (unsigned int i_timer=0; i_timer<m_timingTimers.size(); i_timer++) {
      m_timingTimers[i_timer]->start();
      m_timingTimers[i_timer]->pause();
    }
  }

  if (m_use_timer) m_timingTimers[ITIMER_TOTAL_PROCESSING]->resume();
  if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->resume();

  DataVector<const TrigRoiDescriptor>::const_iterator p_roids;
  DataVector<const LVL1::RecMuonRoI>::const_iterator p_roi;

  // muonRoIs = RecMURoIs, roids = MURoIs 
  p_roids = roids.begin();
  for (p_roi=(muonRoIs).begin(); p_roi!=(muonRoIs).end(); ++p_roi) {

    prepTimer.start();
    std::vector<TrigL2MuonSA::TrackPattern> trackPatterns;
    m_mdtHits_normal.clear();
    m_mdtHits_overlap.clear();
    m_cscHits.clear();
    m_stgcHits.clear();
    m_mmHits.clear();
    m_rpcFitResult.Clear();
    m_tgcFitResult.Clear();

    m_muonRoad.Clear();

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
                                         m_insideOut,
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
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
        	            m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			    m_stgcHits, m_mmHits,
                            trackPatterns, outputTracks, outputID, outputMS);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
      prepTimer.stop();

      if ( m_rpcErrToDebugStream && m_dataPreparator->isRpcFakeRoi() ) {
        ATH_MSG_ERROR("Invalid RoI in RPC data found: event to debug stream");
	TrigL2MuonSA::TrackPattern trackPattern;
	trackPatterns.push_back(trackPattern);
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
        	            m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			    m_stgcHits, m_mmHits,
                            trackPatterns, outputTracks, outputID, outputMS);
	continue;
      }

      // Pattern finding
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
      patternTimer.start();
      sc = m_patternFinder->findPatterns(m_muonRoad,
                                         m_mdtHits_normal,
                                         trackPatterns);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Pattern finder failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
        	            m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			    m_stgcHits, m_mmHits,
                            trackPatterns, outputTracks, outputID, outputMS);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
      patternTimer.stop();

      // Superpoint fit
      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
      stationFitterTimer.start();
      sc = m_stationFitter->findSuperPoints(*p_roi,
                                            m_muonRoad,
                                            m_rpcFitResult,
                                            trackPatterns);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Super point fitter failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
        	            m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			    m_stgcHits, m_mmHits,
                            trackPatterns, outputTracks, outputID, outputMS);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
      stationFitterTimer.stop();

      // Track fitting
      if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
      trackFitterTimer.start();
      sc = m_trackFitter->findTracks(*p_roi,
                                      m_rpcFitResult,
                                      trackPatterns);
                                     
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Track fitter failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
        	            m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			    m_stgcHits, m_mmHits,
                            trackPatterns, outputTracks, outputID, outputMS);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
      trackFitterTimer.stop();

    } else { // Endcap
      ATH_MSG_DEBUG("Endcap");

      prepTimer.start();
      // Data preparation
      m_rpcHits.clear();
      m_tgcHits.clear();
      sc = m_dataPreparator->prepareData(*p_roi,
                                         *p_roids,
                                         m_insideOut,
                                         m_tgcHits,
                                         m_muonRoad,
                                         m_mdtRegion,
                                         m_tgcFitResult,
                                         m_mdtHits_normal,
                                         m_mdtHits_overlap,
                                         m_cscHits,
					 m_stgcHits,
					 m_mmHits);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Data preparation failed");
 	TrigL2MuonSA::TrackPattern trackPattern;
	trackPatterns.push_back(trackPattern);
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
        	            m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			    m_stgcHits, m_mmHits,
                            trackPatterns, outputTracks, outputID, outputMS);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
      prepTimer.stop();

      // Pattern finding
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
      patternTimer.start();
      sc = m_patternFinder->findPatterns(m_muonRoad,
                                         m_mdtHits_normal,
					 m_stgcHits,
					 m_mmHits,
                                         trackPatterns);



      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Pattern finder failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
        	            m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			    m_stgcHits, m_mmHits,
                            trackPatterns, outputTracks, outputID, outputMS);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
      patternTimer.stop();
      
      // Superpoint fit
      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
      stationFitterTimer.start();
      if(!m_use_new_segmentfit){
        sc = m_stationFitter->findSuperPointsSimple(*p_roi,
						    m_muonRoad,
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
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
        	            m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			    m_stgcHits, m_mmHits,
                            trackPatterns, outputTracks, outputID, outputMS);
	continue;
      }

      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
      stationFitterTimer.stop();

      // Track fittingh    
      if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
      trackFitterTimer.start();
      sc = m_trackFitter->findTracks(*p_roi,
                                     m_tgcFitResult,
                                     trackPatterns,
                                     m_muonRoad);

      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Track fitter failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
        	            m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			    m_stgcHits, m_mmHits,
                            trackPatterns, outputTracks, outputID, outputMS);
	continue;
        }
      if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
      trackFitterTimer.stop();
    }
 
    // fix if eta is strange
    const float ETA_LIMIT       = 2.8;
    const float DELTA_ETA_LIMIT = 1.0;
    const float ZERO_LIMIT = 1.e-5;
    for (TrigL2MuonSA::TrackPattern& track : trackPatterns) {
       float roiEta = (*p_roi)->eta();
       if (fabs(track.pt) > ZERO_LIMIT
           && ( fabs(track.etaMap) > ETA_LIMIT || fabs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
          track.etaMap = roiEta;
       }
    }

    // Track extrapolation for ID combined
    if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->resume();
    trackExtraTimer.start();

    sc = m_trackExtrapolator->extrapolateTrack(trackPatterns, m_winPt);

    if (sc != StatusCode::SUCCESS) {
      ATH_MSG_WARNING("Track extrapolator failed");
      // Update output trigger element
      updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
      	                  m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			  m_stgcHits, m_mmHits,
                          trackPatterns, outputTracks, outputID, outputMS);
      continue;
    }
    if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->pause();
    trackExtraTimer.stop();
    
    // Update monitoring variables
    sc = updateMonitor(*p_roi, m_mdtHits_normal, trackPatterns );
    if (sc != StatusCode::SUCCESS) {
      ATH_MSG_WARNING("Failed to update monitoring variables");
      // Update output trigger element
      updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
       	                  m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			  m_stgcHits, m_mmHits,
                          trackPatterns, outputTracks, outputID, outputMS);
      continue;
    }

    // Update output trigger element
    updateOutputObjects(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
    	                m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits, 
			m_stgcHits, m_mmHits,
                        trackPatterns, outputTracks, outputID, outputMS);
    // call the calibration streamer 
    if (m_doCalStream && trackPatterns.size()>0 ) { 
      TrigL2MuonSA::TrackPattern tp = trackPatterns[0];
      if (m_use_timer) m_timingTimers[ITIMER_CALIBRATION_STREAMER]->resume();
      calibrationTimer.start();

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
      if (m_use_timer) m_timingTimers[ITIMER_CALIBRATION_STREAMER]->pause();
      calibrationTimer.stop();

      // if it's a data scouting chain check the buffer length
      if ( m_calDataScouting ) {
        
        if ( updateTriggerElement ) {
          
          ATH_MSG_DEBUG("Updating the trigger element");
          ATH_MSG_DEBUG(">> Retrieved the buffer, with size: " << m_calStreamer->getLocalBufferSize());
	  // create the TrigCompositeContainer to store the calibration buffer
	  // at StatusCode execute() and hltExecute().

	  // add the trigcomposite object to the container
	  xAOD::TrigComposite* tc = new xAOD::TrigComposite();
	  outputComposite.push_back(tc);

	  ATH_MSG_DEBUG("The size of the TrigCompositeContainer is: " << outputComposite.size() );
	  	  
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
  
  if (m_use_timer) m_timingTimers[ITIMER_TOTAL_PROCESSING]->pause();
  
  int nRoI = muonRoIs.size();

  if (m_use_timer) {
     for (unsigned int i_timer=0; i_timer<m_timingTimers.size(); i_timer++) {
         m_timingTimers[i_timer]->propVal(nRoI);
         m_timingTimers[i_timer]->stop();
     }
  }

  ATH_MSG_DEBUG("StatusCode MuFastSteering::findMuonSignature success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

// findMuonSignature of L2 inside-out version
// try to find MS tracks seeded by FTF tracks
StatusCode MuFastSteering::findMuonSignatureIO(const xAOD::TrackParticleContainer&         idtracks,
					       const std::vector<const TrigRoiDescriptor*> roids,
					       const std::vector<const LVL1::RecMuonRoI*>  muonRoIs,
					       DataVector<xAOD::L2CombinedMuon>&           outputCBs,
					       DataVector<xAOD::L2StandAloneMuon>&         outputSAs)
{
  ATH_MSG_DEBUG("StatusCode MuFastSteering::findMuonSignatureIO start");
  StatusCode sc = StatusCode::SUCCESS;
  const float ZERO_LIMIT = 1.e-5;

  auto prepTimer           = Monitored::Timer( "TIME_Data_Preparator" );
  auto patternTimer        = Monitored::Timer( "TIME_Pattern_Finder" );
  auto stationFitterTimer  = Monitored::Timer( "TIME_Station_Fitter" );
  auto trackFitterTimer    = Monitored::Timer( "TIME_Track_Fitter" );
  auto trackExtraTimer     = Monitored::Timer( "TIME_Track_Extrapolator" );
  auto calibrationTimer    = Monitored::Timer( "TIME_Calibration_Streamer" );

  auto monitorIt       = Monitored::Group(m_monTool, prepTimer, patternTimer, stationFitterTimer,
					  trackFitterTimer, trackExtraTimer, calibrationTimer );

  if (m_use_timer) {
    for (unsigned int i_timer=0; i_timer<m_timingTimers.size(); i_timer++) {
      m_timingTimers[i_timer]->start();
      m_timingTimers[i_timer]->pause();
    }
  }

  if (m_use_timer) m_timingTimers[ITIMER_TOTAL_PROCESSING]->resume();
  if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->resume();

  DataVector<const TrigRoiDescriptor>::const_iterator p_roids;
  DataVector<const LVL1::RecMuonRoI>::const_iterator p_roi;

  //int npass = 0;
  // muonRoIs = RecMURoIs, roids = MURoIs
  p_roids = roids.begin();
  for (p_roi=(muonRoIs).begin(); p_roi!=(muonRoIs).end(); ++p_roi) {
    sc = StatusCode::SUCCESS;

    //p_roi = (muonRoIs.begin());
    ATH_MSG_DEBUG("roi eta/phi: " << (*p_roi)->eta() << "/" << (*p_roi)->phi());

    // idtracks loop
    if ( (idtracks).size()== 0) ATH_MSG_DEBUG("IO TEST: xAOD::TrackParticleContainer has 0 tracks --> Can not use FTF tracks...");
    else  ATH_MSG_DEBUG("IO TEST: xAOD::TrackParticleContainer has " << (idtracks).size() << " tracks --> Start inside-out mode!");

    std::vector<TrigL2MuonSA::TrackPattern> trackPatterns;
    int idtrack_idx = -1;
    for (auto idtrack : idtracks) {
      idtrack_idx++;
      ATH_MSG_DEBUG("IO TEST: FTF track key:" << m_FTFtrackKey.key() << " pt = " << idtrack->pt()/1000 << " GeV");
      ATH_MSG_DEBUG("IO TEST: FTF track key:" << m_FTFtrackKey.key() << " eta/phi = " << idtrack->eta() << "/" << idtrack->phi());

      if(idtrack->pt() < m_ftfminPt) {
	ATH_MSG_DEBUG("IO TEST: skip FTF track due to pT threshold: " << m_ftfminPt << " MeV");
	continue;
      }

      prepTimer.start();
      m_mdtHits_normal.clear();
      m_mdtHits_overlap.clear();
      m_cscHits.clear();
      m_stgcHits.clear();
      m_mmHits.clear();
      m_rpcFitResult.Clear();
      m_tgcFitResult.Clear();
      m_rpcHits.clear();
      m_tgcHits.clear();

      m_muonRoad.Clear();
      bool hasSP = false;

      sc = m_ftfRoadDefiner->defineRoad(idtrack, m_muonRoad);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("FtfRoadDefiner failed");
	continue;
      } else {
	ATH_MSG_DEBUG("FtfRoadDefiner::defineRoad success");
      }

      if ( std::abs(idtrack->eta()) < 1.05 ){
	ATH_MSG_DEBUG("FTF track at IP is in  Barrel: " << idtrack->eta());
      } else {
	ATH_MSG_DEBUG("FTF track at IP is in  Endcap: " << idtrack->eta());
      }

      if ( std::abs(m_muonRoad.extFtfMiddleEta) < 1.05 ){ // Barrel Inside-out
	ATH_MSG_DEBUG("m_muonRoad.extFtfMiddleEta Barrel: " << m_muonRoad.extFtfMiddleEta);

	ATH_MSG_DEBUG("Barrel algorithm of IOmode starts");

	m_muonRoad.setScales(m_scaleRoadBarrelInner,
			     m_scaleRoadBarrelMiddle,
			     m_scaleRoadBarrelOuter);

	// Data preparation
	sc = m_dataPreparator->prepareData(*p_roi,
					   *p_roids,
					   m_insideOut,
					   m_rpcHits,
					   m_muonRoad,
					   m_mdtRegion,
					   m_rpcFitResult,
					   m_mdtHits_normal,
					   m_mdtHits_overlap);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Data preparation failed");
	  continue;
	} else {
	  ATH_MSG_DEBUG("Data preparation success");
	}
	if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
	prepTimer.stop();

	// Pattern finding
	if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
	patternTimer.start();
	sc = m_patternFinder->findPatterns(m_muonRoad,
					   m_mdtHits_normal,
					   trackPatterns);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Pattern finder failed");
	  continue;
	}
	if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
	patternTimer.stop();

	// Superpoint fit
	if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
	stationFitterTimer.start();
	sc = m_stationFitter->findSuperPoints(*p_roi,
					      m_muonRoad,
					      m_rpcFitResult,
					      trackPatterns);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Super point fitter failed");
	  continue;
	}
	if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
	stationFitterTimer.stop();

	// Check if at least 1 SP
	for (size_t i=0; i<std::extent<decltype(trackPatterns.back().superPoints), 0>::value; i++) {
	  if ( std::abs(trackPatterns.back().superPoints[i].R) > ZERO_LIMIT && std::abs(trackPatterns.back().superPoints[i].Z) > ZERO_LIMIT ) { // if R and Z exist
	    hasSP = true;
	  }
	}

	// Track fitting
	if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
	trackFitterTimer.start();
	sc = m_trackFitter->findTracks(*p_roi,
				       m_rpcFitResult,
				       trackPatterns);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Track fitter failed");
	  if(hasSP) {
	    storeMuonSA(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
	    		m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits,
	    		m_stgcHits, m_mmHits,
	    		trackPatterns.back(), outputSAs);
	    xAOD::L2CombinedMuon* muonCB = new xAOD::L2CombinedMuon();
	    muonCB->makePrivateStore();
	    muonCB->setStrategy(0);
	    muonCB->setErrorFlag(-9);
	    muonCB->setPt(idtrack->pt());
	    muonCB->setEta(idtrack->eta());
	    muonCB->setPhi(idtrack->phi());
	    muonCB->setCharge(idtrack->charge());
	    ElementLink<xAOD::L2StandAloneMuonContainer> muonSAEL(outputSAs, outputSAs.size()-1);
	    muonCB->setMuSATrackLink(muonSAEL);
	    ElementLink<xAOD::TrackParticleContainer> idtrkEL(idtracks, idtrack_idx);
	    muonCB->setIdTrackLink(idtrkEL);
	    outputCBs.push_back(muonCB);
	  }
	  continue;
        }
	if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
        trackFitterTimer.stop();

      } else { // Endcap Inside-out
	ATH_MSG_DEBUG("m_muonRoad.extFtfMiddleEta Endcap: " << m_muonRoad.extFtfMiddleEta);
	ATH_MSG_DEBUG("Endcap algorithm of IOmode starts");

	prepTimer.start();
	// Data preparation
	sc = m_dataPreparator->prepareData(*p_roi,
					   *p_roids,
					   m_insideOut,
					   m_tgcHits,
					   m_muonRoad,
					   m_mdtRegion,
					   m_tgcFitResult,
					   m_mdtHits_normal,
					   m_mdtHits_overlap,
					   m_cscHits,
					   m_stgcHits,
					   m_mmHits);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Data preparation failed");
	  continue;
	} else{
	  ATH_MSG_DEBUG("Data preparation success");
	}
	if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
	prepTimer.stop();

	// Pattern finding
	if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
	patternTimer.start();
	sc = m_patternFinder->findPatterns(m_muonRoad,
					   m_mdtHits_normal,
					   m_stgcHits,
					   m_mmHits,
					   trackPatterns);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Pattern finder failed");
	  continue;
	}
	if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
	patternTimer.stop();

	// Superpoint fit
	if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
	stationFitterTimer.start();
	sc = m_stationFitter->findSuperPointsSimple(*p_roi,
						    m_muonRoad,
						    m_tgcFitResult,
						    trackPatterns);
	/////csc SuperPoint
	m_cscsegmaker->FindSuperPointCsc(m_cscHits,trackPatterns,m_tgcFitResult,m_muonRoad);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Super point fitter failed");
	  continue;
	}
	if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
	stationFitterTimer.stop();

	// Check if at least 1 SP
	for (size_t i=0; i<std::extent<decltype(trackPatterns.back().superPoints), 0>::value; i++) {
	  if ( std::abs(trackPatterns.back().superPoints[i].R) > ZERO_LIMIT && std::abs(trackPatterns.back().superPoints[i].Z) > ZERO_LIMIT ) { // if R and Z exist
	    hasSP = true;
	  }
	}

	// Track fittingh
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
	trackFitterTimer.start();
        sc = m_trackFitter->findTracks(*p_roi,
				       m_tgcFitResult,
				       trackPatterns,
				       m_muonRoad);
        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Track fitter failed");
	  if(hasSP) {
	    storeMuonSA(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
	    		m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits,
	    		m_stgcHits, m_mmHits,
	    		trackPatterns.back(), outputSAs);
	    xAOD::L2CombinedMuon* muonCB = new xAOD::L2CombinedMuon();
	    muonCB->makePrivateStore();
	    muonCB->setStrategy(0);
	    muonCB->setErrorFlag(-9);
	    muonCB->setPt(idtrack->pt());
	    muonCB->setEta(idtrack->eta());
	    muonCB->setPhi(idtrack->phi());
	    muonCB->setCharge(idtrack->charge());
	    ElementLink<xAOD::L2StandAloneMuonContainer> muonSAEL(outputSAs, outputSAs.size()-1);
	    muonCB->setMuSATrackLink(muonSAEL);
	    ElementLink<xAOD::TrackParticleContainer> idtrkEL(idtracks, idtrack_idx);
	    muonCB->setIdTrackLink(idtrkEL);
	    outputCBs.push_back(muonCB);
	  }
	  continue;
	}
	if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
	trackFitterTimer.stop();

      }

      // fix if eta is strange
      for (unsigned int i=0 ;i<trackPatterns.size(); i++) {
	TrigL2MuonSA::TrackPattern track = trackPatterns[i];
	const float ETA_LIMIT       = 2.8;
	const float DELTA_ETA_LIMIT = 1.0;
	float roiEta = (*p_roi)->eta();
	if (fabs(track.pt) > ZERO_LIMIT
	    && ( fabs(track.etaMap) > ETA_LIMIT || fabs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
          trackPatterns[i].etaMap = roiEta;
        }
      }

      // Update monitoring variables
      sc = updateMonitor(*p_roi, m_mdtHits_normal, trackPatterns );
      if (sc != StatusCode::SUCCESS) {
	ATH_MSG_WARNING("Failed to update monitoring variables");
      }

      // Update output trigger element
      if(hasSP) {
	storeMuonSA(*p_roi, *p_roids, m_muonRoad, m_mdtRegion, m_rpcHits, m_tgcHits,
		    m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits,
		    m_stgcHits, m_mmHits,
		    trackPatterns.back(), outputSAs);
	xAOD::L2CombinedMuon* muonCB = new xAOD::L2CombinedMuon();
	muonCB->makePrivateStore();
	muonCB->setStrategy(0);
	muonCB->setErrorFlag(-9);
	muonCB->setPt(idtrack->pt());
	muonCB->setEta(idtrack->eta());
	muonCB->setPhi(idtrack->phi());
	muonCB->setCharge(idtrack->charge());
	ElementLink<xAOD::L2StandAloneMuonContainer> muonSAEL(outputSAs, outputSAs.size()-1);
	muonCB->setMuSATrackLink(muonSAEL);
	ElementLink<xAOD::TrackParticleContainer> idtrkEL(idtracks, idtrack_idx);
	muonCB->setIdTrackLink(idtrkEL);
	outputCBs.push_back(muonCB);
      }

    }

    if(outputSAs.size()==0) {
      ATH_MSG_DEBUG("outputSAs size = 0 -> push_back dummy");
      m_muonRoad.Clear();
      TrigL2MuonSA::MdtRegion mdtRegion;
      m_rpcHits.clear();
      m_tgcHits.clear();
      m_rpcFitResult.Clear();
      m_tgcFitResult.Clear();
      m_mdtHits_normal.clear();
      m_cscHits.clear();
      m_stgcHits.clear();
      m_mmHits.clear();
      trackPatterns.clear();
      TrigL2MuonSA::TrackPattern trackPattern;
      storeMuonSA(*p_roi, *p_roids, m_muonRoad, mdtRegion, m_rpcHits, m_tgcHits,
      		  m_rpcFitResult, m_tgcFitResult, m_mdtHits_normal, m_cscHits,
      		  m_stgcHits, m_mmHits,
      		  trackPattern, outputSAs);
      xAOD::L2CombinedMuon* muonCB = new xAOD::L2CombinedMuon();
      muonCB->makePrivateStore();
      muonCB->setStrategy(-9);
      muonCB->setErrorFlag(-9);
      muonCB->setPt(0);
      muonCB->setEta(0);
      muonCB->setPhi(0);
      ElementLink<xAOD::L2StandAloneMuonContainer> muonSAEL(outputSAs, outputSAs.size()-1);
      muonCB->setMuSATrackLink(muonSAEL);
      outputCBs.push_back(muonCB);
    }


    ATH_MSG_DEBUG("outputSAs size: " << outputSAs.size());
    ATH_MSG_DEBUG("idtracks size: " << idtracks.size());
    for (unsigned int i = 0; i< outputSAs.size(); i++){
      xAOD::L2StandAloneMuon* outputSA = outputSAs[i];
      ATH_MSG_DEBUG("outputSA pt/eta/phi: " << outputSA->pt() << "/" << outputSA->etaMS() << "/" << outputSA->phiMS());
    }

    ATH_MSG_DEBUG("outputCBs size: " << outputCBs.size());
    for (unsigned int i = 0; i< outputCBs.size(); i++){
      xAOD::L2CombinedMuon* outputCB = outputCBs[i];
      ATH_MSG_DEBUG("outputCB pt/eta/phi: " << outputCB->pt() << "/" << outputCB->eta() << "/" << outputCB->phi());
    }

  }

  if (m_use_timer) m_timingTimers[ITIMER_TOTAL_PROCESSING]->pause();

  if (m_use_timer) {
    for (unsigned int i_timer=0; i_timer<m_timingTimers.size(); i_timer++) {
      m_timingTimers[i_timer]->stop();
    }
  }

  ATH_MSG_DEBUG("StatusCode MuFastSteering::findMuonSignatureIO success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool MuFastSteering::updateOutputObjects(const LVL1::RecMuonRoI*                        roi,
                                         const TrigRoiDescriptor*                       roids,
                                         const TrigL2MuonSA::MuonRoad&                  muonRoad,
                                         const TrigL2MuonSA::MdtRegion&                 mdtRegion,
                                         const TrigL2MuonSA::RpcHits&                   rpcHits,
                                         const TrigL2MuonSA::TgcHits&                   tgcHits,
                                         const TrigL2MuonSA::RpcFitResult&              rpcFitResult,
                                         const TrigL2MuonSA::TgcFitResult&              tgcFitResult,
                                         const TrigL2MuonSA::MdtHits&                   mdtHits,
                                         const TrigL2MuonSA::CscHits&                   cscHits,
                                         const TrigL2MuonSA::StgcHits&                  stgcHits,
                                         const TrigL2MuonSA::MmHits&                    mmHits,
                                         const std::vector<TrigL2MuonSA::TrackPattern>& trackPatterns,
				         DataVector<xAOD::L2StandAloneMuon>&	        outputTracks,
				         TrigRoiDescriptorCollection&  	                outputID,
				         TrigRoiDescriptorCollection&   	        outputMS)
{
  
  if( trackPatterns.size() > 0 ) {

    const TrigL2MuonSA::TrackPattern& pattern = trackPatterns.back();

    // Update output trigger element
    storeMuonSA(roi, roids, muonRoad, mdtRegion, rpcHits, tgcHits,
    	        rpcFitResult, tgcFitResult, mdtHits, cscHits, 
		stgcHits, mmHits,
                pattern, outputTracks);
    storeMSRoiDescriptor(roids, pattern, outputTracks, outputMS);
    storeIDRoiDescriptor(roids, pattern, outputTracks, outputID);

  } else {
    ATH_MSG_DEBUG("Not update output objects because trackPatterns has no object");
  }

  return true;
}


bool MuFastSteering::storeMuonSA(const LVL1::RecMuonRoI*             roi,
                                 const TrigRoiDescriptor*            roids,
             	                 const TrigL2MuonSA::MuonRoad&       muonRoad,
             	                 const TrigL2MuonSA::MdtRegion&      mdtRegion,
             	                 const TrigL2MuonSA::RpcHits&        rpcHits,
             	                 const TrigL2MuonSA::TgcHits&        tgcHits,
             	                 const TrigL2MuonSA::RpcFitResult&   rpcFitResult,
             	                 const TrigL2MuonSA::TgcFitResult&   tgcFitResult,
             	                 const TrigL2MuonSA::MdtHits&        mdtHits,
             	                 const TrigL2MuonSA::CscHits&        cscHits,
				 const TrigL2MuonSA::StgcHits&       stgcHits,
				 const TrigL2MuonSA::MmHits&         mmHits,
             	                 const TrigL2MuonSA::TrackPattern&   pattern,
                                 DataVector<xAOD::L2StandAloneMuon>& outputTracks )
{
  const float ZERO_LIMIT = 1.e-5;

  const int currentRoIId = roids->roiId();

  const EventContext& ctx = getContext();
  const EventIDBase& eventID = ctx.eventID();
  auto eventInfo = SG::makeHandle(m_eventInfoKey, ctx);
  if (!eventInfo.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve xAOD::EventInfo object");
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
  // int bmg = 10;

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

  ATH_MSG_DEBUG("### Hit patterns at the Muon Spectrometer ###");
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
  ATH_MSG_DEBUG("### ************************************* ###");
  ATH_MSG_DEBUG("Estimated muon pt = " << pattern.pt << " GeV");

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
  muonSA->setLvl1Id( eventInfo->extendedLevel1ID() );
  /// Set lumi block
  muonSA->setLumiBlock( eventID.lumi_block() );
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
  /// Set RoI word
  muonSA->setRoIWord( roi->roiWord() );

  /// Set size of storages to be reserved
  muonSA->setRpcHitsCapacity( m_esd_rpc_size );
  muonSA->setTgcHitsCapacity( m_esd_tgc_size );
  muonSA->setMdtHitsCapacity( m_esd_mdt_size );
  muonSA->setCscHitsCapacity( m_esd_csc_size );
  muonSA->setStgcClustersCapacity( m_esd_stgc_size );
  muonSA->setMmClustersCapacity( m_esd_mm_size );

  // MDT hits
  std::vector<std::string> mdtId;
  for (const TrigL2MuonSA::MdtHitData& mdtHit : mdtHits) {
    if ( mdtHit.isOutlier==0 || mdtHit.isOutlier==1 ) {
      muonSA->setMdtHit(mdtHit.OnlineId, mdtHit.isOutlier, mdtHit.Chamber,
                        mdtHit.R, mdtHit.Z, mdtHit.cPhi0, mdtHit.Residual,
                        mdtHit.DriftTime, mdtHit.DriftSpace, mdtHit.DriftSigma);
      mdtId.push_back(mdtHit.Id.getString());
    }
  }
  SG::AuxElement::Accessor< std::vector<std::string> > accessor_mdthitid( "mdtHitId" );
  accessor_mdthitid( *muonSA ) = mdtId;
  
  //CSC hits
  std::vector<float> cscResol;
  for (const TrigL2MuonSA::CscHitData& cscHit : cscHits) {
    if ( 1/*cscHit.MeasuresPhi==0*/ ){
      if ( cscHit.isOutlier==0 || cscHit.isOutlier==1 ) {
        muonSA->setCscHit(cscHit.isOutlier, cscHit.Chamber, cscHit.StationName,
                          cscHit.StationEta, cscHit.StationPhi,
                          cscHit.ChamberLayer, cscHit.WireLayer, cscHit.MeasuresPhi, cscHit.Strip,
                          cscHit.eta, cscHit.phi, cscHit.r, cscHit.z,
                          cscHit.charge, cscHit.time, cscHit.Residual);
	cscResol.push_back(cscHit.resolution);
        ATH_MSG_VERBOSE("CSC Hits stored in xAOD: "
      		<< "OL=" << cscHit.isOutlier << ","
      		<< "Ch=" << cscHit.Chamber << ","
      		<< "StationName=" << cscHit.StationName << ","
      		<< "StationEta=" << cscHit.StationEta << ","
      		<< "StationPhi=" << cscHit.StationPhi << ","
      		<< "ChamberLayer=" << cscHit.ChamberLayer << ","
      		<< "WireLayer=" << cscHit.WireLayer << ","
      		<< "MeasuresPhi=" << cscHit.MeasuresPhi << ","
      		<< "Strip=" << cscHit.Strip << ","
      		<< "eta="  << cscHit.eta << ","
      		<< "phi="  << cscHit.phi << ","
      		<< "r="  << cscHit.r << ","
      		<< "z="  << cscHit.z << ","
      		<< "charge=" << cscHit.charge << ","
      		<< "Rs=" << cscHit.Residual << ","
      		<< "t="  << cscHit.time);
      }
    }
  }
  SG::AuxElement::Accessor< std::vector<float> > accessor_cschitresol( "cscHitResolution" );
  accessor_cschitresol( *muonSA ) = cscResol;

  // RPC hits
  float sumbeta[8]={0};
  float nhit_layer[8]={0};
  for (const TrigL2MuonSA::RpcHitData& rpcHit : rpcHits) {
    muonSA->setRpcHit(rpcHit.layer, rpcHit.measuresPhi,
                      rpcHit.x, rpcHit.y, rpcHit.z,
                      rpcHit.time, rpcHit.distToEtaReadout, rpcHit.distToPhiReadout,
                      rpcHit.stationName);
    ATH_MSG_VERBOSE("RPC hits stored in xAOD: "
      	    << "stationName=" << rpcHit.stationName << ","
      	    << "layer=" << rpcHit.layer << ","
      	    << "measuresPhi=" << rpcHit.measuresPhi << ","
      	    << "x=" << rpcHit.x << ","
      	    << "y=" << rpcHit.y << ","
      	    << "y=" << rpcHit.z);
    
    float dRMS = sqrt( fabs(pattern.etaMap-rpcHit.eta)*fabs(pattern.etaMap-rpcHit.eta) + acos(cos(pattern.phiMS-rpcHit.phi))*acos(cos(pattern.phiMS-rpcHit.phi)) );
    if(dRMS>0.05) continue;
    float muToF = rpcHit.l/1000/(CLHEP::c_light/1000);
    float Tprop = rpcHit.distToPhiReadout/1000*4.8;
    float beta = rpcHit.l/1000/(muToF+rpcHit.time-Tprop+3.125/2)/(CLHEP::c_light/1000);
    sumbeta[rpcHit.layer]=sumbeta[rpcHit.layer]+beta;
    nhit_layer[rpcHit.layer]=nhit_layer[rpcHit.layer]+1;
  }

  std::vector<float> Avebeta_layer;
  for(int i_layer=0;i_layer<8;i_layer++){
    if(nhit_layer[i_layer]!=0)Avebeta_layer.push_back( sumbeta[i_layer]/nhit_layer[i_layer] );
  }
  if(Avebeta_layer.size()>0) muonSA->setBeta( std::accumulate(Avebeta_layer.begin(),Avebeta_layer.end(),0.0)/Avebeta_layer.size() );
  else muonSA->setBeta( 9999 );
  Avebeta_layer.clear();
  
  // TGC hits
  for (const TrigL2MuonSA::TgcHitData& tgcHit : tgcHits) {
    muonSA->setTgcHit(tgcHit.eta, tgcHit.phi, tgcHit.r, tgcHit.z,
                      tgcHit.width, tgcHit.sta, tgcHit.isStrip,
                      tgcHit.bcTag, tgcHit.inRoad);
    ATH_MSG_VERBOSE("TGC hits stored in xAOD: "
      	    << "eta=" << tgcHit.eta << ","
      	    << "phi=" << tgcHit.phi << ","
      	    << "r=" << tgcHit.r << ","
      	    << "z=" << tgcHit.z << ","
      	    << "width=" << tgcHit.width << ","
      	    << "stationNum=" << tgcHit.sta << ","
      	    << "isStrip=" << tgcHit.isStrip << ","
      	    << "bcTag=" << tgcHit.bcTag << ","
      	    << "inRoad=" << tgcHit.inRoad);
  }


  // sTGC clusters
  for(unsigned int i_hit=0; i_hit<stgcHits.size(); i_hit++) {
    if ( stgcHits[i_hit].isOutlier==0 || stgcHits[i_hit].isOutlier==1 ) {


      muonSA->setStgcCluster(stgcHits[i_hit].layerNumber, stgcHits[i_hit].isOutlier, stgcHits[i_hit].channelType,
      			     stgcHits[i_hit].eta, stgcHits[i_hit].phi, stgcHits[i_hit].r, stgcHits[i_hit].z,
      			     stgcHits[i_hit].ResidualR, stgcHits[i_hit].ResidualPhi,
      			     stgcHits[i_hit].stationEta, stgcHits[i_hit].stationPhi, stgcHits[i_hit].stationName);

      ATH_MSG_VERBOSE("sTGC hits stored in xAOD: "
		      << "eta=" << stgcHits[i_hit].eta << ","
		      << "phi=" << stgcHits[i_hit].phi << ","
		      << "r=" << stgcHits[i_hit].r << ","
		      << "z=" << stgcHits[i_hit].z << ","
		      << "z=" << stgcHits[i_hit].ResidualR << ","
		      << "z=" << stgcHits[i_hit].ResidualPhi);
    }
  }

  // MM clusters
  for(unsigned int i_hit=0; i_hit<mmHits.size(); i_hit++) {
    if ( mmHits[i_hit].isOutlier==0 || mmHits[i_hit].isOutlier==1 ) {


      muonSA->setMmCluster(mmHits[i_hit].layerNumber, mmHits[i_hit].isOutlier,
      			   mmHits[i_hit].eta, mmHits[i_hit].phi, mmHits[i_hit].r, mmHits[i_hit].z,
      			   mmHits[i_hit].ResidualR, mmHits[i_hit].ResidualPhi,
      			   mmHits[i_hit].stationEta, mmHits[i_hit].stationPhi, mmHits[i_hit].stationName);

      ATH_MSG_VERBOSE("mm hits stored in xAOD: "
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
        } else if( phi1*phi2 < 0 && std::abs(phi1)>(M_PI/2.) ) {
          double tmp1 = (phi1>0)? phi1 - M_PI : phi1 + M_PI;
          double tmp2 = (phi2>0)? phi2 - M_PI : phi2 + M_PI;
          double tmp  = (tmp1+tmp2)/2.;
          phi  = (tmp>0.)? tmp - M_PI : tmp + M_PI;
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
  outputTracks.push_back(muonSA);

  return true;
}




bool MuFastSteering::storeMSRoiDescriptor(const TrigRoiDescriptor*                  roids,
             	                          const TrigL2MuonSA::TrackPattern&         pattern,
                                          const DataVector<xAOD::L2StandAloneMuon>& outputTracks,
		                          TrigRoiDescriptorCollection&	 	    outputMS)
{
  const float ZERO_LIMIT = 1.e-5;

  const xAOD::L2StandAloneMuon* muonSA = outputTracks[0];

  // store TrigRoiDescriptor
  if (fabs(muonSA->pt()) > ZERO_LIMIT ) {

    // set width of 0.1 so that ID tracking monitoring works
    const float phiHalfWidth = 0.1;
    const float etaHalfWidth = 0.1;

    TrigRoiDescriptor* MSroiDescriptor = new TrigRoiDescriptor(roids->l1Id(),
                                                               roids->roiId(),
                                                               pattern.etaMap,
                                                               pattern.etaMap - etaHalfWidth,
                                                               pattern.etaMap + etaHalfWidth,
                                                               pattern.phiMS,
                                                               pattern.phiMS - phiHalfWidth,
                                                               pattern.phiMS + phiHalfWidth);

    ATH_MSG_VERBOSE("...TrigRoiDescriptor for MS "
      	    << "pattern.etaMap/pattern.phiMS="
      	    << pattern.etaMap << "/" << pattern.phiMS);

    ATH_MSG_VERBOSE("will Record an RoiDescriptor for TrigMoore:"
      	    << " phi=" << MSroiDescriptor->phi()
      	    << ",  eta=" << MSroiDescriptor->eta());

    outputMS.push_back(MSroiDescriptor);

  }

  return true;
}


bool MuFastSteering::storeIDRoiDescriptor(const TrigRoiDescriptor*                  roids,
		                          const TrigL2MuonSA::TrackPattern&         pattern,
                                          const DataVector<xAOD::L2StandAloneMuon>& outputTracks,
		                          TrigRoiDescriptorCollection&	 	    outputID)
{

  if (m_fill_FSIDRoI) {  // this mode will be used in cosmic run, if ID expert want to run full scan FTF.
    TrigRoiDescriptor* IDroiDescriptor = new TrigRoiDescriptor(true);
    outputID.push_back(IDroiDescriptor);
    return true;
  }

  const float ZERO_LIMIT = 1.e-5;

  const double scalePhiWidthForFailure = 2;
  const double scaleRoIforZeroPt = 2;
 
  const xAOD::L2StandAloneMuon* muonSA = outputTracks[0];

  // store TrigRoiDescriptor
  if (fabs(muonSA->pt()) > ZERO_LIMIT ) {

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
    
    ATH_MSG_VERBOSE("...TrigRoiDescriptor for ID "
      	    << "pattern.etaVtx/pattern.phiVtx="
      	    << pattern.etaVtx << "/" << pattern.phiVtx);

    ATH_MSG_VERBOSE("will Record an RoiDescriptor for Inner Detector:"
                  << " phi=" << IDroiDescriptor->phi()
                  << ",  eta=" << IDroiDescriptor->eta());

    outputID.push_back(IDroiDescriptor);

 } else { // pt = 0.
    
    TrigRoiDescriptor* IDroiDescriptor = new TrigRoiDescriptor(roids->l1Id(),
                                                               roids->roiId(),
                                                               roids->eta(),
                                                               roids->eta() - (roids->eta() - roids->etaMinus()) * scaleRoIforZeroPt,
                                                               roids->eta() + (roids->etaPlus() - roids->eta()) * scaleRoIforZeroPt,
                                                               roids->phi(),
                                                               CxxUtils::wrapToPi(roids->phi() - CxxUtils::wrapToPi(roids->phiPlus() - roids->phiMinus())/2. * scaleRoIforZeroPt),
                                                               CxxUtils::wrapToPi(roids->phi() + CxxUtils::wrapToPi(roids->phiPlus() - roids->phiMinus())/2. * scaleRoIforZeroPt));

    ATH_MSG_VERBOSE("will Record an RoiDescriptor for Inner Detector in case with zero pT:"
      	     << " phi=" << IDroiDescriptor->phi()
      	     << ", phi min=" << IDroiDescriptor->phiMinus()
      	     << ", phi max=" << IDroiDescriptor->phiPlus()
      	     << ", eta=" << IDroiDescriptor->eta()
      	     << ", eta min=" << IDroiDescriptor->etaMinus()
      	     << ", eta max=" << IDroiDescriptor->etaPlus());

    outputID.push_back(IDroiDescriptor);
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
   xAOD::L2MuonParameters::ECRegions reg = xAOD::L2MuonParameters::whichECRegion(eta,phi);
   if( reg == xAOD::L2MuonParameters::ECRegions::WeakBFieldA ) {
      
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
      else if( reg == xAOD::L2MuonParameters::ECRegions::WeakBFieldB ) {
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

StatusCode MuFastSteering::updateMonitor(const LVL1::RecMuonRoI*                    roi,
                                         const TrigL2MuonSA::MdtHits&               mdtHits,
                                         std::vector<TrigL2MuonSA::TrackPattern>&   trackPatterns )
{
  // initialize monitored variable
  auto inner_mdt_hits 	= Monitored::Scalar("InnMdtHits", -1);
  auto middle_mdt_hits 	= Monitored::Scalar("MidMdtHits", -1);
  auto outer_mdt_hits 	= Monitored::Scalar("OutMdtHits", -1);
  auto invalid_rpc_roi_number = Monitored::Scalar("InvalidRpcRoINumber", -1);

  auto efficiency 	= Monitored::Scalar("Efficiency", 0);
  auto sag_inverse 	= Monitored::Scalar("SagInv", 9999.);
  auto address 		= Monitored::Scalar("Address", 9999.);
  auto absolute_pt 	= Monitored::Scalar("AbsPt", 9999.);
  auto sagitta	 	= Monitored::Scalar("Sagitta", 9999.);
  auto track_pt 	= Monitored::Scalar("TrackPt", 9999.);

  std::vector<float> t_eta, t_phi;
  std::vector<float> f_eta, f_phi;
  std::vector<float> r_inner, r_middle, r_outer;
  std::vector<float> f_residuals;

  t_eta.clear();
  t_phi.clear();
  f_eta.clear();
  f_phi.clear();
  r_inner.clear();
  r_middle.clear();
  r_outer.clear();
  f_residuals.clear();

  auto track_eta	= Monitored::Collection("TrackEta", t_eta);
  auto track_phi	= Monitored::Collection("TrackPhi", t_phi);
  auto failed_eta	= Monitored::Collection("FailedRoIEta", f_eta);
  auto failed_phi	= Monitored::Collection("FailedRoIPhi", f_phi);
  auto res_inner	= Monitored::Collection("ResInner", r_inner);
  auto res_middle	= Monitored::Collection("ResMiddle", r_middle);
  auto res_outer	= Monitored::Collection("ResOuter", r_outer);
  auto fit_residuals	= Monitored::Collection("FitResiduals", f_residuals);

  auto monitorIt	= Monitored::Group(m_monTool, inner_mdt_hits, middle_mdt_hits, outer_mdt_hits,
                                                invalid_rpc_roi_number,
                                                efficiency, sag_inverse, address, absolute_pt, sagitta, track_pt,
                                                track_eta, track_phi, failed_eta, failed_phi,
                                                res_inner, res_middle, res_outer, fit_residuals );
  
  const float ZERO_LIMIT = 1e-5;

  if( trackPatterns.size() > 0 ) {

    efficiency  = 1;
  
    const TrigL2MuonSA::TrackPattern& pattern = trackPatterns[0];
    float norm = 10.;
    
    float count_inner  = 0;
    float count_middle = 0;
    float count_outer  = 0;

    for (const TrigL2MuonSA::MdtHitData& mdtHit : mdtHits) {
      
      if (fabs(mdtHit.DriftSpace) < ZERO_LIMIT) continue;
      
      char st = mdtHit.cType[1];
      
      if (st=='I') {
        count_inner++;
        r_inner.push_back(mdtHit.Residual/norm);
        if (mdtHit.isOutlier==0) f_residuals.push_back(mdtHit.Residual/norm);
      }
      
      if (st=='M') {
        count_middle++;
        r_middle.push_back(mdtHit.Residual/norm);
        if (mdtHit.isOutlier==0) f_residuals.push_back(mdtHit.Residual/norm);
      }
      
      if (st=='O') {
        count_outer++;
        r_outer.push_back(mdtHit.Residual/norm);
        if (mdtHit.isOutlier==0) f_residuals.push_back(mdtHit.Residual/norm);
      }
    }
    
    inner_mdt_hits  = count_inner;
    middle_mdt_hits = count_middle;
    outer_mdt_hits  = count_outer;

    if ( m_dataPreparator->isRpcFakeRoi() ) 
      invalid_rpc_roi_number = roi->getRoINumber();
    
    track_pt    = (fabs(pattern.pt ) > ZERO_LIMIT)? pattern.charge*pattern.pt: 9999.;
    absolute_pt = fabs(track_pt);

    if ( fabs(pattern.etaMap) > ZERO_LIMIT || fabs(pattern.phiMS) > ZERO_LIMIT ) {
      t_eta.push_back(pattern.etaMap);
      t_phi.push_back(pattern.phiMS);
    }
    if ( fabs(pattern.pt ) < ZERO_LIMIT){
      f_eta.push_back(roi->eta());
      f_phi.push_back(roi->phi());
    }

    sagitta     = (fabs(pattern.barrelSagitta) > ZERO_LIMIT)? pattern.barrelSagitta: 9999.;
    sag_inverse = (fabs(pattern.barrelSagitta) > ZERO_LIMIT)? 1./pattern.barrelSagitta: 9999.;
    address     = pattern.s_address;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
// handler for "UpdateAfterFork")
void MuFastSteering::handle(const Incident& incident) {
  
  if (incident.type()!=AthenaInterprocess::UpdateAfterFork::type()) return;
  ATH_MSG_DEBUG("+-----------------------------------+");
  ATH_MSG_DEBUG("| handle for UpdateAfterFork called |");
  ATH_MSG_DEBUG("+-----------------------------------+");
  
  // Find the Worker ID and create an individual muon buffer name for each worker
  const AthenaInterprocess::UpdateAfterFork& updinc = dynamic_cast<const AthenaInterprocess::UpdateAfterFork&>(incident);
  std::string worker_name = std::to_string(updinc.workerID());
  ATH_MSG_DEBUG("Worker ID = " << worker_name);

  //
  // Create the calibration stream
  if (m_doCalStream) {
    // set a fixed name for the buffer
    m_calBufferName = "/tmp/muonCalStreamOutput";
    m_calStreamer->setBufferName(m_calBufferName);
    m_calStreamer->setInstanceName(worker_name);
    ATH_MSG_DEBUG("MuonCalBufferSize     = " << m_calBufferSize);
    ATH_MSG_DEBUG("MuonCalBufferName     = " << m_calBufferName);

    // if it's not a data scouting chain, open the circular buffer
    if (!m_calDataScouting) {
      StatusCode sc = m_calStreamer->openStream(m_calBufferSize);
      if ( sc != StatusCode::SUCCESS ) {  
	ATH_MSG_ERROR("Failed to open the connection to the circular buffer");
      }
      else {
	ATH_MSG_INFO("Opened the connection to the circular buffer");
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
      if (roi_phi < 0) roi_phi += 2.0 * M_PI;
   
      double etaMin = roi_eta - 0.2;
      double etaMax = roi_eta + 0.2;
      double phiMin = roi_phi - 0.2;
      double phiMax = roi_phi + 0.2;
      if( phiMin < 0 ) phiMin += 2*M_PI;
      if( phiMax < 0 ) phiMax += 2*M_PI;
      if( phiMin > 2*M_PI ) phiMin -= 2*M_PI;
      if( phiMax > 2*M_PI ) phiMax -= 2*M_PI;

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
      if (roi_phi < 0) roi_phi += 2.0 * M_PI;
   
      double etaMin = roi_eta - 0.2;
      double etaMax = roi_eta + 0.2;
      double phiMin = roi_phi - 0.1;
      double phiMax = roi_phi + 0.1;
      if( phiMin < 0 ) phiMin += 2*M_PI;
      if( phiMax < 0 ) phiMax += 2*M_PI;
      if( phiMin > 2*M_PI ) phiMin -= 2*M_PI;
      if( phiMax > 2*M_PI ) phiMax -= 2*M_PI;

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
