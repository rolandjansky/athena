/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MuCalStreamerTool.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"

#include "MuCalDecode/CalibEvent.h"
#include "MuCalDecode/CalibUti.h"

#include "circ/Circ.h"
#include "circ/Circservice.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_MuCalStreamerTool("IID_MuCalStreamerTool", 1, 0);

const InterfaceID& TrigL2MuonSA::MuCalStreamerTool::interfaceID() { return IID_MuCalStreamerTool; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuCalStreamerTool::MuCalStreamerTool(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
   AlgTool(type,name,parent),
   m_msg(0),
   m_storeGate( "StoreGateSvc", name ),
   m_robDataProvider(0),
   m_cid(-1),
   m_iRpcCablingSvc(0),
   m_rpcRawDataProvider("Muon__RPC_RawDataProviderTool"),
   m_rpcGeometrySvc("RPCgeometrySvc",""),
   m_rpcCabling(0),
   m_calibEvent(0),
   m_roi(NULL)
{
   declareInterface<TrigL2MuonSA::MuCalStreamerTool>(this);

   declareProperty("WriteToFile", m_writeToFile=true);
   declareProperty("CalBufferName", m_calBufferName="/tmp/rosati/testOutput");
   declareProperty("CalBufferSize", m_calBufferSize=1024*1024);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuCalStreamerTool::~MuCalStreamerTool() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuCalStreamerTool::initialize()
{
   // Get a message stream instance
   m_msg = new MsgStream( msgSvc(), name() );
   msg() << MSG::DEBUG << "Initializing MuCalStreamerTool - package version " << PACKAGE_VERSION << endreq ;
   
   StatusCode sc;
   sc = AlgTool::initialize();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not initialize the AlgTool base class." << endreq;
      return sc;
   }

   // Locate ROBDataProvider
   std::string serviceName = "ROBDataProvider";
   IService* svc = 0;
   sc = service("ROBDataProviderSvc", svc);
   if(sc.isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve " << serviceName << endreq;
      return sc;
   }
   m_robDataProvider = dynamic_cast<ROBDataProviderSvc*> (svc);
   if( m_robDataProvider == 0 ) {
      msg() << MSG::ERROR << "Could not cast to ROBDataProviderSvc " << endreq;
      return StatusCode::FAILURE;
   }
   msg() << MSG::DEBUG << "Retrieved service " << serviceName << endreq;

   // RPC cabling
   const IRPCcablingServerSvc* pRpcCabGet = 0;
   sc = service("RPCcablingServerSvc",pRpcCabGet,true);
   if( sc.isFailure() ) {
      msg() << MSG::ERROR << "Unable to retrieve the RPC cabling Server Service" << endreq;
      return sc;
   }
   sc = pRpcCabGet->giveCabling(m_iRpcCablingSvc);
   if( sc.isFailure() ) {
      msg() << MSG::ERROR << "Unable to retrieve the RPC cabling Service from Server" << endreq;
      return sc;
    }
   m_rpcCabling = m_iRpcCablingSvc->getRPCCabling();
   // 

   return StatusCode::SUCCESS; 

}


StatusCode TrigL2MuonSA::MuCalStreamerTool::finalize()
{
   msg() << MSG::DEBUG << "Finalizing MuCalStreamerTool - package version " << PACKAGE_VERSION << endreq;
   
   // delete message stream
   if ( m_msg ) delete m_msg;
   
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuCalStreamerTool::openStream()
{

  std::string name = m_calBufferName;
  name += "_"+m_algInstanceName;

  if ( m_cid == -1 ) { 
    if (m_writeToFile) {
      m_outputFile = new std::ofstream(name.c_str());
      m_cid = 1;
      if ( !m_outputFile ) {
	msg() << MSG::WARNING << "Could not open muon calibration output file, name: "
	      << m_calBufferName << endreq;
      } 
      
    } 
    else {
      
      m_cid = CircOpenCircConnection(0, (char*)name.c_str(), m_calBufferSize);
      if( m_cid == -1 ) {
	msg() << MSG::WARNING << "Could not open muon calibration buffer: name="
	      << m_calBufferName << " buffer size=" << m_calBufferSize << endreq;
      }
      
    }
  }
  else {
    msg() << MSG::WARNING << "The muon calibration stream is already open, skip !" << endreq;
  }


  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuCalStreamerTool::closeStream()
{

  std::cout << "AAAAA >> In the closeStream function of the tool m_cid = " << m_cid << std::endl;
  std::string name = m_calBufferName; 
  if (m_writeToFile && m_outputFile) {
    m_outputFile->close();
    delete m_outputFile;
  }
  else if (!m_writeToFile && m_cid>-1) {

    std::cout << "AAAAA >> Now closing the circular buffer, m_cid== " << m_cid << std::endl;

    if (CircCloseCircConnection (0,(char*)name.c_str(),m_cid) != 0 ) {
      msg() << MSG::WARNING << "Could not close the muon calibration stream. Stream name: " 
	    << m_calBufferName << endreq;
    }
    else {
      //
      // reset the cid to avoid multiple closure attempts
      //
      m_cid = -1;
    }

  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuCalStreamerTool::createRoiFragment(const LVL1::RecMuonRoI* roi,
							      TrigL2MuonSA::TrackPattern& track,
							      TrigL2MuonSA::MdtHits& mdtHits,
							      TrigL2MuonSA::RpcHits& rpcHits)
{

  // create the fragment
  // ( dummy input for now )
  
  // init roi pointer
  if ( !roi ) {
    msg() << MSG::ERROR << "Roi not initialized" << endreq;
    return StatusCode::FAILURE;
  }

  m_roi = roi;

  // retrieve the event and trigger info
  const EventInfo* eventInfo(0);
  StatusCode sc = m_storeGate->retrieve(eventInfo);
  if (sc.isFailure()){
    msg() << MSG::FATAL << "Can't get EventInfo object" << endreq;
    return StatusCode::FAILURE;
  }
  
  const EventID* eventId = eventInfo->event_ID();
  if(eventId==0) {
    msg() << MSG::ERROR << "Could not find EventID object" << endreq;
    return StatusCode::FAILURE;
  }
  
  const TriggerInfo* triggerInfo = eventInfo->trigger_info();
  if(triggerInfo==0) {
    msg() << MSG::ERROR << "Could not find TriggerInfo object" << endreq;
    return StatusCode::FAILURE;
  }
  
  uint32_t runId  = eventId->run_number();
  uint32_t lvl1Id = triggerInfo->extendedLevel1ID();

  // get track parameters
  float eta = (float) track.etaVtx;
  float phi = (float) track.phiVtx;
  float pt  = (float) track.pt;


  uint32_t mrods[4];
  LVL2_MUON_CALIBRATION::CalibEvent  event(1,runId,lvl1Id,1,1,mrods,name().c_str(),eta,phi,pt);

  LVL2_MUON_CALIBRATION::MdtCalibFragment mdtFragment;

  // create the MDT fragment
  if ( mdtHits.size()>0 ) {
    sc = createMdtFragment(mdtHits,mdtFragment,phi);
    if ( sc != StatusCode::SUCCESS ) {
      msg() << MSG::ERROR << "Could not create the Mdt fragment of the calibration stream" << endreq;
    } 
    
    // add the mdt fragment to the event
    event << mdtFragment;
  }

  if ( rpcHits.size() > 0 ) {    
    // create the RPC fragment 
    LVL2_MUON_CALIBRATION::RpcCalibFragment rpcFragment;
    
    sc = createRpcFragment(rpcFragment);
    if ( sc != StatusCode::SUCCESS ) {
      msg() << MSG::ERROR << "Could not create the Rpc fragment of the calibration stream" << endreq;
    } 
    event << rpcFragment;
  }


  if (m_writeToFile && m_outputFile) {
    if ( m_algInstanceName=="MuFastSteering_900GeV" ) { 
      uint16_t size = event.size();
      uint8_t* buff = new uint8_t[size];
      event.dumpWords(buff,size);
      m_outputFile->write( (char*) buff, event.size() );
      // CID 22892: DELETE_ARRAY
      // delete buff;
      delete [] buff;
    }
  }
  else if(m_cid != -1) {
    uint16_t size = event.size();
    char* p;
    if ((p = CircReserve (m_cid, m_calibEvent, event.size())) != (char *) -1) {
      uint8_t* buff = reinterpret_cast<uint8_t*>(p);
      event.dumpWords(buff,size);
      CircValidate (m_cid, m_calibEvent, p, event.size() );
      m_calibEvent++;
    }
    else {
      msg() << MSG::ERROR << "Could not dump the event in the calibration stream" << endreq;
    
    }
    
  }

  return StatusCode::SUCCESS;
}

//
// prepare the Mdt fragment of the stream
//
StatusCode TrigL2MuonSA::MuCalStreamerTool::createMdtFragment(TrigL2MuonSA::MdtHits& mdtHits,   
							      LVL2_MUON_CALIBRATION::MdtCalibFragment& mdtFragment, 
							      float trackPhi)
{
  
  TrigL2MuonSA::MdtHits::const_iterator it = mdtHits.begin();
  for ( ; it != mdtHits.end() ; ++it ) {

    int stationName = (*it).name;
    int stationEta  = (*it).StationEta;
    int stationPhi  = (*it).StationPhi;
    int multilayer  = (*it).Multilayer;
    int layer       = (*it).TubeLayer;
    int tube        = (*it).Tube;

    uint32_t ID = LVL2_MUON_CALIBRATION::MdtIdIntoWord(stationName,
						       stationEta,stationPhi,multilayer,layer,tube);

    uint16_t  leadingCoarseTime  = (*it).LeadingCoarseTime;
    uint16_t  leadingFineTime    = (*it).LeadingFineTime;
    uint16_t  trailingCoarseTime = (*it).TrailingCoarseTime;
    uint16_t  trailingFineTime   = (*it).TrailingFineTime;
    uint16_t  adc                = (*it).Adc;

    LVL2_MUON_CALIBRATION::MdtCalibData mdt(ID,leadingCoarseTime,leadingFineTime,
					    trailingCoarseTime,trailingFineTime,adc
					    ,trackPhi);

    mdtFragment << mdt;

  }
  
  return StatusCode::SUCCESS;
}

//
// prepare the Rpc fragment of the stream
//
StatusCode TrigL2MuonSA::MuCalStreamerTool::createRpcFragment(LVL2_MUON_CALIBRATION::RpcCalibFragment& rpcFragment)
{
  
  // access the pad ( to be replaced by the usage of RpcDataPreparator
  unsigned short int subsystemID =  m_roi->subsysID();
  unsigned short int sectorID    =  m_roi->sectorID();
  unsigned short int roiNumber   =  m_roi->getRoINumber();
  
  const RpcPad* rpcPad = 0;
  unsigned int subsys_id = (subsystemID==1)? 0x65 : 0x66;
  unsigned int robId     = (subsys_id << 16) | (sectorID/2);
  
  // now retrieve the rpc pad correspondig to the RoI
  StatusCode sc = getRpcPad(robId,subsystemID,sectorID,roiNumber,rpcPad);
  if( sc != StatusCode::SUCCESS ) {
    msg() << MSG::ERROR << "getRpcPad failed: sc=" << sc << endreq;
    return sc;
  }
  
  if(rpcPad) {
    uint16_t sector = rpcPad->sector();
    uint16_t sysId  = (sector<32)? 0x66 : 0x65;
    uint16_t secId  = sector%32;
    uint16_t padId  = rpcPad->onlineId();
    uint16_t status = rpcPad->status();
    uint16_t error  = rpcPad->errorCode();
    
    LVL2_MUON_CALIBRATION::RpcCalibFragment frag(sysId,secId,padId,status,error);
    rpcFragment = frag;

    RpcPad::const_iterator it3 = rpcPad->begin(); 
    for (; it3!=rpcPad->end() ; ++it3) {
      const RpcCoinMatrix * cma = (*it3);
      
      uint16_t cmaId  = cma->onlineId();
      uint16_t fel1Id = cma->fel1Id();
      uint16_t febcId = cma->febcId();
      uint16_t crc    = cma->crc();
      
      LVL2_MUON_CALIBRATION::RpcCalibData matrix(cmaId,fel1Id,febcId,crc);
             
      //for each cma for over fired channels
      RpcCoinMatrix::const_iterator it5 = (*it3)->begin(); 
      for (; it5 != (*it3)->end() ; ++it5) {
	const RpcFiredChannel * fChannel = (*it5);
	
	uint16_t bcid    = fChannel->bcid();
	uint16_t time    = fChannel->time();
	uint16_t ijk     = fChannel->ijk();
	uint16_t channel = fChannel->channel();
	uint16_t ovl     = fChannel->ovl();
	uint16_t thr     = fChannel->thr();
        
	if(ijk!=7) matrix.getHit(bcid,time,ijk,channel);
	else       matrix.getHit(bcid,time,ijk,ovl,thr);

      } // loop on the matrix hits

      rpcFragment << matrix;
    } // loop on the pad matrices
    
  }
  else {

  }
     

  return StatusCode::SUCCESS;
}

//
// prepare the Mdt fragment of the stream
//
StatusCode TrigL2MuonSA::MuCalStreamerTool::createTgcFragment(TrigL2MuonSA::TgcHits& tgcHits,   
							      LVL2_MUON_CALIBRATION::TgcCalibFragment& tgcFragment)
{

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
//
// retrieve the rpc pad ( to be replaced by the direct usage of the RpcDataPreparator
//
StatusCode TrigL2MuonSA::MuCalStreamerTool::getRpcPad(unsigned int robId, unsigned short int subsystemID,
						      unsigned short int sectorID, unsigned short int roiNumber,
						      const RpcPad* rpcPad)
{
   std::vector<uint32_t> v_robIds;
   v_robIds.push_back(robId);
   
   m_robDataProvider->addROBData(v_robIds);

   rpcPad = 0;

   std::cout << ">>>>> RPC debug " << std::endl;
   std::cout << "robId: 0x" << std::hex << robId << std::dec << std::endl;
   std::cout << "subsystemId: " << subsystemID << std::endl;
   std::cout << "sectodID: " << sectorID << std::endl;
   std::cout << "roiNumber: " << roiNumber << std::endl;

   std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> v_robFragments;
   
   const RpcPadContainer* pRpcPadContainer = Muon::MuonRdoContainerAccess::retrieveRpcPad("RPCPAD");
   if(pRpcPadContainer==0) {
      msg() << MSG::ERROR << "Rpc PAD container not registered by MuonRdoContainerManager" << endreq;
      msg() << MSG::ERROR << "-> Retrieving it from the StoreGate" << endreq;
      StatusCode sc = m_storeGate->retrieve(pRpcPadContainer, "RPCPAD");
      if( sc.isFailure() ) {
	 msg() << MSG::ERROR << "Retrieval of RpcPadContainer failed" << endreq;
	 return sc;
      }
   }

   m_robDataProvider->getROBData(std::vector<uint32_t>(1,robId),v_robFragments);

   Identifier rpc_pad_id;       // identifier for accessing the RPC PAD
   unsigned int logic_sector = 0;
   unsigned short int PADId  = 0;
   bool success = m_rpcCabling->give_PAD_address(subsystemID,sectorID,roiNumber,logic_sector,PADId,rpc_pad_id);
   std::cout << "The logic sector is: " << logic_sector << std::endl;
   std::cout << "The PADid is: " << PADId << std::endl;
 
   if(!success) {
      msg() << MSG::ERROR << "error in give_PAD_address" << endreq;
      return StatusCode::FAILURE;
   }

   RpcPadIdHash padHashFunction = *(m_iRpcCablingSvc->padHashFunction());
   IdentifierHash pad_id = padHashFunction(rpc_pad_id);

   if( m_rpcRawDataProvider->convert(v_robFragments,std::vector<IdentifierHash>(1,pad_id)).isFailure()) {
      msg() << MSG::ERROR << "Failed to convert RPC PAD ID=" << pad_id << endreq;
      return StatusCode::FAILURE;
   }

   RpcPadContainer::const_iterator itPad = pRpcPadContainer->indexFind(pad_id);
   if( itPad==pRpcPadContainer->end() ) {
      msg() << MSG::ERROR << "Failed to retrieve PAD hash Id " << pad_id << endreq;
      return StatusCode::FAILURE;
   }

   rpcPad = *itPad;
   
   return StatusCode::SUCCESS;
}


