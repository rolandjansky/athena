/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>  // for find

#include "TrigMuonEFStandaloneTrackTool.h"

#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/Timeout.h"

#include "TrigConfHLTData/HLTTriggerElement.h"

#include "MuonSegmentCombinerToolInterfaces/IMooSegmentCombinationFinder.h"
#include "MuonRecToolInterfaces/IMuonPatternSegmentAssociationTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonSegment/MuonSegment.h"

// #include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "CscClusterization/ICscClusterBuilder.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuidInterfaces/IMuidBackTracker.h"

#include "TrigMuonToolInterfaces/TrigMuonEFMonVars.h"

#include "TrigNavigation/TriggerElement.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"

#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigInterfaces/IMonitoredAlgo.h"  // for AutoClear
#include "AthContainers/ConstDataVector.h"

// xAOD related includes
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedToolInterfaces/IMuonCandidateTool.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include <fstream>

#include "TrigMuonEFUtils.icc"

#if DEBUG_ROI_VS_FULL
#include <set> //used in the method sanity_check
#endif

#define testRoiDrivenMode false

using namespace std;
using namespace Muon;
using CLHEP::GeV;

//________________________________________________________________________
TrigMuonEFStandaloneTrackTool::TrigMuonEFStandaloneTrackTool(const std::string& type,
							     const std::string& name,
							     const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_regionSelector("RegSelSvc",name),
    m_segmentsFinderTool("Muon::MooSegmentCombinationFinder/TMEF_SegmentFinder",this),
    m_assocTool("Muon::MuonPatternSegmentAssociationTool/MuonPatternSegmentAssociationTool"),
    m_trackBuilderTool("Muon::MuonTrackSteering/TMEF_TracksBuilderTool",this),
    m_trackSummaryTool("Trk::TrackSummaryTool/MuidTrackSummaryTool"),
    m_mdtRawDataProvider("Muon::MDT_RawDataProviderTool/MDT_RawDataProviderTool"),
    m_rpcRawDataProvider("Muon::RPC_RawDataProviderTool/RPC_RawDataProviderTool"),
    m_tgcRawDataProvider("Muon::TGC_RawDataProviderTool/TGC_RawDataProviderTool"),
    m_cscPrepDataProvider("Muon::CscRdoToCscPrepDataTool/CscPrepDataProviderTool"),
    m_mdtPrepDataProvider("Muon::MdtRdoToPrepDataTool/MdtPrepDataProviderTool"),
    m_rpcPrepDataProvider("Muon::RpcRdoToPrepDataTool/RpcPrepDataProviderTool"),
    m_tgcPrepDataProvider("Muon::TgcRdoToPrepDataTool/TgcPrepDataProviderTool"),
    m_cscClusterProvider(""),
    p_ActiveStore(0),
    m_cscIdHelper(0),
    m_mdtIdHelper(0),
    m_rpcIdHelper(0),
    m_tgcIdHelper(0),
    m_firstEvent(true),
    m_useCscData(true), 
    m_useRpcData(true), 
    m_useTgcData(true), 
    m_useTGCInPriorNextBC(false),
    m_useMdtData(1),    //0=NO, 1=ALL(default), 2=BarrelOnly, 3=EndCapsOnly
    m_useMdtSeededDecoding(false),
    m_useRpcSeededDecoding(false),
    m_useTgcSeededDecoding(false),
    m_useCscSeededDecoding(false),
    m_useTgcRobDecoding(false),
    m_useCscRobDecoding(false),
    m_doTimeOutChecks(false),
    m_doTimeOutGuard(false),
    m_useL2Hits(false),
    m_maxCscHits(0),
    m_maxMdtHits(0),
    m_maxRpcHits(0),
    m_maxTgcHits(0),
    m_segmentCombiColl(0),
    m_patternCombiColl(0),
    m_segments(0),
    m_spectrometerTracks(0),
    m_segmentCombiCollInternal(0),
    m_patternCombiCollInternal(0),
    m_segmentsInternal(0),
    m_spectrometerTracksInternal(0),
    m_extrapolatedTracks(0),
    m_spectrometerTrackParticles(0),
    m_spectrometerTrackParticlesAux(0),
    m_roi_num_seg(0),
    m_roi_num_mstrk(0),
    m_roi_num_satrk(0),
    m_totalSegmentCalls(0),
    m_cachedSegmentCalls(0),
    m_totalSpectrometerCalls(0),
    m_cachedSpectrometerCalls(0),
    m_totalExtrapolatedCalls(0),
    m_cachedExtrapolatedCalls(0),
    m_muonCandidateTool("MuonCandidateTool/MuonCandidateTool"),
    m_TrackToTrackParticleConvTool("MuonParticleCreatorTool"),
    m_rpcKey("RPC_Measurements"),
    m_tgcKey("TGC_Measurements"),
    m_tgcKeyNextBC("TGC_MeasurementsNextBC"),
    m_cscKey("CSC_Clusters"),
    m_mdtKey("MDT_DriftCircles"),
    m_ignoreCSC(true)
{
  m_hashlist.reserve(4);

  declareInterface<ITrigMuonStandaloneTrackTool>(this);

  declareProperty("RegionSelectionSvc",  m_regionSelector);
  declareProperty("SegmentsFinderTool",  m_segmentsFinderTool);
  declareProperty("AssociationTool",     m_assocTool);
  declareProperty("TrackBuilderTool",    m_trackBuilderTool);
  declareProperty("TrkSummaryTool",      m_trackSummaryTool);
  declareProperty("MuonCandidateTool",   m_muonCandidateTool);
  declareProperty("TrackToTrackParticleConvTool", m_TrackToTrackParticleConvTool);

  declareProperty ("MdtRawDataProvider",m_mdtRawDataProvider);
  declareProperty ("RpcRawDataProvider",m_rpcRawDataProvider);
  declareProperty ("TgcRawDataProvider",m_tgcRawDataProvider);

  declareProperty("CscPrepDataProvider", m_cscPrepDataProvider);
  declareProperty("MdtPrepDataProvider", m_mdtPrepDataProvider);
  declareProperty("RpcPrepDataProvider", m_rpcPrepDataProvider);
  declareProperty("TgcPrepDataProvider", m_tgcPrepDataProvider);
  declareProperty("CscClusterProvider",  m_cscClusterProvider);

  declareProperty("doCache",             m_doCache=true);

  declareProperty("useMdtData",          m_useMdtData );
  declareProperty("useRpcData",          m_useRpcData );
  declareProperty("useTgcData",          m_useTgcData );
  declareProperty("useTGCInPriorNextBC", m_useTGCInPriorNextBC);
  declareProperty("useCscData",          m_useCscData );
    
    
  declareProperty("useMdtSeededDecoding",    m_useMdtSeededDecoding );
  declareProperty("useRpcSeededDecoding",    m_useRpcSeededDecoding );
  declareProperty("useTgcSeededDecoding",    m_useTgcSeededDecoding );
  declareProperty("useCscSeededDecoding",    m_useCscSeededDecoding );
  declareProperty("useTgcRobDecoding",       m_useTgcRobDecoding );
  declareProperty("useCscRobDecoding",       m_useCscRobDecoding );

  declareProperty("doTimeOutChecks",m_doTimeOutChecks);    
  declareProperty("doTimeOutGuard",m_doTimeOutGuard);
  declareProperty("useL2Hits", m_useL2Hits);
  declareProperty("maxTgcHits",m_maxTgcHits);
  declareProperty("maxCscHits",m_maxCscHits);
  declareProperty("maxRpcHits",m_maxRpcHits);
  declareProperty("maxMdtHits",m_maxMdtHits);
  declareProperty("RpcPrepDataContainer", m_rpcKey);
  declareProperty("TgcPrepDataContainer", m_tgcKey);
  declareProperty("TgcPrepDataContainerNextBC", m_tgcKeyNextBC);
  declareProperty("MdtPrepDataContainer", m_mdtKey);
  declareProperty("CscPrepDataContainer", m_cscKey);
  declareProperty("IgnoreMisalginedCSCs", m_ignoreCSC);

  clearRoiCache();
 
} 

//________________________________________________________________________
TrigMuonEFStandaloneTrackTool::~TrigMuonEFStandaloneTrackTool()
{
  // clean up any xAOD particles 
  if( m_spectrometerTrackParticles ) delete  m_spectrometerTrackParticles;
  if( m_spectrometerTrackParticlesAux ) delete  m_spectrometerTrackParticlesAux;
}

//________________________________________________________________________
StatusCode TrigMuonEFStandaloneTrackTool::initialize()
{

#if DEBUG_ROI_VS_FULL 
  m_fileWithHashIds_rpc.open("HashID_rpc.txt");
  m_fileWithHashIds_tgc.open("HashID_tgc.txt");
  m_fileWithHashIds_mdt.open("HashID_mdt.txt");
  m_fileWithHashIds_csc.open("HashID_csc.txt");
#endif
  //m_SFCache\TrigMap.clear();
  ATH_MSG_DEBUG("clearing SFCacheSCmap (initialize)");
  m_SFCacheSCMap.clear();

  if (msgLvl(MSG::DEBUG)) {
    msg() << MSG::DEBUG
	  << "package version = " << PACKAGE_VERSION << endmsg;
    msg() << MSG::DEBUG
	  << "Properties are set as follows: " << endmsg;
    msg() << MSG::DEBUG
	  << "SegmentsFinderTool             " << m_segmentsFinderTool << endmsg;
    msg() << MSG::DEBUG
	  << "TrackBuilderTool               " << m_trackBuilderTool << endmsg;
    msg() << MSG::DEBUG
	  << "MdtRawDataProvider             " << m_mdtRawDataProvider << endmsg;
    msg() << MSG::DEBUG
	  << "RpcRawDataProvider             " << m_rpcRawDataProvider << endmsg;
    msg() << MSG::DEBUG
	  << "TgcRawDataProvider             " << m_tgcRawDataProvider << endmsg;
    msg() << MSG::DEBUG
	  << "CscPrepDataProvider            " << m_cscPrepDataProvider << endmsg;
    msg() << MSG::DEBUG
	  << "MdtPrepDataProvider            " << m_mdtPrepDataProvider << endmsg;
    msg() << MSG::DEBUG
	  << "RpcPrepDataProvider            " << m_rpcPrepDataProvider << endmsg;
    msg() << MSG::DEBUG
	  << "TgcPrepDataProvider            " << m_tgcPrepDataProvider << endmsg;
    
    msg() << MSG::DEBUG
	  << "doCache                        " << m_doCache << endmsg;   
    
    msg() << MSG::DEBUG
	  << "useMdtData                     " << m_useMdtData << endmsg;
    msg() << MSG::DEBUG
	  << "useRpcData                     " << m_useRpcData << endmsg;
    msg() << MSG::DEBUG
	  << "useTgcData                     " << m_useTgcData << endmsg;
    msg() << MSG::DEBUG
	  << "useTGCInPriorNextBC            " << m_useTGCInPriorNextBC << endmsg;   
    msg() << MSG::DEBUG
	  << "useCscData                     " << m_useCscData << endmsg;
    msg() << MSG::DEBUG
	  << "useMdtSeededDecoding           " << m_useMdtSeededDecoding << endmsg;
    msg() << MSG::DEBUG
	  << "useRpcSeededDecoding           " << m_useRpcSeededDecoding << endmsg;
    msg() << MSG::DEBUG
	  << "useTgcSeededDecoding           " << m_useTgcSeededDecoding << endmsg;
    msg() << MSG::DEBUG
	  << "useCscSeededDecoding           " << m_useCscSeededDecoding << endmsg;
    msg() << MSG::DEBUG
	  << "useTgcRobDecoding           " << m_useTgcRobDecoding << endmsg;
    msg() << MSG::DEBUG
	  << "useCscRobDecoding           " << m_useCscRobDecoding << endmsg;

    msg() << MSG::DEBUG
          << m_decodeMdtBS.name() << "       " << m_decodeMdtBS << endmsg;
    msg() << MSG::DEBUG
          << m_decodeRpcBS.name() << "       " << m_decodeRpcBS << endmsg;

    msg() << MSG::DEBUG
          << m_decodeTgcBS.name() << "       " << m_decodeTgcBS << endmsg;
    msg() << MSG::DEBUG
	  << "doTimeOutChecks                " << m_doTimeOutChecks << endmsg;
    msg() << MSG::DEBUG
	  << "doTimeOutGuard                 " << m_doTimeOutGuard << endmsg;   
    msg() << MSG::DEBUG
	  << "useL2Hits                 " << m_useL2Hits << endmsg;   
    
    msg() << MSG::DEBUG
	  << "maxRpcHits                     " << m_maxRpcHits << endmsg;
    msg() << MSG::DEBUG
	  << "maxCscHits                     " << m_maxCscHits << endmsg;
    msg() << MSG::DEBUG
	  << "maxMdtHits                     " << m_maxMdtHits << endmsg;
    msg() << MSG::DEBUG
	  << "maxTgcHits                     " << m_maxTgcHits << endmsg;
  }
  
  // Get the region selector tool:
  if (m_regionSelector.retrieve().isFailure()) {
    msg() << MSG::FATAL
	  << "Unable to retrieve RegionSelector Svc"
	  << endmsg;
    return StatusCode::FAILURE;
  }
  
  // Retrieve ActiveStore
  StatusCode status = serviceLocator()->service("ActiveStoreSvc", p_ActiveStore);
  if(!status.isSuccess() || 0 == p_ActiveStore) {
    msg() << MSG::ERROR <<" Could not find ActiveStoreSvc " << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve MDT raw data provider tool if needed
  if (m_mdtRawDataProvider.retrieve(DisableTool{ !m_decodeMdtBS }).isSuccess()) {
    msg (MSG::INFO) << "Retrieved " << m_mdtRawDataProvider << endmsg;
  }
  else {
    msg (MSG::FATAL) << "Could not get " << m_mdtRawDataProvider << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve RPC raw data provider tool if needed
  if (m_rpcRawDataProvider.retrieve(DisableTool{ !m_decodeRpcBS }).isSuccess()) {
    msg (MSG::INFO) << "Retrieved " << m_rpcRawDataProvider << endmsg;
  }
  else {
    msg (MSG::FATAL) << "Could not get " << m_rpcRawDataProvider << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve TGC raw data provider tool if needed
  if (m_tgcRawDataProvider.retrieve(DisableTool{ !m_decodeTgcBS }).isSuccess()) {
    msg (MSG::INFO) << "Retrieved " << m_tgcRawDataProvider << endmsg;
  }
  else {
    msg (MSG::FATAL) << "Could not get " << m_tgcRawDataProvider << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve segment maker tool
  status = m_segmentsFinderTool.retrieve();
  if (status.isSuccess()){
    msg() << MSG::INFO << "Retrieved " << m_segmentsFinderTool << endmsg;
  } else {
    msg() << MSG::FATAL << "Could not get " << m_segmentsFinderTool << endmsg;
    return StatusCode::FAILURE;
  }

  
  status = m_assocTool.retrieve();
  if (status.isSuccess()){
    msg() << MSG::INFO << "Retrieved " << m_assocTool << endmsg;
  }else{
    msg() << MSG::FATAL << "Could not get " << m_assocTool <<endmsg;
    return StatusCode::FAILURE;
  }

  status = m_trackBuilderTool.retrieve();
  if (status.isSuccess()){
    ATH_MSG_INFO("Retrieved " << m_trackBuilderTool);
  }
  else{
    ATH_MSG_FATAL("Could not get " << m_trackBuilderTool);
    return StatusCode::FAILURE;
  }
    
  // Retrieve track summary tool
  if (m_trackSummaryTool.retrieve().isSuccess()){
    ATH_MSG_INFO("Retrieved " << m_trackSummaryTool);
  }
  else{
    ATH_MSG_FATAL("Could not get " << m_trackSummaryTool);
    return StatusCode::FAILURE;
  }
      
  
 

  if (m_useCscData) {
    
    status = m_cscPrepDataProvider.retrieve();
    if (status.isSuccess()) {
      msg() << MSG::INFO << "Retrieved " << m_cscPrepDataProvider << endmsg;
    } else {
      msg() << MSG::FATAL << "Could not get " << m_cscPrepDataProvider << endmsg;
      return StatusCode::FAILURE;
    }
    // clusterization tool
    status = m_cscClusterProvider.retrieve();
    if (status.isSuccess()) {
      msg() << MSG::INFO << "Retrieved " << m_cscClusterProvider << endmsg;
    } else {
      msg() << MSG::FATAL << "Could not get " << m_cscClusterProvider << endmsg;
      return StatusCode::FAILURE;
    }
  }
  if (m_useMdtData) {
    status = m_mdtPrepDataProvider.retrieve();
    if (status.isSuccess()) {
      msg() << MSG::INFO << "Retrieved " << m_mdtPrepDataProvider << endmsg;
    } else {
      msg() << MSG::FATAL << "Could not get " << m_mdtPrepDataProvider << endmsg;
      return StatusCode::FAILURE;
    }
  }
  if (m_useRpcData) {
    status = m_rpcPrepDataProvider.retrieve();
    if (status.isSuccess()) {
      msg() << MSG::INFO << "Retrieved " << m_rpcPrepDataProvider << endmsg;
    } else {
      msg() << MSG::FATAL << "Could not get " << m_rpcPrepDataProvider << endmsg;
      return StatusCode::FAILURE;
    }
  }
  if (m_useTgcData) {
    status = m_tgcPrepDataProvider.retrieve();
    if (status.isSuccess()) {
      msg() << MSG::INFO << "Retrieved " << m_tgcPrepDataProvider << endmsg;
    } else {
      msg() << MSG::FATAL << "Could not get " << m_tgcPrepDataProvider << endmsg;
      return StatusCode::FAILURE;
    }
  }  

  // retrive muon candidate tool
  status = m_muonCandidateTool.retrieve();
   if (status.isSuccess()){
    ATH_MSG_INFO("Retrieved " << m_muonCandidateTool);
  }
  else{
    ATH_MSG_FATAL("Could not get " << m_muonCandidateTool);
    return StatusCode::FAILURE;
  }

   // retrive track particle creator tool
  status = m_TrackToTrackParticleConvTool.retrieve();
   if (status.isSuccess()){
    ATH_MSG_INFO("Retrieved " << m_TrackToTrackParticleConvTool);
  }
  else{
    ATH_MSG_FATAL("Could not get " << m_TrackToTrackParticleConvTool);
    return StatusCode::FAILURE;
  }

  StoreGateSvc* detStore;
  if (serviceLocator()->service("DetectorStore", detStore).isSuccess()) {
    const MuonGM::MuonDetectorManager* muonMgr;
    if (detStore->retrieve(  muonMgr ).isFailure()) {
      msg() << MSG::ERROR << " Cannot retrieve MuonGeoModel " << endmsg;
      return StatusCode::FAILURE;
    }
    m_cscIdHelper =  muonMgr->cscIdHelper();
    m_mdtIdHelper =  muonMgr->mdtIdHelper();
    m_rpcIdHelper =  muonMgr->rpcIdHelper();
    m_tgcIdHelper =  muonMgr->tgcIdHelper();
  } else {
    msg() << MSG::ERROR << "DetectorStore not found " << endmsg;
    return StatusCode::FAILURE;
  }


  // register EndEvent incident
  IIncidentSvc* pIncsvc;
  int priority = 100;
  if ( serviceLocator()->service("IncidentSvc", pIncsvc).isSuccess() ) {
    pIncsvc->addListener( this, "EndEvent", priority);
  } else {
    ATH_MSG_ERROR( "Could not register incident service  " );
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_DEBUG("End of init TrigMuonEFStandaloneTrackTool");
  
  //initialise for data handles

  if(!m_rpcKey.initialize()){
    ATH_MSG_ERROR("Couldn't initalize RPC ReadHandleKey");
    return StatusCode::FAILURE;
  }
  if(!m_tgcKey.initialize()){
    ATH_MSG_ERROR("Couldn't initalize TGC ReadHandleKey");
    return StatusCode::FAILURE;
  }
  if(!m_tgcKeyNextBC.initialize()){
    ATH_MSG_ERROR("Couldn't initalize TGCNextBC ReadHandleKey");
    return StatusCode::FAILURE;
  }
  if(!m_mdtKey.initialize()){
    ATH_MSG_ERROR("Couldn't initalize MDT ReadHandleKey");
    return StatusCode::FAILURE;
  }
  if(!m_cscKey.initialize()){
    ATH_MSG_ERROR("Couldn't initalize CSC ReadHandleKey");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode TrigMuonEFStandaloneTrackTool::finalize()
{
  double segmentCacheFraction  = getFraction(m_cachedSegmentCalls,      m_totalSegmentCalls);
  double spectroCacheFraction  = getFraction(m_cachedSpectrometerCalls, m_totalSpectrometerCalls);
  double extrapolCacheFraction = getFraction(m_cachedExtrapolatedCalls, m_totalExtrapolatedCalls);

  ATH_MSG_INFO("Segment making calls           : " << m_totalSegmentCalls << ". Returned from cache: " << m_cachedSegmentCalls 
	       << ". Cached fraction: " << segmentCacheFraction );
  ATH_MSG_INFO("Spectrometer Track making calls: " << m_totalSpectrometerCalls << ". Returned from cache: " << m_cachedSpectrometerCalls 
	       << ". Cached fraction: " << spectroCacheFraction );
  ATH_MSG_INFO("Extrapolated Track making calls: " << m_totalExtrapolatedCalls << ". Returned from cache: " << m_cachedExtrapolatedCalls 
	       << ". Cached fraction: " << extrapolCacheFraction );

#if DEBUG_ROI_VS_FULL 
  m_fileWithHashIds_rpc.close();
  m_fileWithHashIds_tgc.close();
  m_fileWithHashIds_mdt.close();
  m_fileWithHashIds_csc.close();
#endif

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
#if DEBUG_ROI_VS_FULL
void TrigMuonEFStandaloneTrackTool::sanity_check(const std::vector<IdentifierHash>& input_hash_ids, const std::vector<IdentifierHash>& hash_ids_withData, std::ostream& outfile)
{

  std::set<IdentifierHash> inputSet;
  std::set<IdentifierHash> outputSet;
  std::vector<IdentifierHash> inputDuplicates;
  std::vector<IdentifierHash> outputDuplicates;
  std::vector<IdentifierHash> inputUnmatched;
  std::vector<IdentifierHash> outputUnmatched;
   
  //print input and output hash IDs and check for duplicates or unmatched
  outfile << "requested hash IDs:                          ";
  for (auto it = input_hash_ids.begin(); it != input_hash_ids.end(); ++it) {//loop over requested hash IDs
    outfile << *it << " ";
    if (!inputSet.insert(*it).second) //check for duplicate
      inputDuplicates.push_back(*it);
  }
  outfile << "\n";
  outfile << "hash IDs with data:                          ";
  for (auto it = hash_ids_withData.begin(); it != hash_ids_withData.end(); ++it) {//loop over hash IDs with data
    outfile << *it << " ";
    if (!outputSet.insert(*it).second) //check for duplicate
      outputDuplicates.push_back(*it);
    if (inputSet.find(*it) == inputSet.end()) //check for mismatch
      outputUnmatched.push_back(*it);
  }
  outfile << "\n";
  
  for (auto it = input_hash_ids.begin(); it != input_hash_ids.end(); ++it) {//another loop over requested hash IDs
     if (outputSet.find(*it) == outputSet.end()) //check for mismatch
       inputUnmatched.push_back(*it);
  }
  
  
  //print duplicates if any
  if (inputDuplicates.size()) {
     outfile << "requested  hID duplicates:                   ";
     for (auto it = inputDuplicates.begin(); it != inputDuplicates.end(); ++it)
       outfile << *it << " ";
     outfile << "\n";
  }
  if (outputDuplicates.size()) {
     outfile << "hID with data duplicates:                    ";
     for (auto it = outputDuplicates.begin(); it != outputDuplicates.end(); ++it)
       outfile << *it << " ";
     outfile << "\n";
  }
  
  //print unmatched IDs if any
  if (inputUnmatched.size()) {
     outfile << "requested  hID unrecovered with data:        ";
     for (auto it = inputUnmatched.begin(); it != inputUnmatched.end(); ++it)
       outfile << *it << " ";
     outfile << "\n";
  }
  if (outputUnmatched.size()) {
     outfile << "recovered but unrequested hID with data:     ";
     for (auto it = outputUnmatched.begin(); it != outputUnmatched.end(); ++it)
       outfile << *it << " ";
     outfile << "\n";
  }

}
#endif

//________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::unpackTimers( std::vector<TrigTimer*>& timers, unsigned int firstIndex,
						  TrigTimer*& dataPrepTime, TrigTimer*& algTime, TrigTimer*& dataOutputTime ) 
{
  dataPrepTime = 0;
  algTime = 0;
  dataOutputTime = 0;
  unsigned int index = firstIndex;
  if ( timers.size() > index ) {
    dataPrepTime = timers[index];
    ++index;
    if ( timers.size() > index ) {
      algTime  = timers[index];
      ++index;
      if ( timers.size() > index ) {
	dataOutputTime = timers[index];
      }
    }
  }
}


//________________________________________________________________________
HLT::ErrorCode
TrigMuonEFStandaloneTrackTool::getSpectrometerTracks(const IRoiDescriptor* muonRoI,
						     SegmentCache*& cache,
						     TrigMuonEFMonVars& monVars,
						     std::vector<TrigTimer*>& timers )
{
  //
  // Segment finding
  //
  ATH_MSG_DEBUG("In getSpectrometerTracks, about to call findSegments");
  HLT::ErrorCode hltStatus = findSegments(muonRoI, cache, monVars.segs, timers, 0);
  if (hltStatus!=HLT::OK) {
    return hltStatus;
  }    	
  
  
  /// Making Object to cache pointers for segfinder: SC holds Pointers that are all deleted at end of event
  if( not m_segments and not m_segmentCombiColl ) {
    ATH_MSG_DEBUG( "No Muon Segments built. Check configuration! Stop here to process the RoI" );
    return HLT::MISSING_FEATURE;
  }
  //
  // build tracks
  //
  return buildTracks(m_segmentCombiColl, m_segments, cache, monVars.MS, timers, 3);
}


//________________________________________________________________________
HLT::ErrorCode TrigMuonEFStandaloneTrackTool::getExtrapolatedTracks(const IRoiDescriptor* muonRoI,
								    MuonCandidateCollection& candidateCollection,
								    TrackCollection& extrapolatedTracks,
								    SegmentCache*& cache,
								    TrigMuonEFMonVars& monVars,
								    std::vector<TrigTimer*>& timers) 
{

  ATH_MSG_DEBUG("In getExtrapolatedTracks with cache");
  HLT::ErrorCode hltStatus = getSpectrometerTracks( muonRoI, cache, monVars, timers );
  //  if fail to get trks from L2 hits, retry with all hits
  if(m_useL2Hits && (hltStatus==HLT::MISSING_FEATURE || !m_spectrometerTracks || m_spectrometerTracks->empty()) ){
    m_useL2Hits=false;
    hltStatus = getSpectrometerTracks( muonRoI, cache, monVars, timers );
    m_useL2Hits=true;
  }
  if (hltStatus!=HLT::OK && hltStatus!=HLT::MISSING_FEATURE) {
    ATH_MSG_DEBUG( "Failed to build spectrometerTracks" );
    return hltStatus;
  }
  
  if (hltStatus==HLT::MISSING_FEATURE || !m_spectrometerTracks || m_spectrometerTracks->empty()) {
    ATH_MSG_DEBUG( "No Muon Spectrometer track found: stop here to process the RoI." );
    return HLT::MISSING_FEATURE;
  } else {
    ATH_MSG_DEBUG( "Muon Spectrometer Track container retrieved with size : " << m_spectrometerTracks->size() );
  }
       
  //
  // extrapolation
  //
  hltStatus = extrapolate(m_spectrometerTrackParticles, candidateCollection, extrapolatedTracks, cache, monVars.SA, timers, 6);
  if (hltStatus!=HLT::OK && hltStatus !=HLT::MISSING_FEATURE) {
    ATH_MSG_DEBUG( "Failed to extrapolate spectrometer tracks to IP" );
    return hltStatus;
  }

  return hltStatus;
}
//_______________________________________________________________________
void TrigMuonEFStandaloneTrackTool::clearRoiCache(){

  m_MdtRobList.clear();
  m_RpcRobList.clear();
  m_TgcRobList.clear();
  m_CscRobList.clear();

  m_HasMdtRobList = false;
  m_HasRpcRobList = false;
  m_HasTgcRobList = false;
  m_HasCscRobList = false;
}
//_______________________________________________________________________
const std::vector<uint32_t>& TrigMuonEFStandaloneTrackTool::getMdtRobList( const IRoiDescriptor* muonRoI){
  if(!m_HasMdtRobList){
    m_HasMdtRobList = true; 
    if(muonRoI){
      m_regionSelector->DetROBIDListUint(MDT,*muonRoI, m_MdtRobList);
    }
    else {
      m_regionSelector->DetROBIDListUint(MDT, m_MdtRobList);
    }
  }
  return m_MdtRobList;
}
//________________________________________________________________________
const std::vector<uint32_t>& TrigMuonEFStandaloneTrackTool::getRpcRobList( const IRoiDescriptor* muonRoI){
  if(!m_HasRpcRobList){
    m_HasRpcRobList = true;
    if(muonRoI){
      m_regionSelector->DetROBIDListUint(RPC,*muonRoI,m_RpcRobList);
    }
    else{
      m_regionSelector->DetROBIDListUint(RPC,m_RpcRobList);
    }
  }
  return m_RpcRobList;
}
//________________________________________________________________________
const std::vector<uint32_t>& TrigMuonEFStandaloneTrackTool::getTgcRobList( const IRoiDescriptor* muonRoI){
  if(!m_HasTgcRobList){
    m_HasTgcRobList = true;
    if(muonRoI){
      m_regionSelector->DetROBIDListUint(TGC,*muonRoI,m_TgcRobList);
    }
    else{
      m_regionSelector->DetROBIDListUint(TGC,m_TgcRobList);
    }
  }
  return m_TgcRobList;
}
//________________________________________________________________________
const std::vector<uint32_t>& TrigMuonEFStandaloneTrackTool::getCscRobList( const IRoiDescriptor* muonRoI){
  if(!m_HasCscRobList){
    m_HasCscRobList = true;
    if(muonRoI){
      m_regionSelector->DetROBIDListUint(CSC,*muonRoI,m_CscRobList);
    }
    else{
      m_regionSelector->DetROBIDListUint(CSC,m_CscRobList);
    }
  }
  return m_CscRobList;
}
//________________________________________________________________________
HLT::ErrorCode TrigMuonEFStandaloneTrackTool::findSegments(const IRoiDescriptor* muonRoI,
							   SegmentCache*& cache,
							   TrigMuonEFSegmentMonVars& monVars,
							   std::vector<TrigTimer*>& timers, 
							   unsigned int firstTimerIndex )
{
#if DEBUG_ROI_VS_FULL
  m_fileWithHashIds_rpc << "\n#####\n\n";
  m_fileWithHashIds_mdt << "\n#####\n\n";
  m_fileWithHashIds_tgc << "\n#####\n\n";
  m_fileWithHashIds_csc << "\n#####\n\n";
#endif

  ATH_MSG_DEBUG("in findSegments()");
 
  TrigTimer* dataPrepTime   = 0;
  TrigTimer* segFinderTime  = 0;
  TrigTimer* dataOutputTime = 0;
  unpackTimers( timers, firstTimerIndex, dataPrepTime, segFinderTime, dataOutputTime);

  if (dataPrepTime) dataPrepTime->start();
  const xAOD::L2StandAloneMuonContainer* l2cont=0;
  const xAOD::L2StandAloneMuon *l2muon = 0;
  std::string l2muonKey = "HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo";
  if(m_useL2Hits){
    if((*p_ActiveStore)->retrieve(l2cont, l2muonKey).isFailure()) {
      msg() << MSG::DEBUG << " Cannot retrieve L2 Muon Container " << l2muonKey <<" stop processing here"<< endmsg;
    } else msg()<< MSG::DEBUG << " L2 Muon Container retrieved with key " << l2muonKey << endmsg;
  }
  // reset cached pointers
  m_segmentCombiColl=0;
  m_patternCombiColl=0;
  m_segments= 0;
  m_segmentCombiCollInternal=0;
  m_patternCombiCollInternal=0;
  m_segmentsInternal= 0;
  m_spectrometerTracks = 0;
  m_extrapolatedTracks = 0;

  ///// Vectors of prep data collections
  std::vector<const Muon::MdtPrepDataCollection*> mdtCols;
  std::vector<const Muon::MdtPrepDataCollection*> mdtCols2;
  std::vector<const Muon::CscPrepDataCollection*> cscCols;
  std::vector<const Muon::TgcPrepDataCollection*> tgcCols;
  std::vector<const Muon::RpcPrepDataCollection*> rpcCols;
  
  ++m_roi_num_seg;
  
  // These parameters are stored in the EDM.
  // For standard chains they are set from the input Roi.
  // However now we have full scan chains where we ask for a dummy TE.
  // For these cases we for now set etaRoI=phiRoI=0


  if (muonRoI) {
    monVars.phiL2CosmicMuon = muonRoI->phi();
    monVars.etaL2CosmicMuon = muonRoI->eta();
  }

  //
  // get PrepRawData
  //
  int nTgcHits = 0;
  int nMdtHits = 0;
  int nRpcHits = 0;
  int nCscHits = 0;
  
  bool mdtDataFound = true;
  bool cscDataFound = true;

  HLT::ErrorCode hltStatus = HLT::OK;

  std::vector<IdentifierHash>  rpc_hash_ids;
  std::vector<IdentifierHash>  rpc_hash_ids_cache;

  if (m_useRpcData) {
    if ( muonRoI ) m_regionSelector->DetHashIDList(RPC, *muonRoI, rpc_hash_ids);
    else           m_regionSelector->DetHashIDList(RPC, rpc_hash_ids);
  }
  
  
  std::vector<IdentifierHash>  tgc_hash_ids;
  std::vector<IdentifierHash>  tgc_hash_ids_cache;
  if (m_useTgcData){ 
    if ( muonRoI ) m_regionSelector->DetHashIDList(TGC, *muonRoI, tgc_hash_ids);
    else           m_regionSelector->DetHashIDList(TGC, tgc_hash_ids);
  }

  
  std::vector<IdentifierHash> temp_mdt_hash_ids;
  std::vector<IdentifierHash> mdt_hash_ids;
   std::vector<IdentifierHash> mdt_hash_ids_cache;
if (m_useMdtData>0) {
  if ( muonRoI ) {
    m_regionSelector->DetHashIDList(MDT, *muonRoI, temp_mdt_hash_ids);   
  }
  else {
    m_regionSelector->DetHashIDList(MDT, temp_mdt_hash_ids);
  }
  
    
    if (m_useMdtData >1) {
      for (std::vector<IdentifierHash>::const_iterator it=temp_mdt_hash_ids.begin();
	   it!= temp_mdt_hash_ids.end(); ++it) {
	bool select = false;
	if (m_useMdtData == 2) {
	  if ( (*it) < 638 || (*it) >1151 ) //barrel only: BIL to BOG + BIM
	    select = true;
	} else if (m_useMdtData == 3) {
	  if ( (*it) > 637 && (*it) <1152) //all endcap chambers
	    select = true;
	}
	if (select) {
	  mdt_hash_ids.push_back((*it));
	}
      }
      if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE
			   << "Size of the Mdt hash list " << temp_mdt_hash_ids.size()
			   << " reset to " << mdt_hash_ids.size() << endmsg;
    } else {

      mdt_hash_ids = temp_mdt_hash_ids;
    }
  }
  
  std::vector<IdentifierHash> csc_hash_ids;
  std::vector<IdentifierHash> csc_hash_ids_cache;
  if (m_useCscData) {
    if ( muonRoI ) m_regionSelector->DetHashIDList(CSC, *muonRoI, csc_hash_ids);
    else           m_regionSelector->DetHashIDList(CSC, csc_hash_ids);
  }
  
  if (msgLvl(MSG::DEBUG)) {
    msg()<< MSG::DEBUG << "The size of RPC hashId list is " << rpc_hash_ids.size() << endmsg;
    msg()<< MSG::DEBUG << "The size of TGC hashId list is " << tgc_hash_ids.size() << endmsg;
    msg()<< MSG::DEBUG << "The size of MDT hashId list is " << mdt_hash_ids.size() << endmsg;
    msg()<< MSG::DEBUG << "The size of CSC hashId list is " << csc_hash_ids.size() << endmsg;
    msg()<< MSG::DEBUG << "The size of mdt:rpc hashId list is " << mdt_hash_ids_cache.size() << " : "<< rpc_hash_ids_cache.size()<< endmsg;
  }
  else
    {
      if (testRoiDrivenMode)
	{
	  msg()<< MSG::INFO << "The size of RPC hashId list is " << rpc_hash_ids.size() << endmsg;
	  msg()<< MSG::INFO << "The size of TGC hashId list is " << tgc_hash_ids.size() << endmsg;
	  msg()<< MSG::INFO << "The size of MDT hashId list is " << mdt_hash_ids.size() << endmsg;
	  msg()<< MSG::INFO << "The size of CSC hashId list is " << csc_hash_ids.size() << endmsg;
	}
    }
  
  
  // perform the data decoding
  
  std::vector<IdentifierHash> hash_ids_withData;
  
  if (m_useRpcData && !rpc_hash_ids.empty()) {// RPC decoding
    if (m_useRpcSeededDecoding) {// seeded decoding of RPC
      if(m_decodeRpcBS) {// bytestream conversion
        if (m_rpcRawDataProvider->convert( getRpcRobList(muonRoI) ).isSuccess()) {
          ATH_MSG_DEBUG("RPC BS conversion for ROB-based seeded PRD decoding done successfully");
        } else {
          ATH_MSG_WARNING("RPC BS conversion for ROB-based seeded PRD decoding failed");
        }
      }
      if (m_rpcPrepDataProvider->decode( getRpcRobList(muonRoI) ).isSuccess()) {
	ATH_MSG_DEBUG("ROB-based seeded PRD decoding of RPC done successfully");
      } else {
	ATH_MSG_WARNING("ROB-based seeded PRD decoding of RPC failed");
      }

    }
    else {// full decoding of RPC

      if (m_rpcRawDataProvider->convert().isSuccess()) {
	ATH_MSG_DEBUG("RPC BS conversion for full decoding done successfully");
      } else {
	ATH_MSG_WARNING("RPC BS conversion for full decoding failed");
      }

      std::vector<IdentifierHash> input_hash_ids;
      input_hash_ids.reserve(0);
      if(m_rpcPrepDataProvider->decode(input_hash_ids, hash_ids_withData).isSuccess()) {
        ATH_MSG_DEBUG("PRD-based full decoding of RPC done successfully");
#if DEBUG_ROI_VS_FULL
        sanity_check(input_hash_ids, hash_ids_withData, m_fileWithHashIds_rpc);
#endif
      } else {
        ATH_MSG_WARNING("PRD-based full decoding of RPC failed");
      }
      
    }
  }// end of RPC decoding
  
  
  if (m_useMdtData && !mdt_hash_ids.empty()) {// MDT decoding
    if (m_useMdtSeededDecoding) {// seeded decoding of MDT
      if(m_decodeMdtBS) {// bytestream conversion
	if (m_mdtRawDataProvider->convert( getMdtRobList(muonRoI) ).isSuccess()) {
	  ATH_MSG_DEBUG("MDT BS conversion for ROB-based seeded PRD decoding done successfully");
	} else {
	  ATH_MSG_WARNING("MDT BS conversion for ROB-based seeded PRD decoding failed");
	}
      }
      if (m_mdtPrepDataProvider->decode( getMdtRobList(muonRoI) ).isSuccess()) {
	ATH_MSG_DEBUG("ROB-based seeded decoding of MDT done successfully");
      } else {
	ATH_MSG_WARNING("ROB-based seeded decoding of MDT failed");
      }
    } else {// full decoding of MDT
      if (m_mdtRawDataProvider->convert().isSuccess()) {
	ATH_MSG_DEBUG("MDT BS conversion for full decoding done successfully");
      } else {
	ATH_MSG_WARNING("MDT BS conversion for full decoding failed");
      }

      std::vector<IdentifierHash> input_hash_ids;
      input_hash_ids.reserve(0);
      if(m_mdtPrepDataProvider->decode(input_hash_ids, hash_ids_withData).isSuccess()) {
        ATH_MSG_DEBUG("PRD-based full decoding of MDT done successfully");
#if DEBUG_ROI_VS_FULL
        sanity_check(input_hash_ids, hash_ids_withData, m_fileWithHashIds_mdt);
#endif
      } else {
        ATH_MSG_WARNING("PRD-based full decoding of MDT failed");
      }
      
    }
  }// end of MDT decoding
  
  
  if (m_useTgcData && !tgc_hash_ids.empty()) {// TGC decoding
    if (m_useTgcSeededDecoding) {// seeded decoding of TGC
      
      if (m_useTgcRobDecoding) {// ROB-based seeded decoding of TGC is neither available nor needed
        ATH_MSG_DEBUG("ROB-based seeded decoding of TGC requested, which is neither available nor needed. Calling the PRD-based seeded decoding.");
      }
      if (m_decodeTgcBS) {// bytesream conversion
	if (m_tgcRawDataProvider->convert( getTgcRobList(muonRoI) ).isSuccess()) {
	  ATH_MSG_DEBUG("TGC BS conversion for ROB-based seeded PRD decoding done successfully");
	} else {
	  ATH_MSG_WARNING("TGC BS conversion for ROB-based seeded PRD decoding failed");
	}
      }
      
      if (m_tgcPrepDataProvider->decode(tgc_hash_ids, hash_ids_withData).isSuccess()) {
        ATH_MSG_DEBUG("PRD-based seeded decoding of TGC done successfully");
#if DEBUG_ROI_VS_FULL
        sanity_check(tgc_hash_ids, hash_ids_withData, m_fileWithHashIds_tgc);
#endif
        tgc_hash_ids.clear();
        tgc_hash_ids_cache.clear();
        tgc_hash_ids = hash_ids_withData;
        ATH_MSG_DEBUG("TgcHashId vector resized to " << tgc_hash_ids.size());
      } else {
        ATH_MSG_WARNING("PRD-based seeded decoding of TGC failed");
      }
      
    }
    else {// full decoding of TGC
      if (m_tgcRawDataProvider->convert().isSuccess()) {
	ATH_MSG_DEBUG("TGC BS conversion for full decoding done successfully");
      } else {
	ATH_MSG_WARNING("TGC BS conversion for full decoding failed");
      }
      
      std::vector<IdentifierHash> input_hash_ids;
      input_hash_ids.reserve(0);
      if(m_tgcPrepDataProvider->decode(input_hash_ids, hash_ids_withData).isSuccess()) {
        ATH_MSG_DEBUG("PRD-based full decoding of TGC done successfully");
#if DEBUG_ROI_VS_FULL
        sanity_check(input_hash_ids, hash_ids_withData, m_fileWithHashIds_tgc);
#endif
      } else {
        ATH_MSG_WARNING("PRD-based full decoding of TGC failed");
      }
      
    }
  }// end of TGC decoding
  
  
  if (m_useCscData && !csc_hash_ids.empty()) {// CSC decoding
    if (m_useCscSeededDecoding) {// seeded decoding of CSC
      
      if (m_useCscRobDecoding) {// ROB-based seeded decoding of CSC is not available
        ATH_MSG_DEBUG("ROB-based seeded decoding of CSC requested, which is not available. Calling the PRD-based seeded decoding.");
      }
      
      // get PRD
      if (m_cscPrepDataProvider->decode(csc_hash_ids, hash_ids_withData).isSuccess()) {
        ATH_MSG_DEBUG("PRD-based seeded decoding of CSC done successfully");
#if DEBUG_ROI_VS_FULL
        sanity_check(csc_hash_ids, hash_ids_withData, m_fileWithHashIds_csc);
#endif
        csc_hash_ids.clear();
        csc_hash_ids_cache.clear();
        csc_hash_ids = hash_ids_withData;
        ATH_MSG_DEBUG("CscHashId vector resized to " << csc_hash_ids.size());
      } else {
        ATH_MSG_WARNING("PRD-based seeded decoding of CSC failed");
      }
      // get clusters out of PRD
      if(csc_hash_ids.size()!=0) { 
	if (m_cscClusterProvider->getClusters(csc_hash_ids, hash_ids_withData).isSuccess()) { 
	  ATH_MSG_DEBUG("CSC clusters obtained successfully"); 
	  csc_hash_ids.clear(); 
	  csc_hash_ids = hash_ids_withData; 
	  ATH_MSG_DEBUG("CscHashId vector resized to " << csc_hash_ids.size()); 
	} else { 
	  ATH_MSG_WARNING("Preparing CSC clusters failed"); 
	} 
      } else {
	ATH_MSG_DEBUG("CscHashId vector is empty - skipping cluster preparation"); 
      }
      
    }
    else {// full decoding of CSC
      
      std::vector<IdentifierHash> input_hash_ids;
      input_hash_ids.reserve(0);
      //get PRD
      if(m_cscPrepDataProvider->decode(input_hash_ids, hash_ids_withData).isSuccess()) {
        ATH_MSG_DEBUG("PRD-based full decoding of CSC done successfully");
#if DEBUG_ROI_VS_FULL
        sanity_check(input_hash_ids, hash_ids_withData, m_fileWithHashIds_tgc);
#endif
      } else {
        ATH_MSG_WARNING("PRD-based full decoding of CSC failed");
      }
      //get clusters out of PRD
      if(m_cscClusterProvider->getClusters(input_hash_ids, hash_ids_withData).isSuccess()) {
        ATH_MSG_DEBUG("CSC clusters obtained successfully");
      } else {
        ATH_MSG_WARNING("Preparing CSC clusters failed");
      }
      
    }
  }// end of CSC decoding
  
  // end of data decoding
  
  // select non-empty PRD collections for segment finding
  
  // Get RPC container
  if (m_useRpcData && !rpc_hash_ids.empty()) {
    const RpcPrepDataContainer* rpcPrds = 0;
    SG::ReadHandle<Muon::RpcPrepDataContainer> RpcCont(m_rpcKey);
    if( !RpcCont.isValid() ) {
      msg() << MSG::ERROR << " Cannot retrieve RPC PRD Container" << endmsg;
      return HLT::NAV_ERROR;
    }
    else{ 
      rpcPrds=RpcCont.cptr();
      msg()<< MSG::DEBUG << " RPC PRD Container retrieved" << endmsg;
    }
    // Get RPC collections
    RpcPrepDataContainer::const_iterator RPCcoll;
    for(std::vector<IdentifierHash>::const_iterator idit = rpc_hash_ids.begin(); idit != rpc_hash_ids.end(); ++idit) {
      RPCcoll = rpcPrds->indexFind(*idit);
      if( RPCcoll == rpcPrds->end() ) {
        if (msgLvl(MSG::VERBOSE)) {
	       Identifier idColl;
          IdContext rpcContext = m_rpcIdHelper->module_context();
          int  code = m_rpcIdHelper->get_id(*idit, idColl, &rpcContext);
          msg() << MSG::VERBOSE << "get_id code = " << code
                << " collection for rpc id hash = " << (int)*idit
                << " not found in the cont. ext.id = " << m_rpcIdHelper->show_to_string(idColl) << endmsg;
        }
        continue;
      }
      if( (*RPCcoll)->size() == 0)    {
        if (msgLvl(MSG::VERBOSE)) {
          Identifier idColl;
          IdContext rpcContext = m_rpcIdHelper->module_context();
          int  code = m_rpcIdHelper->get_id(*idit, idColl, &rpcContext);
          msg() << MSG::VERBOSE << "get_id code = " << code
                << " collection for rpc id hash = " << (int)*idit
                << " is empty ext.id = " << m_rpcIdHelper->show_to_string(idColl) << endmsg;
        }
        continue;
      }
      
      rpc_hash_ids_cache.push_back(*idit);
      
      nRpcHits+=(*RPCcoll)->size(); // count hits for TrigMuonEFInfo
      rpcCols.push_back(*RPCcoll);
      if (msgLvl(MSG::DEBUG)) 
        msg() << MSG::DEBUG << "Selected Rpc Collection: "
              << m_rpcIdHelper->show_to_string((*RPCcoll)->identify())
              << " (hash = " << (int)*idit
              << ") with size " << (*RPCcoll)->size() << endmsg;
      else if (testRoiDrivenMode) 
        msg() << MSG::INFO << "Selected Rpc Collection: "
				  << m_rpcIdHelper->show_to_string((*RPCcoll)->identify())
              << " (hash = " << (int)*idit
				  << "), with size " << (*RPCcoll)->size() << endmsg;

    }
    if (rpcCols.empty()) {
      if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "No Rpc data collections selected" << endmsg;
    }
  }
  
  // Get MDT container
  if (m_useMdtData && !mdt_hash_ids.empty()) {
    
    const MdtPrepDataContainer* mdtPrds = 0;
    SG::ReadHandle<Muon::MdtPrepDataContainer> MdtCont(m_mdtKey);
    if( !MdtCont.isValid() ) {
      msg() << MSG::ERROR << " Cannot retrieve MDT PRD Container" << endmsg;
      return HLT::NAV_ERROR;
    }
    else{ 
      mdtPrds=MdtCont.cptr();
      msg()<< MSG::DEBUG << " MDT PRD Container retrieved" << endmsg;
    }
    
    // Get MDT collections
    MdtPrepDataContainer::const_iterator MDTcoll;
    for(std::vector<IdentifierHash>::const_iterator idit = mdt_hash_ids.begin();
	idit != mdt_hash_ids.end(); ++idit) {
      MDTcoll = mdtPrds->indexFind(*idit);
      if( MDTcoll == mdtPrds->end() ) {
	if (msgLvl(MSG::VERBOSE)) {
	  Identifier idColl;
	  IdContext mdtContext = m_mdtIdHelper->module_context();
	  int  code = m_mdtIdHelper->get_id(*idit, idColl, &mdtContext);
	  msg() << MSG::VERBOSE << "get_id code = " << code
		<< " collection for mdt id hash = " << (int)*idit
		<< " not found in the cont. ext.id = " << m_mdtIdHelper->show_to_string(idColl) << endmsg;
	}
	continue;
      }
      if( (*MDTcoll)->size() == 0)    {
	if (msgLvl(MSG::VERBOSE)) {
	  Identifier idColl;
	  IdContext mdtContext = m_mdtIdHelper->module_context();
	  int  code = m_mdtIdHelper->get_id(*idit, idColl, &mdtContext);
	  msg() << MSG::VERBOSE << "get_id code = " << code
		<< " collection for mdt id hash = " << (int)*idit
		<< " is empty ext.id = " << m_mdtIdHelper->show_to_string(idColl) << endmsg;
	}
	continue;
      }

      if(m_useL2Hits && l2cont){
	Muon::MdtPrepDataCollection *mdtcollection=new Muon::MdtPrepDataCollection();
	addElement( m_mdtcollCache, mdtcollection );

      	Muon::MdtPrepDataCollection::const_iterator cit_begin = (*MDTcoll)->begin();
      	Muon::MdtPrepDataCollection::const_iterator cit_end = (*MDTcoll)->end();
      	Muon::MdtPrepDataCollection::const_iterator cit = cit_begin;   
      	for( ; cit!=cit_end;++cit ) // first
      	  {

      	    const Muon::MdtPrepData* mdt = *cit;
      	    int TubeLayers = mdt->detectorElement()->getNLayers();
      	    int TubeLayer = m_mdtIdHelper->tubeLayer(mdt->identify());
      	    if(TubeLayer > TubeLayers) TubeLayer -= TubeLayers;
      	    const Amg::Vector3D mdtpos = mdt->detectorElement()->center(TubeLayer,m_mdtIdHelper->tube(mdt->identify()));
      	    for(uint l2=0; l2<l2cont->size(); l2++){
      	      l2muon = l2cont->at(l2);
      	      if(l2muon->nMdtHits()==0){
      		mdtcollection->push_back(new Muon::MdtPrepData(*mdt));
      	      }
      	      for(uint hitnum=0; hitnum<l2muon->nMdtHits(); hitnum++){
      		if(fabs(mdtpos.eta())<1.0 ? (fabs(l2muon->mdtHitR(hitnum)-mdtpos.perp())<0.1 && fabs(l2muon->mdtHitZ(hitnum)-mdtpos.z())<0.001): (fabs(l2muon->mdtHitR(hitnum)-fabs(mdtpos.perp()))<200 && fabs(l2muon->mdtHitZ(hitnum)-mdtpos.z())<0.001)){
      		  if(l2muon->mdtHitOfflineId(hitnum)!=0) continue;

      		  mdtcollection->push_back(new Muon::MdtPrepData(*mdt));
		  
      		}
      	      }
	  
	      
      	    }
      	  }

      	mdtcollection->setIdentifier((*MDTcoll)->identify());
      	nMdtHits+=mdtcollection->size(); // count hits for TrigMuonEFInfo
      	mdtCols2.push_back(mdtcollection);      
      }
      else{
	nMdtHits+=(*MDTcoll)->size(); 
      }

      
      mdt_hash_ids_cache.push_back(*idit);
      mdtCols.push_back(*MDTcoll);
      if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "Selected Mdt Collection: "
			 << m_mdtIdHelper->show_to_string((*MDTcoll)->identify())
			 << " with size " << (*MDTcoll)->size() << endmsg;
      else
	if (testRoiDrivenMode) msg() << MSG::INFO << "Selected Mdt Collection: "
				     << m_mdtIdHelper->show_to_string((*MDTcoll)->identify())
				     << " with size " << (*MDTcoll)->size() << endmsg;
    }
    if (mdtCols.empty()) {
      if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "No Mdt data collections selected" << endmsg;
      mdtDataFound = false;
    }
  }
  
  // Get TGC container
  if (m_useTgcData && !tgc_hash_ids.empty()) {
    
    const TgcPrepDataContainer* tgcPrds = 0;
    SG::ReadHandle<Muon::TgcPrepDataContainer> TgcCont(m_tgcKey);
    if( !TgcCont.isValid() ) {
      msg() << MSG::ERROR << " Cannot retrieve TGC PRD Container" << endmsg;
      return HLT::NAV_ERROR;
    }
    else{ 
      tgcPrds=TgcCont.cptr();
      msg()<< MSG::DEBUG << " MDT PRD Container retrieved" << endmsg;
    }

    // Get TGC collections
    TgcPrepDataContainer::const_iterator TGCcoll;
    for(std::vector<IdentifierHash>::const_iterator idit = tgc_hash_ids.begin();
	idit != tgc_hash_ids.end(); ++idit) {
      TGCcoll = tgcPrds->indexFind(*idit);
      if( TGCcoll == tgcPrds->end() ) {
	if (msgLvl(MSG::VERBOSE)) {
	  Identifier idColl;
	  IdContext tgcContext = m_tgcIdHelper->module_context();
	  int  code = m_tgcIdHelper->get_id(*idit, idColl, &tgcContext);
	  ATH_MSG_VERBOSE("get_id code = " << code
			  << " collection for tgc id hash = " << (int)*idit
			  << " not found in the cont. ext.id = " << m_tgcIdHelper->show_to_string(idColl));
	}
	continue;
      }
      if( (*TGCcoll)->size() == 0)    {
	if (msgLvl(MSG::VERBOSE)) {
	  Identifier idColl;
	  IdContext tgcContext = m_tgcIdHelper->module_context();
	  int  code = m_tgcIdHelper->get_id(*idit, idColl, &tgcContext);
	  msg() << MSG::VERBOSE << "get_id code = " << code
		<< " collection for tgc id hash = " << (int)*idit
		<< " is empty ext.id = " << m_tgcIdHelper->show_to_string(idColl) << endmsg;
	}
	continue;
      }
      
      tgc_hash_ids_cache.push_back(*idit);
      nTgcHits+=(*TGCcoll)->size(); // count hits for TrigMuonEFInfo
      tgcCols.push_back(*TGCcoll);
      if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "Selected Tgc Collection: "
			 << m_tgcIdHelper->show_to_string((*TGCcoll)->identify())
			 << " with size " << (*TGCcoll)->size() << endmsg;
      else
	if (testRoiDrivenMode) msg() << MSG::INFO <<  "Selected Tgc Collection: "
				     << m_tgcIdHelper->show_to_string((*TGCcoll)->identify())
				     << " with size " << (*TGCcoll)->size() << endmsg;
    }
    
    if(m_useTGCInPriorNextBC){
      const TgcPrepDataContainer* tgcPrdsPriorBC = 0;
      
      for(std::vector<IdentifierHash>::const_iterator idit = tgc_hash_ids.begin();
	  idit != tgc_hash_ids.end(); ++idit) {
	TGCcoll = tgcPrdsPriorBC->indexFind(*idit);
	if( TGCcoll == tgcPrdsPriorBC->end() ) {
	  if (msgLvl(MSG::VERBOSE)) {
	    Identifier idColl;
	    IdContext tgcContext = m_tgcIdHelper->module_context();
	    int  code = m_tgcIdHelper->get_id(*idit, idColl, &tgcContext);
	    msg() << MSG::VERBOSE << "get_id code = " << code
		  << " collection for tgc id hash = " << (int)*idit
		  << " not found in the cont. ext.id = " << m_tgcIdHelper->show_to_string(idColl) << endmsg;
	  }
	  continue;
	}
	if( (*TGCcoll)->size() == 0)    {
	  if (msgLvl(MSG::VERBOSE)) {
	    Identifier idColl;
	    IdContext tgcContext = m_tgcIdHelper->module_context();
	    int  code = m_tgcIdHelper->get_id(*idit, idColl, &tgcContext);
	    msg() << MSG::VERBOSE << "get_id code = " << code
		  << " collection for tgc id hash = " << (int)*idit
		  << " is empty ext.id = " << m_tgcIdHelper->show_to_string(idColl) << endmsg;
	  }
	  continue;
	}
	nTgcHits+=(*TGCcoll)->size(); // count hits for TrigMuonEFInfo
	tgcCols.push_back(*TGCcoll);
	if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "Selected Tgc Collection: "
			   << m_tgcIdHelper->show_to_string((*TGCcoll)->identify())
			   << " with size " << (*TGCcoll)->size() << endmsg;
      }
      const TgcPrepDataContainer* tgcPrdsNextBC = 0;
      SG::ReadHandle<Muon::TgcPrepDataContainer> TgcCont(m_tgcKeyNextBC);
      if( !TgcCont.isValid() ) {
	msg() << MSG::ERROR << " Cannot retrieve TGC PRD Container" << endmsg;
	return HLT::NAV_ERROR;
      }
      else{ 
	tgcPrds=TgcCont.cptr();
	msg()<< MSG::DEBUG << " MDT PRD Container retrieved" << endmsg;
      }

      for(std::vector<IdentifierHash>::const_iterator idit = tgc_hash_ids.begin();
	  idit != tgc_hash_ids.end(); ++idit) {
	TGCcoll = tgcPrdsNextBC->indexFind(*idit);
	if( TGCcoll == tgcPrdsNextBC->end() ) {
	  if (msgLvl(MSG::VERBOSE)) {
	    Identifier idColl;
	    IdContext tgcContext = m_tgcIdHelper->module_context();
	    int  code = m_tgcIdHelper->get_id(*idit, idColl, &tgcContext);
	    msg() << MSG::VERBOSE << "get_id code = " << code
		  << " collection for tgc id hash = " << (int)*idit
		  << " not found in the cont. ext.id = " << m_tgcIdHelper->show_to_string(idColl) << endmsg;
	  }
	  continue;
				}
	if( (*TGCcoll)->size() == 0)    {
	  if (msgLvl(MSG::VERBOSE)) {
	    Identifier idColl;
	    IdContext tgcContext = m_tgcIdHelper->module_context();
	    int  code = m_tgcIdHelper->get_id(*idit, idColl, &tgcContext);
	    msg() << MSG::VERBOSE << "get_id code = " << code
		  << " collection for tgc id hash = " << (int)*idit
		  << " is empty ext.id = " << m_tgcIdHelper->show_to_string(idColl) << endmsg;
	  }
	  continue;
	}
	nTgcHits+=(*TGCcoll)->size(); // count hits for TrigMuonEFInfo
	tgcCols.push_back(*TGCcoll);
	if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "Selected Tgc Collection: "
			   << m_tgcIdHelper->show_to_string((*TGCcoll)->identify())
			   << " with size " << (*TGCcoll)->size() << endmsg;
      }
    }
    
    if (tgcCols.empty()) {
      if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "No Tgc data collections selected" << endmsg;
    }
  }
  
  
  // Get CSC container
  if (m_useCscData && !csc_hash_ids.empty()) {
    
    const CscPrepDataContainer* cscPrds = 0;
    SG::ReadHandle<Muon::CscPrepDataContainer> CscCont(m_cscKey);
    if( !CscCont.isValid() ) {
      msg() << MSG::ERROR << " Cannot retrieve CSC PRD Container" << endmsg;
      return HLT::NAV_ERROR;
    }
    else{ 
      cscPrds=CscCont.cptr();
      msg()<< MSG::DEBUG << " CSC PRD Container retrieved" << endmsg;
    }

    // Get CSC collections
    CscPrepDataContainer::const_iterator CSCcoll;
    for(std::vector<IdentifierHash>::const_iterator idit = csc_hash_ids.begin();
	idit != csc_hash_ids.end(); ++idit) {
      if(m_ignoreCSC && (*idit==16 || *idit==17)){
	ATH_MSG_DEBUG("Skipping misaligned chamber with hashid "<<*idit);
	continue;
      }
      CSCcoll = cscPrds->indexFind(*idit);
      if( CSCcoll == cscPrds->end() ) continue;
      if( (*CSCcoll)->size() == 0)    continue;
      
      csc_hash_ids_cache.push_back(*idit);
      nCscHits+=(*CSCcoll)->size(); // count hits for TrigMuonEFInfo
      cscCols.push_back(*CSCcoll);
      if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "Selected Csc Collection: "
			 << m_cscIdHelper->show_to_string((*CSCcoll)->identify())
			 << " with size " << (*CSCcoll)->size() << endmsg;
      else
	if (testRoiDrivenMode) msg() << MSG::INFO << "Selected Csc Collection: "
				     << m_cscIdHelper->show_to_string((*CSCcoll)->identify())
				     << " with size " << (*CSCcoll)->size() << endmsg;
    }
    if (cscCols.empty()) {
      if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "No Csc data collections selected" << endmsg;
      cscDataFound = false;
    }
  }
  
  
  if ( !(cscDataFound || mdtDataFound) ) {
    ATH_MSG_DEBUG("No precision data found: stop here to process the RoI");
    if (!muonRoI) return HLT::OK;
    return HLT::MISSING_FEATURE;
  } 
  
  if (dataPrepTime) dataPrepTime->stop();
  
  // check time-out
  if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
    ATH_MSG_WARNING("Timeout reached. Aborting sequence.");
    return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::TIMEOUT);
  }

#if DEBUG_ROI_VS_FULL
  //rpc

  std::vector<IdentifierHash> rpc_hash_list(rpc_hash_ids_cache);
  std::sort(rpc_hash_list.begin(),rpc_hash_list.end());
  
  if(rpc_hash_ids_cache.size() > 0)
    m_fileWithHashIds_rpc << "selected hash IDs passed to pattern finder:  ";
    
  for  (unsigned int i = 0; i<rpc_hash_list.size(); i++)
    m_fileWithHashIds_rpc << rpc_hash_list[i] << " ";
  
  if(rpc_hash_ids_cache.size() > 0)
    m_fileWithHashIds_rpc << "\n";

  //tgc


  std::vector<IdentifierHash> tgc_hash_list(tgc_hash_ids_cache);
  std::sort(tgc_hash_list.begin(),tgc_hash_list.end());
  
  if(tgc_hash_ids_cache.size() > 0)
    m_fileWithHashIds_tgc << "selected hash IDs passed to pattern finder:  ";

  for (unsigned int i = 0; i<tgc_hash_list.size(); i++)
    {
      m_fileWithHashIds_tgc <<tgc_hash_list[i]<<" ";
    }
  
  if(tgc_hash_ids_cache.size() > 0)
    {
      m_fileWithHashIds_tgc <<"\n";
    }

  //mdt 
  
  std::vector<IdentifierHash> mdt_hash_list(mdt_hash_ids_cache);
  std::sort(mdt_hash_list.begin(),mdt_hash_list.end());
  
  if(mdt_hash_ids_cache.size() > 0)
    m_fileWithHashIds_mdt << "selected hash IDs passed to pattern finder:  ";

  for (unsigned int i = 0; i<mdt_hash_list.size(); i++)
    {
      m_fileWithHashIds_mdt <<mdt_hash_list[i]<<" ";
    }
  
  if(mdt_hash_ids_cache.size() > 0)
    {
      m_fileWithHashIds_mdt <<"\n";
    }

  //csc

  std::vector<IdentifierHash> csc_hash_list(csc_hash_ids_cache);
  std::sort(csc_hash_list.begin(),csc_hash_list.end());
  
  if(csc_hash_ids_cache.size() > 0)
    m_fileWithHashIds_csc << "selected hash IDs passed to pattern finder:  ";

 for (unsigned int i = 0; i<csc_hash_list.size(); i++)
   {
      m_fileWithHashIds_csc <<csc_hash_list[i]<<" ";
    }

 if(csc_hash_ids_cache.size() > 0)
   {
     m_fileWithHashIds_csc <<"\n";
   }
#endif

  //Filling Vector of Detector Elements For Comparison of Repeated ROIS
  m_hashlist.clear();
  m_hashlist.push_back(rpc_hash_ids_cache);
  m_hashlist.push_back(tgc_hash_ids_cache);
  m_hashlist.push_back(mdt_hash_ids_cache);
  m_hashlist.push_back(csc_hash_ids_cache);

  ++m_totalSegmentCalls;
  
  //Creating pointers to TE & SC for RepROI Information used for caching
  ATH_MSG_DEBUG("finding hashlist of size "<<m_hashlist.size()<<" in segment cache of size: "<<m_SFCacheSCMap.size());
  map<std::vector<std::vector<IdentifierHash> >, SegmentCache*>::iterator itSCmap = m_SFCacheSCMap.end();
     
  // Making Object to cache pointers for segfinder: SC holds 
  // pointers that are all deleted at end of event
  cache = 0;
  if (m_doCache) itSCmap = m_SFCacheSCMap.find(m_hashlist);

  if (itSCmap!=m_SFCacheSCMap.end()) {
    ATH_MSG_DEBUG("Repeated ROI");
    ATH_MSG_DEBUG("retrieved segments from cache (roi_num="<<m_roi_num_seg<<"). hashlist size: "<<m_hashlist.size()<<", cache size: "<<m_SFCacheSCMap.size());
    if (segFinderTime) segFinderTime->start();

    cache = itSCmap->second;      
    if(cache->SegColl() ) m_segmentCombiCollInternal = cache->SegColl();
    if(cache->PattColl()) m_patternCombiCollInternal = cache->PattColl();
    if(cache->Segments()) m_segmentsInternal = cache->Segments();
    

    if (segFinderTime) segFinderTime->stop();    
    ++m_cachedSegmentCalls;
    monVars.wasCached = 1;
  } 
  else { // itSCmap == m_SFCacheSCMap.end()

    if (segFinderTime) segFinderTime->start();
    
    ///// Call segment finding tool
    // check preconditions to spot conditions which could lead to
    // very long processing times.
    if (m_doTimeOutGuard) {
      if ((m_maxCscHits > 0 && nCscHits >  m_maxCscHits) ||
	  (m_maxTgcHits > 0 && nTgcHits >  m_maxTgcHits) ||
	  (m_maxRpcHits > 0 && nRpcHits >  m_maxRpcHits) ||
	  (m_maxMdtHits > 0 && nMdtHits >  m_maxMdtHits)) {
	if (segFinderTime) segFinderTime->stop();
	ATH_MSG_WARNING("Timeout guard fired. Aborting sequence. Limits (0 means no limit):"
			<< "CSC hit limit: " << m_maxCscHits << ", counted hits: " << nCscHits << ". "
			<< "TGC hit limit: " << m_maxTgcHits << ", counted hits: " << nTgcHits << ". "
			<< "RPC hit limit: " << m_maxRpcHits << ", counted hits: " << nRpcHits << ". "
			<< "MDT hit limit: " << m_maxMdtHits << ", counted hits: " << nMdtHits << ". ");
	return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::TIMEOUT);
      }
    }
    
    Muon::IMooSegmentCombinationFinder::Output* output = 0;
    if(m_useL2Hits){
      if(mdtCols2.size()>0){
    	output = m_segmentsFinderTool->findSegments(mdtCols2, cscCols, tgcCols, rpcCols);
    	if(!output || (output->segmentCombinations)->size()==0){ 
    	  ATH_MSG_DEBUG("didn't find mstrk with l2 hits, use all mdt hits in roi");
    	  output = m_segmentsFinderTool->findSegments(mdtCols, cscCols, tgcCols, rpcCols);
    	}
      }
      else output = m_segmentsFinderTool->findSegments(mdtCols, cscCols, tgcCols, rpcCols);
    }
    else output = m_segmentsFinderTool->findSegments(mdtCols, cscCols, tgcCols, rpcCols);
    if ( !output ) {
      if (segFinderTime) segFinderTime->stop();
      ATH_MSG_DEBUG("Segment finder return nothing: stop here to process the RoI");
      return HLT::MISSING_FEATURE;
    }
    if (!output->segmentCombinations) {
      if (segFinderTime) segFinderTime->stop();
      ATH_MSG_WARNING("Segment finder returned an invalid pointer for the segmentCombinations");
      cleanSegmentCollections();
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN);
    }
  
    
    /// SegmentCache object takes pointers to be given to m_segmentCombiColl, m_patternCombiColl and segments
    ATH_MSG_DEBUG("SegmentCache object taking pointers");
    m_segmentCombiCollInternal=output->segmentCombinations;
    m_patternCombiCollInternal=output->patternCombinations;
    m_segmentsInternal=output->segmentCollection ;
    
    delete output; // right now offline tool will not delete it
    for(auto mdtcoll : mdtCols2) if(mdtcoll) delete mdtcoll;
    
    if (m_doCache) {
      ATH_MSG_DEBUG("Setting up caching");
      cache = new SegmentCache();
      cache->AddSegmentCollection(m_segmentCombiCollInternal);
      cache->AddPatternCollection(m_patternCombiCollInternal);
      cache->AddSegments(m_segmentsInternal);	 
      cache->SetNROI(m_roi_num_seg);//Setting roi number to SC obj for caching info 
      m_SFCacheSCMap[m_hashlist] = cache;
      ATH_MSG_DEBUG("added segments to cache (roi_num="<<m_roi_num_seg<<"). hashlist size: "<<m_hashlist.size()<<". cache size: "<<m_SFCacheSCMap.size());
    } else { // not using cache
      ATH_MSG_DEBUG("Not using cache. hashlist size: "<<m_hashlist.size()<<". cache size: "<<m_SFCacheSCMap.size());
      cache = 0;
      monVars.wasCached = 0;
    }
    
    ATH_MSG_DEBUG("m_segmentCombiColl = "<< m_segmentCombiCollInternal);
    if(m_segmentCombiCollInternal){
      ATH_MSG_DEBUG("m_segmentCombiColl->size() = "<< m_segmentCombiCollInternal->size());
      // Dump segments
      if (msgLvl(MSG::DEBUG)) {
	msg() << MSG::DEBUG << "REGTEST MuonEF Found " << m_segmentCombiCollInternal->size() << " combined segment collection" << endmsg;
	for(unsigned int nSeg=0; nSeg!=m_segmentCombiCollInternal->size(); nSeg++) {
	  if((*(m_segmentCombiCollInternal))[nSeg]){  msg() << MSG::DEBUG << "Combined segment collection " << nSeg << " contains "
			   << ((*(m_segmentCombiCollInternal))[nSeg])->numberOfStations() << " stations" << endmsg;
	    for(unsigned int nStat=0; nStat!=((*(m_segmentCombiCollInternal))[nSeg])->numberOfStations(); nStat++) {
	      if(((*(m_segmentCombiCollInternal))[nSeg])->stationSegments(nStat)) msg() << MSG::DEBUG << "  - segment collection " << nStat << " contains "
	    		      << ((*(m_segmentCombiCollInternal))[nSeg])->stationSegments(nStat)->size() << " segments" << endmsg;
	    }
	  }
	}
      }
    }

    ATH_MSG_DEBUG("Stopping segFinderTime");
    if (segFinderTime) segFinderTime->stop();
    monVars.wasCached = 0;

  }//end else itSCmap == m_SFCacheSCMap.end()

  ATH_MSG_DEBUG("Starting dataOutputTime");
  if (dataOutputTime) dataOutputTime->start();
  if(m_segmentCombiCollInternal) {
    typedef ConstDataVector<MuonSegmentCombinationCollection> constvec_t;
    constvec_t* tmpvec = new constvec_t (m_segmentCombiCollInternal->begin(),
                                         m_segmentCombiCollInternal->end());
    m_segmentCombiColl = tmpvec->asDataVector();
  }
  if(m_patternCombiCollInternal) {
    typedef ConstDataVector<MuonPatternCombinationCollection> constvec_t;
    constvec_t* tmpvec = new constvec_t (m_patternCombiCollInternal->begin(),
                                         m_patternCombiCollInternal->end());
    m_patternCombiColl = tmpvec->asDataVector();
  }
  if(m_segmentsInternal) {
    typedef ConstDataVector<Trk::SegmentCollection> constvec_t;
    constvec_t* tmpvec = new constvec_t (m_segmentsInternal->begin(),
                                         m_segmentsInternal->end());
    m_segments = tmpvec->asDataVector();
   }
 

  // for deletion at end of event
  ATH_MSG_DEBUG("Doing addElement for m_patternCombiColl");
  addElement( m_patternCombisCache, m_patternCombiColl );
  addElement( m_patternCombisCache, m_patternCombiCollInternal );
  ATH_MSG_DEBUG("Doing addElement for m_segmentCombiColl");
  addElement( m_segmentCombisCache, m_segmentCombiColl );
  addElement( m_segmentCombisCache, m_segmentCombiCollInternal );
  ATH_MSG_DEBUG("Doing addElement for m_segments");
  addElement( m_segmentsCache, m_segments );
  addElement( m_segmentsCache, m_segmentsInternal );

  int nSeg = 0;
  if(m_segmentCombiColl) nSeg = segmentMonitoring(m_segmentCombiColl, monVars);
  if(m_segments){
    std::vector<const Muon::MuonSegment*> muonSegCollection;
    for(Trk::SegmentCollection::const_iterator itSeg = m_segments->begin(); itSeg!= m_segments->end(); ++itSeg){
      if(*itSeg) muonSegCollection.push_back(dynamic_cast<const Muon::MuonSegment*>(*itSeg));
    }
    nSeg = segmentMonitoring(muonSegCollection, monVars);
  }

  monVars.numberOfSegs.push_back(nSeg);
  
  if (dataOutputTime) dataOutputTime->stop();
  
  return hltStatus;
}


//________________________________________________________________________
HLT::ErrorCode
TrigMuonEFStandaloneTrackTool::buildTracks(const MuonSegmentCombinationCollection* segments, const Trk::SegmentCollection* segment_collection,
					   SegmentCache* cache,
					   TrigMuonEFMSMonVars& monVars,
					   std::vector<TrigTimer*>& timers, unsigned int firstTimerIndex )
{
  ATH_MSG_DEBUG("in buildTracks");
  
  TrigTimer* dataPrepTime   = 0;
  TrigTimer* trackFinderTime     = 0;
  TrigTimer* dataOutputTime = 0;
  unpackTimers( timers, firstTimerIndex, dataPrepTime, trackFinderTime, dataOutputTime);

  if(dataPrepTime) dataPrepTime->start();

  ATH_MSG_DEBUG("resetting cached pointers");
  // reset cached pointers
  m_spectrometerTracks = 0;
  m_spectrometerTracksInternal = 0;
  m_extrapolatedTracks = 0;

  HLT::ErrorCode hltStatus = HLT::OK;
  
  ++m_roi_num_mstrk;
  
  if(dataPrepTime) dataPrepTime->stop();
  
  ///// Call track finding tool
  ++m_totalSpectrometerCalls;
  if(trackFinderTime) trackFinderTime->start();
  
  if( m_doCache && cache && cache->SpectrometerTrackColl()) {
    ATH_MSG_DEBUG("retrieving tracks from cache");
    // retrieve tracks from cache
    m_spectrometerTracksInternal=cache->SpectrometerTrackColl();
    ++m_cachedSpectrometerCalls;
    monVars.wasCached = 1;
  } // no cached track found
  else{
    // Call track finding tool
    ATH_MSG_DEBUG("no tracks found in cache, calling track finder tool");
    std::vector<const Muon::MuonSegment*> muonSegCollection;
    if ( segment_collection and segment_collection->size() ) {
      ATH_MSG_VERBOSE("Converting Trk::SegmentCollection into std::vector<const Muon::MuonSegment*>");
      for(Trk::SegmentCollection::const_iterator itSeg = segment_collection->begin(); itSeg!= segment_collection->end(); ++itSeg){
        muonSegCollection.push_back(dynamic_cast<const Muon::MuonSegment*>(*itSeg));
      }
    } else if ( segments and segments->size() ) {
      ATH_MSG_VERBOSE("Converting MuonSegmentCombinationCollection into std::vector<const Muon::MuonSegment*>");
      MuonSegmentCombinationCollection::const_iterator itSegComb = segments->begin(), itSegComb_end = segments->end();
      for ( ; itSegComb != itSegComb_end; ++itSegComb ) {
	if(!(*itSegComb)) continue;
        const MuonSegmentCombination* segComb = *itSegComb;
        unsigned int nStations=0;
	if(segComb) nStations = segComb->numberOfStations();
        for ( unsigned int iStation = 0; iStation < nStations; ++iStation ) {
          const std::vector<const MuonSegment*>* segs = segComb->stationSegments(iStation);
          if ( segs ) muonSegCollection.insert(muonSegCollection.end(), segs->begin(), segs->end());
        }
      }
    }

    if ( muonSegCollection.size() ) {
      ATH_MSG_DEBUG("Doing Muon track finding with muon segment collection of size " << muonSegCollection.size());
      m_spectrometerTracksInternal = m_trackBuilderTool->find(muonSegCollection);
      if ( not m_spectrometerTracksInternal ) {
        ATH_MSG_VERBOSE("No tracks found. Making empty tracks collection.");
        m_spectrometerTracksInternal = new TrackCollection;
      }
    } else {
      ATH_MSG_VERBOSE("No segments found. Making empty tracks collection.");
      m_spectrometerTracksInternal = new TrackCollection;
    }

    if ( m_doCache && cache ) {
      cache->SetSpectrometerTrackCollection( m_spectrometerTracksInternal );
    }
    monVars.wasCached = 0;
    
  }//end of track finding

  if(trackFinderTime) trackFinderTime->stop();
  if(dataOutputTime) dataOutputTime->start();
  m_spectrometerTracks = (new ConstDataVector<TrackCollection>(m_spectrometerTracksInternal->begin(), m_spectrometerTracksInternal->end()))->asDataVector();
  // store for deletion at the end of event
  addElement( m_spectrometerTracksCache, m_spectrometerTracks );
  addElement( m_spectrometerTracksCache, m_spectrometerTracksInternal );
    
  // Dump tracks
  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("REGTEST MuonEF Found " << m_spectrometerTracks->size() << " tracks");
  } else if (testRoiDrivenMode) {
    ATH_MSG_INFO("REGTEST MuonEF Found " << m_spectrometerTracks->size() << " tracks");
  }
  
  if(m_spectrometerTracks) monVars.numberOfTracks.push_back(m_spectrometerTracks->size());
  
  // create track particle container, need to be careful with ownership
  if( m_spectrometerTrackParticles ) delete  m_spectrometerTrackParticles;
  if( m_spectrometerTrackParticlesAux ) delete  m_spectrometerTrackParticlesAux;
  m_spectrometerTrackParticles = new xAOD::TrackParticleContainer();
  m_spectrometerTrackParticlesAux = new xAOD::TrackParticleAuxContainer();
  m_spectrometerTrackParticles->setStore( m_spectrometerTrackParticlesAux );

  unsigned int nTrack=0;
  if(m_spectrometerTracks){
    for (TrackCollection::const_iterator itTrk = m_spectrometerTracks->begin(); itTrk!= m_spectrometerTracks->end(); ++itTrk) {


      const Trk::Track* trk = *itTrk;
      // create track particle - this is needed because later on we use the track particle container (m_spectrometerTrackParticles)
      // for the extrapolation of the muons
      const ElementLink<TrackCollection> trackLink(*m_spectrometerTracks, nTrack);
      m_TrackToTrackParticleConvTool->createParticle( trackLink, m_spectrometerTrackParticles, 0, xAOD::muon);


      ATH_MSG_DEBUG("REGTEST MuonEF - track " << nTrack
		    << " has Author " << trk->info().dumpInfo());
      ++nTrack;
      const Trk::Perigee* perigee = trk->perigeeParameters ();
      
      
      const Amg::Vector3D mom = perigee->momentum();
      double z0 = perigee->parameters()[Trk::z0] ;
      double d0 = perigee->parameters()[Trk::d0] ;
      double phi = perigee->parameters()[Trk::phi0] ;
      double theta = perigee->parameters()[Trk::theta] ;
      //      double charge = perigee->charge();
      double pt     = mom.perp();
      double chi2 = -999.;
      double chi2prob = -999.;
      int    ndof = -999;
      if( trk->fitQuality() ) {
	chi2 = trk->fitQuality()->chiSquared() ;
	ndof = trk->fitQuality()->numberDoF() ;
	if ((chi2/((double)ndof)) < 100.)
    	  chi2prob = Genfun::CumulativeChiSquare(ndof)(chi2);
	else
	  chi2prob = 0.;
      }
      double eta =  -log(tan(theta*0.5));
      
      unsigned short int nRpcEtaHits = 0;
      unsigned short int nRpcPhiHits = 0;
      unsigned short int nTgcEtaHits = 0;
      unsigned short int nTgcPhiHits = 0;
      unsigned short int nCscEtaHits = 0;
      unsigned short int nCscPhiHits = 0;
      unsigned short int nMdtHits = 0;
      
      
      // update track summary
      const Trk::TrackSummary* summary = NULL; 
      Trk::Track& nonConstTrack = const_cast<Trk::Track&>(*trk);
      m_trackSummaryTool->updateTrack(nonConstTrack);
      summary = (nonConstTrack.trackSummary());
      if(summary==0) { 
	ATH_MSG_DEBUG("trackSummary not found for this track, cannot get number of subdetector hits.");
      } 
      else {
	nRpcEtaHits = summary->get(Trk::numberOfRpcEtaHits);
	nRpcPhiHits = summary->get(Trk::numberOfRpcPhiHits);
	nTgcEtaHits = summary->get(Trk::numberOfTgcEtaHits);
	nTgcPhiHits = summary->get(Trk::numberOfTgcPhiHits);
	nCscEtaHits = summary->get(Trk::numberOfCscEtaHits);
	nCscPhiHits = summary->get(Trk::numberOfCscPhiHits);
	nMdtHits    = summary->get(Trk::numberOfMdtHits);   
      }
      
      if (msgLvl(MSG::DEBUG)) 
        ATH_MSG_DEBUG(" REGTEST MuonEF  - parameters are:  pt " << pt
		      << " MeV - eta " << eta
		      << " phi " << phi << " z0/d0 " << z0 << " " << d0);
      else
        if (testRoiDrivenMode) msg() << MSG::INFO << " REGTEST MuonEF  - parameters are:  pt " << pt
                                     << " MeV - eta " << eta
                                     << " phi " << phi << " z0/d0 " << z0 << " " << d0 << endmsg;

      monVars.chi2.push_back(chi2/ndof);
      monVars.chi2Prob.push_back(chi2prob);
      monVars.pt.push_back(fabs(pt*(1./GeV)));
      monVars.phi.push_back(phi);
      monVars.eta.push_back(eta);
      monVars.d0.push_back(d0);
      monVars.z0.push_back(z0);
      
      if( summary!=0 ){
	if (nMdtHits > 0){
	  monVars.nMdt.push_back(nMdtHits);
	}
	if(nRpcEtaHits > 0){
	  monVars.nRpcEta.push_back(nRpcEtaHits);
	}
	if(nRpcPhiHits > 0){
	  monVars.nRpcPhi.push_back(nRpcPhiHits);
	}
	if(nCscEtaHits > 0){
	  monVars.nCscEta.push_back(nCscEtaHits);
	}
	if(nCscPhiHits > 0){
	  monVars.nCscPhi.push_back(nCscPhiHits);
	}
	if(nTgcEtaHits > 0){
	  monVars.nTgcEta.push_back(nTgcEtaHits);
	}
	if(nTgcPhiHits > 0){
	  monVars.nTgcPhi.push_back(nTgcPhiHits);
	}
      } else {
	ATH_MSG_DEBUG("trackSummary not found for this track, cannot get number of subdetector hits.");
      }
      
      if(trk->measurementsOnTrack()){
	monVars.nHit.push_back(trk->measurementsOnTrack()->size());
      } else {
	ATH_MSG_DEBUG("measurementsOnTrack not found for this track, cannot get size.");
      }
    }//loop on MS tracks
  }// valid TrackCollection
  else ATH_MSG_DEBUG("m_spectrometerTracks = "<<m_spectrometerTracks);  
  if(dataOutputTime) dataOutputTime->stop();

  return hltStatus;
}


//________________________________________________________________________
HLT::ErrorCode
TrigMuonEFStandaloneTrackTool::extrapolate(const xAOD::TrackParticleContainer* spectrometerTrackParticles,
					   MuonCandidateCollection& candidateCollection,
					   TrackCollection& extrapolatedTracks,
					   SegmentCache* cache,
					   TrigMuonEFSAMonVars& monVars,
					   std::vector<TrigTimer*>& timers, unsigned int firstTimerIndex )
{
  ATH_MSG_DEBUG("in extrapolate");
  

  // reset cached pointers
  m_extrapolatedTracks = 0;

  TrigTimer* dataPrepTime = 0;
  TrigTimer* trackFinderTime = 0;
  TrigTimer* dataOutputTime = 0;
  unpackTimers(timers, firstTimerIndex, dataPrepTime, trackFinderTime, dataOutputTime);

  if(dataPrepTime) dataPrepTime->start();
  ++m_totalExtrapolatedCalls;
  if(dataPrepTime) dataPrepTime->stop();
 
  if (trackFinderTime) trackFinderTime->start();

  bool needToCacheExtrapTrks=false;
  if( m_doCache && cache && cache->ExtrapolatedTrackColl()) {
    // caching will not work currently
    //    ATH_MSG_ERROR("Caching not currently working with new EDM?");
    // retrieve tracks from cache
    m_extrapolatedTracks = cache->ExtrapolatedTrackColl();
    //    ATH_MSG_DEBUG("Caching active, n(trks) = " << m_extrapolatedTracks->size());
    ++m_cachedExtrapolatedCalls;
    monVars.wasCached = 1;
  } // no cached track found
  else{
    ///// Call extrapolator tool
    ATH_MSG_DEBUG("Call MuonCandidateTool");
    m_muonCandidateTool->create( *spectrometerTrackParticles, candidateCollection, extrapolatedTracks );
    needToCacheExtrapTrks=true;
    monVars.wasCached = 0;
  }//end of else

  if (trackFinderTime) trackFinderTime->stop();
  if (dataOutputTime) dataOutputTime->start();
  
  // Dump tracks
  ATH_MSG_DEBUG("REGTEST MuonEF Found " << candidateCollection.size() << " tracks.");

  unsigned int nTrack=0;
  for(auto trk : extrapolatedTracks) {
    
    ATH_MSG_DEBUG("REGTEST MuonEF - extrapolated track has Author " << trk->info().dumpInfo());
    
    ++nTrack;
    
    const Trk::Perigee* perigee = trk->perigeeParameters ();
    double z0 = perigee->parameters()[Trk::z0] ;
    double d0 = perigee->parameters()[Trk::d0] ;
    double phi = perigee->parameters()[Trk::phi0] ;
    double theta = perigee->parameters()[Trk::theta] ;
    //    double charge = perigee->charge();
    
    
    const Amg::Vector3D mom = perigee->momentum();
    double pt     = mom.perp();
    double chi2 = -999.;
    double chi2prob = -999;
    int    ndof = -999;
    
    if( trk->fitQuality() ){
      chi2 = trk->fitQuality()->chiSquared() ;
      ndof = trk->fitQuality()->numberDoF() ;
      if ((chi2/((double)ndof)) < 100.)
	chi2prob = Genfun::CumulativeChiSquare(ndof)(chi2);
      else
	chi2prob = 0.;
    }
    double eta =  -log(tan(theta*0.5));
    
    unsigned short int nRpcEtaHits = 0;
    unsigned short int nRpcPhiHits = 0;
    unsigned short int nTgcEtaHits = 0;
    unsigned short int nTgcPhiHits = 0;
    unsigned short int nCscEtaHits = 0;
    unsigned short int nCscPhiHits = 0;
    unsigned short int nMdtHits = 0;
    
    // update track summary
    const Trk::TrackSummary* summary = NULL;
    m_trackSummaryTool->updateTrack(*trk);
    summary = (trk->trackSummary());
    if(summary==0) {
      ATH_MSG_DEBUG("trackSummary not found for this track, cannot get number of subdetector hits.");
    }
    else {
      nRpcEtaHits = summary->get(Trk::numberOfRpcEtaHits);
      nRpcPhiHits = summary->get(Trk::numberOfRpcPhiHits);
      nTgcEtaHits = summary->get(Trk::numberOfTgcEtaHits);
      nTgcPhiHits = summary->get(Trk::numberOfTgcPhiHits);
      nCscEtaHits = summary->get(Trk::numberOfCscEtaHits);
      nCscPhiHits = summary->get(Trk::numberOfCscPhiHits);
      nMdtHits    = summary->get(Trk::numberOfMdtHits);
      if ((trk->trackSummary()->get(Trk::numberOfMdtHits) ) > 0){
	monVars.nMdt.push_back(nMdtHits);
      }
      if(nRpcEtaHits > 0){
	monVars.nRpcEta.push_back(nRpcEtaHits);
      }
      if(nRpcPhiHits > 0){
	monVars.nRpcPhi.push_back(nRpcPhiHits);
      }
      if(nCscEtaHits > 0){
	monVars.nCscEta.push_back(nCscEtaHits);
      }
      if(nCscPhiHits > 0){
	monVars.nCscPhi.push_back(nCscPhiHits);
      }
      if(nTgcEtaHits > 0){
	monVars.nTgcEta.push_back(nTgcEtaHits);
      }
      if(nTgcPhiHits > 0){
	monVars.nTgcPhi.push_back(nTgcPhiHits);
      }
    }
    
    if(trk->measurementsOnTrack()){
      monVars.nHit.push_back(trk->measurementsOnTrack()->size());
    } else {
      ATH_MSG_DEBUG("measurementsOnTrack not found for this track, cannot get size.");
    }
    
    ATH_MSG_DEBUG(" REGTEST MuonEF  - parameters are:  pt " << pt
		  << " MeV - eta " << eta
		  << " phi " << phi << " z0/d0 " << z0 << " " << d0);
    
    monVars.chi2.push_back(chi2/ndof);
    monVars.chi2Prob.push_back(chi2prob);
    monVars.pt.push_back(fabs(pt*(1./GeV)));
    monVars.phi.push_back(phi);
    monVars.eta.push_back(eta);
    monVars.d0.push_back(d0);
    monVars.z0.push_back(z0);

    ///////////////////////////////////////////// create xAOD muon for this track

    // create MuonCandidate - do deep copy of the extrapolated track because MuonCandidate takes ownership of the pointer
    /*
    ATH_MSG_DEBUG("Create MuonCandidate");
    MuonCombined::MuonCandidate muonCand(**itMSTrk, new Trk::Track(*trk));
  
    // Use muon creator tool
    ATH_MSG_DEBUG("Call MuonCreatorTool->create");
    xAOD::Muon* muon = m_muonCreatorTool->create( muonCand, *muonContainer, extrapolatedTrackParticles, 0);
    ATH_MSG_DEBUG("Created xAOD::Muon with pt, eta, phi, m = " << muon->pt() << ", " << muon->eta() << ", " << muon->phi() << ", " << muon->m() << ", muon conainter size = " << muonContainer->size() << ", TrackParticleCont size = " << extrapolatedTrackParticles->size() << " TrackCollection size = " << m_extrapolatedTracks->size());
    xAOD::TrackParticle* trackparticle = extrapolatedTrackParticles->back();
    ElementLink< TrackCollection > linkToTrkTrack( *m_extrapolatedTracks, nTrack-1);
    trackparticle->setTrackLink( linkToTrkTrack );
    ATH_MSG_DEBUG("Last xAOD::TrackParticle pt, eta, phi, m = " << trackparticle->pt() << ", " << trackparticle->eta() << ", " << trackparticle->phi() << ", " << trackparticle->m());
    ATH_MSG_DEBUG(" EleLink to Trk::Track isValid = " << trackparticle->trackLink().isValid());
    if(trackparticle->trackLink().isValid()) ATH_MSG_DEBUG("   pT, eta from Trk::Track = " << trackparticle->track()->perigeeParameters()->pT() << ", " << trackparticle->track()->perigeeParameters()->eta());

    ATH_MSG_DEBUG("Finished this track, muon candidate extrapolatedTrack() = " << muonCand.extrapolatedTrack());
    */
  }

  if ( m_doCache && cache && needToCacheExtrapTrks) {
    if(m_extrapolatedTracks) cache->SetExtrapolatedTrackCollection( m_extrapolatedTracks );
  }

  // store for deletion at end of event
  if(m_extrapolatedTracks) addElement( m_extrapolatedTracksCache, m_extrapolatedTracks );
  
  monVars.numberOfTracks.push_back(nTrack);

  if(dataOutputTime) dataOutputTime->stop();

  return HLT::OK;
}


//________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::cleanSegmentCollections()
{
  ATH_MSG_DEBUG("cleanSegmentCollections");

  // should not be called passing collections already attached to the TriggerElement
  
  /// Before deleting the collections, we check whether the objects within them are stored in the association map.
  /// If they are then we remove them from the map (see savannah #71203)

  if (m_segmentCombiColl) {
    if ( m_segmentCombiColl->size() > 0 ) {
      const IMuonPatternSegmentAssociationTool::AssociationMap& assMap = m_assocTool->map();
      IMuonPatternSegmentAssociationTool::AssociationMap* nonConstMap = const_cast<IMuonPatternSegmentAssociationTool::AssociationMap*>(&assMap);
      bool printedHeader = false;
      MuonSegmentCombinationCollection::const_iterator itS = m_segmentCombiColl->begin(), itS_end = m_segmentCombiColl->end();
      for ( ; itS != itS_end; ++itS ) {
        const MuonSegmentCombination* combi = *itS;
        // bug hunting printout
        int n = m_assocTool->count( combi );
        if ( n ) {
	  // bug hunting printout
	  if ( !printedHeader ) {
	    ATH_MSG_INFO(__FUNCTION__ << " Event where by-hand clean-up needed:");
	    printedHeader = true;
	  }
	  ATH_MSG_INFO("  About to Delete segmentCombi @ " << combi << ",so first remove it from "
		       << n << " entrie(s) in associationMap");
        }
	
        // hacky FIX for bug #71203 (clean fix would use assocTool->remove(combi), but that does not exist yet)
        if( nonConstMap ) nonConstMap->erase( combi );
      }
      if ( printedHeader ) msg() << endmsg; // prints out run,event,etc.
    }
    
    delete m_segmentCombiColl;
    m_segmentCombiColl=0;
  }
  
  if (m_patternCombiColl) {
    if ( m_patternCombiColl->size() > 0 ) {
      const IMuonPatternSegmentAssociationTool::AssociationMap& assMap = m_assocTool->map();
      IMuonPatternSegmentAssociationTool::AssociationMap* nonConstMap = const_cast<IMuonPatternSegmentAssociationTool::AssociationMap*>(&assMap);
      bool printedHeader = false;
      // check for each patterncombi if it is still in the association map
      MuonPatternCombinationCollection::const_iterator itP = m_patternCombiColl->begin(), itP_end =m_patternCombiColl->end();
      for ( ; itP != itP_end; ++itP ) {
        const MuonPatternCombination* pat = *itP;
        // check if anything is still pointing to this pattern
        IMuonPatternSegmentAssociationTool::AssociationMap::const_iterator itA = assMap.begin(), itA_end = assMap.end();
        for ( ; itA != itA_end; ++itA ) {
          if ( itA->second == pat ) {
            // bug hunting printout
            if ( !printedHeader ) {
              ATH_MSG_INFO(__FUNCTION__ << " Event where by-hand clean-up needed:");
              printedHeader = true;
            }
	    ATH_MSG_INFO("  About to delete patternCombi @ " << pat << " so first remove it from associationMap");
	    
            // hacky FIX for bug #71203 (clean fix would use assocTool->remove( itA->first ), but that does not exist yet)
            if( nonConstMap ) nonConstMap->erase( itA->first );
          }
        }
      }
      if ( printedHeader ) msg() << endmsg; // prints out run,event,etc.
    }
    
    delete m_patternCombiColl;
    m_patternCombiColl=0;
  }
  if (m_segments) 
    delete m_segments;
  m_segments=0;  
}


//________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::recordSegments()
{
  // store the MuonSegmentCombinationCollection in StoreGate for debug purposes
  MuonSegmentCombinationCollection* segmentCombColl = 0;
  std::string segmentCombCollLocation = "TrigMuonEFSegments";
  if( evtStore()->contains<MuonSegmentCombinationCollection>(segmentCombCollLocation) ){
    if (evtStore()->retrieve(segmentCombColl,segmentCombCollLocation).isFailure()){
      msg() << MSG::WARNING << "Could not retrieve MuonSegmentCombinationCollection at "
	    << segmentCombCollLocation << endmsg;
    }
  }
  else{
    ATH_MSG_DEBUG("Recording new segment combination collection with key "
		  << segmentCombCollLocation);
    segmentCombColl = new MuonSegmentCombinationCollection();
    if(evtStore()->record(segmentCombColl,segmentCombCollLocation,true).isFailure()){
      msg() << MSG::WARNING << "Could not record MuonSegmentCombinationCollection at "
	    << segmentCombCollLocation << endmsg;
      delete segmentCombColl;
      segmentCombColl = 0;
    }
  }
  
  if (segmentCombColl && m_segmentCombiColl) {
    for (MuonSegmentCombinationCollection::const_iterator itSegm = m_segmentCombiColl->begin(); itSegm!= m_segmentCombiColl->end(); ++itSegm) {
      const Muon::MuonSegmentCombination* segm  = *itSegm;
      segmentCombColl->push_back( new Muon::MuonSegmentCombination(*segm) );
    }
  }
}

//________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::recordPatterns()
{
  // store the MuonPatternCombinationCollection in StoreGate for debug purposes
  MuonPatternCombinationCollection* patternCombColl = 0;
  std::string patternCombCollLocation = "TrigMuonEFPatterns";
  if( evtStore()->contains<MuonPatternCombinationCollection>(patternCombCollLocation) ){
    if (evtStore()->retrieve(patternCombColl,patternCombCollLocation).isFailure()){
      msg() << MSG::WARNING << "Could not retrieve MuonPatternCombinationCollection at "
	    << patternCombCollLocation << endmsg;
    }
  }
  else{
    ATH_MSG_DEBUG("Recording new pattern combination collection with key "
		  << patternCombCollLocation);
    
    patternCombColl = new MuonPatternCombinationCollection();
    if(evtStore()->record(patternCombColl,patternCombCollLocation,true).isFailure()){
      msg() << MSG::WARNING << "Could not record MuonPatternCombinationCollection at "
	    << patternCombCollLocation << endmsg;
      delete patternCombColl;
      patternCombColl = 0;
    }
  }
  if (patternCombColl && m_patternCombiColl) {
    for (MuonPatternCombinationCollection::const_iterator itPatt = m_patternCombiColl->begin(); itPatt!= m_patternCombiColl->end(); ++itPatt) {
      const MuonPatternCombination* patt  = *itPatt;
      patternCombColl->push_back( new Muon::MuonPatternCombination(*patt) );
    }
  }

}

//________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::recordSpectrometerTracks()
{
  // store the TrackCollection in StoreGate for debug purposes
  TrackCollection* trackCollection = 0;
  std::string trackCollectionLocation = "TrigMuonEFTBTracks";
  if ( evtStore()->contains<TrackCollection>(trackCollectionLocation) ){
    if (evtStore()->retrieve(trackCollection,trackCollectionLocation).isFailure()){
      msg() << MSG::WARNING << "Could not retrieve TrackCollection at " << trackCollectionLocation
	    << endmsg;
    }
  }
  else {
    ATH_MSG_DEBUG("Recording new collection with key " << trackCollectionLocation);
    trackCollection = new TrackCollection();
    if(evtStore()->record(trackCollection,trackCollectionLocation,true).isFailure()){
      ATH_MSG_WARNING("Could not record TrackCollection at " << trackCollectionLocation);
      delete trackCollection;
      trackCollection=0;
    }
  }
  if (trackCollection && m_spectrometerTracks) { // make sure TrackCollection is valid (could be 0 if SG record / retrieve failed)
    for (TrackCollection::const_iterator itTrk = m_spectrometerTracks->begin(); itTrk!= m_spectrometerTracks->end(); ++itTrk) {
      const Trk::Track* trk = *itTrk;
      trackCollection->push_back( new Trk::Track(*trk) );
    }
           
  }//valid trackCollection
  
}

//________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::setSegmentTimers(HLT::Algo* fexAlgo, 
						     std::vector<TrigTimer*>& timers)
{
  ATH_MSG_INFO("Setting TMEF Segment Timers");
  timers.push_back( fexAlgo->addTimer("TMEF_segDataPrepTime") );
  timers.push_back( fexAlgo->addTimer("TMEF_segFinderTime") );
  timers.push_back( fexAlgo->addTimer("TMEF_segDataOutputTime") );
}

//________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::setSpectrometerTimers(HLT::Algo* fexAlgo,
							  std::vector<TrigTimer*>& timers)
{
  ATH_MSG_INFO("Setting TMEF Spectrometer Timers");
  setSegmentTimers(fexAlgo, timers);
  timers.push_back( fexAlgo->addTimer("TMEF_msTrackDataPrepTime") );
  timers.push_back( fexAlgo->addTimer("TMEF_msTrackFinderTime") );
  timers.push_back( fexAlgo->addTimer("TMEF_msTrackDataOutputTime") );
}

//________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::setExtrapolatedTimers(HLT::Algo* fexAlgo,
							  std::vector<TrigTimer*>& timers)
{
  ATH_MSG_INFO("Setting TMEF SA Timers");
  setSpectrometerTimers(fexAlgo, timers);
  timers.push_back( fexAlgo->addTimer("TMEF_saTrackDataPrepTime") );
  timers.push_back( fexAlgo->addTimer("TMEF_saTrackFinderTime") );
  timers.push_back( fexAlgo->addTimer("TMEF_saTrackDataOutputTime") );
}

