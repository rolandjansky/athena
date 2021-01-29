/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of LArRodDecoder class 
#include "LArByteStream/LArRodDecoder.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "LArByteStream/LArRodBlockStructure.h"
//#include "LArByteStream/LArRodBlockStructure_0.h"
#include "LArByteStream/LArRodBlockTransparentV0.h"
#include "LArByteStream/LArRodBlockCalibrationV0.h"
// IWS 14.07.2005
#include "LArByteStream/LArRodBlockCalibrationV1.h"
// IWS 25.04.2006
#include "LArByteStream/LArRodBlockCalibrationV2.h"
#include "LArByteStream/LArRodBlockCalibrationV3.h"
#include "LArByteStream/LArRodBlockAccumulatedV3.h"
#include "LArByteStream/LArRodBlockPhysicsV0.h"
#include "LArByteStream/LArRodBlockPhysicsV1.h"
#include "LArByteStream/LArRodBlockPhysicsV2.h"
#include "LArByteStream/LArRodBlockPhysicsV3.h"
#include "LArByteStream/LArRodBlockPhysicsV4.h"
#include "LArByteStream/LArRodBlockPhysicsV5.h"
#include "LArByteStream/LArRodBlockPhysicsV6.h"
#include <algorithm>
#include <fstream>

#include "LArRawConditions/LArCalibParams.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include "LArCabling/LArOnOffIdMapping.h"

static const InterfaceID IID_ILArRodDecoder
            ("LArRodDecoder", 1, 0);

LArRodDecoder::LArRodDecoder ( const std::string& type, const std::string& name,const IInterface* parent )
  : AthAlgTool(type,name,parent),
    m_LArCellEthreshold(-100.),
    m_larCell(false), 
    m_readtdc(false),
    m_onlineHelper(0),
    m_doBadChanMasking(false),
    m_badChannelMasker(0)
 {
  declareInterface< LArRodDecoder  >( this );
  declareProperty("IgnoreCheckFEBs",m_IgnoreCheckFEBs);
  declareProperty("CellCorrections",m_LArCellCorrNames ); 
  declareProperty("LArCellEthreshold",m_LArCellEthreshold ); 
  declareProperty("LArCell",m_larCell ); 
  declareProperty("ReadTDC",m_readtdc);
  declareProperty("DelayScale",m_delayScale=(25./240.)*CLHEP::ns);
  declareProperty("FebExchange", m_febExchange=0); //FIXME: Very ugly hack! See explanation in .h file
  declareProperty("FebId1", m_febId1=0);
  declareProperty("FebId2", m_febId2=0);
  declareProperty("FirstSample", m_firstSample=0); //FIXME: Very ugly hack! See explanation in .h file

  declareProperty("BEPreselection",m_vBEPreselection,"For channel-selection: Barrel=0, Endcap=1");
  declareProperty("PosNegPreselection",m_vPosNegPreselection,"For channel-selection: C-Side:0, A-Side: 1");
  declareProperty("FTNumPreselection",m_vFTPreselection,"For channel-selection: Feedthrough numbers (e.g. 0 - 31 for barrel)");
  declareProperty("MultiDSPMode", m_MultiDSPMode=false);
  declareProperty("CheckSum", m_CheckSum=false);
  declareProperty("StatusMask", m_StatusMask=0x00000212);
  declareProperty("RequiredPhysicsNSamples", m_requiredPhysicsNSamples = 0);
}

// destructor 
LArRodDecoder::~LArRodDecoder()
{
}

const InterfaceID& LArRodDecoder::interfaceID( )
{ return IID_ILArRodDecoder;  }

StatusCode
LArRodDecoder::initialize ATLAS_NOT_THREAD_SAFE ()
{
 // Before anything, access message service
  m_StatusNMask = !m_StatusMask;


  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endmsg;
    return sc;
  } 

  IToolSvc* toolSvc;
  sc = service( "ToolSvc",toolSvc);
  if (sc.isFailure())
    {msg(MSG::ERROR) << "Unable to get ToolSvc" << endmsg;
    return sc;
   }
  
 sc = toolSvc->retrieveTool("LArBadChannelMasker/LArRodDecoder_Masker", m_badChannelMasker);
 if(sc.isFailure()) {
   msg(MSG::ERROR) << "Failed to retrieve the LArBadChannelMasker named 'LArRodDecoder_Masker'." << endmsg;
   return sc;
 }
 m_doBadChanMasking = m_badChannelMasker->isMaskingOn();
  
 ATH_CHECK( m_evt.initialize() );

 std::vector<std::string>::const_iterator it = m_LArCellCorrNames.begin(); 
 std::vector<std::string>::const_iterator it_end = m_LArCellCorrNames.end();
 for(; it!=it_end;++it)
   {IAlgTool* tool; 
    CaloCellCorrection* corr; 
    ListItem li(*it);
    if((toolSvc->retrieveTool(li.type(), li.name(), tool)).isFailure() ) 
      {msg(MSG::ERROR) << " Can't get AlgTool for CaloCellCorrection " << endmsg;
       return StatusCode::FAILURE; 
      }

    corr = dynamic_cast<CaloCellCorrection*> (tool); 
    if(!corr  ) 
      {msg(MSG::ERROR) << " Can't d-cast to CaloCellCorrection*  " << endmsg;
       return StatusCode::FAILURE; 
      }
    m_LArCellCorrTools.push_back(corr); 
  } 

 if(m_larCell) {  
   LArRoI_Map* roiMap;
   if((toolSvc->retrieveTool("LArRoI_Map", roiMap )).isFailure() )
     {msg(MSG::ERROR) << " Can't get AlgTool LArRoI_Map " << endmsg;
      return StatusCode::FAILURE; 
     }
     m_makeCell.setThreshold(m_LArCellEthreshold);
     m_makeCell.initialize( roiMap, &m_LArCellCorrTools ); 
   }	

   //Build list of preselected Feedthroughs
   if (m_vBEPreselection.size() &&  m_vPosNegPreselection.size() && m_vFTPreselection.size()) {
     ATH_MSG_INFO("Building list of selected feedthroughs");
     std::vector<int>::const_iterator it_BE=m_vBEPreselection.begin();
     for (;it_BE!=m_vBEPreselection.end();it_BE++) {
       const unsigned BE=*it_BE;
       std::vector<int>::const_iterator it_PN=m_vPosNegPreselection.begin();
       for (;it_PN!=m_vPosNegPreselection.end();it_PN++) {
	 const unsigned PN=*it_PN;
	 std::vector<int>::const_iterator it_FT=m_vFTPreselection.begin();
	 for (;it_FT!=m_vFTPreselection.end();it_FT++) {
	   HWIdentifier finalFTId=m_onlineHelper->feedthrough_Id(BE,PN,*it_FT);
	   unsigned int finalFTId32 = finalFTId.get_identifier32().get_compact();
	   ATH_MSG_INFO("Adding feedthrough Barrel/Endcap=" << BE << " pos/neg=" << PN << " FT=" << *it_FT 
			<< " (0x" << std::hex << finalFTId32 << std::dec << ")");
	   m_vFinalPreselection.push_back(finalFTId32);
	 }
       }
     }
     std::sort(m_vFinalPreselection.begin(),m_vFinalPreselection.end());
   }//end if something set
   else {
     if (m_vBEPreselection.size() ||  m_vPosNegPreselection.size() || m_vFTPreselection.size()) {
       msg(MSG::ERROR) << "Feedthrough preselection: jobOption inconsistency! "<< endmsg;
       msg(MSG::ERROR) << "Need to set all three jobOptions BEPreselection PNPreselecton and FTPreselection" << endmsg;
       return StatusCode::FAILURE;
     }
     ATH_MSG_DEBUG("No feedthrough preselection by jobOptions.");
   }
   return sc;
}



void LArRodDecoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                   const uint32_t* p, uint32_t n, LArDigitContainer& coll, CaloGain::CaloGain RequestedGain) const
{ // Digit pointer
  LArDigit * dg=0 ;
  CaloGain::CaloGain calogain;
  uint32_t gain;
  int fcNb;
  std::vector<short> samples;
  LArRodBlockStructure* BlStruct=prepareBlockStructure(robFrag, p, n, RequestedGain);
  if (!BlStruct) return;

  do {
      HWIdentifier fId( Identifier32(BlStruct->getFEBID()) );
      unsigned int fId32 = fId.get_identifier32().get_compact();
      if (!m_onlineHelper->isValidId(fId)) {
	msg(MSG::WARNING) << "Invalid FEB identifer 0x" << std::hex << fId32 << std::dec << ". Skipping" << endmsg;
	continue;
      }
      // std::cout << "digit FEBID=" << std::hex<<  " " <<fId32 << std::dec<<std::endl;
      if(m_febExchange) {
        if     (fId32 == m_febId1) {
          fId = HWIdentifier(Identifier32(m_febId2));
          fId32 = fId.get_identifier32().get_compact();
        }
        else if(fId32 == m_febId2) {
          fId = HWIdentifier(Identifier32(m_febId1));
          fId32 = fId.get_identifier32().get_compact();
        }
      }
      
      // IWS 24.01.2006 protection against NULL events (null pointer to rawdata block) 
      if (!BlStruct->hasRawDataBlock())
      {
	ATH_MSG_DEBUG("No Raw Data for this FEB - NULL event for FEBID 0x"<< std::hex << BlStruct->getFEBID() << std::dec);
        continue; 
      }

      // RL 04.17.2008 skip check for some FEBs
      int do_check=1;
      std::vector<unsigned int>::const_iterator it_feb     = m_IgnoreCheckFEBs.begin(); 
      std::vector<unsigned int>::const_iterator it_feb_end = m_IgnoreCheckFEBs.end();
      for(; it_feb!=it_feb_end;++it_feb)
	if(fId==*it_feb) {
	  do_check=0;
	  break;
	}
      if(do_check) {
	//WL 31.10.2007 //check RodStatus-word to catch corrupt events
	if (BlStruct->getStatus() & m_StatusNMask) {
	  msg(MSG::WARNING) << "RodStatus&0x" << std::hex << m_StatusNMask << " indicates corrupt data for FEB  "<< std::hex << fId32 << std::dec <<".  Ignored." << endmsg;
	  continue;
	}
      }
      // RL 05.11.2007 checksum
      if(m_CheckSum) {
	const uint32_t onsum  = BlStruct->onlineCheckSum();
	const uint32_t offsum = BlStruct->offlineCheckSum();
	if(onsum!=offsum) {
	  msg(MSG::WARNING) << "Checksum error for FEB: " << MSG::hex << fId32 << endmsg;
	  msg(MSG::WARNING) << " online checksum  = " << MSG::hex << onsum  << endmsg;
	  msg(MSG::WARNING) << " offline checksum = " << MSG::hex << offsum << MSG::dec << endmsg;
	  continue;
	}
      }

      if (m_vFinalPreselection.size()) {
	const unsigned int ftId=m_onlineHelper->feedthrough_Id(fId).get_identifier32().get_compact();
	if (!std::binary_search(m_vFinalPreselection.begin(), m_vFinalPreselection.end(),ftId)) {
	  ATH_MSG_DEBUG("Feedthrough with id 0x" << MSG::hex << ftId << MSG::dec <<" not in preselection. Ignored.");
	  continue;
	}
      }
      const int NthisFebChannel=m_onlineHelper->channelInSlotMax(fId);
      while (BlStruct->getNextRawData(fcNb,samples,gain))
	{if (fcNb>=NthisFebChannel)
	  continue;
	if (samples.size()==0) continue; // Ignore missing cells
	HWIdentifier cId = m_onlineHelper->channel_Id(fId,fcNb);
	calogain=(CaloGain::CaloGain)gain;
	dg = new LArDigit(cId, calogain, samples);
	samples.clear();
	coll.push_back(dg);
	}
    }
   while (BlStruct->nextFEB()); //Get NextFeb
   return;
}

//*******


void LArRodDecoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                   const uint32_t* p, uint32_t n, LArRawChannelContainer& coll, CaloGain::CaloGain RequestedGain) const
{ 
  int32_t energy;
  int32_t time;
  int32_t quality;
  uint32_t gain;
  int fcNb;
  HWIdentifier cId;
  LArRodBlockStructure* BlStruct=prepareBlockStructure(robFrag, p, n, RequestedGain);
  if (!BlStruct) return;

  do {
      HWIdentifier fId( Identifier32(BlStruct->getFEBID()) );
      unsigned int fId32 = fId.get_identifier32().get_compact();
      if (!m_onlineHelper->isValidId(fId)) {
	msg(MSG::WARNING) << "Invalid FEB identifer " << std::hex << fId32 << std::dec << ". Skipping" << endmsg;
	continue;
      }
      // std::cout << "rawChan FEBID=" << std::hex <<fId32 << std::dec<<std::endl;

      if(m_febExchange) {
        if     (fId32 == m_febId1) {
          fId = HWIdentifier(Identifier32(m_febId2));
          fId32 = fId.get_identifier32().get_compact();
        }
        else if(fId32 == m_febId2) {
          fId = HWIdentifier(Identifier32(m_febId1));
          fId32 = fId.get_identifier32().get_compact();
        }
      }
      if (!BlStruct->hasPhysicsBlock()) {
        ATH_MSG_DEBUG("No Physics Data for this FEB - NULL event for FEBID 0x" << std::hex << BlStruct->getFEBID() << std::dec);
        continue; 
      }

      // RL 04.17.2008 skip check for some FEBs
      int do_check=1;
      std::vector<unsigned int>::const_iterator it_feb     = m_IgnoreCheckFEBs.begin(); 
      std::vector<unsigned int>::const_iterator it_feb_end = m_IgnoreCheckFEBs.end();
      for(; it_feb!=it_feb_end;++it_feb)
	if(fId==*it_feb) {
	  do_check=0;
	  break;
	}
      if(do_check) {
	//WL 31.10.2007 //check RodStatus-word to catch corrupt events
	if (BlStruct->getStatus() & m_StatusNMask) {
	  msg(MSG::WARNING) << "RodStatus&0x" << std::hex << m_StatusNMask << " indicates corrupt data for FEB  "<< std::hex << fId32 << std::dec <<".  Ignored." << endmsg;
	  continue;
	}
      }
      
      // RL 05.11.2007 checksum
      if(m_CheckSum) {
	const uint32_t onsum  = BlStruct->onlineCheckSum();
	const uint32_t offsum = BlStruct->offlineCheckSum();
	if(onsum!=offsum) {
	  msg(MSG::WARNING) << "Checksum error:" << endmsg;
	  msg(MSG::WARNING) << " online checksum  = " << MSG::hex << onsum  << endmsg;
	  msg(MSG::WARNING) << " offline checksum = " << MSG::hex << offsum << endmsg;
	  continue;
	}
      }

      if (m_vFTPreselection.size()) {
	int ftId=m_onlineHelper->feedthrough_Id(fId).get_identifier32().get_compact();
	if (!std::binary_search(m_vFTPreselection.begin(), m_vFTPreselection.end(),ftId)) {
	  ATH_MSG_DEBUG("Feedthrough with id " << std::hex << ftId << std::dec <<" not in preselection. Ignored.");
	  continue;
	}
      }
      const int NthisFebChannel=m_onlineHelper->channelInSlotMax(fId);
      while (BlStruct->getNextEnergy(fcNb,energy,time,quality,gain)) {
	if (fcNb>=NthisFebChannel)
	  continue;
	cId = m_onlineHelper->channel_Id(fId,fcNb);
        uint16_t iquality = 0;
        uint16_t iprovenance = 0x1000;
        if (quality>0) {
            iprovenance |= 0x2000;
            iquality = (quality & 0xFFFF);
        } 
	LArRawChannel chan(cId, energy, time, iquality, iprovenance, (CaloGain::CaloGain)gain);
	coll.push_back(chan);  
      }
    }
   while (BlStruct->nextFEB()); //Get NextFeb
   return;
}


void LArRodDecoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                   const uint32_t* p,
                                   uint32_t n,
                                   LArCalibDigitContainer& coll,
                                   CaloGain::CaloGain RequestedGain,
                                   const LArCalibLineMapping& calibLineMapping,
                                   const LArOnOffIdMapping& onOffIdMapping) const
{ // CalibDigit pointer
  LArCalibDigit * dg=0 ;
  uint32_t gain;
  CaloGain::CaloGain calogain;
  int fcNb;
  uint16_t dac, delay;
  bool ispulsed;
  std::vector<short> samples;

  ATH_MSG_VERBOSE("FillCollection for LArCalibDigitContainer is called.");
  LArRodBlockStructure* BlStruct=prepareBlockStructure(robFrag, p, n, RequestedGain);
  if (!BlStruct) return;
  if (BlStruct->canSetCalibration()) {
    dac=BlStruct->getDAC();
    delay=BlStruct->getDelay();
    //    int iFeb=0, iCan=0; //For debug purpose
    do
      {
	// IWS 24.01.2006 protection against NULL events (null pointer to rawdata block) 
	if (!BlStruct->hasRawDataBlock()) {
	  ATH_MSG_DEBUG("No Raw Data for this FEB - NULL event for FEBID " << std::hex << BlStruct->getFEBID());
	  continue; 
	}
	HWIdentifier fId(Identifier32(BlStruct->getFEBID()));
        unsigned int fId32 = fId.get_identifier32().get_compact();
	// RL 20.09.2006 protection against 0 FebId
        if (!fId32){
	  ATH_MSG_DEBUG("Bad FebID=0x"<< std::hex << BlStruct->getFEBID() << " found for this FEB, skipping it!");
	  continue; 
	}
	// RL 04.17.2008 skip check for some FEBs
	int do_check=1;
	std::vector<unsigned int>::const_iterator it_feb     = m_IgnoreCheckFEBs.begin(); 
	std::vector<unsigned int>::const_iterator it_feb_end = m_IgnoreCheckFEBs.end();
	for(; it_feb!=it_feb_end;++it_feb)
	  if(fId==*it_feb) {
	    do_check=0;
	    break;
	  }
	if(do_check) {
	  //WL 31.10.2007 //check RodStatus-word to catch corrupt events
	  if (BlStruct->getStatus() & m_StatusNMask) {
	    msg(MSG::WARNING) << "RodStatus&0x" << std::hex << m_StatusNMask << " indicates corrupt data for FEB  "<< std::hex << fId32 << std::dec <<".  Ignored." << endmsg;
	    continue;
	  }
	}
       // RL 05.11.2007 checksum
	if(m_CheckSum) {
	  uint32_t onsum  = BlStruct->onlineCheckSum();
	  uint32_t offsum = BlStruct->offlineCheckSum();
	  if(onsum!=offsum) {
	    msg(MSG::WARNING) << "Checksum error:" << endmsg;
	    msg(MSG::WARNING) << " online checksum  = " << MSG::hex << onsum  << endmsg;
	    msg(MSG::WARNING) << " offline checksum = " << MSG::hex << offsum << endmsg;
	    continue;
	  }
	}
        if(m_febExchange) {
          if     (fId32 == m_febId1) {
            fId = HWIdentifier(Identifier32(m_febId2));
            fId32 = fId.get_identifier32().get_compact();
          }
          else if(fId32 == m_febId2) {
            fId = HWIdentifier(Identifier32(m_febId1));
            fId32 = fId.get_identifier32().get_compact();
          }
        }

	if (m_vFinalPreselection.size()) {
          unsigned int ftId=m_onlineHelper->feedthrough_Id(fId).get_identifier32().get_compact();
	  if (!std::binary_search(m_vFinalPreselection.begin(), m_vFinalPreselection.end(),ftId)) {
	    ATH_MSG_DEBUG("Feedthrough with id 0x" << std::hex << ftId << std::dec <<" not in preselection. Ignored.");
	    continue;
	  }
	}
	const int NthisFebChannel=m_onlineHelper->channelInSlotMax(fId);
	while (BlStruct->getNextRawData(fcNb,samples,gain))
	  {if (fcNb>=NthisFebChannel)
	    continue;
	  if (samples.size()==0) continue; // Ignore missing cells
	  HWIdentifier cId = m_onlineHelper->channel_Id(fId,fcNb);
	  ispulsed=BlStruct->getPulsed(fcNb);
	  
	  calogain=(CaloGain::CaloGain)gain;
	  dg = new LArCalibDigit(cId, calogain, samples, dac, delay, ispulsed);
	  samples.clear();
	  coll.push_back(dg);
	  }
      }
    while (BlStruct->nextFEB()); //Get NextFeb
  }
  else {//Not a calibration data block, try to get from database
    ATH_MSG_VERBOSE("Not LArCalibDigit data block found. Building it using DB values");
    //1st step, get Calib board config object
    const LArCalibParams* calibParams = nullptr;
    StatusCode sc=detStore()->retrieve(calibParams);
    if (sc.isFailure())
      {msg(MSG::ERROR) << "Cannot load LArCalibParams from DetStore!" << endmsg;
      return;
      }
    //2st step, get Event number
    SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
    if (!evt.isValid()) {
      ATH_MSG_ERROR("Cannot get EventInfo");
      return; //Return empty container. 
    }
    const unsigned eventNb=evt->eventNumber();
    const std::vector<HWIdentifier>* calibChannelIDs;
    do { //Loop over all FEBs in this ROD
      
      // IWS 24.01.2006 protection against NULL events (null pointer to rawdata block) 
      if (!BlStruct->hasRawDataBlock()) {
	ATH_MSG_DEBUG("No Raw Data for this FEB - NULL event for FEBID 0x" << std::hex << BlStruct->getFEBID() << std::dec);
	continue; 
      }
      HWIdentifier fId(Identifier32(BlStruct->getFEBID()));
      unsigned int fId32 = fId.get_identifier32().get_compact();
      // RL 20.09.2006 protection against 0 FebId
      if (!fId32) {
	ATH_MSG_DEBUG("Bad FebID=0x" << std::hex << BlStruct->getFEBID() << " found for this FEB, skipping it!" << std::dec);
        continue; 
      }
      if(m_febExchange) {
        if     (fId32 == m_febId1) {
          fId = HWIdentifier(Identifier32(m_febId2));
          fId32 = fId.get_identifier32().get_compact();
        }
        else if(fId32 == m_febId2) {
          fId = HWIdentifier(Identifier32(m_febId1));
          fId32 = fId.get_identifier32().get_compact();
        }
      }
      fcNb=0;
      HWIdentifier cId;
      do { //Search for the first connected channel of this FEB (probably the first one...)
	fcNb++; 
	cId = m_onlineHelper->channel_Id(fId,fcNb);
	calibChannelIDs=&calibLineMapping.calibSlotLine(cId);
      }
      while ( (!onOffIdMapping.isOnlineConnected(cId) || calibChannelIDs->size()==0) && fcNb<128); // This is the right  conditions to exit the loop!
      
      if ( calibChannelIDs->size()==0 ) {
	msg(MSG::ERROR) << "Cannot get calibration Channel ID for FEB " << std::hex << fId32 << std::dec << endmsg;
	return;
      }

      uint16_t dac;
      uint16_t delay;

      std::vector<HWIdentifier>::const_iterator csl_it=calibChannelIDs->begin();
      //Derive DAC and Delay value from this channel
      dac=calibParams->DAC(eventNb,*csl_it);
      delay=calibParams->Delay(eventNb,*csl_it);

      //Now start looping over channels in FEB
      const int NthisFebChannel=m_onlineHelper->channelInSlotMax(fId);
      //std::cout << "Processing FEB #" << iFeb++ << std::endl;
      
      //int iCan=0;

      fcNb=0;
      while (BlStruct->getNextRawData(fcNb,samples,gain))
	{
	  if (fcNb>=NthisFebChannel) continue;
	  if (samples.size()==0) continue; // Ignore missing cells
	  cId = m_onlineHelper->channel_Id(fId,fcNb);
	  calibChannelIDs=&calibLineMapping.calibSlotLine(cId);
	  //if (calibChannelIDs->size()==0) 
	  //continue; //Disconnected channel
	  //For the time being, I assume we are in H8 and have only one calib channel per FEB channel

	  if (calibChannelIDs->size()!=0) {
	    csl_it=calibChannelIDs->begin();
	    ispulsed=calibParams->isPulsed(eventNb,*csl_it);
 	    
	  } else ispulsed=0;
	  calogain=(CaloGain::CaloGain)gain;
	  dg = new LArCalibDigit(cId, calogain, samples, dac, delay, ispulsed);
	  samples.clear();
	  
	  coll.push_back(dg);
	  //iCan++;
	}
    }
    while (BlStruct->nextFEB()); //Get NextFeb
  }
  coll.setDelayScale(m_delayScale);
  return;
}

void LArRodDecoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                   const uint32_t* p,
                                   uint32_t n,
                                   LArAccumulatedCalibDigitContainer& coll,
                                   CaloGain::CaloGain RequestedGain,
                                   const LArCalibLineMapping& calibLineMapping) const
{ // Accumulated Digit pointer
  LArAccumulatedCalibDigit * dg=0 ;
  CaloGain::CaloGain calogain;
  uint32_t gain, ntrigger;
  int dac, delay, NStep=-1, StepIndex=-1;
  bool ispulsed;
  uint16_t ispulsed_int;
  int bitShift; 
  uint32_t FirstGoodFEB=1;
  int fcNb;
  std::vector< uint32_t > samplesSum;   
  std::vector< uint32_t > samples2Sum;   

  //  for(int i=0;i<16;i++)
  //  std::cout << " -     " << std::hex << p[i] << std::endl;

  LArRodBlockStructure* BlStruct=prepareBlockStructure(robFrag, p, n, RequestedGain);
  if (!BlStruct) return;
  do
    {
      // IWS 24.01.2006 protection against NULL events (null pointer to rawdata block) 
      if (!BlStruct->hasCalibBlock())
	{  
	  ATH_MSG_DEBUG("No Calib Data for this FEB - NULL event");
	  continue; 
	}
      if(FirstGoodFEB) 
	{
	  FirstGoodFEB=0;
	}
      HWIdentifier fId(Identifier32(BlStruct->getFEBID()));
      unsigned int fId32 = fId.get_identifier32().get_compact();
      // RL 20.09.2006 protection against 0 FebId
      if (!fId32){
	ATH_MSG_DEBUG("Bad FebID=0x" << std::hex << BlStruct->getFEBID() << " found for this FEB, skipping it!" << std::dec);
	continue; 
      }
      // RL 04.17.2008 skip check for some FEBs
      int do_check=1;
      std::vector<unsigned int>::const_iterator it_feb     = m_IgnoreCheckFEBs.begin(); 
      std::vector<unsigned int>::const_iterator it_feb_end = m_IgnoreCheckFEBs.end();
      for(; it_feb!=it_feb_end;++it_feb)
	if(fId==*it_feb) {
	  do_check=0;
	  break;
	}
      if(do_check) {
	//WL 31.10.2007 //check RodStatus-word to catch corrupt events
	if (BlStruct->getStatus() & m_StatusNMask) {
	  msg(MSG::WARNING) << "RodStatus&0x" << std::hex << m_StatusNMask << " indicates corrupt data for FEB  "<< std::hex << fId32 << std::dec <<".  Ignored." << endmsg;
	  continue;
	}
      }
      // RL 05.11.2007 checksum
      if(m_CheckSum) {
	uint32_t onsum  = BlStruct->onlineCheckSum();
	uint32_t offsum = BlStruct->offlineCheckSum();
	if(onsum!=offsum) {
	  msg(MSG::WARNING) << "Checksum error:" << endmsg;
	  msg(MSG::WARNING) << " online checksum  = " << MSG::hex << onsum  << endmsg;
	  msg(MSG::WARNING) << " offline checksum = " << MSG::hex << offsum << endmsg;
	  continue;
	}
      }
      if(m_febExchange) {
        if     (fId32 == m_febId1) {
          fId = HWIdentifier(Identifier32(m_febId2));
          fId32 = fId.get_identifier32().get_compact();
        }
        else if(fId32 == m_febId2) {
          fId = HWIdentifier(Identifier32(m_febId1));
          fId32 = fId.get_identifier32().get_compact();
        }
      }

      if (m_vFinalPreselection.size()) {
	unsigned int ftId=m_onlineHelper->feedthrough_Id(fId).get_identifier32().get_compact();
	if (!std::binary_search(m_vFinalPreselection.begin(), m_vFinalPreselection.end(),ftId)) {
	  ATH_MSG_DEBUG("Feedthrough with id " << std::hex << ftId << std::dec <<" not in preselection. Ignored.");
	  continue;
	}
      }
      int NthisFebChannel=m_onlineHelper->channelInSlotMax(fId);
      uint32_t idum=0;
      while (BlStruct->getNextAccumulatedCalibDigit(fcNb,samplesSum,samples2Sum,idum,gain))
	{
	  ispulsed_int=0;
	  bitShift=0;
	  if (fcNb>=NthisFebChannel)continue;
	  if (samplesSum.size()==0) continue; // Ignore missing cells
	  HWIdentifier cId = m_onlineHelper->channel_Id(fId,fcNb);
	  calogain=(CaloGain::CaloGain)gain;
	  ntrigger=BlStruct->getNTrigger();
	  dac=BlStruct->getDAC();
	  delay=BlStruct->getDelay();
	  NStep=BlStruct->getNStep();
	  if(!NStep) NStep=1; // To be able to decode v6 code
	  StepIndex=BlStruct->getStepIndex();
	  // 08.08.2005 IWS get calib line
	  const std::vector<HWIdentifier>& calibChannelIDs = calibLineMapping.calibSlotLine(cId);
	  if (calibChannelIDs.size()==0) { 
	        samplesSum.clear();
	        samples2Sum.clear();
		continue;// connected channel 
	  }
	  //GR: Loop over all the calib chans instead of just looking at the first one and set four bit isPulsed int
	  for(std::vector<HWIdentifier>::const_iterator csl_it=calibChannelIDs.begin(); csl_it!=calibChannelIDs.end();++csl_it){
	    uint32_t calibLine = m_onlineHelper->channel(*csl_it);
	    ispulsed=BlStruct->getPulsed(calibLine);
 	    ispulsed_int=( ispulsed_int | ((uint16_t)ispulsed<<bitShift) );  
	    bitShift++;
	  }

	  dg=new LArAccumulatedCalibDigit(cId, calogain, samplesSum, samples2Sum, ntrigger, dac, delay, 
					  (uint16_t)ispulsed_int, NStep, StepIndex);

	  coll.push_back(dg); 
	} // End while
    }//End do loop of FEBs
  while (BlStruct->nextFEB()); //Get NextFeb

  return;
}

void LArRodDecoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                   const uint32_t* p, uint32_t n, LArAccumulatedDigitContainer& coll, CaloGain::CaloGain RequestedGain) const
{ // Accumulated Digit pointer
  LArAccumulatedDigit * dg=0 ;
  CaloGain::CaloGain calogain;
  uint32_t gain, ntrigger;
  //int NStep=-1, StepIndex=-1;
  int fcNb;
  std::vector<uint32_t> sampleSum;   
  std::vector< uint32_t > sampleSquare;   

  //  for(int i=0;i<16;i++)
  //  std::cout << " -     " << std::hex << p[i] << std::endl;

  LArRodBlockStructure* BlStruct=prepareBlockStructure(robFrag, p, n, RequestedGain);
  if (!BlStruct) return;
  do
    {
      // IWS 24.01.2006 protection against NULL events (null pointer to rawdata block) 
      if (!BlStruct->hasAccumBlock()) {
	ATH_MSG_DEBUG("No Accum Data for this FEB - NULL event");
	continue; 
      }
      HWIdentifier fId(Identifier32(BlStruct->getFEBID()));
      unsigned int fId32 = fId.get_identifier32().get_compact();
      // RL 20.09.2006 protection against 0 FebId
      if (!fId32) {
	ATH_MSG_DEBUG("Bad FebID=0x" << std::hex << BlStruct->getFEBID() << " found for this FEB, skipping it!" << std::dec);
	continue; 
      }
      // RL 04.17.2008 skip check for some FEBs
      int do_check=1;
      std::vector<unsigned int>::const_iterator it_feb     = m_IgnoreCheckFEBs.begin(); 
      std::vector<unsigned int>::const_iterator it_feb_end = m_IgnoreCheckFEBs.end();
      for(; it_feb!=it_feb_end;++it_feb)
	if(fId==*it_feb) {
	  do_check=0;
	  break;
	}
      if(do_check) {
	//WL 31.10.2007 //check RodStatus-word to catch corrupt events
	if (BlStruct->getStatus() & m_StatusNMask) {
	  msg(MSG::WARNING) << "RodStatus&0x" << std::hex << m_StatusNMask << " indicates corrupt data for FEB  "<< std::hex << fId32 << std::dec <<".  Ignored." << endmsg;
	  continue;
	}
      }
      // RL 05.11.2007 checksum
      if(m_CheckSum) {
	uint32_t onsum  = BlStruct->onlineCheckSum();
	uint32_t offsum = BlStruct->offlineCheckSum();
	if(onsum!=offsum) {
	  msg(MSG::WARNING) << "Checksum error:" << endmsg;
	  msg(MSG::WARNING) << " online checksum  = " << MSG::hex << onsum  << endmsg;
	  msg(MSG::WARNING) << " offline checksum = " << MSG::hex << offsum << endmsg;
	  continue;
	}
      }
      if(m_febExchange) {
        if     (fId32 == m_febId1) {
          fId = HWIdentifier(Identifier32(m_febId2));
          fId32 = fId.get_identifier32().get_compact();
        }
        else if(fId32 == m_febId2) {
          fId = HWIdentifier(Identifier32(m_febId1));
          fId32 = fId.get_identifier32().get_compact();
        }
      }

      if (m_vFinalPreselection.size()) {
	unsigned int ftId=m_onlineHelper->feedthrough_Id(fId).get_identifier32().get_compact();
	if (!std::binary_search(m_vFinalPreselection.begin(), m_vFinalPreselection.end(),ftId)) {
	  ATH_MSG_DEBUG("Feedthrough with id " << std::hex << ftId << std::dec <<" not in preselection. Ignored.");
	  continue;
	}
      }
      int NthisFebChannel=m_onlineHelper->channelInSlotMax(fId);
      ntrigger=BlStruct->getNTrigger();
      //NStep=BlStruct->getNStep();
      //if(!NStep) NStep=1; // To be able to decode v6 code
      //StepIndex=BlStruct->getStepIndex();
      while (BlStruct->getNextAccumulatedDigit(fcNb,sampleSum,sampleSquare,gain)) {
	  if (fcNb>=NthisFebChannel)continue;
	  if (sampleSquare.size()==0) continue; // Ignore missing cells
	  HWIdentifier cId = m_onlineHelper->channel_Id(fId,fcNb);
	  calogain=(CaloGain::CaloGain)gain;
	  dg=new LArAccumulatedDigit(cId,calogain,sampleSum,sampleSquare,ntrigger);
	  coll.push_back(dg); 
	} // End while
    }//End do loop of FEBs
  while (BlStruct->nextFEB()); //Get NextFeb

  return;
}

void LArRodDecoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                   const uint32_t* p, uint32_t n, LArFebHeaderContainer& coll,const CaloGain::CaloGain RequestedGain) const
{
  LArFebHeader* larFebHeader;
  //uint32_t NWtot=0;
  HWIdentifier FEBID;

  //  for(int i=0;i<16;i++)
  //  std::cout << " -     " << std::hex << p[i] << std::endl;

  LArRodBlockStructure* BlStruct=prepareBlockStructure(robFrag, p, n, RequestedGain);
  if (!BlStruct) return;

  do{
    //Read first FEB. The header of this feb is combined with the ROD-Header
    //NWtot=BlStruct->getNumberOfWords();
    //if(NWtot<=4) continue;

    FEBID=HWIdentifier(Identifier32(BlStruct->getFEBID()));
    unsigned int FEBID32 = FEBID.get_identifier32().get_compact();
    if (!m_onlineHelper->isValidId(FEBID)) {
      msg(MSG::WARNING) << "Invalid FEB identifer " << std:: hex << FEBID32 << std::dec << ". Skipping" << endmsg;
      continue;
    }

    if(m_febExchange) {
      if     (FEBID32 == m_febId1) {
          FEBID   = HWIdentifier(Identifier32(m_febId2));
          FEBID32 = FEBID.get_identifier32().get_compact();
      }
      else if(FEBID32 == m_febId2) {
          FEBID = HWIdentifier(Identifier32(m_febId1));
          FEBID32 = FEBID.get_identifier32().get_compact();
      }
    }
 
    if (m_vFinalPreselection.size()) {
      const unsigned int ftId=m_onlineHelper->feedthrough_Id(FEBID).get_identifier32().get_compact();
      if (!std::binary_search(m_vFinalPreselection.begin(), m_vFinalPreselection.end(),ftId)) {
	ATH_MSG_DEBUG("Feedthrough with id 0x" << std::hex << ftId << std::dec <<" not in preselection. Ignored.");
	continue;
      }
    }



    larFebHeader=new LArFebHeader(FEBID);
    //setROD header data
    
    larFebHeader->SetFormatVersion(robFrag.rod_version());
    larFebHeader->SetSourceId(robFrag.rod_source_id());
    larFebHeader->SetRunNumber(robFrag.rod_run_no());
    larFebHeader->SetELVL1Id(robFrag.rod_lvl1_id());
    larFebHeader->SetBCId(robFrag.rod_bc_id());
    larFebHeader->SetLVL1TigType(robFrag.rod_lvl1_trigger_type());
    larFebHeader->SetDetEventType(robFrag.rod_detev_type());
  
    //set DSP data
    const unsigned nsample=BlStruct->getNumberOfSamples();
    const uint32_t status= BlStruct->getStatus();
    larFebHeader->SetDspCodeVersion(BlStruct->getDspCodeVersion()); 
    larFebHeader->SetDspEventCounter(BlStruct->getDspEventCounter()); 
    larFebHeader->SetRodResults1Size(BlStruct->getResults1Size()); 
    larFebHeader->SetRodResults2Size(BlStruct->getResults2Size()); 
    larFebHeader->SetRodRawDataSize(BlStruct->getRawDataSize()); 
    larFebHeader->SetNbSweetCells1(BlStruct->getNbSweetCells1()); 
    larFebHeader->SetNbSweetCells2(BlStruct->getNbSweetCells2()); 
    larFebHeader->SetNbSamples(nsample); 
    larFebHeader->SetOnlineChecksum(BlStruct->onlineCheckSum());
    larFebHeader->SetOfflineChecksum(BlStruct->offlineCheckSum());

    if(!BlStruct->hasControlWords()) {
      larFebHeader->SetFebELVL1Id(robFrag.rod_lvl1_id());
      larFebHeader->SetFebBCId(robFrag.rod_bc_id());
    } else {
      const uint16_t evtid = BlStruct->getCtrl1(0) & 0x1f;
      const uint16_t bcid  = BlStruct->getCtrl2(0) & 0x1fff;

      larFebHeader->SetFebELVL1Id(evtid);
      larFebHeader->SetFebBCId(bcid);
      for(int iadc=0;iadc<16;iadc++) {
	larFebHeader->SetFebCtrl1(BlStruct->getCtrl1(iadc));
	larFebHeader->SetFebCtrl2(BlStruct->getCtrl2(iadc));
	larFebHeader->SetFebCtrl3(BlStruct->getCtrl3(iadc));
      }
      for(unsigned int i = 0; i<nsample; i++ ) {
	larFebHeader->SetFebSCA(BlStruct->getRadd(0,i) & 0xff);
      }
    }
    larFebHeader->SetRodStatus(status);
    coll.push_back(larFebHeader);
  
  } while (BlStruct->nextFEB()); //Get NextFeb
}


LArRodBlockStructure*
LArRodDecoder::prepareBlockStructure1 (const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag) const
{ 
  //Get version and blocktype form header
  eformat::helper::Version ver(robFrag.rod_version());
  const uint16_t rodMinorVersion=ver.minor_version();
  const uint32_t rodBlockType=robFrag.rod_detev_type()&0xff;

  const unsigned MAXMINOR = 12;
  const unsigned MAXTYPE = 10;

  if (rodMinorVersion > MAXMINOR || rodBlockType > MAXTYPE) {
    msg(MSG::ERROR) << "Bad Rod block type " <<  rodBlockType
                    << " / " << rodMinorVersion << endmsg;
    return nullptr;
  }
  std::vector<std::unique_ptr<LArRodBlockStructure> >& blstructs =
    *m_blstructs.get();
  unsigned int index = rodBlockType * (MAXMINOR+1) + rodMinorVersion;
  if (blstructs.empty()) {
    blstructs.resize ((MAXMINOR+1)*(MAXTYPE+1));
  }
  if (!blstructs[index]) {
    blstructs[index] = makeBlockStructure (rodBlockType, rodMinorVersion);
  }
  if (!blstructs[index]) {
    msg(MSG::ERROR) << "Bad Rod block type " <<  rodBlockType
                    << " / " << rodMinorVersion << endmsg;
    return nullptr;
  }

#ifndef NDEBUG
  ATH_MSG_DEBUG("Found version " << rodMinorVersion << " of Rod block type " << rodBlockType);
#endif

  return blstructs[index].get();
}


