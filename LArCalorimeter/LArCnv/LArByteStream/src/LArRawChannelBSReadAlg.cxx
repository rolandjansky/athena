#include "LArRawChannelBSReadAlg.h"
#include "LArIdentifier/LArOnlineID.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "LArRawEvent/LArRawChannelContainer.h"
#include "eformat/Version.h"
#include "eformat/index.h"
//#include "eformat/Issue.h"


#include "LArByteStream/LArRodBlockStructure.h"
#include "LArByteStream/LArRodBlockPhysicsV5.h"
#include "LArByteStream/LArRodBlockPhysicsV6.h"


LArRawChannelBSReadAlg::LArRawChannelBSReadAlg(const std::string& name, ISvcLocator* pSvcLocator) :  
  AthReentrantAlgorithm(name, pSvcLocator) {}

  

StatusCode LArRawChannelBSReadAlg::initialize() {
  ATH_CHECK(m_rawChannelKey.initialize());
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
  
  for (const uint32_t* robPtr : larRobs->second) {
    OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment rob(robPtr);
    ATH_MSG_VERBOSE("Decoding ROB fragment 0x" << std::hex << rob.rob_source_id () << " with " << std::dec << rob.rod_fragment_size_word() << "ROB words");

    if (rob.rod_fragment_size_word() <3) {
      ATH_MSG_ERROR("Encountered corrupt ROB fragment, less than 3 words!");
      return StatusCode::FAILURE;
    }
    
    const uint32_t* pData=rob.rod_data();
    const uint32_t  nData=rob.rod_ndata();
 
    eformat::helper::Version ver(rob.rod_version());
    const uint16_t rodMinorVersion=ver.minor_version();
    const uint32_t rodBlockType=rob.rod_detev_type()&0xff;
    ATH_MSG_VERBOSE("Found version " << rodMinorVersion <<  " of Rod Block Type  " <<  rodBlockType);

    std::unique_ptr<LArRodBlockStructure> rodBlock;


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
	return StatusCode::FAILURE;
      }// end switch(rodMinorVersion)
    }//end of rodBlockType==4
    else {
      ATH_MSG_ERROR("Found unsupported Rod block type " << rodBlockType);
      return StatusCode::FAILURE;
    }

    if (!rodBlock->setFragment(pData,nData)) {
      ATH_MSG_ERROR("Failed to assign fragment pointer to LArRodBlockStructure");
      return StatusCode::FAILURE;
    }

    //Loop over FEBs in ROD:
    do {
      HWIdentifier fId( Identifier32(rodBlock->getFEBID()) );
      if (!m_onlineId->isValidId(fId)) {
	ATH_MSG_WARNING("Invalid FEB identifer 0x" << std::hex << fId.get_identifier32().get_compact() 
			<< ". Skipping");
	continue;
      }
      
      //FIXME: Add status-word and checksum verification to catch corrupt events


      int32_t energy;
      int32_t time;
      int32_t quality;
      uint32_t gain;
      int fcNb;
      const int NthisFebChannel=m_onlineId->channelInSlotMax(fId);
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
