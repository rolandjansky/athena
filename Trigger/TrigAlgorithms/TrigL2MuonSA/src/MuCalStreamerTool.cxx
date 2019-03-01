/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MuCalStreamerTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
//#include "MuonRDO/RpcPadContainer.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "MuonRDO/RpcPadContainer.h"
#include "Identifier/IdentifierHash.h"

#include "MuCalDecode/CalibEvent.h"
#include "MuCalDecode/CalibUti.h"

#include "circ/Circ.h"
#include "circ/Circservice.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "CxxUtils/checker_macros.h"
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_MuCalStreamerTool("IID_MuCalStreamerTool", 1, 0);

const InterfaceID& TrigL2MuonSA::MuCalStreamerTool::interfaceID() { return IID_MuCalStreamerTool; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuCalStreamerTool::MuCalStreamerTool(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
   AthAlgTool(type,name,parent),
   m_storeGate( "StoreGateSvc", name ),
   m_regionSelector( "RegSelSvc", name ),
   m_robDataProvider( "ROBDataProviderSvc", name ),
   m_cid(-1),
   m_calibEvent(0),
   m_roi(NULL),
   m_tgcDataPreparator("TrigL2MuonSA::TgcDataPreparator")  
{
   declareInterface<TrigL2MuonSA::MuCalStreamerTool>(this);
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
   StatusCode sc;
   sc = AthAlgTool::initialize();
   if (!sc.isSuccess()) {
     ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
      return sc;
   }

   // Retrieve the RPC cabling service
   ServiceHandle<IRPCcablingServerSvc> RpcCabGet ("RPCcablingServerSvc", name());
   ATH_CHECK( RpcCabGet.retrieve() );
   ATH_CHECK( RpcCabGet->giveCabling(m_rpcCabling) );
   m_rpcCablingSvc = m_rpcCabling->getRPCCabling();
   if ( !m_rpcCablingSvc ) {
     ATH_MSG_ERROR("Could not retrieve the RPC cabling svc");
     return StatusCode::FAILURE;
   } 

   // retrieve the TGC cabling svc
   //   ServiceHandle<ITGCcablingServerSvc> TgcCabGet ("TGCCablingServerSvc", name());
   //  sc = TgcCabGet.retrieve();
   //  if ( sc != StatusCode::SUCCESS ) {
   //   ATH_MSG_ERROR("Could not retrieve the TGC cabling service");
   //   return StatusCode::FAILURE;
   //  }
   // sc = TgcCabGet->giveCabling(m_tgcCabling);
   // if ( sc != StatusCode::SUCCESS ) {
   //   ATH_MSG_ERROR("Could not retrieve the TGC Cabling Server");
   //   return sc;
   // }
   // m_tgcCablingSvc = m_tgcCabling->getTGCCabling();
   //if ( !m_tgcCablingSvc ) {
   //  ATH_MSG_ERROR("Could not retrieve the TGC cabling svc");
   //  return StatusCode::FAILURE;
   //} 

   ATH_CHECK( m_tgcDataPreparator.retrieve() );
   ATH_MSG_DEBUG("Retrieved service " << m_tgcDataPreparator); 

   // locate the region selector
   ATH_CHECK( m_regionSelector.retrieve() );
   ATH_MSG_DEBUG("Retrieved the region selector");

   // Locate ROBDataProvider
   ATH_CHECK( m_robDataProvider.retrieve() );
   ATH_MSG_DEBUG("Retrieved service " << m_robDataProvider.name()); 

   // initialize the local vector buffer
   m_localBuffer = new std::vector<int>();

   m_localBufferSize = 0;

   return StatusCode::SUCCESS; 

}


StatusCode TrigL2MuonSA::MuCalStreamerTool::finalize()
{
  ATH_MSG_DEBUG("Finalizing MuCalStreamerTool - package version " << PACKAGE_VERSION);
   
   // delete the calibration buffer
   if ( m_localBuffer ) delete m_localBuffer; 

   StatusCode sc = AthAlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuCalStreamerTool::openStream(int calBufferSize)
{

  std::string name = m_calBufferName;
  name += "_"+m_algInstanceName;

  if ( m_cid == -1 ) { 
    if (m_writeToFile) {
      m_outputFile = new std::ofstream(name.c_str());
      m_cid = 1;
      if ( !m_outputFile ) {
	ATH_MSG_WARNING("Could not open muon calibration output file, name: " << name);
      } 
      
    } 
    else {
      char* chr ATLAS_THREAD_SAFE = const_cast<char*>(name.c_str());
      m_cid = CircOpenCircConnection(0, chr, calBufferSize);
      if( m_cid == -1 ) {
	ATH_MSG_WARNING("Could not open muon calibration buffer: name="
			<< name << " buffer size=" << calBufferSize);
      }
      else {
	ATH_MSG_INFO("Opening muon calibration stream. Buffer name: " 
		     << name << " buffer size: " << calBufferSize);
      }
      
    }
  }
  else {
    ATH_MSG_WARNING("The muon calibration stream is already open, skip !");
  }


  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuCalStreamerTool::closeStream()
{

  std::string name = m_calBufferName+"_"+m_algInstanceName; 

  if (m_writeToFile && m_outputFile) {
    m_outputFile->close();
    delete m_outputFile;
    m_outputFile = NULL;
  }
  else if ( !m_writeToFile && m_cid>-1) {
    char* chr ATLAS_THREAD_SAFE = const_cast<char*>(name.c_str());
    if (CircCloseCircConnection (0,chr,m_cid) != 0 ) {
      ATH_MSG_WARNING("Could not close the muon calibration stream. Stream name: " << m_calBufferName);
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

void TrigL2MuonSA::MuCalStreamerTool::clearLocalBuffer()
{
  
  ATH_MSG_DEBUG("Local buffer size before clear: " << m_localBuffer->size());

  m_localBuffer->clear();
  m_localBufferSize = 0;

  ATH_MSG_DEBUG("Local buffer size after clear: " << m_localBuffer->size());

  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuCalStreamerTool::createRoiFragment(const LVL1::RecMuonRoI* roi,
							      TrigL2MuonSA::TrackPattern& track,
							      TrigL2MuonSA::MdtHits& mdtHits,
							      TrigL2MuonSA::RpcHits& rpcHits,
							      TrigL2MuonSA::TgcHits& tgcHits,
							      int calBufferSize,
							      bool doDataScouting,
							      bool& updateTriggerElement)
{

  // create the fragment
  // ( dummy input for now )

  // skip the event if it's a data scouting chain and it's a noise burst
  unsigned int totalHits = mdtHits.size()+rpcHits.size()+tgcHits.size();
  if ( doDataScouting && totalHits > 500 ) {
    ATH_MSG_DEBUG("Too many hits: skip the RoI");
    updateTriggerElement=false;
    return StatusCode::SUCCESS;
  } 
  
  // init roi pointer
  if ( !roi ) {
    ATH_MSG_ERROR("Roi not initialized");
    return StatusCode::FAILURE;
  }

  m_roi = roi;

  // retrieve the event and trigger info
  const EventInfo* eventInfo(0);
  StatusCode sc = m_storeGate->retrieve(eventInfo);
  if (sc.isFailure()){
    ATH_MSG_FATAL("Can't get EventInfo object");
    return StatusCode::FAILURE;
  }
  
  const EventID* eventId = eventInfo->event_ID();
  if(eventId==0) {
    ATH_MSG_ERROR("Could not find EventID object");
    return StatusCode::FAILURE;
  }
  
  const TriggerInfo* triggerInfo = eventInfo->trigger_info();
  if(triggerInfo==0) {
    ATH_MSG_ERROR("Could not find TriggerInfo object");
    return StatusCode::FAILURE;
  }
  
  uint32_t runId  = eventId->run_number();
  uint32_t lvl1Id = triggerInfo->extendedLevel1ID();

  // get track parameters
  float eta = (float) track.etaVtx;
  float phi = (float) track.phiVtx;
  float pt  = (float) track.pt;

  uint32_t mrods[4] = {0, 0, 0, 0};
  // prepare the header information
  std::vector<uint32_t> robIdList;
  double etaMin = m_roi->eta()-0.05;
  double etaMax = m_roi->eta()+0.05;
  double phi_roi = m_roi->phi();
  double phiMin = m_roi->phi()-0.05;
  double phiMax = m_roi->phi()+0.05;
  if( phi_roi < 0 ) phi_roi += 2*CLHEP::pi;     
  if( phiMin < 0 ) phiMin += 2*CLHEP::pi;  
  if( phiMax < 0 ) phiMax += 2*CLHEP::pi; 
  
  TrigRoiDescriptor roiDescr( m_roi->eta(), etaMin, etaMax, phi_roi, phiMin, phiMax );     
    
  const IRoiDescriptor* iroi = (IRoiDescriptor*) &roiDescr;                                                        
  m_regionSelector->DetROBIDListUint(MDT, *iroi, robIdList);  

  // dump the list of robs for debugging 
  int isize = robIdList.size()<5 ? robIdList.size() : 4; 
  for (int ii = 0 ; ii<isize ; ++ii ) {
    ATH_MSG_DEBUG("robId: 0x" << std::hex << robIdList.at(ii) << std::dec);
    mrods[ii] = robIdList.at(ii);
  }  
  
  // get the list of TGC robs
  std::vector<uint32_t> tgcRobIdList;
  m_regionSelector->DetROBIDListUint(TGC, *iroi, tgcRobIdList);  
  ATH_MSG_DEBUG("Size of the tgc rob list: " << tgcRobIdList.size());

  // get the list of CSC robs
  std::vector<uint32_t> cscRobIdList;
  m_regionSelector->DetROBIDListUint(CSC, *iroi, cscRobIdList);  
  ATH_MSG_DEBUG("Size of the tgc rob list: " << cscRobIdList.size());

  LVL2_MUON_CALIBRATION::CalibEvent  event(1,runId,lvl1Id,1,1,mrods,name().c_str(),eta,phi,pt);
  LVL2_MUON_CALIBRATION::MdtCalibFragment mdtFragment;

  // create the MDT fragment
  if ( mdtHits.size()>0 ) {
    sc = createMdtFragment(mdtHits,mdtFragment,phi);
    if ( sc != StatusCode::SUCCESS ) {
      ATH_MSG_ERROR("Could not create the Mdt fragment of the calibration stream");
    } 
    
    // add the mdt fragment to the event
    event << mdtFragment;
  }

  // create the RPC fragment 
  if ( rpcHits.size() > 0 ) {    
    LVL2_MUON_CALIBRATION::RpcCalibFragment rpcFragment;
    
    sc = createRpcFragment(roi, rpcFragment);
    if ( sc != StatusCode::SUCCESS ) {
      ATH_MSG_WARNING("Could not create the Rpc fragment of the calibration stream");
    } 
    else {
      ATH_MSG_DEBUG("Adding the RPC fragment to the calibration stream");
      event << rpcFragment;
    }
  }

  if ( tgcHits.size() > 0 ) {    
    LVL2_MUON_CALIBRATION::TgcCalibFragment tgcFragment;

    sc = createTgcFragment(tgcRobIdList,tgcFragment);
    if ( sc != StatusCode::SUCCESS ) {
      ATH_MSG_ERROR("Could not create the Tgc fragment of the calibration stream");
    } 
    else {
      ATH_MSG_DEBUG("Adding the TGC fragment to the calibration stream");
      event << tgcFragment;
    }
  }



  // if there is any CSC rob, add also the CSC fragment
  if ( cscRobIdList.size()>0 ) {
    
    LVL2_MUON_CALIBRATION::CscCalibFragment cscFragment;
    sc = createCscFragment(cscRobIdList,cscFragment);
    if ( sc != StatusCode::SUCCESS ) {
      ATH_MSG_ERROR("Could not create the Csc fragment of the calibration stream");
    } 
    else {
      ATH_MSG_DEBUG("Adding the CSC fragment to the calibration stream");
      event << cscFragment;
    }

  } 



  if (m_writeToFile && m_outputFile) {                  
    uint16_t eventSize = event.size();       
    if (eventSize>1000) return StatusCode::SUCCESS;
    uint8_t* buff = new uint8_t[eventSize];                                                                              
    event.dumpWords(buff,eventSize); 
    m_outputFile->write( (char*) buff, event.size() );                                                                   
    // CID 22892: DELETE_ARRAY 
    // delete buff; 
    delete [] buff; 
  }                                                                                                                        
  else if ( !doDataScouting && m_cid != -1) {  
    uint16_t eventSize = event.size();
    char* p = NULL;  
    if ((p = CircReserve (m_cid, m_calibEvent, event.size())) != (char *) -1) { 
      uint8_t* buff = reinterpret_cast<uint8_t*>(p); 
      uint16_t eventSize8bits = eventSize;
      // encode the event
      event.dumpWords(buff,eventSize); 
      
      // dump the encoded event to the screen
      uint16_t eventSize32bits = eventSize8bits/4;
      ATH_MSG_DEBUG("Size of the CIRCULAR buffer in 8 and 32 bits words: " << eventSize8bits << " " 
		    << eventSize32bits);
      for ( uint16_t words = 0 ; words != eventSize32bits ; words++)  {                 
	uint32_t byte1 = *(buff+words*4);     
	uint32_t byte2 = *(buff+1+words*4); 
	uint32_t byte3 = *(buff+2+words*4); 
	uint32_t byte4 = *(buff+3+words*4);                                                                                  
	ATH_MSG_DEBUG("byte1 = 0x" << std::hex << (byte1) << std::dec);
	ATH_MSG_DEBUG("byte2 = 0x" << std::hex << (byte2) << std::dec);
	ATH_MSG_DEBUG("byte3 = 0x" << std::hex << (byte3) << std::dec);
	ATH_MSG_DEBUG("byte4 = 0x" << std::hex << (byte4) << std::dec);

      	uint32_t dataWord = (byte4 << 24) + (byte3 << 16) + (byte2 << 8) + byte1 ;
	
      	//	std::cout << "Number of data words: " << words << std::endl;
      	ATH_MSG_DEBUG("Data word " << words << " = " << std::hex << "0x" << dataWord << std::dec);
      	
	
      } 
      
      // dump the words to the circular buffer   
      CircValidate (m_cid, m_calibEvent, p, event.size() ); 
      
    }                          
    else {  
      ATH_MSG_DEBUG("Could not dump the event in the calibration stream circular buffer");
    }  
  }                                                                                                                        
  else if ( doDataScouting ) { 

    uint16_t eventSize_ds = event.size();  
    if (eventSize_ds>1000) return StatusCode::SUCCESS;

    uint8_t* buff_ds = new uint8_t[eventSize_ds]; 

    // encode the event                                                                                                    
    uint16_t eventSize8bits = eventSize_ds; 
    uint16_t eventSize32bits = eventSize8bits/4;
    event.dumpWords(buff_ds,eventSize_ds); 

    // fill the local buffer 
    // dump the words also in the local buffer     
    // dump the encoded event to the screen
    ATH_MSG_DEBUG("Size of the DATASCOUTING buffer in 32 bits words: " << eventSize32bits);
    for ( uint16_t words = 0 ; words != eventSize32bits ; words++)  {                 
      uint32_t byte1 = *(buff_ds+words*4);  
      uint32_t byte2 = *(buff_ds+1+words*4); 
      uint32_t byte3 = *(buff_ds+2+words*4);  
      uint32_t byte4 = *(buff_ds+3+words*4); 

      // encoding in big-endian for now ( revert order for little-endian )
      uint32_t dataWord = (byte4 << 24) + (byte3 << 16) + (byte2 << 8) + byte1 ;
      //	std::cout << "Number of data words: " << words << std::endl;
      ATH_MSG_DEBUG("Data word " << words << " = " << std::hex << "0x" << dataWord << std::dec);
      
      m_localBuffer->push_back(dataWord);
    }
    m_localBufferSize += eventSize32bits; 

    if ( m_localBufferSize< calBufferSize ) { 
      ATH_MSG_DEBUG("Local buffer size = " << m_localBufferSize);
      ATH_MSG_DEBUG("Trigger element not to be updated yet ");
      updateTriggerElement = false; 
    }                                                                                                                      
    else {
      ATH_MSG_DEBUG("Local buffer size = " << m_localBufferSize);
      ATH_MSG_DEBUG("Attach the buffer to the trigger element ");

      updateTriggerElement = true;
    } 

    delete [] buff_ds;
  }                
                                                                                                      
  m_calibEvent++;
  
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

//////////////////////////////////////////////////////////////////////////////////////////
//
// prepare the Rpc fragment of the stream
//
//////////////////////////////////////////////////////////////////////////////////////////
StatusCode TrigL2MuonSA::MuCalStreamerTool::createRpcFragment(const LVL1::RecMuonRoI* roi,
							      LVL2_MUON_CALIBRATION::RpcCalibFragment& rpcFragment)
{

  // exit if it's not in the barrel
  if ( roi->sysID() != 0 ) return StatusCode::SUCCESS;
 
  // retrieve the padId from the RecMuonRoi
  unsigned int    roIWord=roi->roiWord();

  //  decode  roIWord
  unsigned int sectorAddress = (roIWord & 0x003FC000) >> 14;
  unsigned int sectorRoIOvl  = (roIWord & 0x000007FC) >> 2;
  unsigned int side =  sectorAddress & 0x00000001;
  unsigned int sector = (sectorAddress & 0x0000003e) >> 1;
  unsigned int roiNumber =  sectorRoIOvl & 0x0000001F;
  //  unsigned int padNumber = roiNumber/4; 
  

  unsigned int logic_sector;
  unsigned short int PADId;
  Identifier padId;
  
  // retrieve the pad container
  const RpcPadContainer* rpcPadContainer; 
  StatusCode sc = m_storeGate->retrieve(rpcPadContainer,"RPCPAD");
  if ( sc != StatusCode::SUCCESS ) { 
    ATH_MSG_ERROR("Could not retrieve the ");
    return sc;
  }

//  std::cout << ">>>>>>>>>>>>>>>>>>> ROI PARAMETERS" << std::endl;
//  std::cout << "sectorAddress: " << sectorAddress << std::endl;
//  std::cout << "sectorRoIOvl : " << sectorRoIOvl << std::endl;
//  std::cout << "side         : " << side << std::endl;
//  std::cout << "sector       : " << sector << std::endl;
//  std::cout << "roiNumber    : " << roiNumber << std::endl;
//  std::cout << "padNumber    : " << padNumber << std::endl;

  unsigned int padIdHash;

  if ( m_rpcCablingSvc->give_PAD_address( side, sector, roiNumber, logic_sector, PADId, padIdHash) ) {

    RpcPadContainer::const_iterator itPad = rpcPadContainer->indexFind(padIdHash);  
    if( itPad==rpcPadContainer->end() ) {        
      ATH_MSG_WARNING("Failed to retrieve PAD hash Id " << padIdHash);  
      return StatusCode::FAILURE;                         
    } 

    const RpcPad* rpcPad = *itPad;
    //unsigned int subsys_id = (subsystemID==1)? 0x65 : 0x66;
    //  unsigned int robId     = (subsys_id << 16) | (sectorID/2);
    
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
      ATH_MSG_WARNING("Can't initialize the RpcPad");
      return StatusCode::FAILURE;
    }
  }
  else {
    ATH_MSG_WARNING("Can't get the pad address from the rpc cabling service");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//
// prepare the TGC fragment of the stream
//
////////////////////////////////////////////////////////////////////////////////
StatusCode TrigL2MuonSA::MuCalStreamerTool::createTgcFragment(std::vector<uint32_t>& tgcRobIdList,
							      LVL2_MUON_CALIBRATION::TgcCalibFragment& tgcFragment)
{


  if ( tgcRobIdList.size()<1 ) {
    ATH_MSG_DEBUG("No TGC Rob found");
    return StatusCode::SUCCESS;
  } 

  // system Id = 1 should correspond to the endcap
  uint16_t systemId = 1;
  // this is the subdetector Id
  uint16_t subSystemId = ( (tgcRobIdList.at(0) & 0xff0000) >> 16 );
  uint16_t rdoId = ( tgcRobIdList.at(0) & 0xff );

  uint16_t roiNumber = 0;

  tgcFragment = LVL2_MUON_CALIBRATION::TgcCalibFragment(systemId,subSystemId,rdoId,roiNumber);

  // get the list of hash ID's from the TGC data preparator
  std::vector<IdentifierHash> tgcHashList = m_tgcDataPreparator->getHashList();

  // retrieve the tgcrdo container
  const TgcRdoContainer* tgcRdoContainer = Muon::MuonRdoContainerAccess::retrieveTgcRdo("TGCRDO");
  if( tgcRdoContainer==0 ) {
    ATH_MSG_DEBUG("Tgc RDO container not registered by MuonRdoContainerManager");
    ATH_MSG_DEBUG("-> Retrieving it from the StoreGate");
    StatusCode sc = m_storeGate->retrieve(tgcRdoContainer, "TGCRDO");
    if( sc.isFailure() ) {
      ATH_MSG_ERROR("Could not retrieve the TgcRdoContainer");
      return sc;
    }
  }

  // now get the list of ROB Ids and from the the subdetector ID ( that corresponds to the
  // subsystem ID
  

  LVL2_MUON_CALIBRATION::TgcCalibData tgc_payload;
  TgcRdoContainer::const_iterator it = tgcRdoContainer->begin();
  
  for ( ; it != tgcRdoContainer->end() ; ++it )  {

    ATH_MSG_DEBUG("Adding the hits from a new TGC chamber");

    // loop on the element of the TGC RDO
    ATH_MSG_DEBUG("Number of hits: " << (*it)->size());

    TgcRdo::const_iterator itRaw = (*it)->begin();
    for ( ; itRaw != (*it)->end() ; ++itRaw )  {

      const TgcRawData* raw = (*itRaw);
      LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTHIT roh;
      LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTTRIPLETSTRIP tps;
      LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_READOUTTRACKLET rot;
      LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_HIPT hpt;
      LVL2_MUON_CALIBRATION::TGC_BYTESTREAM_SL sl;


      switch (raw->type()) {
      case TgcRawData::TYPE_HIT:
	{
	  roh.channel = raw->channel()-40;
	  roh.sbId = raw->slbId();
	  roh.sbType = raw->slbType();
	  roh.adj = raw->isAdjacent();
	  roh.ldbId = raw->sswId();
	  roh.bcBitmap = bcBitmap(raw->bcTag());
	  roh.tracklet = raw->tracklet();
	  roh.ok = 1;
	  
	}
	break;
      case TgcRawData::TYPE_TRACKLET:
	if (raw->slbType() == TgcRawData::SLB_TYPE_TRIPLET_STRIP)
	  {
	    tps.phi = raw->position();
	    tps.seg = raw->segment();
	    tps.sbId = raw->slbId();
	    tps.ldbId = raw->sswId();
	    tps.bcBitmap = bcBitmap(raw->bcTag());
	    tps.slbType = TgcRawData::SLB_TYPE_TRIPLET_STRIP;
	    tps.ok = 1;
	  }
	else
	  {
	    rot.rphi = raw->position();
	    rot.subm = raw->subMatrix();
	    rot.seg = raw->segment();
	    rot.delta = raw->delta();
	    rot.sbId = raw->slbId();
	    rot.ldbId = raw->sswId();
	    rot.bcBitmap = bcBitmap(raw->bcTag());
	    rot.slbType = raw->slbType();
	    if (raw->slbType() == TgcRawData::SLB_TYPE_INNER_STRIP)
	      rot.slbType = 4;
	    rot.ok = 1;
	  }
	break;
      case TgcRawData::TYPE_HIPT:
	{
	  hpt.delta = raw->delta();
	  hpt.hitId = raw->hitId();
	  hpt.hipt = raw->isHipt();
	  hpt.cand = raw->index();
	  hpt.chip = raw->chip();
	  hpt.sector = raw->sector();
	  hpt.fwd = raw->isForward();
	  hpt.strip = raw->isStrip();
	  hpt.bcBitmap = bcBitmap(raw->bcTag());
	}
	break;
      case TgcRawData::TYPE_SL:
	{
	  sl.roi = raw->roi();
	  sl.overlap = raw->isOverlap();
	  sl.threshold = raw->threshold();
	  sl.sign = raw->isMuplus();
	  sl.cand = raw->index();
	  sl.sector = raw->sector();
	  sl.fwd = raw->isForward();
	  sl.bcBitmap = bcBitmap(raw->bcTag());
	  sl.cand2plus = raw->cand3plus();
	}
	break;
      default:
	{
	  ATH_MSG_ERROR("Invalid TgcRawData type: " << raw->type());
	  return StatusCode::FAILURE;
	}
      }	
      
      tgc_payload.getHit(roh,tps,rot,hpt,sl);
      
    }
  }
 
  tgcFragment << tgc_payload;
  
  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
//
// prepare the CSC fragment of the stream
//
////////////////////////////////////////////////////////////////////////////////
StatusCode TrigL2MuonSA::MuCalStreamerTool::createCscFragment(std::vector<uint32_t>& cscRobIdList,
							      LVL2_MUON_CALIBRATION::CscCalibFragment& cscFragment) 
{
  
  // retreive the csc rob data
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFragments;
  m_robDataProvider->getROBData(cscRobIdList,robFragments);

  // transfer the rob data to the CSC fragment
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>::const_iterator it;
  for ( it = robFragments.begin() ; it != robFragments.end() ; ++it ) {
    
    LVL2_MUON_CALIBRATION::CscCalibData CscData;
    uint32_t rod_words     = (**it).rod_fragment_size_word(); 
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType wr;
    (*it)->rod_start(wr); 

    for (uint32_t i=0;i<rod_words;++i) {
      // CID 22907: CAST_TO_QUALIFIED_TYPE
      // CscData << static_cast<const uint32_t>(*(wr+i));
      CscData << static_cast<uint32_t>(*(wr+i));
    }            
    cscFragment << CscData;
  }

  return StatusCode::SUCCESS;
}

