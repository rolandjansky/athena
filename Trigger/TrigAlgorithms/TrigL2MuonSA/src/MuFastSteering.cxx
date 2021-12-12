/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuFastSteering.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODTrigMuon/L2CombinedMuonAuxContainer.h"
#include "xAODTrigMuon/TrigMuonDefs.h"
#include "xAODTrigger/MuonRoI.h"

#include "CxxUtils/phihelper.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

#include "AthenaMonitoringKernel/Monitored.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

MuFastSteering::MuFastSteering(const std::string& name, ISvcLocator* svc)
  : AthReentrantAlgorithm(name, svc),
    m_timerSvc("TrigTimerSvc", name),
    m_recMuonRoIUtils()
{
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode MuFastSteering::initialize()
{
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



  // Locate DataPreparator
  ATH_CHECK(m_dataPreparator.retrieve());

  // Locate PatternFinder
  ATH_CHECK(m_patternFinder.retrieve());

  // Locate StationFitter
  ATH_CHECK(m_stationFitter.retrieve());

  // Locate TrackFitter
  ATH_CHECK(m_trackFitter.retrieve());

  // Locate TrackExtrapolator
  ATH_CHECK(m_trackExtrapolator.retrieve());

  // BackExtrapolator services
  ATH_CHECK(m_backExtrapolatorTool.retrieve());

  // CscSegmentMaker
  ATH_CHECK(m_cscsegmaker.retrieve());

  // FtfRoadDefiner
  ATH_CHECK(m_ftfRoadDefiner.retrieve());

  // Set service tools
  m_trackExtrapolator->setExtrapolatorTool(&m_backExtrapolatorTool);
  m_dataPreparator->setExtrapolatorTool(&m_backExtrapolatorTool);

  // set road width in case TGC/RPC readout failure
  m_dataPreparator->setRoadWidthForFailure(m_rWidth_RPC_Failed, m_rWidth_TGC_Failed);

  m_dataPreparator->setRpcGeometry(m_use_rpc);
  m_dataPreparator->setRoIBasedDataAccess(m_use_RoIBasedDataAccess_MDT,
                                          m_use_RoIBasedDataAccess_RPC,
                                          m_use_RoIBasedDataAccess_TGC,
                                          m_use_RoIBasedDataAccess_CSC,
					  m_use_RoIBasedDataAccess_STGC,
					  m_use_RoIBasedDataAccess_MM);

  // set data or MC flag
  ATH_CHECK(m_dataPreparator->setMCFlag(m_use_mcLUT));

  ////stationfit mc flag
  ATH_CHECK(m_stationFitter->setMCFlag(m_use_mcLUT));
  ATH_CHECK(m_trackFitter->setMCFlag(m_use_mcLUT));
  m_trackFitter -> setUseEIFromBarrel( m_use_endcapInnerFromBarrel );

  // DataHandles for AthenaMT
  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_roiCollectionKey.initialize());
  ATH_CHECK(m_run2recRoiCollectionKey.initialize(!m_useRun3Config));
  ATH_CHECK(m_recRoiCollectionKey.initialize(m_useRun3Config));
  // for Inside-Out mode ---
  ATH_CHECK(m_FTFtrackKey.initialize(m_insideOut));
  ATH_CHECK(m_outputCBmuonCollKey.initialize(m_insideOut));
  // ----
  ATH_CHECK(m_muFastContainerKey.initialize());
  ATH_CHECK(m_muCompositeContainerKey.initialize());
  ATH_CHECK(m_muIdContainerKey.initialize());

  ATH_CHECK(m_muMsContainerKey.initialize());
  if (not m_monTool.name().empty()) {
    ATH_CHECK(m_monTool.retrieve());
  }
  ATH_MSG_DEBUG( "topoRoad = " << m_topoRoad);

  if (m_fill_FSIDRoI) {
    ATH_MSG_INFO("will fill " << m_muIdContainerKey.key() << " in Full Scan mode. Please check if it's correct.");
  }


  //
  // Initialize the calibration streamer  
  // 

  if (m_doCalStream) {
    // retrieve the calibration streamer
    ATH_CHECK(m_calStreamer.retrieve());
    // set properties
    m_calStreamer->setBufferName(m_calBufferName);
    m_calStreamer->setInstanceName( "0");
    ATH_MSG_DEBUG("Initialized the Muon Calibration Streamer. Buffer name: " << m_calBufferName 
		  << ", buffer size: " << m_calBufferSize 
		  << " doDataScouting: "  << m_calDataScouting);

    //open the stream
    StatusCode sc = StatusCode::SUCCESS;
    sc = m_calStreamer->openStream(m_calBufferSize);
    if ( sc != StatusCode::SUCCESS ) {  
      ATH_MSG_ERROR("Failed to open the connection to the circular buffer");       }
    else {
      ATH_MSG_INFO("Opened the connection to the circular buffer");
    }
  }
    

  return StatusCode::SUCCESS;
}