//________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::declareSegmentMonitoringVariables(HLT::Algo* fexAlgo, 
								      TrigMuonEFMonVars& allMonVars)
{
  ATH_MSG_DEBUG("declaring monitoring variables");
  TrigMuonEFSegmentMonVars& monVars = allMonVars.segs;

  // Segment monitoring variables
  fexAlgo->declareMonitoredVariable     ("EF_segs_wasCached",  monVars.wasCached);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_nstations",  monVars.numberOfStations, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_nsegcomb",   monVars.numberOfSegComb, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_nsegs",      monVars.numberOfSegs, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_phi",        monVars.phi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_eta",        monVars.eta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_chi2pndof",  monVars.chi2pNdof, IMonitoredAlgo::AutoClear);

  fexAlgo->declareMonitoredStdContainer ("EF_segs_nMdt",       monVars.nMdt, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_nCsc",       monVars.nCsc, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_nRpc",       monVars.nRpc, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_nTgc",       monVars.nTgc, IMonitoredAlgo::AutoClear); 

  fexAlgo->declareMonitoredStdContainer ("EF_segs_mdt_radius", monVars.mdtRadius, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_mdt_tdc",    monVars.mdtTDC, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_mdt_adc",    monVars.mdtADC, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_mdt_z",      monVars.mdtZ, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_mdt_perp",   monVars.mdtPerp, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_segs_mdt_eta",    monVars.mdtEta, IMonitoredAlgo::AutoClear);

  fexAlgo->declareMonitoredVariable ("l2_cosmic_phi", monVars.phiL2CosmicMuon);
  fexAlgo->declareMonitoredVariable ("l2_cosmic_eta", monVars.etaL2CosmicMuon);

  fexAlgo->declareMonitoredVariable ("ef_phi", monVars.efPhi);
  fexAlgo->declareMonitoredVariable ("ef_eta", monVars.efEta);

}

//________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::declareSpectrometerMonitoringVariables(HLT::Algo* fexAlgo, 
									   TrigMuonEFMonVars& allMonVars)
{
  // segment monitoring variables
  declareSegmentMonitoringVariables(fexAlgo, allMonVars);

  // MS track monitoring Variables
  TrigMuonEFMSMonVars& monVars = allMonVars.MS;
  fexAlgo->declareMonitoredVariable     ("EF_mstrks_wasCached",  monVars.wasCached);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_n",          monVars.numberOfTracks, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_chi2",       monVars.chi2, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_chi2Prob",   monVars.chi2Prob, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_pt",         monVars.pt, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_phi",        monVars.phi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_eta",        monVars.eta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_d0",         monVars.d0, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_z0",         monVars.z0, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_nMdt",       monVars.nMdt, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_nRpcEta",    monVars.nRpcEta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_nRpcPhi",    monVars.nRpcPhi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_nCscEta",    monVars.nCscEta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_nCscPhi",    monVars.nCscPhi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_nTgcEta",    monVars.nTgcEta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_nTgcPhi",    monVars.nTgcPhi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_mstrks_nHit",       monVars.nHit, IMonitoredAlgo::AutoClear);
}

