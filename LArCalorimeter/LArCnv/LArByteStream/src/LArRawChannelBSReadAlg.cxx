#include "LArRawChannelBSReadAlg.h"
#include "LArIdentifier/LArOnlineID.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "eformat/Version.h"
#include "eformat/index.h"


#include "LArByteStream/LArRodBlockStructure.h"
#include "LArByteStream/LArRodBlockPhysicsV5.h"
#include "LArByteStream/LArRodBlockPhysicsV6.h"


LArRawChannelBSReadAlg::LArRawChannelBSReadAlg(const std::string& name, ISvcLocator* pSvcLocator) :  
  AthReentrantAlgorithm(name, pSvcLocator) {}

  

StatusCode LArRawChannelBSReadAlg::initialize() {
  if (m_rawChannelKey.key().size()>0) {
    ATH_CHECK(m_rawChannelKey.initialize());
  }
  else {
    m_doRawChannels=false;
  }

  if (m_digitKey.key().size()>0) {
    ATH_CHECK(m_digitKey.initialize());
  }
  else {
    m_doDigits=false;
  }

  if (m_febHeaderKey.key().size()>0) {
    ATH_CHECK(m_febHeaderKey.initialize());
  }
  else {
    m_doFebHeaders=false;
  }
  

  ATH_CHECK(m_robDataProviderSvc.retrieve());
  ATH_CHECK(detStore()->retrieve(m_onlineId,"LArOnlineID"));  
  return StatusCode::SUCCESS;
}     
  
StatusCode LArRawChannelBSReadAlg::finalize() {
  return StatusCode::SUCCESS;
} 

StatusCode LArRawChannelBSReadAlg::execute(const EventContext& ctx) const {

  //Write output via write handle
  SG::WriteHandle<LArRawChannelContainer>outputContainer(m_rawChannelKey,ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<LArRawChannelContainer>()));

  const RawEvent* fullEvent=m_robDataProviderSvc->getEvent(ctx);
  std::map<eformat::SubDetectorGroup, std::vector<const uint32_t*> > rawEventTOC;
  eformat::helper::build_toc(*fullEvent, rawEventTOC);
  auto larRobs=rawEventTOC.find(eformat::LAR);
  if (larRobs==rawEventTOC.end()) {
     ATH_MSG_DEBUG("No LAr data found in this event. Recording empty LArRawChannelContainer");
     return StatusCode::SUCCESS;
  } 
  
  
  std::unique_ptr<LArRodBlockStructure> rodBlock;
  uint16_t rodMinorVersion=0x0;
  uint32_t rodBlockType=0x0;


  for (const uint32_t* robPtr : larRobs->second) {
    OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment rob(robPtr);
    ATH_MSG_VERBOSE("Decoding ROB fragment 0x" << std::hex << rob.rob_source_id () << " with " << std::dec << rob.rod_fragment_size_word() << "ROB words");

    if (rob.rod_fragment_size_word() <3) {
      ATH_MSG_ERROR("Encountered corrupt ROB fragment, less than 3 words!");
      if (m_failOnCorruption) {
	return StatusCode::FAILURE;
      }else 
	continue;
    }
    
     eformat::helper::Version ver(rob.rod_version());
    //(re-)init rodBlock only once per event or if (very unlikly or even impossible) some FEBs have a differnt firmware
    if (rodBlock==nullptr || rodMinorVersion !=ver.minor_version() || rodBlockType!=(rob.rod_detev_type()&0xff)) {
      rodMinorVersion=ver.minor_version();
      rodBlockType=rob.rod_detev_type()&0xff;
      ATH_MSG_VERBOSE("Found version " << rodMinorVersion <<  " of Rod Block Type  " <<  rodBlockType);
      if (rodBlockType==4) { //Physics mode
	switch(rodMinorVersion) {
	case 12: //Physics mode v6 09.03.2011 for LHC
	  rodBlock.reset(new LArRodBlockPhysicsV6);
	  break;
	case 11: //Physics mode v5 16.06.2008 for LHC 
	case 10: //Physics mode v5 16.06.2008 for LHC 
	  rodBlock.reset(new LArRodBlockPhysicsV5);
	  break;
	default:
	  ATH_MSG_ERROR("Found unsupported ROD Block version " << rodMinorVersion 
			<< " of ROD block type " << rodBlockType);
	  return m_failOnCorruption ? StatusCode::FAILURE : StatusCode::SUCCESS;
	}// end switch(rodMinorVersion)
      }//end of rodBlockType==4
      else {
	ATH_MSG_ERROR("Found unsupported Rod block type " << rodBlockType);
	return m_failOnCorruption ? StatusCode::FAILURE : StatusCode::SUCCESS;
      }
    }//End if need to re-init RodBlock

    const uint32_t* pData=rob.rod_data();
    const uint32_t  nData=rob.rod_ndata();
    if (!rodBlock->setFragment(pData,nData)) {
      ATH_MSG_ERROR("Failed to assign fragment pointer to LArRodBlockStructure");
      return StatusCode::FAILURE;
    }

    if(m_verifyChecksum) {
      const uint32_t onsum  = rodBlock->onlineCheckSum();
      const uint32_t offsum = rodBlock->offlineCheckSum();
      if(onsum!=offsum) {
	ATH_MSG_ERROR("Checksum error:");
	ATH_MSG_ERROR("online checksum  = 0x" << MSG::hex << onsum);
	ATH_MSG_ERROR("offline checksum = 0x" << MSG::hex << offsum << MSG::dec);
	if (m_failOnCorruption) 
	  return StatusCode::FAILURE;
	else
	  continue; //Jump to the next ROD-block
      }
    }

    //Loop over FEBs in ROD:
    do {
      HWIdentifier fId(Identifier32(rodBlock->getFEBID()));
      if (!m_onlineId->isValidId(fId)) {
	ATH_MSG_WARNING("Invalid FEB identifer 0x" << std::hex << fId.get_identifier32().get_compact() 
			<< ". Skipping");
	continue;
      }
      
      const int NthisFebChannel=m_onlineId->channelInSlotMax(fId);
      int32_t energy;
      int32_t time;
      int32_t quality;
      uint32_t gain;
      int fcNb;
      while (rodBlock->getNextEnergy(fcNb,energy,time,quality,gain)) {
	if (fcNb>=NthisFebChannel)
	  continue;

	HWIdentifier cId = m_onlineId->channel_Id(fId,fcNb);
        uint16_t iquality = 0;
        uint16_t iprovenance = 0x1000;
        if (quality>0) {
            iprovenance |= 0x2000;
            iquality = (quality & 0xFFFF);
        } 
	outputContainer->emplace_back(cId, energy, time, iquality, iprovenance, (CaloGain::CaloGain)gain);
      }//end getNextEnergyLoop
    }while (rodBlock->nextFEB()); //Get NextFeb
  } //end loop over ROBs
  return StatusCode::SUCCESS;
}