StatusCode MuFastSteering::finalize()
{
  // close the calibration stream
  if ( m_doCalStream ) {
    StatusCode sc = m_calStreamer->closeStream();
      if  ( sc != StatusCode::SUCCESS ) {
	ATH_MSG_ERROR("Failed to close the calibration stream");}
      else {
	ATH_MSG_INFO("Calibration stream closed");
      }
  } 

  return StatusCode::SUCCESS;
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

const xAOD::MuonRoI* matchingRecRoI( uint32_t roiWord,
				     const xAOD::MuonRoIContainer& collection )
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

StatusCode MuFastSteering::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("StatusCode MuFastSteering::execute start");
  ATH_MSG_DEBUG("InsideOutMode: " << m_insideOut);
  ATH_MSG_DEBUG("Multi-TrackMode: " << m_multiTrack << "/ run for endcap RoI -> " << m_doEndcapForl2mt);

  auto totalTimer = Monitored::Timer( "TIME_Total" );
  auto monitorIt	= Monitored::Group(m_monTool, totalTimer );
  totalTimer.start();

  ATH_MSG_DEBUG("Event context << " << ctx );

  // retrieve with ReadHandle
  auto roiCollectionHandle = SG::makeHandle( m_roiCollectionKey, ctx );
  const TrigRoiDescriptorCollection *roiCollection = roiCollectionHandle.cptr();
  if (!roiCollectionHandle.isValid()){
    ATH_MSG_ERROR("ReadHandle for TrigRoiDescriptorCollection key:" << m_roiCollectionKey.key() << " isn't Valid");
    return StatusCode::FAILURE;
  }

  if(m_useRun3Config) {

  auto recRoiCollectionHandle = SG::makeHandle( m_recRoiCollectionKey, ctx );
  const xAOD::MuonRoIContainer *recRoiCollection = recRoiCollectionHandle.cptr();
  if (!recRoiCollectionHandle.isValid()){
    ATH_MSG_ERROR("ReadHandle for xAOD::MuonRoIContainer key:" << m_recRoiCollectionKey.key() << " isn't Valid");
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
  std::vector< const xAOD::MuonRoI* > recRoIVector;
  std::vector< const xAOD::MuonRoI* > surrRoIs;

  for (size_t size=0; size<roiCollection->size(); size++){
    const xAOD::MuonRoI* recRoI = matchingRecRoI( roiCollection->at(size)->roiWord(),  *recRoiCollection );
    if( recRoI == nullptr ) continue;
    recRoIVector.push_back(recRoI);
    ATH_MSG_DEBUG("REGTEST: " << m_recRoiCollectionKey.key() << " eta/phi = " << (recRoI)->eta() << "/" << (recRoI)->phi());
    ATH_MSG_DEBUG("REGTEST: " << m_recRoiCollectionKey.key() << " size = " << recRoIVector.size());
  }

  bool dynamicDeltaRpc = false;
  int nPassedBarrelSurrRoi = 0;
  if(m_topoRoad ){
    for( const auto recRoI: *recRoiCollection ){
      if(std::find(recRoIVector.begin(), recRoIVector.end(), recRoI) != recRoIVector.end()) continue;

      bool surrounding = false;
      for( const auto matchedRoI: recRoIVector ){
        float deta = std::abs(recRoI->eta() - matchedRoI->eta());
        float dphi = std::abs(recRoI->phi() - matchedRoI->phi());
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
      if( std::abs((recRoI)->eta()) <= 1.05 && (recRoI)->getThrNumber() >= 1 )nPassedBarrelSurrRoi++;
    }
    ATH_MSG_DEBUG( "nPassedBarrelSurrRoi = " << nPassedBarrelSurrRoi);
    //dynamicDeltaRpcMode
    if( nPassedBarrelSurrRoi >= 1 )
      dynamicDeltaRpc = true;
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
    if (!trackHandle.isValid()){
      ATH_MSG_ERROR("ReadHandle for TrackParticleContainer key:" << m_FTFtrackKey.key() << " isn't Valid");
      return StatusCode::FAILURE;
    }
    const xAOD::TrackParticleContainer *tracks = trackHandle.cptr();

    ATH_CHECK(findMuonSignatureIO(*tracks, internalRoI, recRoIVector,
				  *muonCBColl, *muFastContainer, dynamicDeltaRpc, ctx ));

    ATH_MSG_DEBUG("REGTEST: xAOD::L2CombinedMuonContainer key:" << m_outputCBmuonCollKey.key() << " size = " << muonCBColl->size());
    for (const auto p_CBmuon : *muonCBColl){
      ATH_MSG_DEBUG("REGTEST: xAOD::L2CombinedMuonContainer key:" << m_outputCBmuonCollKey.key() << " pt = " << (*p_CBmuon).pt() << " GeV");
      ATH_MSG_DEBUG("REGTEST: xAOD::L2CombinedMuonContainer key:" << m_outputCBmuonCollKey.key() << " eta/phi = " << (*p_CBmuon).eta() << "/" << (*p_CBmuon).phi());
    }

  }
  else if(m_multiTrack){ //multi-track SA mode
    ATH_MSG_DEBUG("start multi-track SA mode...");
    ATH_CHECK(findMultiTrackSignature(internalRoI, recRoIVector, *muFastContainer, dynamicDeltaRpc, ctx));
  }
  else {
    // to StatusCode findMuonSignature()
    ATH_CHECK(findMuonSignature(internalRoI, recRoIVector,
				*muFastContainer, *muIdContainer, *muMsContainer, dynamicDeltaRpc, ctx));
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

  }
  else { // use Run2 L1Muon EDM

  auto recRoiCollectionHandle = SG::makeHandle( m_run2recRoiCollectionKey, ctx );
  const DataVector<LVL1::RecMuonRoI> *recRoiCollection = recRoiCollectionHandle.cptr();
  if (!recRoiCollectionHandle.isValid()){
    ATH_MSG_ERROR("ReadHandle for DataVector<LVL1::RecMuonRoI> key:" << m_run2recRoiCollectionKey.key() << " isn't Valid");
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
    if( recRoI == nullptr ) continue;
    recRoIVector.push_back(recRoI);
    ATH_MSG_DEBUG("REGTEST: " << m_recRoiCollectionKey.key() << " eta/phi = " << (recRoI)->eta() << "/" << (recRoI)->phi());
    ATH_MSG_DEBUG("REGTEST: " << m_recRoiCollectionKey.key() << " size = " << recRoIVector.size());
  }

  bool dynamicDeltaRpc = false;
  int nPassedBarrelSurrRoi = 0;
  if(m_topoRoad ){
    for( const auto recRoI: *recRoiCollection ){
      if(std::find(recRoIVector.begin(), recRoIVector.end(), recRoI) != recRoIVector.end()) continue;

      bool surrounding = false;
      for( const auto matchedRoI: recRoIVector ){
        float deta = std::abs(recRoI->eta() - matchedRoI->eta());
        float dphi = std::abs(recRoI->phi() - matchedRoI->phi());
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
      if( std::abs((recRoI)->eta()) <= 1.05 && (recRoI)->getThresholdNumber() >= 1 )nPassedBarrelSurrRoi++;
    }
    ATH_MSG_DEBUG( "nPassedBarrelSurrRoi = " << nPassedBarrelSurrRoi);
    //dynamicDeltaRpcMode
    if( nPassedBarrelSurrRoi >= 1 )
      dynamicDeltaRpc = true;
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
    if (!trackHandle.isValid()){
      ATH_MSG_ERROR("ReadHandle for TrackParticleContainer key:" << m_FTFtrackKey.key() << " isn't Valid");
      return StatusCode::FAILURE;
    }
    const xAOD::TrackParticleContainer *tracks = trackHandle.cptr();

    ATH_CHECK(findMuonSignatureIO(*tracks, internalRoI, recRoIVector,
				  *muonCBColl, *muFastContainer, dynamicDeltaRpc, ctx ));

    ATH_MSG_DEBUG("REGTEST: xAOD::L2CombinedMuonContainer key:" << m_outputCBmuonCollKey.key() << " size = " << muonCBColl->size());
    for (const auto p_CBmuon : *muonCBColl){
      ATH_MSG_DEBUG("REGTEST: xAOD::L2CombinedMuonContainer key:" << m_outputCBmuonCollKey.key() << " pt = " << (*p_CBmuon).pt() << " GeV");
      ATH_MSG_DEBUG("REGTEST: xAOD::L2CombinedMuonContainer key:" << m_outputCBmuonCollKey.key() << " eta/phi = " << (*p_CBmuon).eta() << "/" << (*p_CBmuon).phi());
    }

  }
  else if(m_multiTrack){ //multi-track SA mode
    ATH_MSG_DEBUG("start multi-track SA mode...");
    ATH_CHECK(findMultiTrackSignature(internalRoI, recRoIVector, *muFastContainer, dynamicDeltaRpc, ctx));
  }
  else {
    // to StatusCode findMuonSignature()
    ATH_CHECK(findMuonSignature(internalRoI, recRoIVector,
				*muFastContainer, *muIdContainer, *muMsContainer, dynamicDeltaRpc, ctx));
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

  }

  totalTimer.stop();

  ATH_MSG_DEBUG("StatusCode MuFastSteering::execute() success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode MuFastSteering::findMuonSignature(const std::vector<const TrigRoiDescriptor*>& roids,
                                             const std::vector<const LVL1::RecMuonRoI*>&  muonRoIs,
				             DataVector<xAOD::L2StandAloneMuon>& 	outputTracks,
					     TrigRoiDescriptorCollection& 		outputID,
					     TrigRoiDescriptorCollection&		outputMS,
					     const bool                                 dynamicDeltaRpc,
					     const EventContext&                        ctx) const
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

  TrigL2MuonSA::RpcHits      rpcHits;
  TrigL2MuonSA::TgcHits      tgcHits;
  TrigL2MuonSA::MdtRegion    mdtRegion;
  TrigL2MuonSA::MuonRoad     muonRoad;
  TrigL2MuonSA::RpcFitResult rpcFitResult;
  TrigL2MuonSA::TgcFitResult tgcFitResult;
  TrigL2MuonSA::MdtHits      mdtHits_normal;
  TrigL2MuonSA::MdtHits      mdtHits_overlap;
  TrigL2MuonSA::CscHits      cscHits;
  TrigL2MuonSA::StgcHits     stgcHits;
  TrigL2MuonSA::MmHits       mmHits;

  DataVector<const TrigRoiDescriptor>::const_iterator p_roids;
  DataVector<const LVL1::RecMuonRoI>::const_iterator p_roi;

  // muonRoIs = RecMURoIs, roids = MURoIs
  p_roids = roids.begin();
  for (p_roi=(muonRoIs).begin(); p_roi!=(muonRoIs).end(); ++p_roi) {

    prepTimer.start();
    std::vector<TrigL2MuonSA::TrackPattern> trackPatterns;
    rpcHits.clear();
    tgcHits.clear();
    mdtRegion.Clear();
    muonRoad.Clear();
    rpcFitResult.Clear();
    tgcFitResult.Clear();
    mdtHits_normal.clear();
    mdtHits_overlap.clear();
    cscHits.clear();
    stgcHits.clear();
    mmHits.clear();

    if ( m_recMuonRoIUtils.isBarrel(*p_roi) ) { // Barrel
      ATH_MSG_DEBUG("Barrel");

      muonRoad.setScales(m_scaleRoadBarrelInner,
			 m_scaleRoadBarrelMiddle,
			 m_scaleRoadBarrelOuter);

      // Data preparation
      sc = m_dataPreparator->prepareData(*p_roi,
                                         *p_roids,
                                         m_insideOut,
                                         rpcHits,
                                         muonRoad,
                                         mdtRegion,
                                         rpcFitResult,
                                         mdtHits_normal,
                                         mdtHits_overlap,
                                         dynamicDeltaRpc);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Data preparation failed");
	TrigL2MuonSA::TrackPattern trackPattern;
	trackPatterns.push_back(trackPattern);
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
      prepTimer.stop();

      // Pattern finding
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
      patternTimer.start();
      sc = m_patternFinder->findPatterns(muonRoad,
                                         mdtHits_normal,
                                         trackPatterns);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Pattern finder failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
      patternTimer.stop();

      // Superpoint fit
      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
      stationFitterTimer.start();
      sc = m_stationFitter->findSuperPoints(*p_roids,
                                            muonRoad,
                                            rpcFitResult,
                                            trackPatterns);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Super point fitter failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
      stationFitterTimer.stop();

      // Track fitting
      if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
      trackFitterTimer.start();
      sc = m_trackFitter->findTracks(*p_roids,
				     rpcFitResult, 
				     trackPatterns);

      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Track fitter failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
      trackFitterTimer.stop();

    } else { // Endcap
      ATH_MSG_DEBUG("Endcap");

      prepTimer.start();
      // Data preparation
      sc = m_dataPreparator->prepareData(*p_roi,
                                         *p_roids,
                                         m_insideOut,
                                         tgcHits,
                                         muonRoad,
                                         mdtRegion,
                                         tgcFitResult,
                                         mdtHits_normal,
                                         mdtHits_overlap,
                                         cscHits,
					 stgcHits,
					 mmHits);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Data preparation failed");
 	TrigL2MuonSA::TrackPattern trackPattern;
	trackPatterns.push_back(trackPattern);
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
      prepTimer.stop();

      // Pattern finding
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
      patternTimer.start();
      sc = m_patternFinder->findPatterns(muonRoad,
                                         mdtHits_normal,
					 stgcHits,
					 mmHits,
                                         trackPatterns);



      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Pattern finder failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
      patternTimer.stop();

      // Superpoint fit
      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
      stationFitterTimer.start();
      if(!m_use_new_segmentfit){
        sc = m_stationFitter->findSuperPointsSimple(*p_roids,
						    muonRoad,
						    tgcFitResult,
						    trackPatterns);
      }else{
        sc = m_stationFitter->findSuperPoints(*p_roids,
                                              muonRoad,
                                              tgcFitResult,
                                              trackPatterns);
      }
      /////csc SuperPoint
      m_cscsegmaker->FindSuperPointCsc(cscHits,trackPatterns,tgcFitResult,muonRoad);

      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Super point fitter failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }

      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
      stationFitterTimer.stop();

      // Track fittingh
      if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
      trackFitterTimer.start();
      sc = m_trackFitter->findTracks(*p_roids,
                                     tgcFitResult,
                                     trackPatterns,
                                     muonRoad);

      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Track fitter failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
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
       if (std::abs(track.pt) > ZERO_LIMIT
           && ( std::abs(track.etaMap) > ETA_LIMIT || std::abs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
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
      updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
      	                  rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			  stgcHits, mmHits,
                          trackPatterns, outputTracks, outputID, outputMS, ctx);
      continue;
    }
    if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->pause();
    trackExtraTimer.stop();

    // Update monitoring variables
    sc = updateMonitor(*p_roi, mdtHits_normal, trackPatterns );
    if (sc != StatusCode::SUCCESS) {
      ATH_MSG_WARNING("Failed to update monitoring variables");
      // Update output trigger element
      updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
       	                  rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			  stgcHits, mmHits,
                          trackPatterns, outputTracks, outputID, outputMS, ctx);
      continue;
    }

    // Update output trigger element
    updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
    	                rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			stgcHits, mmHits,
                        trackPatterns, outputTracks, outputID, outputMS, ctx);


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

StatusCode MuFastSteering::findMuonSignature(const std::vector<const TrigRoiDescriptor*>& roids,
                                             const std::vector<const xAOD::MuonRoI*>&   muonRoIs,
				             DataVector<xAOD::L2StandAloneMuon>& 	outputTracks,
					     TrigRoiDescriptorCollection& 		outputID,
					     TrigRoiDescriptorCollection&		outputMS,
					     const bool                                 dynamicDeltaRpc,
					     const EventContext&                        ctx) const
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

  TrigL2MuonSA::RpcHits      rpcHits;
  TrigL2MuonSA::TgcHits      tgcHits;
  TrigL2MuonSA::MdtRegion    mdtRegion;
  TrigL2MuonSA::MuonRoad     muonRoad;
  TrigL2MuonSA::RpcFitResult rpcFitResult;
  TrigL2MuonSA::TgcFitResult tgcFitResult;
  TrigL2MuonSA::MdtHits      mdtHits_normal;
  TrigL2MuonSA::MdtHits      mdtHits_overlap;
  TrigL2MuonSA::CscHits      cscHits;
  TrigL2MuonSA::StgcHits     stgcHits;
  TrigL2MuonSA::MmHits       mmHits;

  DataVector<const TrigRoiDescriptor>::const_iterator p_roids;
  DataVector<const xAOD::MuonRoI>::const_iterator p_roi;

  // muonRoIs = RecMURoIs, roids = MURoIs
  p_roids = roids.begin();
  for (p_roi=(muonRoIs).begin(); p_roi!=(muonRoIs).end(); ++p_roi) {

    prepTimer.start();
    std::vector<TrigL2MuonSA::TrackPattern> trackPatterns;
    rpcHits.clear();
    tgcHits.clear();
    mdtRegion.Clear();
    muonRoad.Clear();
    rpcFitResult.Clear();
    tgcFitResult.Clear();
    mdtHits_normal.clear();
    mdtHits_overlap.clear();
    cscHits.clear();
    stgcHits.clear();
    mmHits.clear();

    if ( m_recMuonRoIUtils.isBarrel(*p_roi) ) { // Barrel
      ATH_MSG_DEBUG("Barrel");

      muonRoad.setScales(m_scaleRoadBarrelInner,
			 m_scaleRoadBarrelMiddle,
			 m_scaleRoadBarrelOuter);

      // Data preparation
      sc = m_dataPreparator->prepareData(*p_roi,
                                         *p_roids,
                                         m_insideOut,
                                         rpcHits,
                                         muonRoad,
                                         mdtRegion,
                                         rpcFitResult,
                                         mdtHits_normal,
                                         mdtHits_overlap,
                                         dynamicDeltaRpc);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Data preparation failed");
	TrigL2MuonSA::TrackPattern trackPattern;
	trackPatterns.push_back(trackPattern);
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
      prepTimer.stop();

      // Pattern finding
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
      patternTimer.start();
      sc = m_patternFinder->findPatterns(muonRoad,
                                         mdtHits_normal,
                                         trackPatterns);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Pattern finder failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
      patternTimer.stop();

      // Superpoint fit
      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
      stationFitterTimer.start();
      sc = m_stationFitter->findSuperPoints(*p_roids,
                                            muonRoad,
                                            rpcFitResult,
                                            trackPatterns);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Super point fitter failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
      stationFitterTimer.stop();

      // Track fitting
      if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
      trackFitterTimer.start();
      sc = m_trackFitter->findTracks(*p_roids,
                                      rpcFitResult,
                                      trackPatterns);

      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Track fitter failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
      trackFitterTimer.stop();

    } else { // Endcap
      ATH_MSG_DEBUG("Endcap");

      prepTimer.start();
      // Data preparation
      sc = m_dataPreparator->prepareData(*p_roi,
                                         *p_roids,
                                         m_insideOut,
                                         tgcHits,
                                         muonRoad,
                                         mdtRegion,
                                         tgcFitResult,
                                         mdtHits_normal,
                                         mdtHits_overlap,
                                         cscHits,
					 stgcHits,
					 mmHits);
      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Data preparation failed");
 	TrigL2MuonSA::TrackPattern trackPattern;
	trackPatterns.push_back(trackPattern);
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
      prepTimer.stop();

      // Pattern finding
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
      patternTimer.start();
      sc = m_patternFinder->findPatterns(muonRoad,
                                         mdtHits_normal,
					 stgcHits,
					 mmHits,
                                         trackPatterns);



      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Pattern finder failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }
      if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
      patternTimer.stop();

      // Superpoint fit
      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
      stationFitterTimer.start();
      if(!m_use_new_segmentfit){
        sc = m_stationFitter->findSuperPointsSimple(*p_roids,
						    muonRoad,
						    tgcFitResult,
						    trackPatterns);
      }else{
        sc = m_stationFitter->findSuperPoints(*p_roids,
                                              muonRoad,
                                              tgcFitResult,
                                              trackPatterns);
      }
      /////csc SuperPoint
      m_cscsegmaker->FindSuperPointCsc(cscHits,trackPatterns,tgcFitResult,muonRoad);

      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Super point fitter failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
	continue;
      }

      if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
      stationFitterTimer.stop();

      // Track fittingh
      if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
      trackFitterTimer.start();
      sc = m_trackFitter->findTracks(*p_roids,
                                     tgcFitResult,
                                     trackPatterns,
                                     muonRoad);

      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Track fitter failed");
        // Update output trigger element
        updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
        	            rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			    stgcHits, mmHits,
                            trackPatterns, outputTracks, outputID, outputMS, ctx);
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
       if (std::abs(track.pt) > ZERO_LIMIT
           && ( std::abs(track.etaMap) > ETA_LIMIT || std::abs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
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
      updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
      	                  rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			  stgcHits, mmHits,
                          trackPatterns, outputTracks, outputID, outputMS, ctx);
      continue;
    }
    if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->pause();
    trackExtraTimer.stop();

    // Update monitoring variables
    sc = updateMonitor(*p_roi, mdtHits_normal, trackPatterns );
    if (sc != StatusCode::SUCCESS) {
      ATH_MSG_WARNING("Failed to update monitoring variables");
      // Update output trigger element
      updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
       	                  rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			  stgcHits, mmHits,
                          trackPatterns, outputTracks, outputID, outputMS, ctx);
      continue;
    }

    // Update output trigger element
    updateOutputObjects(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
    	                rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
			stgcHits, mmHits,
                        trackPatterns, outputTracks, outputID, outputMS, ctx);


    //-----------------------
    // call the calibration streamer
    //--------------------------- 
    if (m_doCalStream && trackPatterns.size()>0 ) { 
      TrigL2MuonSA::TrackPattern tp = trackPatterns[0];
      bool updateTriggerElement = false;
      //int calBS = m_calBufferSize;
      bool calDS = m_calDataScouting;
      sc = m_calStreamer->createRoiFragment(*p_roi,tp,mdtHits_normal,
       					    rpcHits,
       					    tgcHits,
       					    //calBS,
       					    calDS,
       					    updateTriggerElement,ctx); 
      if (sc != StatusCode::SUCCESS ) {  
	ATH_MSG_WARNING("Calibration streamer: create Roi Fragment failed");
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
					       DataVector<xAOD::L2StandAloneMuon>&         outputSAs,
					       const bool                                  dynamicDeltaRpc,
					       const EventContext&                         ctx) const
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

  if (m_use_timer){
    for (int i_timer = m_timingTimers.size()-1; i_timer>=0; --i_timer){
      m_timingTimers[i_timer]->start();
      if (i_timer != ITIMER_TOTAL_PROCESSING && i_timer != ITIMER_TOTAL_PROCESSING) m_timingTimers[i_timer]->pause();
    }
  }

  TrigL2MuonSA::RpcHits      rpcHits;
  TrigL2MuonSA::TgcHits      tgcHits;
  TrigL2MuonSA::MdtRegion    mdtRegion;
  TrigL2MuonSA::MuonRoad     muonRoad;
  TrigL2MuonSA::RpcFitResult rpcFitResult;
  TrigL2MuonSA::TgcFitResult tgcFitResult;
  TrigL2MuonSA::MdtHits      mdtHits_normal;
  TrigL2MuonSA::MdtHits      mdtHits_overlap;
  TrigL2MuonSA::CscHits      cscHits;
  TrigL2MuonSA::StgcHits     stgcHits;
  TrigL2MuonSA::MmHits       mmHits;

  DataVector<const TrigRoiDescriptor>::const_iterator p_roids;

  p_roids = roids.begin();
  for (const auto p_roi : muonRoIs) {
    ATH_MSG_DEBUG("roi eta/phi: " << (*p_roi).eta() << "/" << (*p_roi).phi());

    // idtracks loop
    if ( (idtracks).empty() ) ATH_MSG_DEBUG("IO TEST: xAOD::TrackParticleContainer has 0 tracks --> Can not use FTF tracks...");
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
      rpcHits.clear();
      tgcHits.clear();
      mdtRegion.Clear();
      muonRoad.Clear();
      rpcFitResult.Clear();
      tgcFitResult.Clear();
      mdtHits_normal.clear();
      mdtHits_overlap.clear();
      cscHits.clear();
      stgcHits.clear();
      mmHits.clear();
      trackPatterns.clear();

      sc = m_ftfRoadDefiner->defineRoad(idtrack, muonRoad);
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

      if ( m_recMuonRoIUtils.isBarrel(p_roi) ) { // Barrel Inside-out
	ATH_MSG_DEBUG("muonRoad.extFtfMiddleEta Barrel: " << muonRoad.extFtfMiddleEta);

	ATH_MSG_DEBUG("Barrel algorithm of IOmode starts");

	muonRoad.setScales(m_scaleRoadBarrelInner,
			   m_scaleRoadBarrelMiddle,
			   m_scaleRoadBarrelOuter);

	// Data preparation
	sc = m_dataPreparator->prepareData(p_roi,
					   *p_roids,
					   m_insideOut,
					   rpcHits,
					   muonRoad,
					   mdtRegion,
					   rpcFitResult,
					   mdtHits_normal,
					   mdtHits_overlap,
					   dynamicDeltaRpc);
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
	sc = m_patternFinder->findPatterns(muonRoad,
					   mdtHits_normal,
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
	sc = m_stationFitter->findSuperPoints(*p_roids,
					      muonRoad,
					      rpcFitResult,
					      trackPatterns);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Super point fitter failed");
	  continue;
	}
	if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
	stationFitterTimer.stop();

	// Track fitting
	if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
	trackFitterTimer.start();
	sc = m_trackFitter->findTracks(*p_roids,
				       rpcFitResult,
				       trackPatterns);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Track fitter failed");
	  continue;
        }
	if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
        trackFitterTimer.stop();

      } else { // Endcap Inside-out
	ATH_MSG_DEBUG("muonRoad.extFtfMiddleEta Endcap: " << muonRoad.extFtfMiddleEta);
	ATH_MSG_DEBUG("Endcap algorithm of IOmode starts");

	prepTimer.start();
	// Data preparation
	sc = m_dataPreparator->prepareData(p_roi,
					   *p_roids,
					   m_insideOut,
					   tgcHits,
					   muonRoad,
					   mdtRegion,
					   tgcFitResult,
					   mdtHits_normal,
					   mdtHits_overlap,
					   cscHits,
					   stgcHits,
					   mmHits);
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
	sc = m_patternFinder->findPatterns(muonRoad,
					   mdtHits_normal,
					   stgcHits,
					   mmHits,
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
	sc = m_stationFitter->findSuperPointsSimple(*p_roids,
						    muonRoad,
						    tgcFitResult,
						    trackPatterns);
	/////csc SuperPoint
	m_cscsegmaker->FindSuperPointCsc(cscHits,trackPatterns,tgcFitResult,muonRoad);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Super point fitter failed");
	  continue;
	}
	if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
	stationFitterTimer.stop();

	// Track fittingh
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
	trackFitterTimer.start();
        sc = m_trackFitter->findTracks(*p_roids,
				       tgcFitResult,
				       trackPatterns,
				       muonRoad);
        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Track fitter failed");
	  continue;
	}
	if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
	trackFitterTimer.stop();

      }

      // fix if eta is strange
      TrigL2MuonSA::TrackPattern track = trackPatterns.back();
      const float ETA_LIMIT       = 2.8;
      const float DELTA_ETA_LIMIT = 1.0;
      float roiEta = (*p_roi).eta();
      if (std::abs(track.pt) > ZERO_LIMIT
	  && ( std::abs(track.etaMap) > ETA_LIMIT || std::abs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
	trackPatterns.back().etaMap = roiEta;
      }

      // Update monitoring variables
      sc = updateMonitor(p_roi, mdtHits_normal, trackPatterns );
      if (sc != StatusCode::SUCCESS) {
	ATH_MSG_WARNING("Failed to update monitoring variables");
      }

      // Update output trigger element
      if ( std::abs(trackPatterns.back().pt) > ZERO_LIMIT ) {
	storeMuonSA(p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		    rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
		    stgcHits, mmHits,
		    trackPatterns.back(), outputSAs, ctx);
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
      muonRoad.Clear();
      mdtRegion.Clear();
      rpcHits.clear();
      tgcHits.clear();
      rpcFitResult.Clear();
      tgcFitResult.Clear();
      mdtHits_normal.clear();
      cscHits.clear();
      stgcHits.clear();
      mmHits.clear();
      trackPatterns.clear();
      TrigL2MuonSA::TrackPattern trackPattern;
      storeMuonSA(p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
      		  rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
      		  stgcHits, mmHits,
      		  trackPattern, outputSAs, ctx);
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
    for (auto outputSA : outputSAs){
      ATH_MSG_DEBUG("outputSA pt/eta/phi: " << outputSA->pt() << "/" << outputSA->etaMS() << "/" << outputSA->phiMS());
    }

    ATH_MSG_DEBUG("outputCBs size: " << outputCBs.size());
    for (auto outputCB : outputCBs){
      ATH_MSG_DEBUG("outputCB pt/eta/phi: " << outputCB->pt() << "/" << outputCB->eta() << "/" << outputCB->phi());
    }

    p_roids++;
    if (p_roids==roids.end()) break;
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

// findMuonSignature of L2 inside-out version
// try to find MS tracks seeded by FTF tracks
StatusCode MuFastSteering::findMuonSignatureIO(const xAOD::TrackParticleContainer&         idtracks,
					       const std::vector<const TrigRoiDescriptor*> roids,
					       const std::vector<const xAOD::MuonRoI*>     muonRoIs,
					       DataVector<xAOD::L2CombinedMuon>&           outputCBs,
					       DataVector<xAOD::L2StandAloneMuon>&         outputSAs,
					       const bool                                  dynamicDeltaRpc,
					       const EventContext&                         ctx) const
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

  if (m_use_timer){
    for (int i_timer = m_timingTimers.size()-1; i_timer>=0; --i_timer){
      m_timingTimers[i_timer]->start();
      if (i_timer != ITIMER_TOTAL_PROCESSING && i_timer != ITIMER_TOTAL_PROCESSING) m_timingTimers[i_timer]->pause();
    }
  }

  TrigL2MuonSA::RpcHits      rpcHits;
  TrigL2MuonSA::TgcHits      tgcHits;
  TrigL2MuonSA::MdtRegion    mdtRegion;
  TrigL2MuonSA::MuonRoad     muonRoad;
  TrigL2MuonSA::RpcFitResult rpcFitResult;
  TrigL2MuonSA::TgcFitResult tgcFitResult;
  TrigL2MuonSA::MdtHits      mdtHits_normal;
  TrigL2MuonSA::MdtHits      mdtHits_overlap;
  TrigL2MuonSA::CscHits      cscHits;
  TrigL2MuonSA::StgcHits     stgcHits;
  TrigL2MuonSA::MmHits       mmHits;

  DataVector<const TrigRoiDescriptor>::const_iterator p_roids;

  p_roids = roids.begin();
  for (const auto p_roi : muonRoIs) {
    ATH_MSG_DEBUG("roi eta/phi: " << (*p_roi).eta() << "/" << (*p_roi).phi());

    // idtracks loop
    if ( (idtracks).empty() ) ATH_MSG_DEBUG("IO TEST: xAOD::TrackParticleContainer has 0 tracks --> Can not use FTF tracks...");
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
      rpcHits.clear();
      tgcHits.clear();
      mdtRegion.Clear();
      muonRoad.Clear();
      rpcFitResult.Clear();
      tgcFitResult.Clear();
      mdtHits_normal.clear();
      mdtHits_overlap.clear();
      cscHits.clear();
      stgcHits.clear();
      mmHits.clear();
      trackPatterns.clear();

      sc = m_ftfRoadDefiner->defineRoad(idtrack, muonRoad);
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

      if ( m_recMuonRoIUtils.isBarrel(p_roi) ) { // Barrel Inside-out
	ATH_MSG_DEBUG("muonRoad.extFtfMiddleEta Barrel: " << muonRoad.extFtfMiddleEta);

	ATH_MSG_DEBUG("Barrel algorithm of IOmode starts");

	muonRoad.setScales(m_scaleRoadBarrelInner,
			   m_scaleRoadBarrelMiddle,
			   m_scaleRoadBarrelOuter);

	// Data preparation
	sc = m_dataPreparator->prepareData(p_roi,
					   *p_roids,
					   m_insideOut,
					   rpcHits,
					   muonRoad,
					   mdtRegion,
					   rpcFitResult,
					   mdtHits_normal,
					   mdtHits_overlap,
					   dynamicDeltaRpc);
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
	sc = m_patternFinder->findPatterns(muonRoad,
					   mdtHits_normal,
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
	sc = m_stationFitter->findSuperPoints(*p_roids,
					      muonRoad,
					      rpcFitResult,
					      trackPatterns);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Super point fitter failed");
	  continue;
	}
	if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
	stationFitterTimer.stop();

	// Track fitting
	if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
	trackFitterTimer.start();
	sc = m_trackFitter->findTracks(*p_roids,
				       rpcFitResult,
				       trackPatterns);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Track fitter failed");
	  continue;
        }
	if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
        trackFitterTimer.stop();

      } else { // Endcap Inside-out
	ATH_MSG_DEBUG("muonRoad.extFtfMiddleEta Endcap: " << muonRoad.extFtfMiddleEta);
	ATH_MSG_DEBUG("Endcap algorithm of IOmode starts");

	prepTimer.start();
	// Data preparation
	sc = m_dataPreparator->prepareData(p_roi,
					   *p_roids,
					   m_insideOut,
					   tgcHits,
					   muonRoad,
					   mdtRegion,
					   tgcFitResult,
					   mdtHits_normal,
					   mdtHits_overlap,
					   cscHits,
					   stgcHits,
					   mmHits);
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
	sc = m_patternFinder->findPatterns(muonRoad,
					   mdtHits_normal,
					   stgcHits,
					   mmHits,
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
	sc = m_stationFitter->findSuperPointsSimple(*p_roids,
						    muonRoad,
						    tgcFitResult,
						    trackPatterns);
	/////csc SuperPoint
	m_cscsegmaker->FindSuperPointCsc(cscHits,trackPatterns,tgcFitResult,muonRoad);
	if (!sc.isSuccess()) {
	  ATH_MSG_WARNING("Super point fitter failed");
	  continue;
	}
	if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
	stationFitterTimer.stop();

	// Track fittingh
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
	trackFitterTimer.start();
        sc = m_trackFitter->findTracks(*p_roids,
				       tgcFitResult,
				       trackPatterns,
				       muonRoad);
        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Track fitter failed");
	  continue;
	}
	if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
	trackFitterTimer.stop();

      }

      // fix if eta is strange
      TrigL2MuonSA::TrackPattern track = trackPatterns.back();
      const float ETA_LIMIT       = 2.8;
      const float DELTA_ETA_LIMIT = 1.0;
      float roiEta = (*p_roi).eta();
      if (std::abs(track.pt) > ZERO_LIMIT
	  && ( std::abs(track.etaMap) > ETA_LIMIT || std::abs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
	trackPatterns.back().etaMap = roiEta;
      }

      // Update monitoring variables
      sc = updateMonitor(p_roi, mdtHits_normal, trackPatterns );
      if (sc != StatusCode::SUCCESS) {
	ATH_MSG_WARNING("Failed to update monitoring variables");
      }

      // Update output trigger element
      if ( std::abs(trackPatterns.back().pt) > ZERO_LIMIT ) {
	storeMuonSA(p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		    rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
		    stgcHits, mmHits,
		    trackPatterns.back(), outputSAs, ctx);
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
      muonRoad.Clear();
      mdtRegion.Clear();
      rpcHits.clear();
      tgcHits.clear();
      rpcFitResult.Clear();
      tgcFitResult.Clear();
      mdtHits_normal.clear();
      cscHits.clear();
      stgcHits.clear();
      mmHits.clear();
      trackPatterns.clear();
      TrigL2MuonSA::TrackPattern trackPattern;
      storeMuonSA(p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
      		  rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
      		  stgcHits, mmHits,
      		  trackPattern, outputSAs, ctx);
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
    for (auto outputSA : outputSAs){
      ATH_MSG_DEBUG("outputSA pt/eta/phi: " << outputSA->pt() << "/" << outputSA->etaMS() << "/" << outputSA->phiMS());
    }

    ATH_MSG_DEBUG("outputCBs size: " << outputCBs.size());
    for (auto outputCB : outputCBs){
      ATH_MSG_DEBUG("outputCB pt/eta/phi: " << outputCB->pt() << "/" << outputCB->eta() << "/" << outputCB->phi());
    }

    p_roids++;
    if (p_roids==roids.end()) break;
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

// findMuonSignature of L2 multi-track SA version
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode MuFastSteering::findMultiTrackSignature(const std::vector<const TrigRoiDescriptor*>& roids,
                                                  const std::vector<const LVL1::RecMuonRoI*>&  muonRoIs,
                                                  DataVector<xAOD::L2StandAloneMuon>&          outputTracks,
                                                  const bool                                   dynamicDeltaRpc,
                                                  const EventContext&                          ctx) const
{
  ATH_MSG_DEBUG("StatusCode MuFastSteering::findMultiTrackSignature start");
  StatusCode sc = StatusCode::SUCCESS;
  const float ZERO_LIMIT = 1.e-5;

  // for RPC clustering and clusterRoad
  std::vector<TrigL2MuonSA::RpcFitResult>   clusterFitResults;
  std::vector< TrigL2MuonSA::MuonRoad >     clusterRoad;
  std::vector<TrigL2MuonSA::MdtHits>        mdtHits_cluster_normal;


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

  TrigL2MuonSA::RpcHits      rpcHits;
  TrigL2MuonSA::TgcHits      tgcHits;
  TrigL2MuonSA::MdtRegion    mdtRegion;
  TrigL2MuonSA::MuonRoad     muonRoad;
  TrigL2MuonSA::RpcFitResult rpcFitResult;
  TrigL2MuonSA::TgcFitResult tgcFitResult;
  TrigL2MuonSA::MdtHits      mdtHits_normal;
  TrigL2MuonSA::MdtHits      mdtHits_overlap;
  TrigL2MuonSA::CscHits      cscHits;
  TrigL2MuonSA::StgcHits     stgcHits;
  TrigL2MuonSA::MmHits       mmHits;

  DataVector<const TrigRoiDescriptor>::const_iterator p_roids;
  DataVector<const LVL1::RecMuonRoI>::const_iterator p_roi;

  // muonRoIs = RecMURoIs, roids = MURoIs
  p_roids = roids.begin();
  for (p_roi=(muonRoIs).begin(); p_roi!=(muonRoIs).end(); ++p_roi) {

    prepTimer.start();
    std::vector<TrigL2MuonSA::TrackPattern> trackPatterns;
    rpcHits.clear();
    tgcHits.clear();
    mdtRegion.Clear();
    muonRoad.Clear();
    rpcFitResult.Clear();
    tgcFitResult.Clear();
    mdtHits_normal.clear();
    mdtHits_overlap.clear();
    cscHits.clear();
    stgcHits.clear();
    mmHits.clear();

    clusterFitResults.clear();
    clusterRoad.clear();
    mdtHits_cluster_normal.clear();

    if ( m_recMuonRoIUtils.isBarrel(*p_roi) ) { // Barrel
      ATH_MSG_DEBUG("Barrel");

      muonRoad.setScales(m_scaleRoadBarrelInner,
			 m_scaleRoadBarrelMiddle,
			 m_scaleRoadBarrelOuter);

      // Data preparation
      sc = m_dataPreparator->prepareData(*p_roi,
                                         *p_roids,
                                         clusterRoad,
                                         clusterFitResults,
                                         mdtHits_normal,
                                         mdtHits_overlap,
                                         mdtHits_cluster_normal,
                                         dynamicDeltaRpc);

      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Data preparation failed");
	continue;
      }
      ATH_MSG_DEBUG("clusterRoad size = " << clusterRoad.size());

      if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
      prepTimer.stop();

      for(unsigned int i_road = 0; i_road < clusterRoad.size(); i_road++){
        // Pattern finding
        std::vector<TrigL2MuonSA::TrackPattern> tmp_trkPats; tmp_trkPats.clear();

        if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
        patternTimer.start();
        sc = m_patternFinder->findPatterns(clusterRoad.at(i_road),
                                           mdtHits_cluster_normal.at(i_road),
                                           tmp_trkPats);
        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Pattern finder failed");
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
        patternTimer.stop();

        // Superpoint fit
        if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
        stationFitterTimer.start();
        sc = m_stationFitter->findSuperPoints(*p_roids,
                                              clusterRoad.at(i_road),
                                              clusterFitResults.at(i_road),
                                              tmp_trkPats);
        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Super point fitter failed");
          // Update output trigger element
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
        stationFitterTimer.stop();

        // Track fitting
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
        trackFitterTimer.start();
        sc = m_trackFitter->findTracks(*p_roids,
                                       clusterFitResults.at(i_road),
                                       tmp_trkPats);

        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Track fitter failed");
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
        trackFitterTimer.stop();

        // fix if eta is strange
        const float ETA_LIMIT       = 2.8;
        const float DELTA_ETA_LIMIT = 1.0;
        for (TrigL2MuonSA::TrackPattern& track : tmp_trkPats) {
          float roiEta = (*p_roi)->eta();
          if (std::abs(track.pt) > ZERO_LIMIT
              && ( std::abs(track.etaMap) > ETA_LIMIT || std::abs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
            track.etaMap = roiEta;
          }
        }

        // Track extrapolation for ID combined
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->resume();
        trackExtraTimer.start();

        sc = m_trackExtrapolator->extrapolateTrack(tmp_trkPats, m_winPt);
        ATH_MSG_DEBUG("test trackExtrapolator end");

        if (sc != StatusCode::SUCCESS) {
          ATH_MSG_WARNING("Track extrapolator failed");
          // Update output trigger element
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->pause();
        trackExtraTimer.stop();

        if(tmp_trkPats.size() > 0){
          ATH_MSG_DEBUG("temp pT calculated 2mu-in-1RoI alg = " << tmp_trkPats[0].pt << " GeV");
          if( (std::abs(tmp_trkPats[0].barrelSagitta) < ZERO_LIMIT &&
               std::abs(tmp_trkPats[0].barrelRadius) < ZERO_LIMIT) ||
               std::abs(tmp_trkPats[0].pt) < ZERO_LIMIT )
            continue;
          trackPatterns.push_back(tmp_trkPats[0]);
        }

	storeMuonSA(*p_roi, *p_roids, clusterRoad.at(i_road), mdtRegion, rpcHits, tgcHits,
		    clusterFitResults.at(i_road), tgcFitResult, mdtHits_cluster_normal.at(i_road), cscHits,
		    stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);

      } // end the clusterRoad loop
      if(trackPatterns.empty()){
	ATH_MSG_DEBUG("multi-track SA falied to reconstruct muons");
 	TrigL2MuonSA::TrackPattern trackPattern;
	trackPatterns.push_back(trackPattern);
	storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		    rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
		    stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);

	continue;
      }
    } else { // Endcap
      ATH_MSG_DEBUG("Endcap");
      if(!m_doEndcapForl2mt){
        ATH_MSG_DEBUG("multi-track SA does nothings and skips for EndcapRoI");
      } else {
        prepTimer.start();
        // Data preparation
        sc = m_dataPreparator->prepareData(*p_roi,
                                           *p_roids,
                                           m_insideOut,
                                           tgcHits,
                                           muonRoad,
                                           mdtRegion,
                                           tgcFitResult,
                                           mdtHits_normal,
                                           mdtHits_overlap,
                                           cscHits,
                                           stgcHits,
                                           mmHits);
        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Data preparation failed");
          TrigL2MuonSA::TrackPattern trackPattern;
          trackPatterns.push_back(trackPattern);
          // Update output trigger element
          storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
                      rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
                      stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
        prepTimer.stop();

        // Pattern finding
        if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
        patternTimer.start();
        sc = m_patternFinder->findPatterns(muonRoad,
                                           mdtHits_normal,
                                           stgcHits,
                                           mmHits,
                                           trackPatterns);



        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Pattern finder failed");
          // Update output trigger element
          storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		      rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
                      stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
        patternTimer.stop();

        // Superpoint fit
        if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
        stationFitterTimer.start();
        if(!m_use_new_segmentfit){
          sc = m_stationFitter->findSuperPointsSimple(*p_roids,
	       					      muonRoad,
                                                      tgcFitResult,
                                                      trackPatterns);
        }else{
          sc = m_stationFitter->findSuperPoints(*p_roids,
                                                muonRoad,
                                                tgcFitResult,
                                                trackPatterns);
        }
        /////csc SuperPoint
        m_cscsegmaker->FindSuperPointCsc(cscHits,trackPatterns,tgcFitResult,muonRoad);

        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Super point fitter failed");
          storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		      rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
                      stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
          continue;
        }

        if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
        stationFitterTimer.stop();

        // Track fittingh
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
        trackFitterTimer.start();
        sc = m_trackFitter->findTracks(*p_roids,
                                       tgcFitResult,
                                       trackPatterns,
                                       muonRoad);

        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Track fitter failed");
          storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		      rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
                      stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
        trackFitterTimer.stop();

        // fix if eta is strange
        const float ETA_LIMIT       = 2.8;
        const float DELTA_ETA_LIMIT = 1.0;
        for (TrigL2MuonSA::TrackPattern& track : trackPatterns) {
          float roiEta = (*p_roi)->eta();
          if (std::abs(track.pt) > ZERO_LIMIT
              && ( std::abs(track.etaMap) > ETA_LIMIT || std::abs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
            track.etaMap = roiEta;
          }
        }

        // Track extrapolation for ID combined
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->resume();
        trackExtraTimer.start();

        sc = m_trackExtrapolator->extrapolateTrack(trackPatterns, m_winPt);

        if (sc != StatusCode::SUCCESS) {
          ATH_MSG_WARNING("Track extrapolator failed");
          storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
                      rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
                      stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->pause();
        trackExtraTimer.stop();

	storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		    rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
		    stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
      }
    }
    // Update monitoring variables
    sc = updateMonitor(*p_roi, mdtHits_normal, trackPatterns );
    if (sc != StatusCode::SUCCESS) {
      ATH_MSG_WARNING("Failed to update monitoring variables");
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

  ATH_MSG_DEBUG("StatusCode MuFastSteering::findMultiTrackSignature success");
  return StatusCode::SUCCESS;
}

// findMuonSignature of L2 multi-track SA version
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode MuFastSteering::findMultiTrackSignature(const std::vector<const TrigRoiDescriptor*>& roids,
                                                  const std::vector<const xAOD::MuonRoI*>&     muonRoIs,
                                                  DataVector<xAOD::L2StandAloneMuon>&          outputTracks,
                                                  const bool                                   dynamicDeltaRpc,
                                                  const EventContext&                          ctx) const
{
  ATH_MSG_DEBUG("StatusCode MuFastSteering::findMultiTrackSignature start");
  StatusCode sc = StatusCode::SUCCESS;
  const float ZERO_LIMIT = 1.e-5;

  // for RPC clustering and clusterRoad
  std::vector<TrigL2MuonSA::RpcFitResult>   clusterFitResults;
  std::vector< TrigL2MuonSA::MuonRoad >     clusterRoad;
  std::vector<TrigL2MuonSA::MdtHits>        mdtHits_cluster_normal;


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

  TrigL2MuonSA::RpcHits      rpcHits;
  TrigL2MuonSA::TgcHits      tgcHits;
  TrigL2MuonSA::MdtRegion    mdtRegion;
  TrigL2MuonSA::MuonRoad     muonRoad;
  TrigL2MuonSA::RpcFitResult rpcFitResult;
  TrigL2MuonSA::TgcFitResult tgcFitResult;
  TrigL2MuonSA::MdtHits      mdtHits_normal;
  TrigL2MuonSA::MdtHits      mdtHits_overlap;
  TrigL2MuonSA::CscHits      cscHits;
  TrigL2MuonSA::StgcHits     stgcHits;
  TrigL2MuonSA::MmHits       mmHits;

  DataVector<const TrigRoiDescriptor>::const_iterator p_roids;
  DataVector<const xAOD::MuonRoI>::const_iterator p_roi;

  // muonRoIs = RecMURoIs, roids = MURoIs
  p_roids = roids.begin();
  for (p_roi=(muonRoIs).begin(); p_roi!=(muonRoIs).end(); ++p_roi) {

    prepTimer.start();
    std::vector<TrigL2MuonSA::TrackPattern> trackPatterns;
    rpcHits.clear();
    tgcHits.clear();
    mdtRegion.Clear();
    muonRoad.Clear();
    rpcFitResult.Clear();
    tgcFitResult.Clear();
    mdtHits_normal.clear();
    mdtHits_overlap.clear();
    cscHits.clear();
    stgcHits.clear();
    mmHits.clear();

    clusterFitResults.clear();
    clusterRoad.clear();
    mdtHits_cluster_normal.clear();

    if ( m_recMuonRoIUtils.isBarrel(*p_roi) ) { // Barrel
      ATH_MSG_DEBUG("Barrel");

      muonRoad.setScales(m_scaleRoadBarrelInner,
			 m_scaleRoadBarrelMiddle,
			 m_scaleRoadBarrelOuter);

      // Data preparation
      sc = m_dataPreparator->prepareData(*p_roi,
                                         *p_roids,
                                         clusterRoad,
                                         clusterFitResults,
                                         mdtHits_normal,
                                         mdtHits_overlap,
                                         mdtHits_cluster_normal,
                                         dynamicDeltaRpc);

      if (!sc.isSuccess()) {
	ATH_MSG_WARNING("Data preparation failed");
	continue;
      }
      ATH_MSG_DEBUG("clusterRoad size = " << clusterRoad.size());

      if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
      prepTimer.stop();

      for(unsigned int i_road = 0; i_road < clusterRoad.size(); i_road++){
        // Pattern finding
        std::vector<TrigL2MuonSA::TrackPattern> tmp_trkPats; tmp_trkPats.clear();

        if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
        patternTimer.start();
        sc = m_patternFinder->findPatterns(clusterRoad.at(i_road),
                                           mdtHits_cluster_normal.at(i_road),
                                           tmp_trkPats);
        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Pattern finder failed");
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
        patternTimer.stop();

        // Superpoint fit
        if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
        stationFitterTimer.start();
        sc = m_stationFitter->findSuperPoints(*p_roids,
                                              clusterRoad.at(i_road),
                                              clusterFitResults.at(i_road),
                                              tmp_trkPats);
        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Super point fitter failed");
          // Update output trigger element
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
        stationFitterTimer.stop();

        // Track fitting
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
        trackFitterTimer.start();
        sc = m_trackFitter->findTracks(*p_roids,
                                       clusterFitResults.at(i_road),
                                       tmp_trkPats);

        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Track fitter failed");
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
        trackFitterTimer.stop();

        // fix if eta is strange
        const float ETA_LIMIT       = 2.8;
        const float DELTA_ETA_LIMIT = 1.0;
        for (TrigL2MuonSA::TrackPattern& track : tmp_trkPats) {
          float roiEta = (*p_roi)->eta();
          if (std::abs(track.pt) > ZERO_LIMIT
              && ( std::abs(track.etaMap) > ETA_LIMIT || std::abs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
            track.etaMap = roiEta;
          }
        }

        // Track extrapolation for ID combined
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->resume();
        trackExtraTimer.start();

        sc = m_trackExtrapolator->extrapolateTrack(tmp_trkPats, m_winPt);
        ATH_MSG_DEBUG("test trackExtrapolator end");

        if (sc != StatusCode::SUCCESS) {
          ATH_MSG_WARNING("Track extrapolator failed");
          // Update output trigger element
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->pause();
        trackExtraTimer.stop();

        if(tmp_trkPats.size() > 0){
          ATH_MSG_DEBUG("temp pT calculated 2mu-in-1RoI alg = " << tmp_trkPats[0].pt << " GeV");
          if( (std::abs(tmp_trkPats[0].barrelSagitta) < ZERO_LIMIT &&
               std::abs(tmp_trkPats[0].barrelRadius) < ZERO_LIMIT) ||
               std::abs(tmp_trkPats[0].pt) < ZERO_LIMIT )
            continue;
          trackPatterns.push_back(tmp_trkPats[0]);
        }

	storeMuonSA(*p_roi, *p_roids, clusterRoad.at(i_road), mdtRegion, rpcHits, tgcHits,
		    clusterFitResults.at(i_road), tgcFitResult, mdtHits_cluster_normal.at(i_road), cscHits,
		    stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);

      } // end the clusterRoad loop
      if(trackPatterns.empty()){
	ATH_MSG_DEBUG("multi-track SA falied to reconstruct muons");
 	TrigL2MuonSA::TrackPattern trackPattern;
	trackPatterns.push_back(trackPattern);
	storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		    rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
		    stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);

	continue;
      }
    } else { // Endcap
      ATH_MSG_DEBUG("Endcap");
      if(!m_doEndcapForl2mt){
        ATH_MSG_DEBUG("multi-track SA does nothings and skips for EndcapRoI");
      } else {
        prepTimer.start();
        // Data preparation
        sc = m_dataPreparator->prepareData(*p_roi,
                                           *p_roids,
                                           m_insideOut,
                                           tgcHits,
                                           muonRoad,
                                           mdtRegion,
                                           tgcFitResult,
                                           mdtHits_normal,
                                           mdtHits_overlap,
                                           cscHits,
                                           stgcHits,
                                           mmHits);
        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Data preparation failed");
          TrigL2MuonSA::TrackPattern trackPattern;
          trackPatterns.push_back(trackPattern);
          // Update output trigger element
          storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
                      rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
                      stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_DATA_PREPARATOR]->pause();
        prepTimer.stop();

        // Pattern finding
        if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->resume();
        patternTimer.start();
        sc = m_patternFinder->findPatterns(muonRoad,
                                           mdtHits_normal,
                                           stgcHits,
                                           mmHits,
                                           trackPatterns);



        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Pattern finder failed");
          // Update output trigger element
          storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		      rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
                      stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_PATTERN_FINDER]->pause();
        patternTimer.stop();

        // Superpoint fit
        if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->resume();
        stationFitterTimer.start();
        if(!m_use_new_segmentfit){
          sc = m_stationFitter->findSuperPointsSimple(*p_roids,
	       					      muonRoad,
                                                      tgcFitResult,
                                                      trackPatterns);
        }else{
          sc = m_stationFitter->findSuperPoints(*p_roids,
                                                muonRoad,
                                                tgcFitResult,
                                                trackPatterns);
        }
        /////csc SuperPoint
        m_cscsegmaker->FindSuperPointCsc(cscHits,trackPatterns,tgcFitResult,muonRoad);

        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Super point fitter failed");
          storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		      rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
                      stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
          continue;
        }

        if (m_use_timer) m_timingTimers[ITIMER_STATION_FITTER]->pause();
        stationFitterTimer.stop();

        // Track fittingh
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->resume();
        trackFitterTimer.start();
        sc = m_trackFitter->findTracks(*p_roids,
                                       tgcFitResult,
                                       trackPatterns,
                                       muonRoad);

        if (!sc.isSuccess()) {
          ATH_MSG_WARNING("Track fitter failed");
          storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		      rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
                      stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_FITTER]->pause();
        trackFitterTimer.stop();

        // fix if eta is strange
        const float ETA_LIMIT       = 2.8;
        const float DELTA_ETA_LIMIT = 1.0;
        for (TrigL2MuonSA::TrackPattern& track : trackPatterns) {
          float roiEta = (*p_roi)->eta();
          if (std::abs(track.pt) > ZERO_LIMIT
              && ( std::abs(track.etaMap) > ETA_LIMIT || std::abs(track.etaMap-roiEta) > DELTA_ETA_LIMIT ) ) {
            track.etaMap = roiEta;
          }
        }

        // Track extrapolation for ID combined
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->resume();
        trackExtraTimer.start();

        sc = m_trackExtrapolator->extrapolateTrack(trackPatterns, m_winPt);

        if (sc != StatusCode::SUCCESS) {
          ATH_MSG_WARNING("Track extrapolator failed");
          storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
                      rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
                      stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
          continue;
        }
        if (m_use_timer) m_timingTimers[ITIMER_TRACK_EXTRAPOLATOR]->pause();
        trackExtraTimer.stop();

	storeMuonSA(*p_roi, *p_roids, muonRoad, mdtRegion, rpcHits, tgcHits,
		    rpcFitResult, tgcFitResult, mdtHits_normal, cscHits,
		    stgcHits, mmHits, trackPatterns.back(), outputTracks, ctx);
      }
    }
    // Update monitoring variables
    sc = updateMonitor(*p_roi, mdtHits_normal, trackPatterns );
    if (sc != StatusCode::SUCCESS) {
      ATH_MSG_WARNING("Failed to update monitoring variables");
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

  ATH_MSG_DEBUG("StatusCode MuFastSteering::findMultiTrackSignature success");
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
				         TrigRoiDescriptorCollection&   	        outputMS,
					 const EventContext&                            ctx) const
{

  if( trackPatterns.size() > 0 ) {

    const TrigL2MuonSA::TrackPattern& pattern = trackPatterns.back();

    // Update output trigger element
    storeMuonSA(roi, roids, muonRoad, mdtRegion, rpcHits, tgcHits,
    	        rpcFitResult, tgcFitResult, mdtHits, cscHits,
		stgcHits, mmHits,
                pattern, outputTracks, ctx);
    storeMSRoiDescriptor(roids, pattern, outputTracks, outputMS);
    storeIDRoiDescriptor(roids, pattern, outputTracks, outputID);

  } else {
    ATH_MSG_DEBUG("Not update output objects because trackPatterns has no object");
  }

  return true;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool MuFastSteering::updateOutputObjects(const xAOD::MuonRoI*                           roi,
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
				         TrigRoiDescriptorCollection&   	        outputMS,
					 const EventContext&                            ctx) const
{

  if( trackPatterns.size() > 0 ) {

    const TrigL2MuonSA::TrackPattern& pattern = trackPatterns.back();

    // Update output trigger element
    storeMuonSA(roi, roids, muonRoad, mdtRegion, rpcHits, tgcHits,
    	        rpcFitResult, tgcFitResult, mdtHits, cscHits,
		stgcHits, mmHits,
                pattern, outputTracks, ctx);
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
                                 DataVector<xAOD::L2StandAloneMuon>& outputTracks,
                                 const EventContext&                 ctx ) const
{
  const float ZERO_LIMIT = 1.e-5;

  const int currentRoIId = roids->roiId();

  const EventIDBase& eventID = ctx.eventID();
  auto eventInfo = SG::makeHandle(m_eventInfoKey, ctx);
  if (!eventInfo.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve xAOD::EventInfo object");
    return false;
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

    float dRMS = std::sqrt( std::abs(pattern.etaMap-rpcHit.eta)*std::abs(pattern.etaMap-rpcHit.eta) + std::acos(std::cos(pattern.phiMS-rpcHit.phi))*std::acos(std::cos(pattern.phiMS-rpcHit.phi)) );
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
    if ( std::abs(pattern.superPoints[inner].R) > ZERO_LIMIT && std::abs(pattern.superPoints[inner].Z) > ZERO_LIMIT ) { // if R and Z exist
      if ( tgcFitResult.isSuccess && std::abs(tgcFitResult.tgcInn[3]) > ZERO_LIMIT ) { // if phi exist
        float theta = std::atan(pattern.superPoints[inner].R/std::abs(pattern.superPoints[inner].Z));
        float eta = (std::tan(theta/2.)!=0.)? -std::log(std::tan(theta/2.))*pattern.superPoints[inner].Z/std::abs(pattern.superPoints[inner].Z): 0.;
        muonSA->setTrackPosition( pattern.superPoints[inner].R, pattern.superPoints[inner].Z, eta, tgcFitResult.tgcInn[1] );
      }
    }

    // Middle
    if ( std::abs(pattern.superPoints[middle].R) > ZERO_LIMIT && std::abs(pattern.superPoints[middle].Z) > ZERO_LIMIT ) { // if R and Z exist
      float phi = 0;
      if (tgcFitResult.isSuccess && ( std::abs(tgcFitResult.tgcMid1[3]) > ZERO_LIMIT || std::abs(tgcFitResult.tgcMid2[3]) > ZERO_LIMIT )) { // if phi exist
        double phi1 = tgcFitResult.tgcMid1[1];
        double phi2 = tgcFitResult.tgcMid2[1];
        if ( tgcFitResult.tgcMid1[3]==0. || tgcFitResult.tgcMid2[3]==0. ) {
          if ( std::abs(tgcFitResult.tgcMid1[3]) > ZERO_LIMIT ) phi = phi1;
          if ( std::abs(tgcFitResult.tgcMid2[3]) > ZERO_LIMIT ) phi = phi2;
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
      float theta = std::atan(pattern.superPoints[middle].R/std::abs(pattern.superPoints[middle].Z));
      float eta = (std::tan(theta/2.)!=0.)? -std::log(std::tan(theta/2.))*pattern.superPoints[middle].Z/std::abs(pattern.superPoints[middle].Z): 0.;
      muonSA->setTrackPosition( pattern.superPoints[middle].R, pattern.superPoints[middle].Z, eta, phi );
    }

  } else { // barrel

    // Middle
    if ( std::abs(pattern.superPoints[middle].R) > ZERO_LIMIT && std::abs(pattern.superPoints[middle].Z) > ZERO_LIMIT ) { // if R and Z exist
      float phi = 0;
      if (rpcFitResult.isSuccess) {
        phi = rpcFitResult.phi;
      } else {
        phi = roi->phi();
      }
      float theta = std::atan(pattern.superPoints[middle].R/std::abs(pattern.superPoints[middle].Z));
      float eta = (std::tan(theta/2.)!=0.)? -std::log(std::tan(theta/2.))*pattern.superPoints[middle].Z/std::abs(pattern.superPoints[middle].Z): 0.;
      muonSA->setTrackPosition( pattern.superPoints[middle].R, pattern.superPoints[middle].Z, eta, phi );
    }

    // Not stored outer position for the moment as the phi is not available

  }
  outputTracks.push_back(muonSA);

  return true;
}

bool MuFastSteering::storeMuonSA(const xAOD::MuonRoI*                roi,
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
                                 DataVector<xAOD::L2StandAloneMuon>& outputTracks,
                                 const EventContext&                 ctx ) const
{
  const float ZERO_LIMIT = 1.e-5;

  const int currentRoIId = roids->roiId();

  const EventIDBase& eventID = ctx.eventID();
  auto eventInfo = SG::makeHandle(m_eventInfoKey, ctx);
  if (!eventInfo.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve xAOD::EventInfo object");
    return false;
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
  /// Set RoI system ID (or system ID; Barrel=0, Endcap=1, Forward=2)
  muonSA->setRoiSystem( roi->getSource() );
  /// Set RoI subsystem ID (0=-z,1=+z)
  muonSA->setRoiSubsystem( 1 - roi->getHemisphere() );
  /// Set RoI sector ID
  muonSA->setRoiSector( roi->getSectorID() );
  /// Set RoI number
  muonSA->setRoiNumber( roi->getRoI() );
  /// Set RoI threshold number
  muonSA->setRoiThreshold( roi->getThrNumber() );
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

    float dRMS = std::sqrt( std::abs(pattern.etaMap-rpcHit.eta)*std::abs(pattern.etaMap-rpcHit.eta) + std::acos(std::cos(pattern.phiMS-rpcHit.phi))*std::acos(std::cos(pattern.phiMS-rpcHit.phi)) );
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
    if ( std::abs(pattern.superPoints[inner].R) > ZERO_LIMIT && std::abs(pattern.superPoints[inner].Z) > ZERO_LIMIT ) { // if R and Z exist
      if ( tgcFitResult.isSuccess && std::abs(tgcFitResult.tgcInn[3]) > ZERO_LIMIT ) { // if phi exist
        float theta = std::atan(pattern.superPoints[inner].R/std::abs(pattern.superPoints[inner].Z));
        float eta = (std::tan(theta/2.)!=0.)? -std::log(std::tan(theta/2.))*pattern.superPoints[inner].Z/std::abs(pattern.superPoints[inner].Z): 0.;
        muonSA->setTrackPosition( pattern.superPoints[inner].R, pattern.superPoints[inner].Z, eta, tgcFitResult.tgcInn[1] );
      }
    }

    // Middle
    if ( std::abs(pattern.superPoints[middle].R) > ZERO_LIMIT && std::abs(pattern.superPoints[middle].Z) > ZERO_LIMIT ) { // if R and Z exist
      float phi = 0;
      if (tgcFitResult.isSuccess && ( std::abs(tgcFitResult.tgcMid1[3]) > ZERO_LIMIT || std::abs(tgcFitResult.tgcMid2[3]) > ZERO_LIMIT )) { // if phi exist
        double phi1 = tgcFitResult.tgcMid1[1];
        double phi2 = tgcFitResult.tgcMid2[1];
        if ( tgcFitResult.tgcMid1[3]==0. || tgcFitResult.tgcMid2[3]==0. ) {
          if ( std::abs(tgcFitResult.tgcMid1[3]) > ZERO_LIMIT ) phi = phi1;
          if ( std::abs(tgcFitResult.tgcMid2[3]) > ZERO_LIMIT ) phi = phi2;
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
      float theta = std::atan(pattern.superPoints[middle].R/std::abs(pattern.superPoints[middle].Z));
      float eta = (std::tan(theta/2.)!=0.)? -std::log(std::tan(theta/2.))*pattern.superPoints[middle].Z/std::abs(pattern.superPoints[middle].Z): 0.;
      muonSA->setTrackPosition( pattern.superPoints[middle].R, pattern.superPoints[middle].Z, eta, phi );
    }

  } else { // barrel

    // Middle
    if ( std::abs(pattern.superPoints[middle].R) > ZERO_LIMIT && std::abs(pattern.superPoints[middle].Z) > ZERO_LIMIT ) { // if R and Z exist
      float phi = 0;
      if (rpcFitResult.isSuccess) {
        phi = rpcFitResult.phi;
      } else {
        phi = roi->phi();
      }
      float theta = std::atan(pattern.superPoints[middle].R/std::abs(pattern.superPoints[middle].Z));
      float eta = (std::tan(theta/2.)!=0.)? -std::log(std::tan(theta/2.))*pattern.superPoints[middle].Z/std::abs(pattern.superPoints[middle].Z): 0.;
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
		                          TrigRoiDescriptorCollection&	 	    outputMS) const
{
  const float ZERO_LIMIT = 1.e-5;

  const xAOD::L2StandAloneMuon* muonSA = outputTracks[0];

  float mseta = pattern.etaMap;
  float msphi = pattern.phiMS;

  // store TrigRoiDescriptor
  if (std::abs(muonSA->pt()) < ZERO_LIMIT ) {
    mseta = roids->eta();
    msphi = roids->phi();
  }

  // set width of 0.1 so that ID tracking monitoring works
  const float phiHalfWidth = 0.1;
  const float etaHalfWidth = 0.1;

  TrigRoiDescriptor* MSroiDescriptor = new TrigRoiDescriptor(roids->roiWord(),
							     roids->l1Id(),
							     roids->roiId(),
							     mseta,
							     mseta - etaHalfWidth,
							     mseta + etaHalfWidth,
							     msphi,
							     msphi - phiHalfWidth,
							     msphi + phiHalfWidth);

  ATH_MSG_VERBOSE("...TrigRoiDescriptor for MS "
		  << "mseta/msphi="
		  << mseta << "/" << msphi);

  ATH_MSG_VERBOSE("will Record an RoiDescriptor for TrigMoore:"
		  << " phi=" << MSroiDescriptor->phi()
		  << ",  eta=" << MSroiDescriptor->eta());

  outputMS.push_back(MSroiDescriptor);

  return true;
}


bool MuFastSteering::storeIDRoiDescriptor(const TrigRoiDescriptor*                  roids,
		                          const TrigL2MuonSA::TrackPattern&         pattern,
                                          const DataVector<xAOD::L2StandAloneMuon>& outputTracks,
		                          TrigRoiDescriptorCollection&	 	    outputID) const
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
  if (std::abs(muonSA->pt()) > ZERO_LIMIT ) {

    // patch for the ID RoI descriptor
    float phiHalfWidth = 0.1;
    float etaHalfWidth = 0.1;

    // 2010 runs
    //      if ( std::abs(pattern.etaVtx)>1 && std::abs(pattern.etaVtx)<1.5 ) {
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

    TrigRoiDescriptor* IDroiDescriptor = new TrigRoiDescriptor(roids->roiWord(),
                                                               roids->l1Id(),
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

    TrigRoiDescriptor* IDroiDescriptor = new TrigRoiDescriptor(roids->roiWord(),
                                                               roids->l1Id(),
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

int MuFastSteering::L2MuonAlgoMap(const std::string& name) const
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

float MuFastSteering::getRoiSizeForID(bool isEta, const xAOD::L2StandAloneMuon* muonSA) const
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
         if( std::abs(eta) < 1.5 ) {
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
         else if( std::abs(eta) < 2.0 ) {
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
                                         std::vector<TrigL2MuonSA::TrackPattern>&   trackPatterns ) const
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

      if (std::abs(mdtHit.DriftSpace) < ZERO_LIMIT) continue;

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

    track_pt    = (std::abs(pattern.pt ) > ZERO_LIMIT)? pattern.charge*pattern.pt: 9999.;
    absolute_pt = std::abs(track_pt);

    if ( std::abs(pattern.etaMap) > ZERO_LIMIT || std::abs(pattern.phiMS) > ZERO_LIMIT ) {
      t_eta.push_back(pattern.etaMap);
      t_phi.push_back(pattern.phiMS);
    }
    if ( std::abs(pattern.pt ) < ZERO_LIMIT){
      f_eta.push_back(roi->eta());
      f_phi.push_back(roi->phi());
    }

    sagitta     = (std::abs(pattern.barrelSagitta) > ZERO_LIMIT)? pattern.barrelSagitta: 9999.;
    sag_inverse = (std::abs(pattern.barrelSagitta) > ZERO_LIMIT)? 1./pattern.barrelSagitta: 9999.;
    address     = pattern.s_address;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode MuFastSteering::updateMonitor(const xAOD::MuonRoI*                       roi,
                                         const TrigL2MuonSA::MdtHits&               mdtHits,
                                         std::vector<TrigL2MuonSA::TrackPattern>&   trackPatterns ) const
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

      if (std::abs(mdtHit.DriftSpace) < ZERO_LIMIT) continue;

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

    track_pt    = (std::abs(pattern.pt ) > ZERO_LIMIT)? pattern.charge*pattern.pt: 9999.;
    absolute_pt = std::abs(track_pt);

    if ( std::abs(pattern.etaMap) > ZERO_LIMIT || std::abs(pattern.phiMS) > ZERO_LIMIT ) {
      t_eta.push_back(pattern.etaMap);
      t_phi.push_back(pattern.phiMS);
    }
    if ( std::abs(pattern.pt ) < ZERO_LIMIT){
      f_eta.push_back(roi->eta());
      f_phi.push_back(roi->phi());
    }

    sagitta     = (std::abs(pattern.barrelSagitta) > ZERO_LIMIT)? pattern.barrelSagitta: 9999.;
    sag_inverse = (std::abs(pattern.barrelSagitta) > ZERO_LIMIT)? 1./pattern.barrelSagitta: 9999.;
    address     = pattern.s_address;
  }

  return StatusCode::SUCCESS;
}