//__________________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::declareExtrapolatedMonitoringVariables(HLT::Algo* fexAlgo, 
									   TrigMuonEFMonVars& allMonVars)
{
  // declare segment + spectrometer track monitoring variables
  declareSpectrometerMonitoringVariables(fexAlgo, allMonVars);

  // Extrapolated track monitoring Variables
  TrigMuonEFSAMonVars& monVars = allMonVars.SA;
  fexAlgo->declareMonitoredVariable     ("EF_satrks_wasCached",       monVars.wasCached);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_n",               monVars.numberOfTracks, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_chi2",            monVars.chi2, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_pt",              monVars.pt, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_phi",             monVars.phi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_eta",             monVars.eta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_d0",              monVars.d0, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_z0",              monVars.z0, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_nMdt",            monVars.nMdt, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_nRpcEta",         monVars.nRpcEta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_nRpcPhi",         monVars.nRpcPhi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_nCscEta",         monVars.nCscEta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_nCscPhi",         monVars.nCscPhi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_nTgcEta",         monVars.nTgcEta, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_nTgcPhi",         monVars.nTgcPhi, IMonitoredAlgo::AutoClear);
  fexAlgo->declareMonitoredStdContainer ("EF_satrks_nHit",            monVars.nHit, IMonitoredAlgo::AutoClear);
}