LArRodBlockStructure*
LArRodDecoder::prepareBlockStructure(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                     const uint32_t* p, uint32_t n, const CaloGain::CaloGain RequestedGain) const
{ 
#ifndef NDEBUG
  ATH_MSG_DEBUG("Prepare LArRodBlockStructure. Got a fragement of size " << n);
#endif

  LArRodBlockStructure* BlStruct = prepareBlockStructure1 (robFrag);
  if (!BlStruct) {
    return nullptr;
  }

  //BlStruct->dumpFragment(v); // For testing purpose
  if (!BlStruct->setFragment(p,n)) {
    constexpr int maxMess = 100;
    static std::atomic<int> nMess = 1;
    int thismess = nMess++;
    if (thismess < maxMess) {
      msg(MSG::ERROR) << "Could not set fragment (wrong number of samples in data ?) - container will not be filled" << endmsg;
      if (thismess == maxMess)
        msg(MSG::ERROR) << "This message will not be repeated" << endmsg;
    }
    return NULL;
  }
#ifndef NDEBUG
  ATH_MSG_VERBOSE("Set Fragment at address "<<  &(p[0]) << " " << p[5]);
#endif

  BlStruct->setGain(RequestedGain); //Will be ignored if BlockStructure does not support fixed gains.    
  //FIXME: Very ugly hack! See explanation in LArRodDecoder.h 
  if (m_firstSample) {
    BlStruct->setFirstSample(m_firstSample);
  }
  return BlStruct;
}


std::unique_ptr<LArRodBlockStructure>
LArRodDecoder::makeBlockStructure (unsigned int rodBlockType,
                                   unsigned int rodMinorVersion) const
{
  switch (rodBlockType) {
  case 2:
    // RodBlockType 2 = Transparent mode only
    switch (rodMinorVersion) {
    case 0: // Transparent mode v0 05.01.2004
    case 1: // Transparent mode v0
    case 2: // Transparent mode v0
    case 3: // Transparent mode v0
    case 4: // Transparent mode v0
      return std::make_unique<LArRodBlockTransparentV0<LArRodBlockHeaderTransparentV0> >();
    case 5: // Calibration (Transparent mode) v1 17.01.2006
      return std::make_unique<LArRodBlockCalibrationV1>();
    case 6: // Calibration (Transparent mode) v3 31.05.2006
    case 7: // Calibration (Transparent mode) v3
    case 8: // Calibration (Transparent mode) v3
    case 9: // Calibration (Transparent mode) v3
    case 10:// Calibration (Transparent mode) v3
    case 11:// Calibration (Transparent mode) v3
    case 12:// Calibration (Transparent mode) v3
      return std::make_unique<LArRodBlockCalibrationV3>();
    default:
      break;
    }
    break;

  case 3:
    // RodBlockType 3 = Test mode
    return std::make_unique<LArRodBlockTransparentV0<LArRodBlockHeaderTransparentV0> >();

  case 4:
    // RodBlockType 4 = Physics mode
    switch (rodMinorVersion) {
    case 0: // Physics mode v0 05.01.2004 first draft
      return std::make_unique<LArRodBlockPhysicsV0>();
    case 1: // Physics mode v1 19.08.2004 only small differences
      return std::make_unique<LArRodBlockPhysicsV1>();
    case 2: // Physics mode v2 05.10.2004 adapted to real DSP data
    case 3: // Physics mode v2
    case 4: // Physics mode v2
    case 5: // Physics mode v2
    case 6: // Physics mode v2
    case 7: // Physics mode v2
    case 8: // Physics mode v2
      return std::make_unique<LArRodBlockPhysicsV2>();
    case 9: // Physics mode v4 10.07.2007 for commissioning
      return std::make_unique<LArRodBlockPhysicsV4>();
    case 10: // Physics mode v5 16.06.2008 for LHC 
    case 11: // Physics mode v5 16.06.2008 for LHC
      {
        auto bl = std::make_unique<LArRodBlockPhysicsV5>();
        if (m_requiredPhysicsNSamples > 0) {
          bl->setRequiredNSamples(m_requiredPhysicsNSamples);
        }
        return bl;
      }
    case 12: // Physics mode v5 09.03.2011 for LHC
      {
        auto bl = std::make_unique<LArRodBlockPhysicsV6>();
        if (m_requiredPhysicsNSamples > 0) {
          bl->setRequiredNSamples(m_requiredPhysicsNSamples);
        }
        return bl;
      }
    default:
      break;
    }
    break;

  case 5:
    // RodBlockType 5 = Physics simulation mode
    // Physics mode v3 11.04.2005 for simulation
    return std::make_unique<LArRodBlockPhysicsV3>();

  case 6:
    // RodBlockType 6 = Physics test mode
    switch (rodMinorVersion) {
    case 0: // Physics mode v0 05.01.2004 first draft
      return std::make_unique<LArRodBlockPhysicsV0>();
    case 1: // Physics mode v2 05.10.2004 adapted to real DSP data
    case 2: // Physics mode v2
      return std::make_unique<LArRodBlockPhysicsV2>();
    default:
      break;
    }
    break;

  case 7:
    // RodBlockType 7 = Calibration mode
    switch (rodMinorVersion) {
    case 0: // Calibration mode v0  05.01.2004
      return std::make_unique<LArRodBlockCalibrationV0<LArRodBlockHeaderCalibrationV0> >();
    case 1: // Calibration mode v1  17.01.2006
    case 2: // Calibration mode v1 
    case 3: // Calibration mode v1 
    case 4: // Calibration mode v1 
      return std::make_unique<LArRodBlockCalibrationV1>();
    case 5: // Calibration mode v2  26.04.2006
      return std::make_unique<LArRodBlockCalibrationV2>();
    case 6: // Calibration mode v3  31.05.2006
    case 7: // Calibration mode v3
    case 8: // Calibration mode v3
    case 9: // Calibration mode v3
    case 10:// Calibration mode v3
    case 11:// Calibration mode v3
    case 12:// Calibration mode v3
      return std::make_unique<LArRodBlockCalibrationV3>();
    default:
      break;
    }
    break;

  case 10:
    // RodBlockType 10 = Accumulated mode (used for pre-processed pedestal runs)
    // Accumulated mode v3 10.06.2008
    return std::make_unique<LArRodBlockAccumulatedV3>();

  default:
    break;
  }

  return std::unique_ptr<LArRodBlockStructure>();
}
