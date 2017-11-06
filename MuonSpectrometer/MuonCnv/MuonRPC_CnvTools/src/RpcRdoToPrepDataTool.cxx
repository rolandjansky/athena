/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "RpcRdoToPrepDataTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "RPCcablingInterface/CablingRPCBase.h"
#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"

#include "RPCcablingInterface/RpcPadIdHash.h"
#include "AthenaKernel/IOVSvcDefs.h"


#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"

#include "TrkSurfaces/Surface.h"
//#include "StoreGate/StoreGateSvc.h"
 
#include "MuonCnvToolInterfaces/IDC_Helper.h"

// BS access 
//#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

//Muon cool db access
#include "MuonCondInterface/IRPCConditionsSvc.h"

#include <fstream>
#include <string>

using namespace MuonGM;
using namespace Trk;

/////////////////////////////////////////////////////////////////////////////

Muon::RpcRdoToPrepDataTool::RpcRdoToPrepDataTool( const std::string& type, const std::string& name,
						  const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    //m_factor(0.0),
    //m_rpcOffset(0),
    //m_print_prepData(0),                  //!< if 1 write a summary at the collections found
    m_etaphi_coincidenceTime(0.),         //!< time for phi*eta coincidence 
    m_overlap_timeTolerance(0.),          //!< tolerance of the timing calibration 
    m_processingData(false),              //!< data or MC 
    m_producePRDfromTriggerWords(false),  //!< if 1 store as prd the trigger hits 
    m_solvePhiAmbiguities(true),          //!< toggle on/off the removal of phi ambiguities
    m_doingSecondLoopAmbigColls(false),   //!< true if running a second loop over ambiguous collections in RoI-based mode
    m_reduceCablingOverlap(true),         //!< toggle on/off the overlap removal
    m_timeShift(0.),                      //!< any global time shift ?!
    m_useBStoRdoTool(false),              //!< true if running trigger (EF) on BS input
    m_decodeData(true),                   //!< toggle on/off the decoding of RPC RDO into RpcPrepData
    m_muonMgr(nullptr),
    m_rpcHelper(nullptr),
    m_rpcCabling(nullptr),
    //m_padHashIdHelper(0),
    m_rawDataProviderTool("Muon::RPC_RawDataProviderTool/RPC_RawDataProviderTool"),
    m_rpcRdoDecoderTool("Muon::RpcRDO_Decoder"),
    m_rSummarySvc("RPCCondSummarySvc", name),
    m_fullEventDone(false)
{
  declareInterface<Muon::IMuonRdoToPrepDataTool>(this);

  // declare any properties here
  declareProperty("etaphi_coincidenceTime",    m_etaphi_coincidenceTime     = 20.);//!< 15 ns should be the max.diff. in prop.time in phi and eta strips
  declareProperty("overlap_timeTolerance",     m_overlap_timeTolerance      = 10.);//!<  3 ns is the resolution of the RPC readout electronics
  declareProperty("processingData",            m_processingData             = false);
  declareProperty("solvePhiAmbiguities",       m_solvePhiAmbiguities        = true);
  declareProperty("produceRpcCoinDatafromTriggerWords",m_producePRDfromTriggerWords = true);
  declareProperty("reduceCablingOverlap",      m_reduceCablingOverlap       = true);
  declareProperty("timeShift",                 m_timeShift                  = -12.5); // Zmumu muons are at t=0 in PRD time-domain
  
  declareProperty("useBStoRdoTool",            m_useBStoRdoTool       = false);
  declareProperty("DecodeData",                m_decodeData = true );
  // tools 
  declareProperty ("RawDataProviderTool",      m_rawDataProviderTool);
  declareProperty ("RdoDecoderTool",           m_rpcRdoDecoderTool);
  
  declareProperty("RPCInfoFromDb",             m_RPCInfoFromDb  = false );
  // DataHandle
  declareProperty("OutputCollection",          	m_rdoContainerKey = std::string("RPCPAD"),"RpcPadContainer to retrieve");
  declareProperty("TriggerOutputCollection", 	m_rpcPrepDataContainerKey = std::string("RPC_Measurements"),"Muon::RpcPrepDataContainer to record");
  declareProperty("InputCollection", 		m_rpcCoinDataContainerKey = std::string("RPC_triggerHits"),"Muon::RpcCoinDataContainer to record");
  
}

//___________________________________________________________________________
StatusCode Muon::RpcRdoToPrepDataTool::queryInterface( const InterfaceID& riid, void** ppvIf ) {
  if ( riid == IMuonRdoToPrepDataTool::interfaceID() ) {
    *ppvIf = (IMuonRdoToPrepDataTool*)this;
    addRef();
    return StatusCode::SUCCESS;
  }   
    
  return AlgTool::queryInterface( riid, ppvIf );
}

//___________________________________________________________________________
StatusCode Muon::RpcRdoToPrepDataTool::initialize() {

  // perform necessary one-off initialization

  msg (MSG::INFO) <<"package version = "<<PACKAGE_VERSION<<endmsg;

  msg (MSG::INFO) <<"properties are "<<endmsg;
  msg (MSG::INFO) <<"processingData                     "<<m_processingData <<endmsg;
  msg (MSG::INFO) <<"produceRpcCoinDatafromTriggerWords "<<m_producePRDfromTriggerWords <<endmsg;
  msg (MSG::INFO) <<"reduceCablingOverlap               "<<m_reduceCablingOverlap <<endmsg;
  msg (MSG::INFO) <<"solvePhiAmbiguities                "<<m_solvePhiAmbiguities  <<endmsg;
  msg (MSG::INFO) <<"timeShift                          "<<m_timeShift  <<endmsg;
  if (m_solvePhiAmbiguities && (!m_reduceCablingOverlap)) {
    msg (MSG::WARNING) << "Inconsistent setting of properties (solvePhiAmbiguities entails reduceCablingOverlap)"<<endmsg;
    msg (MSG::WARNING) << "Resetting reduceCablingOverlap to true "<<endmsg;
    m_reduceCablingOverlap  = true;
  }
  msg (MSG::INFO) <<"etaphi_coincidenceTime             "<<m_etaphi_coincidenceTime<<endmsg;
  msg (MSG::INFO) <<"overlap_timeTolerance              "<<m_overlap_timeTolerance <<endmsg;
  msg (MSG::INFO) <<"Correct prd time from cool db      "<<m_RPCInfoFromDb         <<endmsg;
  
  StatusCode sc;
  
  /// get the detector descriptor manager
  sc = detStore()->retrieve( m_muonMgr );
  if (sc.isFailure()) {
    msg (MSG::FATAL) << " Cannot retrieve the MuonDetectorManager " << endmsg;
    return sc;
  }
  else ATH_MSG_VERBOSE(" MuonDetectorManager retrieved");
  
  
  // Get RpcRawDataProviderTool
  if (m_useBStoRdoTool && m_rawDataProviderTool.retrieve().isFailure()) {
    msg (MSG::FATAL) << "Failed to retrieve " << m_rawDataProviderTool << endmsg;
    return StatusCode::FAILURE;
  } else
    msg (MSG::INFO) << "Retrieved Tool " << m_rawDataProviderTool << endmsg;
  
  // Get RpcRdoDecoderTool
  if (m_rpcRdoDecoderTool.retrieve().isFailure()) {
    msg (MSG::FATAL) << "Failed to retrieve " << m_rpcRdoDecoderTool << endmsg;
    return StatusCode::FAILURE;
  } else
    msg (MSG::INFO) << "Retrieved Tool " << m_rpcRdoDecoderTool << endmsg;
  

  /// init rpcidhelper 
  m_rpcHelper = m_muonMgr->rpcIdHelper();
  // get here the pad id helper 
  //m_padHashIdHelper = new RpcPadIdHash();
  
  // get RPC cablingSvc
  const IRPCcablingServerSvc* RpcCabGet = 0;
  sc = service("RPCcablingServerSvc", RpcCabGet);
  if (sc.isFailure()) {
    msg (MSG::FATAL) << "Could not get RPCcablingServerSvc !" << endmsg;
    return StatusCode::FAILURE;
  }
  else ATH_MSG_VERBOSE(" RPCcablingServerSvc retrieved");
  
  sc = RpcCabGet->giveCabling(m_rpcCabling);
  if (sc.isFailure()) {
    msg (MSG::FATAL) << "Could not get RPCcablingSvc from the Server !" << endmsg;
    m_rpcCabling = 0;
    return StatusCode::FAILURE;
  } 
  else {
    //ATH_MSG_VERBOSE(" RPCcablingSvc obtained ");
    msg (MSG::VERBOSE) << "RPCcablingSvc obtained: " << (dynamic_cast<const Service*>(m_rpcCabling))->name() << endmsg;
  }
  
  std::string svcName=m_rpcCabling->rpcCabSvcType();
  
  msg(MSG::INFO)<<"Rpc Cabling Svc name is "<<svcName<<endmsg;
  
  // LBTAG 29/01/10: case RPCcabling or RPCcablingSim
  if (svcName.find("sim")!=std::string::npos) {
    // LBTAG 29/01/10: case RPCcabling
    if (svcName.find("simulationLikeInitialization")!=std::string::npos) m_processingData = true;
    
    // LBTAG 29/01/10: case MuonRPC_Cabling maps from files
    else if (svcName.find("simLike_MapsFromFiles")!=std::string::npos) m_processingData = true;
    
    // LBTAG 29/01/10: case RPCcablingSim
    else m_processingData = false;
  }

  // LBTAG 29/01/10: case MuonRPC_Cabling maps from COOL
  else 
  {
    //Data-Style Cabling Svc 
    m_processingData = true;
  }
  
    
  if (m_RPCInfoFromDb){
     StatusCode status = m_rSummarySvc.retrieve();
     if (status != StatusCode::SUCCESS) {
       ATH_MSG_WARNING (  "Could not retrieve RPC Info from Db"  );
       return StatusCode::FAILURE;
     }
   }

  // check if initializing of DataHandle objects success
  if (m_rdoContainerKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("ReadHandleKey for RpcPadContainer initialize Failure");
    return StatusCode::FAILURE;
  }

  if (m_rpcPrepDataContainerKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("WriteHandleKey for Muon::RpcPrepDataContainer initialize Failure");
    return StatusCode::FAILURE;
  }

  if (m_rpcCoinDataContainerKey.initialize().isFailure() ) {
    ATH_MSG_ERROR("WriteHandleKey for Muon::RpcCoinDataContainer initialize Failure");
    return StatusCode::FAILURE;
  }  
   
  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode Muon::RpcRdoToPrepDataTool::finalize()
{   
  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode Muon::RpcRdoToPrepDataTool::decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect )
{

  int sizeVectorRequested = idVect.size();
  ATH_MSG_DEBUG("Decode method called for "<<sizeVectorRequested<<" offline collections");
  if (sizeVectorRequested == 0) ATH_MSG_DEBUG("Decoding the entire event");
  
  StatusCode status = StatusCode::SUCCESS;
    
  // clear output vector of selected data collections containing data 
  idWithDataVect.clear();
  
  // create an empty vector of hash ids to be decoded (will be filled if RoI-based and left empty if full-scan)
  std::vector<IdentifierHash> idVectToBeDecoded;
  idVectToBeDecoded.reserve(0);
  
  // record the output containers if necessary (if first time in the event)
  bool firstTimeInTheEvent=false;
  status=manageOutputContainers(firstTimeInTheEvent);
  if (status.isFailure()) return status;
  if (firstTimeInTheEvent) {
    if (sizeVectorRequested == 0)
      m_fullEventDone=true;
    else m_fullEventDone=false;
  }
  else {
    if (m_fullEventDone) {
      ATH_MSG_DEBUG("Whole event has already been decoded; nothing to do.");
      return status;
    }
    if (sizeVectorRequested == 0) m_fullEventDone = true;
  }
  
  if (sizeVectorRequested != 0) {
    // the program goes in here only if RoI-based decoding has been called and the full event is not already decoded
    // this code ensures decoding of every offline hash id is called only once
    for (auto itHashId=idVect.begin(); itHashId!=idVect.end(); ++itHashId) {
      if (m_decodedOfflineHashIds.insert(*itHashId).second)
        idVectToBeDecoded.push_back(*itHashId);
    }
    if (idVectToBeDecoded.size()==0) {
      ATH_MSG_DEBUG("All requested offline collections have already been decoded; nothing to do.");
      return status;
    } else {
      ATH_MSG_DEBUG(idVectToBeDecoded.size() << " offline collections have not yet been decoded and will be decoded now.");
      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "The list of offline collection hash ids to be decoded: ";
        for (auto itHashId=idVectToBeDecoded.begin(); itHashId!=idVectToBeDecoded.end(); ++itHashId)
          msg(MSG::VERBOSE) << (int)*itHashId << " ";
        msg(MSG::VERBOSE) << endmsg;
      }
    }
  }
  
  
  // if RPC decoding is switched off stop here
  if( !m_decodeData ) {
    ATH_MSG_DEBUG("Stored empty container. Decoding RPC RDO into RPC PrepRawData is switched off");
    return status;
  }

  ATH_MSG_DEBUG("Decoding RPC RDO into RPC PrepRawData");
  
  // if the vector requested has size 0, we need to perform a scan of the entire RDO container
  // otherwise select the pads to be decoded
  std::vector<IdentifierHash> rdoHashVec;
  if (sizeVectorRequested != 0) {
    ATH_MSG_DEBUG("Looking for pads IdHash to be decoded for the requested collection Ids");
    StatusCode sc = m_rpcCabling->giveRDO_fromPRD(idVectToBeDecoded, rdoHashVec);
    if (StatusCode::SUCCESS != sc) return sc; //!< fix me - define a SW type of error 
  }
  
  /// RPC context
  IdContext rpcContext = m_rpcHelper->module_context();
  
  if(m_useBStoRdoTool) { 
    // we come here if the entire rdo container is not yet in SG (i.e. in EF with BS input) 
    // ask RpcRawDataProviderTool to decode the list of robs and to fill the rdo IDC
    if (sizeVectorRequested == 0) {
      if (m_rawDataProviderTool->convert().isFailure()) {
	msg (MSG::FATAL) << "BS conversion into RDOs failed" << endmsg;
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("BS conversion into RDOs for the entire event done !");
    }
    else {
      if (m_rawDataProviderTool->convert(rdoHashVec).isFailure()) {
	msg (MSG::FATAL) << "BS conversion into RDOs failed" << endmsg;
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("BS conversion into RDOs for the selected collections done !");
    }
  }
  // we come here if the rdo container is already in SG (for example in MC RDO!) => the ContainerManager return NULL
  ATH_MSG_DEBUG("Retrieving Rpc PAD container from the store");      
  auto rdoContainerHandle = SG::makeHandle(m_rdoContainerKey);
  if (!rdoContainerHandle.isValid()) {
    ATH_MSG_WARNING("Retrieval of Mdt RDO container failed !");
    return StatusCode::SUCCESS;                                        
  }    
                                                           
  ///////////// here the RDO container is retrieved and filled -whatever input type we start with- => check the size 
  RpcPadContainer::const_iterator rdoColli;
  if (rdoContainerHandle->begin() == rdoContainerHandle->end()) {
    // empty pad container - no rpc rdo in this event
    ATH_MSG_DEBUG("Empty pad container - no rpc rdo in this event ");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Not empty pad container in this event ");
  
  // start here to process the RDO (for the full event of for a fraction of it)
  bool processingetaview = true;
  bool processingphiview = false;
  if (!m_solvePhiAmbiguities) processingetaview=false;
  m_doingSecondLoopAmbigColls = false;
  while (processingetaview || processingphiview || (!m_solvePhiAmbiguities)) {
    int ipad = 0;
    int nPrepRawData =0;
    int nPhiPrepRawData =0;
    int nEtaPrepRawData =0;
    if (processingphiview) m_ambiguousCollections.clear();
    if (msgLvl(MSG::DEBUG)) {
      if (processingetaview) msg (MSG::DEBUG) <<"*** Processing eta view "<<endmsg;
      else msg (MSG::DEBUG) <<"*** Processing phi view "<<endmsg;
    }
    
    const RpcPad* rdoColl;
    // seeded decoding
    if (sizeVectorRequested != 0) {
      ATH_MSG_DEBUG("Start loop over pads hashes - seeded mode ");
      RpcPadContainer::const_iterator rdoColli;
      const RpcPad* rdoColl;
      for (std::vector<IdentifierHash>::iterator iPadHash  = rdoHashVec.begin(); iPadHash != rdoHashVec.end(); ++iPadHash) {
        rdoColli =  rdoContainerHandle->indexFind(*iPadHash);
        if(rdoColli != rdoContainerHandle->end()) {
          rdoColl = *rdoColli;
          ++ipad;
       
          ATH_MSG_DEBUG("A new pad here n. "<<ipad<<", online id " << (int)(rdoColl->identifyHash()) << ", with " << rdoColl->size() <<" CM inside ");
          StatusCode sc = processPad(rdoColl, 
              processingetaview, 
              processingphiview, 
              nPrepRawData,   
              idVectToBeDecoded, 
              idWithDataVect, 
              rpcContext);
          if (sc != StatusCode::SUCCESS) return sc;
        }
        else {
          ATH_MSG_DEBUG("Requested pad with online id "<<(int)(*iPadHash)<<" not found in the rdoContainerHandle.");   
        }// end of if (pad collection exist in the container)
      }// end loop over requested pads hashes
    }
    else {// unseeded // whole event         
      ATH_MSG_DEBUG("Start loop over pads - unseeded mode ");
      for (RpcPadContainer::const_iterator rdoColli = rdoContainerHandle->begin(); rdoColli!=rdoContainerHandle->end(); ++rdoColli) {            
        // loop over all elements of the pad container 
        rdoColl = *rdoColli;
        if (rdoColl->empty()) continue;
        ++ipad;
        ATH_MSG_DEBUG("A new pad here n. "<<ipad<<", online id " << (int)(rdoColl->identifyHash()) << ", with " << rdoColl->size() <<" CM inside ");
        StatusCode sc = processPad(rdoColl, 
            processingetaview, 
            processingphiview, 
            nPrepRawData, 
            idVectToBeDecoded, 
            idWithDataVect, 
            rpcContext);
        if (sc != StatusCode::SUCCESS) return sc;
      }// end loop over pads
    }
    
    if (processingetaview) {    
      processingetaview=false;
      processingphiview=true;
      nEtaPrepRawData = nPrepRawData;
      ATH_MSG_DEBUG("*** "<<nEtaPrepRawData<<" eta PrepRawData registered");
    }
    else {    
      processingphiview=false;
      nPhiPrepRawData = nPrepRawData-nEtaPrepRawData;
      ATH_MSG_DEBUG("*** "<<nPhiPrepRawData<<" phi PrepRawData registered");
      if (!m_ambiguousCollections.empty()) {
        //loop again for unrequested collections stored with ambiguous phi hits
        m_doingSecondLoopAmbigColls=true;
        processingetaview=true;
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << m_ambiguousCollections.size() << " ambiguous collections were stored: ";
        idVectToBeDecoded.clear();
        rdoHashVec.clear();
        for (auto itAmbiColl = m_ambiguousCollections.begin(); itAmbiColl != m_ambiguousCollections.end(); ++itAmbiColl) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << (int)*itAmbiColl << " ";
          idVectToBeDecoded.push_back(*itAmbiColl);
          m_decodedOfflineHashIds.insert(*itAmbiColl);
        }
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endmsg;
        StatusCode sc = m_rpcCabling->giveRDO_fromPRD(idVectToBeDecoded, rdoHashVec);
        if (StatusCode::SUCCESS != sc) return sc;
        if (m_rawDataProviderTool->convert(rdoHashVec).isFailure()) {
          msg (MSG::FATAL) << "BS conversion into RDOs failed" << endmsg;
          return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG("BS conversion into RDOs for the selected collections done !");
      }
    }
    if (! m_solvePhiAmbiguities) {
      ATH_MSG_DEBUG("*** "<<nPrepRawData<<" PrepRawData registered");
      break;
    }
  }

  ATH_MSG_DEBUG("*** Final Cleanup ");
  //remove empty collections listed in idWithDataVect
  std::vector<IdentifierHash> temIdWithDataVect;
  int nn=0;
  for (std::vector<IdentifierHash>::const_iterator ii=idWithDataVect.begin(); ii!=idWithDataVect.end();++ii) {    
    ++nn;
    ATH_MSG_VERBOSE("hash = "<<(int)(*ii)<<" is the "<<nn<<"-th in this event ");
    RpcPrepDataContainer::const_iterator rpcColli = m_rpcPrepDataContainer->indexFind(*ii);
    if (rpcColli != m_rpcPrepDataContainer->end()) {      
      if (!(*rpcColli)->empty()) {	
	temIdWithDataVect.push_back(*ii);
	ATH_MSG_VERBOSE("Accepting non empty coll. "
			<<m_rpcHelper->show_to_string((*rpcColli)->identify())
			<<" hash = "<<(int)*ii<<" in PREPDATA container at "<<m_rpcPrepDataContainer);
      }
      else {	
	ATH_MSG_DEBUG("Removing from the rpc prep data container empty coll. "
		      <<m_rpcHelper->show_to_string((*rpcColli)->identify())
		      <<" hash = "<<(int)*ii<<" in PREPDATA container at "<<m_rpcPrepDataContainer);
	delete m_rpcPrepDataContainer->removeCollection(*ii); // from Scott Snyder #43275 // fix mem.leak
      }            
    }
    else
      ATH_MSG_DEBUG("collection with id = "<<(int)(*ii)<<" not found in the PREPDATA container at "<<m_rpcPrepDataContainer);
    }
    idWithDataVect.clear();
    idWithDataVect = temIdWithDataVect;
    
    //sort and remove duplicate entries in idWithDataVect
    ATH_MSG_DEBUG("sorting and removing duplicates in the accepted collections vector");
    sort( idWithDataVect.begin(), idWithDataVect.end() );
    idWithDataVect.erase( unique( idWithDataVect.begin(), idWithDataVect.end() ), idWithDataVect.end() );

    if (msgLvl(MSG::DEBUG)) {
      for (IdentifierHash hashId : idWithDataVect) {
        ATH_MSG_DEBUG("Accepted collection with hashId: " << (unsigned int)hashId);
      }
    }
    
    return status;
}


//___________________________________________________________________________
StatusCode Muon::RpcRdoToPrepDataTool::decode( const std::vector<uint32_t>& robIds )
{
  
  // ROB-based decoding is only applied in seeded mode. Full scan should use the hashId-based method with empty requested collections vector.
  
  int sizeVectorRequested = robIds.size();
  ATH_MSG_DEBUG("Decode method called for " << sizeVectorRequested << " ROBs");
  
  StatusCode status = StatusCode::SUCCESS;
  
  std::vector<uint32_t> robIdsToBeDecoded;
  robIdsToBeDecoded.reserve(robIds.size());
  
  // record the output containers if necessary (if first time in the event)
  bool firstTimeInTheEvent=false;
  status=manageOutputContainers(firstTimeInTheEvent);
  if (status.isFailure()) return status;
  if (firstTimeInTheEvent) {
    m_fullEventDone=false;
  }
  else {
    if (m_fullEventDone) {
      ATH_MSG_DEBUG("Whole event has already been decoded; nothing to do.");
      return status;
    }
  }
  
  //check which of the requested robs are not yet decoded
  for (uint32_t robid : robIds) {
    if (m_decodedRobIds.insert(robid).second)
      robIdsToBeDecoded.push_back(robid);
  }
  
  if (robIdsToBeDecoded.size()==0) {
    ATH_MSG_DEBUG("All requested ROBs have already been decoded; nothing to do.");
    return status;
  } else {
    ATH_MSG_DEBUG(robIdsToBeDecoded.size() << " ROBs have not yet been decoded and will be decoded now.");
      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "The list of ROB Ids to be decoded: ";
        for (uint32_t robid : robIdsToBeDecoded) msg(MSG::VERBOSE) << "0x" << MSG::hex << robid << MSG::dec << " ";
        msg(MSG::VERBOSE) << endmsg;
      }
  }
  
  //if all robs will be decoded after the current execution of the method, set the flag m_fullEventDone
  if (m_decodedRobIds.size() == m_rpcCabling->giveFullListOfRobIds().size()) m_fullEventDone=true;
  
  // if RPC decoding is switched off stop here
  if( !m_decodeData ) {
    ATH_MSG_DEBUG("Stored empty container. Decoding RPC RDO into RPC PrepRawData is switched off");
    return status;
  }


  ATH_MSG_DEBUG("Decoding RPC RDO into RPC PrepRawData");
  
  /// RPC context
  IdContext rpcContext = m_rpcHelper->module_context();
  
  if(m_useBStoRdoTool) {
    // we come here if the entire rdo container is not yet in SG (i.e. in EF with BS input) 
    // ask RpcRawDataProviderTool to decode the list of robs and to fill the rdo IDC
    CHECK( m_rawDataProviderTool->convert(robIdsToBeDecoded) );
    ATH_MSG_DEBUG("BS conversion into RDOs for the selected ROBs done !");
  }
  
  // we come here if the rdo container is already in SG (for example in MC RDO!) => the ContainerManager return NULL
  ATH_MSG_DEBUG("Retrieving Rpc PAD container from the store");
  auto rdoContainerHandle = SG::makeHandle(m_rdoContainerKey);
  if (!rdoContainerHandle.isValid()) {
    ATH_MSG_WARNING("Retrieval of Mdt RDO container failed !");
    return StatusCode::SUCCESS;                                        
  }    
  // here the RDO container is retrieved and filled -whatever input type we start with- => check the size 
  if (rdoContainerHandle->begin() == rdoContainerHandle->end()) {
    // empty pad container - no rpc rdo in this event
    ATH_MSG_DEBUG("Empty pad container - no rpc rdo in this event ");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Not empty pad container in this event ");
  
  // obtain a list of PADs (RDOs) to be processed
  std::vector<IdentifierHash> rdoHashVec;
  rdoHashVec.reserve(13*robIdsToBeDecoded.size()); // most ROBs have 13 RDOs, some have less
  CHECK( m_rpcCabling->giveRDO_fromROB(robIdsToBeDecoded, rdoHashVec) );
  
  std::vector<IdentifierHash> idVect; //vector passed to processPad - if empty, turns off decoding of additional RDOs for ambiguity solving
  std::vector<IdentifierHash> idWithDataVect; //vector passed to processPad - filled with IDs of created PrepRawData collections

  // start here to process the RDOs
  bool processingetaview = true;
  bool processingphiview = false;
  if (!m_solvePhiAmbiguities) processingetaview=false;
  m_doingSecondLoopAmbigColls = false;
  while (processingetaview || processingphiview || (!m_solvePhiAmbiguities)) {
    int ipad = 0;
    int nPrepRawData =0;
    int nPhiPrepRawData =0;
    int nEtaPrepRawData =0;
    if (processingphiview) m_ambiguousCollections.clear();
    
    if (processingetaview) ATH_MSG_DEBUG("*** Processing eta view ");
    else ATH_MSG_DEBUG("*** Processing phi view ");
    
    // seeded decoding (for full scan, use the hashId-based method)
    ATH_MSG_DEBUG("Start loop over pads hashes - seeded mode ");
    
    RpcPadContainer::const_iterator rdoColli;
    const RpcPad* rdoColl;
    for (IdentifierHash padHashId : rdoHashVec) {
      rdoColli = rdoContainerHandle->indexFind(padHashId);
      if (rdoColli == rdoContainerHandle->end()) {
        ATH_MSG_DEBUG("Requested pad with online id " << (unsigned int)padHashId << " not found in the rdoContainerHandle.");
        continue;
      }
      rdoColl = *rdoColli;
      ++ipad;
      ATH_MSG_DEBUG("A new pad here n." << ipad << ", online id " << (int)(rdoColl->identifyHash()) << ", with " << rdoColl->size() <<" CM inside ");
      CHECK(
        processPad(rdoColl, 
                   processingetaview, 
                   processingphiview, 
                   nPrepRawData,   
                   idVect, 
                   idWithDataVect, 
                   rpcContext)
      );
    }
    
    if (processingetaview) {
      processingetaview=false;
      processingphiview=true;
      nEtaPrepRawData = nPrepRawData;
      ATH_MSG_DEBUG("*** "<<nEtaPrepRawData<<" eta PrepRawData registered");
    }
    else {
      processingphiview=false;
      nPhiPrepRawData = nPrepRawData-nEtaPrepRawData;
      ATH_MSG_DEBUG("*** "<<nPhiPrepRawData<<" phi PrepRawData registered");
    }
    if (! m_solvePhiAmbiguities) {
      ATH_MSG_DEBUG("*** "<<nPrepRawData<<" PrepRawData registered");
      break;
    }
  }
  
  
  ATH_MSG_DEBUG("*** Final Cleanup ");
  //remove empty collections listed in idWithDataVect
  std::vector<IdentifierHash> temIdWithDataVect;
  int nn=0;
  for (IdentifierHash hashId : idWithDataVect) {   
    ++nn;
    ATH_MSG_VERBOSE("collection n. " << nn << " has hashId = " << (unsigned int)hashId);
    RpcPrepDataContainer::const_iterator rpcColli = m_rpcPrepDataContainer->indexFind(hashId);
    if (rpcColli != m_rpcPrepDataContainer->end()) {
      if (!(*rpcColli)->empty()) {
        temIdWithDataVect.push_back(hashId);
        ATH_MSG_VERBOSE("Accepting non empty coll. "
                     << m_rpcHelper->show_to_string((*rpcColli)->identify())
                     << " hashId = " << (unsigned int)hashId
                     << " in PREPDATA container at " << m_rpcPrepDataContainer);
      }
      else {
        ATH_MSG_DEBUG("Removing from the rpc prep data container empty coll. "
                   << m_rpcHelper->show_to_string((*rpcColli)->identify())
                   << " hashId = " << (unsigned int)hashId
                   << " in PREPDATA container at " << m_rpcPrepDataContainer);
        delete m_rpcPrepDataContainer->removeCollection(hashId);
      }
    }
    else
      ATH_MSG_DEBUG("collection with hashId = " << (unsigned int)hashId << " not found in the PREPDATA container at " << m_rpcPrepDataContainer);
    }
    idWithDataVect.clear();
    idWithDataVect = temIdWithDataVect;
    
    //sort and remove duplicate entries in idWithDataVect
    ATH_MSG_DEBUG("sorting and removing duplicates in the accepted collections vector");
    sort( idWithDataVect.begin(), idWithDataVect.end() );
    idWithDataVect.erase( unique( idWithDataVect.begin(), idWithDataVect.end() ), idWithDataVect.end() );
    
    if (msgLvl(MSG::DEBUG)) {
      for (IdentifierHash hashId : idWithDataVect) {
        ATH_MSG_DEBUG("Accepted collection with hashId: " << (unsigned int)hashId);
      }
    }
  
  return status;
}


//___________________________________________________________________________
void Muon::RpcRdoToPrepDataTool::printInputRdo()
{
  msg (MSG::INFO) << "********************************************************************************************************" << endmsg;
  msg (MSG::INFO) << "***************** Listing RpcPad Collections --- i.e. input RDO ****************************************" << endmsg;
  
  /// RPC context
  IdContext rpcContext = m_rpcHelper->module_context();
  /// RPC RDO container --- assuming it is available
  auto rdoContainerHandle = SG::makeHandle(m_rdoContainerKey);
  if (!rdoContainerHandle.isValid()) {
    ATH_MSG_WARNING("Retrieval of Mdt RDO container failed !");
    return ;                                        
  }                                                                 

  

  if (rdoContainerHandle->size() <= 0)msg (MSG::INFO) << "No RpcPad collections found" << endmsg;
  
  int ncoll = 0;
  int ictphi = 0;
  int icteta = 0;
  int icttrg = 0;
  msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
  
  int ipad = 0;
  const RpcPad * rdoColl;
  for (RpcPadContainer::const_iterator rdoColli = rdoContainerHandle->begin(); rdoColli!=rdoContainerHandle->end(); ++rdoColli) {
    // loop over all elements of the pad container 
    rdoColl = *rdoColli;
    if (rdoColl->empty()) continue;
    ++ipad;
    uint16_t padId     = rdoColl->onlineId(); 
    uint16_t sectorId  = rdoColl->sector();
    ATH_MSG_INFO("*** Pad online Id "
		 <<padId<<" m_logic sector ID "<< sectorId
		 <<" # of CM inside is "<< rdoColl->size());
    IdentifierHash rpcHashId = rdoColl->identifyHash();
    Identifier rdoId;
    int  code = m_rpcHelper->get_id(rpcHashId, rdoId, &rpcContext);
    if  (code != 0) 
      msg (MSG::INFO) <<" A problem in hash -> id conversion for hashId= "<<(int)rpcHashId<<endmsg;
    std::string extIdstring = m_rpcHelper->show_to_string(rdoId);
    ATH_MSG_INFO("*** Offine HashId = "<<static_cast<unsigned int>(rpcHashId)<<" extended = "<<extIdstring);

    // For each pad, loop on the coincidence matrices
    RpcPad::const_iterator itCM   = rdoColl->begin();
    RpcPad::const_iterator itCM_e = rdoColl->end();
    int icm = 0;
    int icphi = 0;
    int iceta = 0;
    int ictrg = 0;
    for (; itCM != itCM_e ; ++itCM) {
      icm++;
      bool etaview = false;
      if (m_processingData) etaview = true;
      bool highPtCm = false;
      // Get CM online Id
      uint16_t cmaId = (*itCM)->onlineId();
      if (cmaId<4) {
	//    msg (MSG::INFO) << " low pt ";
	if (cmaId<2) {    
	  etaview = true;
	  if (m_processingData) {
	    etaview = false;
	  }
	  //        msg (MSG::INFO) << " eta view = "<<etaview<<endmsg;
	}
	
	else {
	  //msg (MSG::INFO) << " eta view = "<<etaview<<endmsg;
	}
      }
      else {
	//msg (MSG::INFO) << " high pt ";
	highPtCm = true;
	if (cmaId<6) {
	  etaview = true;
	  if (m_processingData) {
	    etaview = false;
	  }
	  //msg (MSG::INFO) << " eta view = "<<etaview<<endmsg;
	}
	else {
	  //msg (MSG::INFO) << " eta view = "<<etaview<<endmsg;
	}
      }
      ATH_MSG_INFO("*** CM online Id "
		   <<cmaId<<" eta view = "<< etaview
		   <<" high pT = "<< highPtCm<<" # of hits = "<<(*itCM)->size());
      
      // For each CM, loop on the fired channels
      RpcCoinMatrix::const_iterator itD   = (*itCM)->begin();
      RpcCoinMatrix::const_iterator itD_e = (*itCM)->end();
      int idata = 0;
      if (itD == itD_e) {
	msg (MSG::INFO) << "Empty CM"<<endmsg;
      }
      for (; itD != itD_e ; ++itD) {
	idata++;
	
	const RpcFiredChannel * rpcChan = (*itD);
	if (msgLvl(MSG::INFO)) {
	  msg (MSG::INFO) <<"***** RpcFiredChannel: bcid "<<rpcChan->bcid()<<" time "
			  <<rpcChan->time()<<" ijk "<<rpcChan->ijk();
	  if (rpcChan->ijk()<7)  msg() <<" ch "<<rpcChan->channel();
	  msg() <<endmsg;
	  if (rpcChan->ijk()==6) ++ictrg;
	}
      }// end loop over hits 
    }// end loop over CM
    icttrg += ictrg;
    ictphi += icphi;
    icteta += iceta;
  }//end loop over pads
  ncoll = ipad;
  msg (MSG::INFO) <<"*** Event  Summary: "
		  <<ncoll <<" Collections / "
		  <<icttrg<<" trigger hits / "
		  <<ictphi<<" phi hits / "
		  <<icteta<<" eta hits "<<endmsg;
  msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
}

void Muon::RpcRdoToPrepDataTool::printPrepData()
{
  msg (MSG::INFO) << "********************************************************************************************************" << endmsg;
  msg (MSG::INFO) << "***************** Listing RpcPrepData collections content **********************************************" << endmsg;
  
  if (m_rpcPrepDataContainer->size() <= 0)msg (MSG::INFO) << "No RpcPrepRawData collections found" << endmsg;
  //else msg (MSG::INFO) << "Number of RpcPrepRawData collections found in this event is "<< << endmsg;
  
  int ncoll = 0;
  int ict = 0;
  int ictphi = 0;
  int ictamb = 0;
  int icteta = 0;
  int icttrg = 0;
  msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
  for (IdentifiableContainer<Muon::RpcPrepDataCollection>::const_iterator rpcColli = m_rpcPrepDataContainer->begin();
       rpcColli!=m_rpcPrepDataContainer->end(); ++rpcColli) {

    const Muon::RpcPrepDataCollection* rpcColl = *rpcColli;
        
    if ( rpcColl->size() > 0 ) {
      msg (MSG::INFO) <<"PrepData Collection ID "<<m_rpcHelper->show_to_string(rpcColl->identify())<<endmsg;
      RpcPrepDataCollection::const_iterator it_rpcPrepData;
      int icc = 0;
      int iccphi = 0;
      int icceta = 0;
      //            int icctrg = 0;
      for (it_rpcPrepData=rpcColl->begin(); it_rpcPrepData != rpcColl->end(); it_rpcPrepData++) {
	icc++;
	ict++;
	//                 if ((*it_rpcPrepData)->triggerInfo() > 0) 
	//                 {
	//                     icctrg++;
	//                     icttrg++;
	//                 }
	//                 else
	//                 {                    
	if (m_rpcHelper->measuresPhi((*it_rpcPrepData)->identify())) {
	  iccphi++;
	  ictphi++;
	  if ((*it_rpcPrepData)->ambiguityFlag()>1) ictamb++;
	}
	else {    
	  icceta++;
	  icteta++;
	}                    
	//                 }
	msg (MSG::INFO) <<ict<<" in this coll. "<<icc<<" prepData id = "
			<<m_rpcHelper->show_to_string((*it_rpcPrepData)->identify())
			<<" time "<<(*it_rpcPrepData)->time()/*<<" triggerInfo "<<(*it_rpcPrepData)->triggerInfo()*/
			<<" ambiguityFlag "<<(*it_rpcPrepData)->ambiguityFlag()<<endmsg;
      }
      ncoll++;
      msg (MSG::INFO) <<"*** Collection "<<ncoll<<" Summary: "
	//                <<icctrg<<" trigger hits / "
		      <<iccphi<<" phi hits / "
		      <<icceta<<" eta hits "<<endmsg;
      msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
    }
  }
  msg (MSG::INFO) <<"*** Event  Summary: "
		  <<ncoll <<" Collections / "
		  <<icttrg<<" trigger hits / "
		  <<ictphi<<" phi hits / "
		  <<icteta<<" eta hits "<<endmsg;
  msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
  
  // and now coincidence data 
  printCoinData();
}

void Muon::RpcRdoToPrepDataTool::printCoinData()
{
  msg (MSG::INFO) << "********************************************************************************************************" << endmsg;
  msg (MSG::INFO) << "***************** Listing RpcCoinData collections content **********************************************" << endmsg;
  
  if (m_rpcCoinDataContainer->size() <= 0)msg (MSG::INFO) << "No RpcCoinData collections found" << endmsg;
  //else msg (MSG::INFO) << "Number of RpcPrepRawData collections found in this event is "<< << endmsg;
  
  int ncoll = 0;
  int ict = 0;
  int ictphi = 0;
  int icteta = 0;
  int ictphilc = 0;
  int ictphihc = 0;
  int ictetalc = 0;
  int ictetahc = 0;
  msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
  for (IdentifiableContainer<Muon::RpcCoinDataCollection>::const_iterator rpcColli = m_rpcCoinDataContainer->begin();
       rpcColli!=m_rpcCoinDataContainer->end(); ++rpcColli) {

        const Muon::RpcCoinDataCollection* rpcColl = *rpcColli;
        
        if ( rpcColl->size() > 0 ) {	  
	  msg (MSG::INFO) <<"CoinData Collection ID "<<m_rpcHelper->show_to_string(rpcColl->identify())<<endmsg;
	  RpcCoinDataCollection::const_iterator it_rpcCoinData;
	  int icc = 0;
	  int iccphi = 0;
	  int icceta = 0;
	  int iccphilc = 0;
	  int iccetahc = 0;
	  int iccphihc = 0;
	  int iccetalc = 0;
	  //            int icctrg = 0;
	  for (it_rpcCoinData=rpcColl->begin(); it_rpcCoinData != rpcColl->end(); it_rpcCoinData++) {
	    icc++;
	    ict++;
	    //                 if ((*it_rpcPrepData)->triggerInfo() > 0) 
	    //                 {
	    //                     icctrg++;
	    //                     icttrg++;
	    //                 }
	    //                 else
	    //                 {                    
	    if (m_rpcHelper->measuresPhi((*it_rpcCoinData)->identify())) {
	      
	      iccphi++;
	      ictphi++;
	      if ( (*it_rpcCoinData)->isLowPtCoin() ) {		
		iccphilc++;
		ictphilc++;
	      }
	      else if ((*it_rpcCoinData)->isHighPtCoin()) {
		iccphihc++;
		ictphihc++;
	      }                    
	    }
	    else {
	      icceta++;
	      icteta++;
	      if ( (*it_rpcCoinData)->isLowPtCoin() ) {
		iccetalc++;
		ictetalc++;
	      }
	      else if ((*it_rpcCoinData)->isHighPtCoin()) {
		iccetahc++;
		ictetahc++;
	      }
	    }                    
	    //                 }
	    msg (MSG::INFO) <<ict<<" in this coll. "<<icc<<" coinData id = "
			    <<m_rpcHelper->show_to_string((*it_rpcCoinData)->identify())
			    <<" time "<<(*it_rpcCoinData)->time()<<" ijk = "<<(*it_rpcCoinData)->ijk()/*<<" triggerInfo "<<(*it_rpcPrepData)->triggerInfo()*/
			    <<" cm/pad/sl ids = "
			    <<(*it_rpcCoinData)->parentCmId()<<"/"<<(*it_rpcCoinData)->parentPadId()<<"/"<<(*it_rpcCoinData)->parentSectorId()<<"/"
			    <<" isLowPtCoin/HighPtCoin/LowPtInputToHighPt "
			    <<(*it_rpcCoinData)->isLowPtCoin() <<"/"<<(*it_rpcCoinData)->isHighPtCoin() <<"/"<<(*it_rpcCoinData)->isLowPtInputToHighPtCm() 
			    <<endmsg;
	  }
	  ncoll++;
	  msg (MSG::INFO) <<"*** Collection "<<ncoll<<" Summary: "
	    //                <<icctrg<<" trigger hits / "
			  <<iccphi<<" phi coin. hits / "
			  <<icceta<<" eta coin. hits \n"
			  <<iccphilc<<" phi lowPt / "
			  <<iccphihc<<" phi highPt / "
			  <<iccetalc<<" eta lowPt / "
			  <<iccetahc<<" eta highPt coincidences  "
			  <<endmsg;
	  msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
        }
  }
  msg (MSG::INFO) <<"*** Event  Summary: "
		  <<ncoll <<" Collections / "
		  <<ictphi<<" phi coin. hits / "
		  <<icteta<<" eta coin. hits \n"
		  <<ictphilc<<" phi lowPt / "
		  <<ictphihc<<" phi highPt / "
		  <<ictetalc<<" eta lowPt / "
		  <<ictetahc<<" eta highPt coincidences  "
		  <<endmsg;
  msg (MSG::INFO) <<"--------------------------------------------------------------------------------------------"<<endmsg;
}


StatusCode Muon::RpcRdoToPrepDataTool::processPad(const RpcPad *rdoColl, 
                                                  bool& processingetaview, 
                                                  bool& processingphiview,
                                                  int& nPrepRawData,  
                                                  std::vector<IdentifierHash>& idVect, // if empty, turns off decoding of additional RDOs for ambiguity solving
                                                  std::vector<IdentifierHash>& idWithDataVect, // filled with IDs of created PrepRawData collections
                                                  IdContext& rpcContext)
{
   
  ATH_MSG_DEBUG("***************** Start of processPad eta/phiview "
		<<processingetaview<<"/"<<processingphiview
		<<" ---# of coll.s with data until now is "<<idWithDataVect.size());
  //{processPad
  //Get pad online id and sector id
  uint16_t padId     = rdoColl->onlineId(); 
  uint16_t sectorId  = rdoColl->sector();
  ATH_MSG_DEBUG("***************** for Pad online Id "<<padId<<" m_logic sector ID "<< sectorId);
  
  // Create an RPC PrepDataCollection
  Identifier oldId;
  Identifier oldIdTrg;
  ATH_MSG_VERBOSE("Init pointer to RpcPrepDataCollection ");
  RpcPrepDataCollection * collection(0);
  RpcCoinDataCollection * collectionTrg(0);
  IdentifierHash rpcHashId;
  
  // For each pad, loop on the coincidence matrices
  RpcPad::const_iterator itCM   = rdoColl->begin();
  RpcPad::const_iterator itCM_e = rdoColl->end();
  int icm = 0;
  for (; itCM != itCM_e ; ++itCM) {

    icm++;
    bool etaview = false;
    if (m_processingData) etaview = true;
    bool highPtCm = false;
    // Get CM online Id
    uint16_t cmaId = (*itCM)->onlineId();
    ATH_MSG_DEBUG("A new CM here n. "<<icm<<" CM online ID "<<cmaId
		  <<" with n. of hits =  "<<(*itCM)->size());
    if (cmaId<4) {
      ATH_MSG_DEBUG(" low pt ");
      if (cmaId<2) {
	etaview = true;
	if (m_processingData) {
	  etaview = false;
	}
	ATH_MSG_DEBUG( " eta view = "<<etaview);
      }
      
      else {
	ATH_MSG_DEBUG(" eta view = "<<etaview);
      }
    }
    else {
      ATH_MSG_DEBUG(" high pt ");
      highPtCm = true;
      if (cmaId<6) {
	etaview = true;
	if (m_processingData) {
	  etaview = false;
	}
	ATH_MSG_DEBUG(" eta view = "<<etaview);
      }
      else {
	ATH_MSG_DEBUG(" eta view = "<<etaview);
      }
    }
    
    if (processingetaview && (!etaview)) continue;
    if (processingphiview && etaview)    continue;
    
    // For each CM, loop on the fired channels
    RpcCoinMatrix::const_iterator itD   = (*itCM)->begin();
    RpcCoinMatrix::const_iterator itD_e = (*itCM)->end();
    int idata = 0;
    if (itD == itD_e) {
      ATH_MSG_DEBUG("Empty CM");
    }
    for (; itD != itD_e ; ++itD) {      
      idata++;
      // trigger related quantities 
      unsigned short threshold = 99;
      unsigned short overlap   = 99;
      
      // flags defining the processing mode of this hit 
      bool solvePhiAmb_thisHit   = m_solvePhiAmbiguities;
      bool reduceCablOvl_thisHit = m_reduceCablingOverlap;
      
      ATH_MSG_DEBUG("A new CM Hit "<<idata);
      const RpcFiredChannel * rpcChan = (*itD);
      if (msgLvl(MSG::DEBUG)) {
	msg (MSG::DEBUG) <<"RpcFiredChannel: bcid "<<rpcChan->bcid()<<" time "
			 <<rpcChan->time()<<" ijk "<<rpcChan->ijk();
	if (rpcChan->ijk()<7)  msg() <<" ch "<<rpcChan->channel();
	msg() <<endmsg;
      }

      // check if trigger hit             
      // select the cases: ijk = 0 and high p, ijk= 6, ijk=7
      bool triggerHit = false;
      bool toSkip     = false;
      processTriggerHitHypothesis(itD, itD_e, highPtCm, triggerHit, threshold, overlap, toSkip);
      if (toSkip) continue;
      if (triggerHit) {
	// here ijk = 6 or ijk = 0 in high pt cm
	// keep all pivot + trigger info (even if duplicated [should never happen, for pivot hits])
	solvePhiAmb_thisHit   = false;
	reduceCablOvl_thisHit = false;
	ATH_MSG_DEBUG("RpcFiredChannel: it's a triggerHit or a lowPt coinc. in a high pt CM \n"
		      <<"    ijk = "<<rpcChan->ijk()<<" isHighPtCM "<<highPtCm<<" thr/ovl = "<<threshold<<"/"<<overlap);
      }
      
      // here decode (get offline ids for the online indices of this hit)
      double time = 0.;
      std::vector<Identifier>* digitVec = 
	m_rpcRdoDecoderTool->getOfflineData(rpcChan, sectorId, padId, cmaId, time);
      time += (double)m_timeShift;
      
      if (digitVec==NULL) {
	msg (MSG::ERROR) << "Error in the RPC RDO decoder " << endmsg;
	return StatusCode::RECOVERABLE;
      }
      
      int  nMatchingEtaHits  = 0;
      int  nDuplicatePhiHits = 0;
      bool unsolvedAmbiguity = false;
      bool notFinished       = true;
      // allow for 2 iterations in case there are phi digits without matching eta (eta inefficiency)
      // all eta digits, not already recorded, will be registered as PrepRawData
      // and all phi digits, not yet recorded and with a eta digit in the same module
      // and gap, will produce a PrepRawData.
      // Phi digits without a eta match will not be recorded at the first iteration.
      // If all phi digits do not have a eta match, they will be all recorded as PrepRawData
      // in the second iteration (the ambiguity will remain unsolved)
      while (notFinished) {	
	//                             m_log << MSG::VERBOSE
	//                                 << "STARTOFWHILE - NotFinished = "<< notFinished
	//                                 <<" unsolvedAmbiguity = "<<unsolvedAmbiguity<<" processingphiview "<<processingphiview
	//                                 <<" digitVec of size "<<digitVec->size()<<endmsg;
	// Loop on the digits corresponding to the fired channel
	std::vector<Identifier>::iterator itVec   = digitVec->begin();
	std::vector<Identifier>::iterator itVec_e = digitVec->end();
	
	unsigned int offlineDataSize = digitVec->size();
	ATH_MSG_DEBUG("size of the corresponding list of ID = "<< offlineDataSize );
	if (offlineDataSize <=0 ) {
	  ATH_MSG_DEBUG("going to next CM hit");
	  notFinished = false;
	  continue;
	}
	for ( ; itVec!=itVec_e; ++itVec) {	  
	  // Prepare the prepdata for this identifier 
	  // channel Id
	  Identifier channelId = *itVec;
	  Identifier parentId = m_rpcHelper->elementID(channelId);
	  if (m_rpcHelper->get_hash(parentId, rpcHashId, &rpcContext)) {
	    msg (MSG::ERROR) << "Unable to get RPC hash id from RPC collection " 
			     << "context begin_index = " << rpcContext.begin_index()
			     << " context end_index  = " << rpcContext.end_index()
			     << " the identifier is "
			     << endmsg;
	    parentId.show();
	  }
	  // SS 14/10/2008 try this (doesn't change unseeded behaviour)
	  //                     if ( !isRequested(idVect, rpcHashId) ) {
	  //                         if (m_debug) m_log << MSG::DEBUG << "this offline identifier does not belong to the selection of collections requested - skip "<<endmsg;
	  //                         continue;
	  //                     }
	  if ( msgLvl (MSG::DEBUG) ) {	    
	    msg (MSG::DEBUG) << "CM Hit decoded into offline Id " 
			     << m_rpcHelper->show_to_string(channelId) 
			     << " time "<<time<<endmsg; 
	    msg (MSG::DEBUG) << "           Parent collection   " 
			     << m_rpcHelper->show_to_string(parentId)
			     << " oldID = "<<m_rpcHelper->show_to_string(oldId)
			     << " oldIDtrg = "<<m_rpcHelper->show_to_string(oldIdTrg)
			     << endmsg;
	  }
	  bool hasAMatchingEtaHit = 0;
	  // current collection has Id "parentId"; get it from the container !
	  if (triggerHit) {	    
	    if ( (oldIdTrg != parentId) || collectionTrg == 0 ) {
	      // Get collection from IDC if it exists, or create it and add it if not.
	      ATH_MSG_DEBUG(" Looking/Creating a collection with ID = "
			    <<m_rpcHelper->show_to_string(parentId)<<" hash = "
			    <<static_cast<unsigned int>(rpcHashId)<<" in COINDATA container at "<<m_rpcCoinDataContainer);
	      collectionTrg = Muon::IDC_Helper::getCollection<RpcCoinDataContainer, RpcIdHelper>(parentId, m_rpcCoinDataContainer , m_rpcHelper, msg());
	      if ( collectionTrg ==0 ) msg (MSG::WARNING) <<"Failed to get/create RpcCoinData collection"<<endmsg;
	      oldIdTrg = parentId;
	      ATH_MSG_DEBUG(" Resetting oldIDtrg to current parentID = "<<m_rpcHelper->show_to_string(oldIdTrg));
	    }
	  }
	  else {
	    if ( (oldId    != parentId) || collection    == 0 ) {	      
	      // Get collection from IDC if it exists, or create it and add it if not.
	      ATH_MSG_DEBUG(" Looking/Creating a collection with ID = "
			    <<m_rpcHelper->show_to_string(parentId)<<" hash = "
			    <<static_cast<unsigned int>(rpcHashId)<<" in PREPDATA container at "<<m_rpcPrepDataContainer);
	      collection =	Muon::IDC_Helper::getCollection<RpcPrepDataContainer, RpcIdHelper>(parentId,
										   		   m_rpcPrepDataContainer,
										   		   m_rpcHelper,
										   		   msg() );
	      if (collection) {                            
           bool alreadyRecorded = false;
           for (std::vector<IdentifierHash>::const_iterator ii=idWithDataVect.begin(); ii!=idWithDataVect.end();++ii) {		  
             if (*ii == rpcHashId) {		    
               alreadyRecorded = true;
               break;
             }
           }
           if (!alreadyRecorded) {		  
             idWithDataVect.push_back(rpcHashId); // Record that this collection contains data                 
             ATH_MSG_DEBUG(" A new collection add to the RPC PrepData container with identifier "
             <<m_rpcHelper->show_to_string(collection->identify()));
           }
	      }
	      else msg (MSG::WARNING) <<"Failed to get/create RpcPrepData collection"<<endmsg;
	      oldId = parentId;
	      ATH_MSG_DEBUG(" Resetting oldID to current parentID = "
			    <<m_rpcHelper->show_to_string(oldId));
	    }
	  }
          
	  
	  // check if the data has already ben recorded
	  // (if you want to reduce the redundancy due to cabling overlap and if the collection is not empty)
	  bool duplicate = false;
	  if (reduceCablOvl_thisHit) {	    
	    if (collection->begin()!=collection->end()) {                        
	      RpcPrepDataCollection::iterator it_rpcPrepData;
	      int icc = 0;
	      ATH_MSG_VERBOSE("Check for duplicates in coll. with size "<<collection->size());
	      int current_dbphi = 0;
	      int current_dbz   = 0;
	      int current_gg    = 0;
	      if (processingphiview) {		
          current_dbphi = m_rpcHelper->doubletPhi(channelId);
          current_dbz   = m_rpcHelper->doubletZ(channelId);
          current_gg    = m_rpcHelper->gasGap(channelId);
          ATH_MSG_VERBOSE("Check also for eta hits matching dbz, dbphi, gg  "
            <<current_dbz<<" "<<current_dbphi<<" "<< current_gg);
	      }
	      
	      for (it_rpcPrepData=collection->begin(); it_rpcPrepData != collection->end(); it_rpcPrepData++) {
		icc++;
		if ( channelId == (*it_rpcPrepData)->identify() &&
		     fabs(time - (*it_rpcPrepData)->time()) < m_overlap_timeTolerance ) {
		  duplicate = true;
		  hasAMatchingEtaHit = false; // we don't want to increment the number of strips with
		  // a matching eta due to a cabling overlap 
		  ATH_MSG_VERBOSE("Duplicated RpcPrepData(not recorded) = "
				  << m_rpcHelper->show_to_string(channelId));
		  float previous_time = (*it_rpcPrepData)->time();
		  // choose the smallest time within timeTolerance 
		  if (time < previous_time) {		    
		    (*it_rpcPrepData)->m_time = time;
		    ATH_MSG_DEBUG("time of the prd previously stored is now updated with current hit time: "
				  <<previous_time<<" -> "<<(*it_rpcPrepData)->time());
		  }
		  break; // this break is why we cannot have
		  //        solvePhiAmb_thisHit = true and reduceCablOvl_thisHit= false
		}
		if (processingphiview) {
		  if (solvePhiAmb_thisHit) {
		    if (!unsolvedAmbiguity) {
		      if (m_rpcHelper->measuresPhi( (*it_rpcPrepData)->identify() )==0) {
			// check if there's a eta hit in the same gap
			// of the RPC module (doubletZ, doubletPhi, gg)
			if (current_dbz == m_rpcHelper->doubletZ( (*it_rpcPrepData)->identify() ) ) {
			  if (current_dbphi == m_rpcHelper->doubletPhi( (*it_rpcPrepData)->identify() ) ) {                                                    
			    if (current_gg == m_rpcHelper->gasGap( (*it_rpcPrepData)->identify() ) ) {                                                        
			      if ( fabs(time - (*it_rpcPrepData)->time()) < m_etaphi_coincidenceTime ) {
				hasAMatchingEtaHit = true;
				ATH_MSG_VERBOSE("There's a matching eta hit with id "
						<<m_rpcHelper->show_to_string((*it_rpcPrepData)->identify()));
				//here there can be a break ? NO, we need to keep looping in order to check
				// if this preprawdata has been already recorded (due to cabling overlaps)
			      }
			    }
			  }                                            
			}
		      }
		    }
		  }
		}                                    
	      }
	      if (hasAMatchingEtaHit)
		nMatchingEtaHits++; //Number of phi strips (possibly corresponding to this CM hit)
	      // with a matching eta
	      if (processingphiview && duplicate)
		nDuplicatePhiHits++;//Number of phi strips (possibly corresponding to this CM hit)
	      // already in the collection
	    } // if collection is not empty                         
	  } // end of if reduceCablingOverlap
          
	  
	  if (msgLvl(MSG::VERBOSE)) {	    
	    if (solvePhiAmb_thisHit && (!etaview)) 
	      msg (MSG::VERBOSE) 
		<< "nMatchingEtaHits = "
		<<nMatchingEtaHits
		<<" hasAMatchingEtaHit = "
		<<hasAMatchingEtaHit<<endmsg;
	  }
                    
	  if (!duplicate) {
	    ATH_MSG_VERBOSE(" solvePhiAmb_thisHit, processingetaview, processingphiview, hasAMatchingEtaHit, unsolvedAmbiguity "
	             <<solvePhiAmb_thisHit<<" "<<processingetaview<<" "<<processingphiview<<" "<<hasAMatchingEtaHit<<" "<<unsolvedAmbiguity);
	    if ( (!solvePhiAmb_thisHit) || processingetaview ||
		 (processingphiview && (hasAMatchingEtaHit || unsolvedAmbiguity)) ) {	      
		
	      if (unsolvedAmbiguity) {
          if (idVect.empty()) {//full-scan mode
            ATH_MSG_DEBUG("storing data even if unsolvedAmbiguity");
          } else {
            //if in RoI mode and the collection was not requested in this event, add it to ambiguousCollections
            ATH_MSG_DEBUG("unsolvedAmbiguity is true, adding collection with hash = "<<(int)rpcHashId<<" to ambiguous collections vector");
            if (!m_decodedOfflineHashIds.empty() && m_decodedOfflineHashIds.find(rpcHashId)==m_decodedOfflineHashIds.end()) {
              m_ambiguousCollections.insert(rpcHashId);
              ATH_MSG_DEBUG("collection not yet processed; added to ambiguous collection vector; going to the next offline channel ID");
              continue; //go to the next possible offline channel ID
            }
            else if (!m_doingSecondLoopAmbigColls) {
              m_ambiguousCollections.insert(rpcHashId);
              ATH_MSG_DEBUG("collection already processed and doingSecondLoopAmbigColls=false; added to ambiguous collection vector; going to the next offline channel ID");
              continue;
            } else {
              ATH_MSG_DEBUG("collection already processed and doingSecondLoopAmbigColls=true; trying to store data even if unsolvedAmbiguity");
            }
          }
        }
	      // det element
	      const RpcReadoutElement *descriptor = m_muonMgr->getRpcReadoutElement(channelId);
	      
	      // here check validity
	      // if invalid, reset flags
	      if (descriptor == NULL) {
		hasAMatchingEtaHit = false;
		duplicate          = false;
		msg (MSG::WARNING) <<"Detector Element not found for Identifier from the cabling service <"
				   <<m_rpcHelper->show_to_string(channelId)<<">  =>>ignore this hit"<<endmsg;
		continue;
	      }
	      else if (!descriptor->containsId(channelId)) {
		hasAMatchingEtaHit = false;
		duplicate          = false;
		if (m_rpcHelper->stationNameString(m_rpcHelper->stationName(channelId))=="BOG") 
		  ATH_MSG_DEBUG("Identifier from the cabling service <"
				<<m_rpcHelper->show_to_string(channelId)<<"> inconsistent with the geometry of detector element <"
				<<m_rpcHelper->show_to_string(descriptor->identify())<<">  =>>ignore this hit /// there are unmasked channels in BOG");
		else
		  msg (MSG::WARNING) <<"Identifier from the cabling service <"
				     <<m_rpcHelper->show_to_string(channelId)<<"> inconsistent with the geometry of detector element <"
				     <<m_rpcHelper->show_to_string(descriptor->identify())<<">  =>>ignore this hit"<<endmsg;
		continue;
	      }
              
	      // 
	      // Global position
	      Amg::Vector3D tempGlobalPosition = descriptor->stripPos(channelId);
	      //const Trk::GlobalPosition *globalPosition = new Trk::GlobalPosition(tempGlobalPosition);
	      ATH_MSG_VERBOSE("RPC RDO->PrepRawdata: global position ("
			      <<tempGlobalPosition.x()<<", "
			      <<tempGlobalPosition.y()<<", "
			      <<tempGlobalPosition.z()<<") ");
	      // Local position
	      Amg::Vector2D pointLocPos;
	      descriptor->surface(channelId).globalToLocal(tempGlobalPosition,tempGlobalPosition,pointLocPos);
	      //if (fabs(pointLocPos->y())>0.001) 
	      //  std::cout<<" Local Position for id = "<<m_rpcHelper->show_to_string(channelId)
	      //<<" is ("<<pointLocPos->x()<<",  "<<pointLocPos->y()<<")"<<std::endl;
	      // the globalToLocal should not fail, if it does produce a WARNING
	      
	      // List of Digits in the cluster (self)
	      std::vector<Identifier> identifierList;        
	      identifierList.push_back(channelId);
	      
	      // width of the cluster (self) 
	      float stripWidth   = descriptor->StripWidth( m_rpcHelper->measuresPhi(channelId) );
	      
	      // Error matrix
	      double errPos      = stripWidth / sqrt(12.0);
	      //CLHEP::HepSymMatrix mat = CLHEP::HepSymMatrix(1,1);
	      Amg::MatrixX mat(1,1);
	      mat.setIdentity();
	      mat *= errPos*errPos;
	      //Trk::CovarianceMatrix * cov = new Trk::CovarianceMatrix(mat);
	      //const Trk::ErrorMatrix * errMat = new Trk::ErrorMatrix(cov);
	      // check if this is a triggerINFO rather then a real hit
	      // Create a new PrepData
	      int ambiguityFlag = 0;
	      if (solvePhiAmb_thisHit) {
		if (processingetaview) ambiguityFlag = 1;
		if (unsolvedAmbiguity) ambiguityFlag = offlineDataSize;
		else if (hasAMatchingEtaHit) ambiguityFlag = nMatchingEtaHits;
	      }
	      

	      //correct prd time from cool db
	      if (m_RPCInfoFromDb){
		ATH_MSG_DEBUG( " Time correction from COOL " << " size of  RPC_TimeMapforStrip " <<m_rSummarySvc->RPC_TimeMapforStrip().size() );	     
		std::vector<double> StripTimeFromCool;
		if( m_rSummarySvc->RPC_TimeMapforStrip().find(channelId) != m_rSummarySvc->RPC_TimeMapforStrip().end()){
		  StripTimeFromCool   = m_rSummarySvc->RPC_TimeMapforStrip     ().find(channelId)->second ;		 
		  ATH_MSG_DEBUG(" Time "<< time << " Time correction from COOL for jstrip " <<StripTimeFromCool.at(0) );		  
		  time -= StripTimeFromCool.at(0) ;		
		}
	      }
		


	      if (triggerHit) {
		ATH_MSG_DEBUG("producing a new  RpcCoinData");
		
		RpcCoinData *newCoinData = new RpcCoinData(channelId,
							   rpcHashId,
							   pointLocPos,
							   identifierList,
							   new Amg::MatrixX(mat),
							   descriptor,
							   (float)time,
							   ambiguityFlag,
							   rpcChan->ijk(), threshold, overlap,
							   cmaId, padId, sectorId,
							   !(highPtCm));
		//if (fabs(pointLocPos->y())>0.001) std::cout<<"NEW RPCCOINDATA with id, hash, descriptor "<<m_rpcHelper->show_to_string(channelId)<<" "<<(unsigned int)rpcHashId<<" "<<(long)descriptor<<std::endl;
		
		
		// record the new data in the collection     
		ATH_MSG_DEBUG(" Adding RpcCoinData @ "<<newCoinData
			      <<" to collection "<<m_rpcHelper->show_to_string(collectionTrg->identify()));
		
		newCoinData->setHashAndIndex(collectionTrg->identifyHash(), collectionTrg->size());
		collectionTrg->push_back(newCoinData);
	      } // end of to be stored now for RpcCoinData
	      else {		   
		ATH_MSG_DEBUG("producing a new  RpcPrepData with "
			      <<"ambiguityFlag = "
			      <<ambiguityFlag);
		
		//std::cout<<" before creating the prd time = "<<time<<" (float)time = "<<(float)time<<std::endl;
		  
		
		RpcPrepData *newPrepData = new RpcPrepData(channelId,
							   rpcHashId,
							   pointLocPos,
							   identifierList,
							   new Amg::MatrixX(mat),
							   descriptor,
							   (float)time,
							   ambiguityFlag);
		//if (fabs(pointLocPos->y())>0.001) std::cout<<"NEW RPCPREPDATA with id, hash, descriptor "<<m_rpcHelper->show_to_string(channelId)<<" "<<(unsigned int)rpcHashId<<" "<<(long)descriptor<<std::endl;
		
		
		// record the new data in the collection     
		ATH_MSG_DEBUG(" Adding digit @ "<<newPrepData
			      <<" to collection "<<m_rpcHelper->show_to_string(collection->identify()));
		
		newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
		collection->push_back(newPrepData);
		// here one should reset ambiguityFlag for the prepdata registered before the current one
		// (from the same RDO hit) if nMatchingEtaHits > 1
		nPrepRawData++;
	      } // end of to be stored now for RpcPrepData
	    } // end of to be stored now 
	  } // this hit was not yet recorded
	  else {	    
	    // this hit was already recorded 
	    ATH_MSG_DEBUG("digit already in the collection ");
	  }                                
	}// end loop over possible offline identifiers corresponding to this CM hit 
   ATH_MSG_VERBOSE("processingphiview, nMatchingEtaHits, nDuplicatePhiHits, unsolvedAmbiguity, solvePhiAmb_thisHit : "
            <<processingphiview<<", "<<nMatchingEtaHits<<", "<<nDuplicatePhiHits<<", "<<unsolvedAmbiguity<<", "<<solvePhiAmb_thisHit);
	if ( (processingphiview && (nMatchingEtaHits==0)) &&
	     (nDuplicatePhiHits == 0)                     && 
	     (!unsolvedAmbiguity)                         &&
	     (solvePhiAmb_thisHit) )
	  {
	    unsolvedAmbiguity = true;
	    // no eta hits matching any phi digit
	    // loop once again and store all phi digits potentially generating this CM hit
	    ATH_MSG_DEBUG("No eta prepData matching any phi hit from this CM hit \n" <<
			  "loop once again and store all phi digits potentially generating this CM hit");
	  }
	else if (unsolvedAmbiguity) notFinished = false; 
	else notFinished = false;
      }//end of not finished 
      delete digitVec;
    }// end loop over CM hits 
  }// end loop over CMs
  
  ATH_MSG_DEBUG("***************** Stop  of processPad eta/phiview "
		<<processingetaview<<"/"<<processingphiview<<" ---# of coll.s with data now is "<<idWithDataVect.size()
		<< "***************** for Pad online Id "<<padId<<" m_logic sector ID "<< sectorId);

  return StatusCode::SUCCESS;
}

StatusCode Muon::RpcRdoToPrepDataTool::manageOutputContainers(bool& firstTimeInTheEvent)
{
  if(!evtStore()->contains<Muon::RpcPrepDataContainer>(m_rpcPrepDataContainerKey.key())) {  
    firstTimeInTheEvent = true;

    SG::WriteHandle< Muon::RpcPrepDataContainer > rpcPrepDataHandle(m_rpcPrepDataContainerKey);
    StatusCode status = rpcPrepDataHandle.record(std::make_unique<Muon::RpcPrepDataContainer>(m_rpcHelper->module_hash_max()));

    if (status.isFailure() || !rpcPrepDataHandle.isValid() ) 	{
      ATH_MSG_FATAL("Could not record container of RPC PrepData Container at " << m_rpcPrepDataContainerKey.key());
      return status;
    } else {
      m_rpcPrepDataContainer = rpcPrepDataHandle.ptr();
      ATH_MSG_DEBUG("RPC PrepData Container recorded in StoreGate with key " << m_rpcPrepDataContainerKey.key());
    }
    
    if (m_producePRDfromTriggerWords){
      /// create an empty RPC trigger hit container for filling
      SG::WriteHandle< Muon::RpcCoinDataContainer > rpcCoinDataHandle(m_rpcCoinDataContainerKey);
      status = rpcCoinDataHandle.record(std::make_unique<Muon::RpcCoinDataContainer>(m_rpcHelper->module_hash_max()));

      if (status.isFailure() || !rpcPrepDataHandle.isValid() ) 	{
        ATH_MSG_FATAL("Could not record container of RPC TrigCoinData Container at " << m_rpcPrepDataContainerKey.key());
        return status;
      } else {
        m_rpcCoinDataContainer = rpcCoinDataHandle.ptr();
	ATH_MSG_DEBUG("RPC TrigCoinData Container recorded in StoreGate with key " << m_rpcPrepDataContainerKey.key());
      }      
      ATH_MSG_VERBOSE(" RpcCoinDataContainer created");
    } 
    m_decodedOfflineHashIds.clear();
    m_decodedRobIds.clear();
  }
  else {
    ATH_MSG_DEBUG("RPC PrepData Container is already in StoreGate ");
    if (m_producePRDfromTriggerWords){
      if (!evtStore()->contains<Muon::RpcCoinDataContainer>(m_rpcCoinDataContainerKey.key())) {
        ATH_MSG_FATAL("Muon::RpcPrepDataContainer found while Muon::RpcCoinDataContainer not found in Event Store");
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("RPC CoinData Container is already in StoreGate ");
    }
  }
  return StatusCode::SUCCESS;
}


void Muon::RpcRdoToPrepDataTool::processTriggerHitHypothesis(RpcCoinMatrix::const_iterator itD, RpcCoinMatrix::const_iterator itD_end, bool highPtCm, 
							     // the previous arg.s are inputs 
							     bool& triggerHit,
							     unsigned short& threshold,
							     unsigned short& overlap,
							     bool& toSkip)
{
  toSkip = false;
  const RpcFiredChannel * rpcChan = (*itD);
  if ((highPtCm && rpcChan->ijk() < 2) || (rpcChan->ijk()>5)) {
    ATH_MSG_VERBOSE("RpcFiredChannel: it's a trigger hit");
    triggerHit = true;
    
    // triggerHit 
    if (!m_producePRDfromTriggerWords) {
      //skip if not storing the trigger info 
      toSkip = true;
      return;
    }
    if ( rpcChan->ijk() == 7 ) {
      // the info in ijk 7 refer to the previous CM hit with ijk  6 => skip
      toSkip = true;
      return;
    }
    if ( rpcChan->ijk() == 6 ) {
      std::string cmtype;
      // look for the subsequent ijk  7 to define threshold and overlap
      if (msgLvl(MSG::VERBOSE)) {
        cmtype    = " in low  pT CM ";
        if (highPtCm) cmtype = " in high pT CM ";
        msg( MSG::VERBOSE ) 
          <<"This hit: ijk = "<<rpcChan->ijk()<<cmtype<<" bcid is "<<rpcChan->bcid()
          <<" time is "<<rpcChan->time()<<" ch "<<rpcChan->channel()<<endmsg;
      }
      RpcCoinMatrix::const_iterator itDnext =   itD+1;
      while (itDnext != itD_end) {
        const RpcFiredChannel * rpcChanNext = (*itDnext);
        if (msgLvl(MSG::VERBOSE)) {
          msg( MSG::VERBOSE ) 
            <<"Next hit: ijk = "<<rpcChanNext->ijk()<<cmtype<<" bcid is "<<rpcChan->bcid()
            <<" time is "<<rpcChanNext->time();
          if (rpcChanNext->ijk()<7)  msg() <<" ch "<<rpcChanNext->channel()<<endmsg;
        }
        if (rpcChanNext->ijk()==7) {        
          ATH_MSG_VERBOSE("next has ijk=7 ");
          if (rpcChanNext->bcid() == rpcChan->bcid() && rpcChanNext->time() == rpcChan->time()) {
            ATH_MSG_VERBOSE("bdid/tick match; assigning thr/overlap  = "<<rpcChanNext->thr()<<"/"<<rpcChanNext->ovl());
            threshold = rpcChanNext->thr();
            overlap   = rpcChanNext->ovl();
          }
          else {
            msg( MSG::WARNING )<<"ijk =7 after a ijk = 6 BUT bdid/tick don't match - will not assign threshold/overlap "<<endmsg;
          }
          break;
        }
        else {
	  //std::cout<<"processing trigger hit with ijk = 6; next is not ijk 7"<<std::endl;
          if (rpcChanNext->ijk()==6) {
            ++itDnext;
	    //std::cout<<"next has ijk 6; try next to next"<<std::endl;
          }
          else {
            msg( MSG::WARNING )
              <<"RPC cm hit with ijk = 6 not followed by ijk = 6 or 7 - will not assign threshold / overlap"<<endmsg;
            break;
          }       
        }
      }
    }
  }
  else {  
    triggerHit = false;
    return;
  }
  ATH_MSG_VERBOSE("RPC trigger hit; ijk = "<<rpcChan->ijk()<<" threshold / overlap = "<<threshold<<"/"<<overlap);
}