//________________________________________________________________________
void TrigMuonEFStandaloneTrackTool::handle(const Incident &inc)
{
  if (inc.type() == "EndEvent"){
    ATH_MSG_DEBUG( "=====> End of Event" );    
    
    // delete the SegmentCache objects we created with new
    for (map<std::vector<std::vector<IdentifierHash> >, SegmentCache*>::iterator itSCmap=m_SFCacheSCMap.begin(); itSCmap!=m_SFCacheSCMap.end(); ++itSCmap) {
      delete itSCmap->second;
    }

    ATH_MSG_DEBUG("clearing SFCacheSCmap");
    m_SFCacheSCMap.clear();
    m_hashlist.clear(); 
    m_assocTool->reset();
    
    m_roi_num_seg=0; 
    m_roi_num_mstrk = 0;
    m_roi_num_satrk = 0;

    clearCacheVector( m_spectrometerTracksCache );
    clearCacheVector( m_extrapolatedTracksCache );
    clearCacheVector( m_patternCombisCache );
    clearCacheVector( m_segmentCombisCache );
    clearCacheVector( m_segmentsCache );
    clearCacheVector(m_mdtcollCache);

  }
}

//________________________________________________________________________
HLT::ErrorCode 
TrigMuonEFStandaloneTrackTool::getSegments(const IRoiDescriptor* muonRoI,
					   TrigMuonEFMonVars& monVars,
					   std::vector<TrigTimer*>& timers )
{
  SegmentCache* cache = 0;
  return findSegments(muonRoI, cache, monVars.segs, timers, 0 );
}

//________________________________________________________________________
HLT::ErrorCode
TrigMuonEFStandaloneTrackTool::getSpectrometerTracks(const IRoiDescriptor* muonRoI,
						     TrigMuonEFMonVars& monVars,
						     std::vector<TrigTimer*>& timers )
{
  SegmentCache* cache = 0;
  return getSpectrometerTracks( muonRoI, cache, monVars, timers );
}

//________________________________________________________________________
HLT::ErrorCode 
TrigMuonEFStandaloneTrackTool::getExtrapolatedTracks(const IRoiDescriptor* muonRoI,
						     MuonCandidateCollection& candidateCollection,
						     TrackCollection& extrapolatedTracks,
						     TrigMuonEFMonVars& monVars,
						     std::vector<TrigTimer*>& timers) 
{
  SegmentCache* cache = 0;
  ATH_MSG_DEBUG("In getExtrapolatedTracks without cache");
  return getExtrapolatedTracks( muonRoI, candidateCollection, extrapolatedTracks, cache, monVars, timers );
}

//________________________________________________________________________
const MuonSegmentCombinationCollection* 
TrigMuonEFStandaloneTrackTool::segmentCombis()
{
  return m_segmentCombiColl;
}

//________________________________________________________________________
const MuonPatternCombinationCollection* 
TrigMuonEFStandaloneTrackTool::patternCombis()
{
  return m_patternCombiColl;
}

//________________________________________________________________________
const Trk::SegmentCollection*
TrigMuonEFStandaloneTrackTool::segments()
{ 
  return m_segments;
}

//________________________________________________________________________
const TrackCollection*
TrigMuonEFStandaloneTrackTool::spectrometerTracks()
{ 
  return m_spectrometerTracks; 
}

//________________________________________________________________________
const TrackCollection*
TrigMuonEFStandaloneTrackTool::extrapolatedTracks()
{ 
  return m_extrapolatedTracks;
}

//________________________________________________________________________
const MuonSegmentCombinationCollection* 
TrigMuonEFStandaloneTrackTool::segmentCombisToAttach()
{ 
  removeElement(m_segmentCombisCache, m_segmentCombiColl);
  return m_segmentCombiColl;
}

//________________________________________________________________________
const MuonPatternCombinationCollection* 
TrigMuonEFStandaloneTrackTool::patternCombisToAttach()
{ 
  removeElement(m_patternCombisCache, m_patternCombiColl);
  return m_patternCombiColl; 
}

//________________________________________________________________________
const Trk::SegmentCollection* 
TrigMuonEFStandaloneTrackTool::segmentsToAttach() 
{
  removeElement(m_segmentsCache, m_segments );
  return m_segments;
}

//________________________________________________________________________
const TrackCollection*
TrigMuonEFStandaloneTrackTool::spectrometerTracksToAttach()
{ 
  removeElement(m_spectrometerTracksCache, m_spectrometerTracks);
  return m_spectrometerTracks; 
}

//________________________________________________________________________
const TrackCollection*
TrigMuonEFStandaloneTrackTool::extrapolatedTracksToAttach()
{ 
  removeElement(m_extrapolatedTracksCache, m_extrapolatedTracks);
  return m_extrapolatedTracks; 
}

//________________________________________________________________________
const xAOD::TrackParticleContainer*
TrigMuonEFStandaloneTrackTool::trackParticleContainerToAttach()
{
  const xAOD::TrackParticleContainer* coll = m_spectrometerTrackParticles;
  m_spectrometerTrackParticles=0;
  return coll;
}

//________________________________________________________________________
const xAOD::TrackParticleAuxContainer*
TrigMuonEFStandaloneTrackTool::trackParticleAuxContainerToAttach()
{
  const xAOD::TrackParticleAuxContainer* coll = m_spectrometerTrackParticlesAux;
  m_spectrometerTrackParticlesAux=0;
  return coll;
}


//________________________________________________________________________
std::vector<std::vector<IdentifierHash> > TrigMuonEFStandaloneTrackTool::getHashList(const IRoiDescriptor* muonRoI){

  std::vector<std::vector<IdentifierHash> > CurrentHashlist;
  CurrentHashlist.clear();



  std::vector<IdentifierHash>  rpc_hash_ids;
  std::vector<IdentifierHash>  rpc_hash_ids_cache;

  if (m_useRpcData) {
    if ( muonRoI ) m_regionSelector->DetHashIDList(RPC, *muonRoI, rpc_hash_ids);
    else           m_regionSelector->DetHashIDList(RPC, rpc_hash_ids);
  }
  
  
  std::vector<IdentifierHash>  tgc_hash_ids;
  std::vector<IdentifierHash>  tgc_hash_ids_cache;
  if (m_useTgcData){ 
    if ( muonRoI ) m_regionSelector->DetHashIDList(TGC, *muonRoI, tgc_hash_ids);
    else           m_regionSelector->DetHashIDList(TGC, tgc_hash_ids);
  }

  
  std::vector<IdentifierHash> temp_mdt_hash_ids;
  std::vector<IdentifierHash> mdt_hash_ids;
  std::vector<IdentifierHash> mdt_hash_ids_cache;
  if (m_useMdtData>0) {
    if ( muonRoI ) {
      m_regionSelector->DetHashIDList(MDT, *muonRoI, temp_mdt_hash_ids);   
    }
    else {
      m_regionSelector->DetHashIDList(MDT, temp_mdt_hash_ids);
    }
  
    if (m_useMdtData >1) {
      for (std::vector<IdentifierHash>::const_iterator it=temp_mdt_hash_ids.begin();
	   it!= temp_mdt_hash_ids.end(); ++it) {
	bool select = false;
	if (m_useMdtData == 2) {
	  if ( (*it) < 638 || (*it) >1151 ) //barrel only: BIL to BOG + BIM
	    select = true;
	} else if (m_useMdtData == 3) {
	  if ( (*it) > 637 && (*it) <1152) //all endcap chambers
	    select = true;
	}
	if (select) {
	  mdt_hash_ids.push_back((*it));
	}
      }
    } else {
      mdt_hash_ids = temp_mdt_hash_ids;
    }
  }
  
  std::vector<IdentifierHash> csc_hash_ids;
  std::vector<IdentifierHash> csc_hash_ids_cache;
  if (m_useCscData) {
    if ( muonRoI ) m_regionSelector->DetHashIDList(CSC, *muonRoI, csc_hash_ids);
    else           m_regionSelector->DetHashIDList(CSC, csc_hash_ids);
  }
  
  
  
  

  CurrentHashlist.push_back(rpc_hash_ids);
  CurrentHashlist.push_back(tgc_hash_ids);
  CurrentHashlist.push_back(mdt_hash_ids);
  CurrentHashlist.push_back(csc_hash_ids);

  return CurrentHashlist;

}

//________________________________________________________________________
int TrigMuonEFStandaloneTrackTool::segmentMonitoring(const MuonSegmentCombinationCollection* segmentCombiColl, TrigMuonEFSegmentMonVars& monVars){
  
  int nSeg = 0;
  if(!segmentCombiColl){
    ATH_MSG_WARNING("MuonSegmentCombinationCollection is empty. Segment monitoring will not be done");
    return nSeg;
  }
  monVars.numberOfSegComb.push_back(segmentCombiColl->size());
  for (MuonSegmentCombinationCollection::const_iterator sc = segmentCombiColl->begin();
       sc != segmentCombiColl->end(); ++sc){
    if(*sc){
      monVars.numberOfStations.push_back((*sc)->numberOfStations());
      for(unsigned int nStat=0; nStat < (*sc)->numberOfStations(); nStat++) {
	const std::vector<const MuonSegment*>* statSegments = (*sc)->stationSegments(nStat);
	nSeg += segmentMonitoring(*statSegments, monVars);
      }
    }
  }
  
  return nSeg;
}

//________________________________________________________________________
int TrigMuonEFStandaloneTrackTool::segmentMonitoring(const std::vector< const MuonSegment*>& segmentVector, TrigMuonEFSegmentMonVars& monVars)
{
  for(std::vector< const MuonSegment* >::const_iterator segment=segmentVector.begin();
      segment!=segmentVector.end(); ++segment){
    monVars.phi.push_back( (*segment)->globalDirection().phi() );
    //float eta =  -std::log(tan(0.5*(*segment)->globalDirection().theta()));
    float eta =  (*segment)->globalDirection().eta();
    monVars.eta.push_back( eta );
    // Not optimal if nSeg>1 ...
    monVars.efPhi=(*segment)->globalDirection().phi();
    monVars.efEta=eta;
    /// get chi2 / ndof
    if((*segment)->fitQuality()) {
      const double chi2 = (*segment)->fitQuality()->chiSquared();
      const double dof = (*segment)->fitQuality()->doubleNumberDoF();
      monVars.chi2pNdof.push_back( ((float) (chi2 / dof)) );
    }//if FitQuality() valid
    int nMdt = 0; int nCsc = 0; int nRpc = 0; int nTgc = 0;
    for(unsigned int irio=0; irio < (*segment)->numberOfContainedROTs(); ++irio) {
      
      Trk::RIO_OnTrack* rio = const_cast<Trk::RIO_OnTrack*> ((*segment)->rioOnTrack(irio));
      if(!rio) continue;
      Identifier rioId = rio->identify();
      if(m_mdtIdHelper->is_mdt(rioId)) {
	nMdt++;
	const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(rio);
	if(mdt!=0){
	  const MdtPrepData* mdtPrd = mdt->prepRawData();
	  monVars.mdtRadius.push_back(mdt->driftRadius());
	  monVars.mdtZ.push_back(mdt->globalPosition().z());
	  monVars.mdtPerp.push_back(mdt->globalPosition().perp());
	  monVars.mdtEta.push_back(mdt->globalPosition().eta());
	  monVars.mdtTDC.push_back(mdtPrd->tdc());
	  monVars.mdtADC.push_back(mdtPrd->adc());
	  
	}  else ATH_MSG_ERROR("cannot cast to MdtDriftCircle ");
      } else if (m_cscIdHelper->is_csc(rioId)) {
	nCsc++;
      } else if (m_rpcIdHelper->is_rpc(rioId)) {
	nRpc++;
      } else if (m_tgcIdHelper->is_tgc(rioId)){
	nTgc++;
      }
    } // end loop on rio
    monVars.nMdt.push_back(nMdt);
    monVars.nCsc.push_back(nCsc);
    monVars.nRpc.push_back(nRpc);
    monVars.nTgc.push_back(nTgc);
  } // end loop on segs

  return segmentVector.size();
}



