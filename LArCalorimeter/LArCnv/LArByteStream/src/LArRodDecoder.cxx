/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

static const InterfaceID IID_ILArRodDecoder
            ("LArRodDecoder", 1, 0);

LArRodDecoder::LArRodDecoder ( const std::string& type, const std::string& name,const IInterface* parent )
  : AthAlgTool(type,name,parent),
    m_larblockstruct(0),
    m_larCablingSvc("LArCablingLegacyService"),
    m_LArCellEthreshold(-100.),
    m_larCell(false), 
    m_readtdc(false),
    m_onlineHelper(0),
    m_doBadChanMasking(false),
    m_badChannelMasker(0),
    m_rodTranspV0(0),
    m_rodCalibV0(0),
    m_rodCalibV1(0),
    m_rodCalibV2(0),
    m_rodCalibV3(0),
    m_rodAccumV3(0),
    m_rodPhysicsV0(0),
    m_rodPhysicsV1(0),
    m_rodPhysicsV2(0),
    m_rodPhysicsV3(0),
    m_rodPhysicsV4(0),
    m_rodPhysicsV5(0),
    m_rodPhysicsV6(0),
    m_robFrag(0),
    m_larCellFromDigit(0),
    m_error(0)
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
  declareProperty("UseCellMakerTool",m_useCellMakerTool=false);
  declareProperty("MultiDSPMode", m_MultiDSPMode=false);
  declareProperty("CheckSum", m_CheckSum=false);
  declareProperty("StatusMask", m_StatusMask=0x00000212);
  declareProperty("RequiredPhysicsNSamples", m_requiredPhysicsNSamples = 0);
  m_robFrag=NULL;
}

// destructor 
LArRodDecoder::~LArRodDecoder()
{
}

const InterfaceID& LArRodDecoder::interfaceID( )
{ return IID_ILArRodDecoder;  }

StatusCode
LArRodDecoder::initialize()
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
  
  sc=m_larCablingSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve LArCablingLegacyService" << endmsg;
    return StatusCode::FAILURE;
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
      //std::cout << " Can't get AlgTool for CaloCellCorrection\n ";
       return StatusCode::FAILURE; 
      }

    corr = dynamic_cast<CaloCellCorrection*> (tool); 
    if(!corr  ) 
      {msg(MSG::ERROR) << " Can't d-cast to CaloCellCorrection*  " << endmsg;
      //std::cout << " Can't d-cast to CaloCellCorrection* \n";
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

   //Fill Map for RodBlockStructure/Version number
   m_BlStructArray.resize(12); //Reserve space for 11 block types some of the do not (yet) exist.

   m_rodTranspV0  = (LArRodBlockStructure *) new LArRodBlockTransparentV0<LArRodBlockHeaderTransparentV0>;
   m_rodCalibV0   = (LArRodBlockStructure *) new LArRodBlockCalibrationV0<LArRodBlockHeaderCalibrationV0>;
   m_rodCalibV1   = (LArRodBlockStructure *) new LArRodBlockCalibrationV1;
   m_rodCalibV2   = (LArRodBlockStructure *) new LArRodBlockCalibrationV2;
   m_rodCalibV3   = (LArRodBlockStructure *) new LArRodBlockCalibrationV3;
   m_rodAccumV3   = (LArRodBlockStructure *) new LArRodBlockAccumulatedV3;
   m_rodPhysicsV0 = (LArRodBlockStructure *) new LArRodBlockPhysicsV0;
   m_rodPhysicsV1 = (LArRodBlockStructure *) new LArRodBlockPhysicsV1;
   m_rodPhysicsV2 = (LArRodBlockStructure *) new LArRodBlockPhysicsV2;
   m_rodPhysicsV3 = (LArRodBlockStructure *) new LArRodBlockPhysicsV3;
   m_rodPhysicsV4 = (LArRodBlockStructure *) new LArRodBlockPhysicsV4;
   m_rodPhysicsV5 = (LArRodBlockStructure *) new LArRodBlockPhysicsV5;
   m_rodPhysicsV6 = (LArRodBlockStructure *) new LArRodBlockPhysicsV6;
   // Only implemented for physics V5
   if (m_requiredPhysicsNSamples > 0) {
	((LArRodBlockPhysicsV5*)m_rodPhysicsV5)->setRequiredNSamples(m_requiredPhysicsNSamples);
	((LArRodBlockPhysicsV6*)m_rodPhysicsV6)->setRequiredNSamples(m_requiredPhysicsNSamples);
   }
   //m_BlStructArray[0].push_back();                               // obsolete old type
   //m_BlStructArray[1] does not exists

   // RodBlockType 2 = Transparent mode only
   m_BlStructArray[2].push_back(m_rodTranspV0);  //0  Transparent mode v0 05.01.2004
   m_BlStructArray[2].push_back(m_rodTranspV0);  //1  Transparent mode v0
   m_BlStructArray[2].push_back(m_rodTranspV0);  //2  Transparent mode v0
   m_BlStructArray[2].push_back(m_rodTranspV0);  //3  Transparent mode v0
   m_BlStructArray[2].push_back(m_rodTranspV0);  //4  Transparent mode v0
   m_BlStructArray[2].push_back(m_rodCalibV1);   //5  Calibration (Transparent mode) v1 17.01.2006
   m_BlStructArray[2].push_back(m_rodCalibV3);   //6  Calibration (Transparent mode) v3 31.05.2006
   m_BlStructArray[2].push_back(m_rodCalibV3);   //7  Calibration (Transparent mode) v3
   m_BlStructArray[2].push_back(m_rodCalibV3);   //8  Calibration (Transparent mode) v3
   m_BlStructArray[2].push_back(m_rodCalibV3);   //9  Calibration (Transparent mode) v3
   m_BlStructArray[2].push_back(m_rodCalibV3);   //10 Calibration (Transparent mode) v3
   m_BlStructArray[2].push_back(m_rodCalibV3);   //11 Calibration (Transparent mode) v3
   m_BlStructArray[2].push_back(m_rodCalibV3);   //12 Calibration (Transparent mode) v3

   // RodBlockType 3 = Test mode
   m_BlStructArray[3].push_back(m_rodTranspV0);  //Test mode (same output as above)

   // RodBlockType 4 = Physics mode
   m_BlStructArray[4].push_back(m_rodPhysicsV0); //0  Physics mode v0 05.01.2004 first draft
   m_BlStructArray[4].push_back(m_rodPhysicsV1); //1  Physics mode v1 19.08.2004 only small differences
   m_BlStructArray[4].push_back(m_rodPhysicsV2); //2  Physics mode v2 05.10.2004 adapted to real DSP data
   m_BlStructArray[4].push_back(m_rodPhysicsV2); //3  Physics mode v2 
   m_BlStructArray[4].push_back(m_rodPhysicsV2); //4  Physics mode v2 
   m_BlStructArray[4].push_back(m_rodPhysicsV2); //5  Physics mode v2 
   m_BlStructArray[4].push_back(m_rodPhysicsV2); //6  Physics mode v2 
   m_BlStructArray[4].push_back(m_rodPhysicsV2); //7  Physics mode v2 
   m_BlStructArray[4].push_back(m_rodPhysicsV2); //8  Physics mode v2 
   m_BlStructArray[4].push_back(m_rodPhysicsV4); //9  Physics mode v4 10.07.2007 for commissioning
   m_BlStructArray[4].push_back(m_rodPhysicsV5); //10 Physics mode v5 16.06.2008 for LHC 
   m_BlStructArray[4].push_back(m_rodPhysicsV5); //11 Physics mode v5 16.06.2008 for LHC 
   m_BlStructArray[4].push_back(m_rodPhysicsV6); //12 Physics mode v5 09.03.2011 for LHC 

   // RodBlockType 5 = Physics simulation mode
   m_BlStructArray[5].push_back(m_rodPhysicsV3); //0  Physics mode v3 11.04.2005 for simulation

   // RodBlockType 6 = Physics test mode
   m_BlStructArray[6].push_back(m_rodPhysicsV0); //1  Physics mode v0 05.01.2004 first draft
   m_BlStructArray[6].push_back(m_rodPhysicsV2); //2  Physics mode v2 05.10.2004 adapted to real DSP data
   m_BlStructArray[6].push_back(m_rodPhysicsV2); //3  Physics mode v2

   // RodBlockType 7 = Calibration mode
   m_BlStructArray[7].push_back(m_rodCalibV0);  //0  Calibration mode v0  05.01.2004
   m_BlStructArray[7].push_back(m_rodCalibV1);  //1  Calibration mode v1  17.01.2006
   m_BlStructArray[7].push_back(m_rodCalibV1);  //2  Calibration mode v1 
   m_BlStructArray[7].push_back(m_rodCalibV1);  //3  Calibration mode v1
   m_BlStructArray[7].push_back(m_rodCalibV1);  //4  Calibration mode v1
   m_BlStructArray[7].push_back(m_rodCalibV2);  //5  Calibration mode v2  26.04.2006
   m_BlStructArray[7].push_back(m_rodCalibV3);  //6  Calibration mode v3  31.05.2006
   m_BlStructArray[7].push_back(m_rodCalibV3);  //7  Calibration mode v3
   m_BlStructArray[7].push_back(m_rodCalibV3);  //8  Calibration mode v3
   m_BlStructArray[7].push_back(m_rodCalibV3);  //9  Calibration mode v3
   m_BlStructArray[7].push_back(m_rodCalibV3);  //10 Calibration mode v3
   m_BlStructArray[7].push_back(m_rodCalibV3);  //11 Calibration mode v3
   m_BlStructArray[7].push_back(m_rodCalibV3);  //12 Calibration mode v3

   //m_BlStructArray[8] does not exists
   //m_BlStructArray[9] does not exists

   // RodBlockType 10 = Accumulated mode (used for pre-processed pedestal runs)
   m_BlStructArray[10].push_back(m_rodAccumV3);  //0  Accumulated mode v3 10.06.2008
   m_BlStructArray[10].push_back(m_rodAccumV3);  //1  Accumulated mode v3
   m_BlStructArray[10].push_back(m_rodAccumV3);  //2  Accumulated mode v3
   m_BlStructArray[10].push_back(m_rodAccumV3);  //3  Accumulated mode v3
   m_BlStructArray[10].push_back(m_rodAccumV3);  //4  Accumulated mode v3
   m_BlStructArray[10].push_back(m_rodAccumV3);  //5  Accumulated mode v3
   m_BlStructArray[10].push_back(m_rodAccumV3);  //6  Accumulated mode v3
   m_BlStructArray[10].push_back(m_rodAccumV3);  //7  Accumulated mode v3
   m_BlStructArray[10].push_back(m_rodAccumV3);  //8  Accumulated mode v3
   m_BlStructArray[10].push_back(m_rodAccumV3);  //9  Accumulated mode v3 
   m_BlStructArray[10].push_back(m_rodAccumV3);  //10 Accumulated mode v3 
   m_BlStructArray[10].push_back(m_rodAccumV3);  //11 Accumulated mode v3 
   m_BlStructArray[10].push_back(m_rodAccumV3);  //12 Accumulated mode v3 

   m_larblockstruct = (LArRodBlockStructure*)NULL;

   if (m_useCellMakerTool) {
     IAlgTool* algTool;
     sc=toolSvc->retrieveTool("LArCellBuilderDriver",algTool);
     if (sc.isFailure()) {
       msg(MSG::ERROR) << "Unable to retrieve LArCellBuilderDriver" << endmsg;
       msg(MSG::ERROR) << "No LArDigit to LArCell conversion" << endmsg;
       // This should not prevent the tool to be initialized
       sc = StatusCode::SUCCESS;
     } else {
       if(!(m_larCellFromDigit=dynamic_cast<LArCellBuilderDriver*>(algTool))){
	 msg(MSG::ERROR) << "Unable to dynamic cast LArCellBuilderDriver" <<endmsg;
	 return StatusCode::FAILURE;
       }
       sc = m_larCellFromDigit->initialize();
       if (sc.isFailure()) {
	 msg(MSG::ERROR) << "Unable to initialize LArCellBuilderDriver" << endmsg;
       }
       // Warn the tools that they are not supposed to fill
       // the LArRawChannelCollection
       m_larCellFromDigit->initEventTools();
     }
   }//if m_useCellMakerTool


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



StatusCode LArRodDecoder::finalize()
{ // Clean up matrix of RodBlockStructures
 delete m_rodTranspV0 ;
 delete m_rodCalibV0  ;
 delete m_rodCalibV1  ;
 delete m_rodCalibV2  ;
 delete m_rodCalibV3  ;
 delete m_rodAccumV3  ;
 delete m_rodPhysicsV0;
 delete m_rodPhysicsV1;
 delete m_rodPhysicsV2;
 delete m_rodPhysicsV3;
 delete m_rodPhysicsV4;
 delete m_rodPhysicsV5;
 delete m_rodPhysicsV6;

 ATH_MSG_VERBOSE("Cleanup of LArRodBlockStructures finished");
 return StatusCode::SUCCESS;
}


void LArRodDecoder::fillCollection(const uint32_t* p, uint32_t n, LArDigitContainer& coll, CaloGain::CaloGain RequestedGain) //const
{ // Digit pointer
  LArDigit * dg=0 ;
  CaloGain::CaloGain calogain;
  uint32_t gain;
  int fcNb;
  std::vector<short> samples;
  LArRodBlockStructure* BlStruct=prepareBlockStructure(p, n, RequestedGain);
  if (!BlStruct) return;


  //std::cout << "Decoding LArDigits from ROD block size=0x" << std::hex << n << std::endl;
  //for(int i=0;i<n;i++)
  //  std::cout << "0x" << std::hex << i << " -     0x" << std::hex << p[i] << std::endl;

  //int iFeb=0;
  //int iCanTot=0,iCan=0; //For debug purpose
   do
    {
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
      //std::cout << "Processing FEB #" << iFeb++ << " FEBID " << std::hex << BlStruct->getFEBID()  << std::endl;
      while (BlStruct->getNextRawData(fcNb,samples,gain))
	{if (fcNb>=NthisFebChannel)
	  continue;
	if (samples.size()==0) continue; // Ignore missing cells
	HWIdentifier cId = m_onlineHelper->channel_Id(fId,fcNb);
	calogain=(CaloGain::CaloGain)gain;
	dg = new LArDigit(cId, calogain, samples);
	samples.clear();
	//std::cout << "Processing channel id " << iCan++ << " " << std::hex << fcNb << " " << cId << " Gain=" << gain << " samples "  << samples[0] << " " 
	//	  << samples[1] << " " << samples[2] << " " << samples[3] << " " << samples[4] << std::dec << std::endl;
	//std::cout << "Feb id=" << BlStruct->getFEBID()  << " ChannelID=" << fcNb << std::endl;
	coll.push_back(dg);
	}
    }
   while (BlStruct->nextFEB()); //Get NextFeb
   //std::cout << "Number of FEB processed " << iFeb << " fragment size = " << n << std::endl;
   return;
}

//*******


void LArRodDecoder::fillCollection(const uint32_t* p, uint32_t n, LArRawChannelContainer& coll, CaloGain::CaloGain RequestedGain) 
{ 
  int32_t energy;
  int32_t time;
  int32_t quality;
  uint32_t gain;
  int fcNb;
  HWIdentifier cId;
  LArRodBlockStructure* BlStruct=prepareBlockStructure(p, n, RequestedGain);
  if (!BlStruct) return;

  //std::cout << "Decoding LArRawChannels from ROD size=0x" << std::hex << n << std::endl;
  //for(int i=0;i<n;i++)
  //  std::cout << "0x" << std::hex << i << " -     0x" << std::hex << p[i] << std::endl;

  //int iFeb=0;
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
      //std::cout << "Processing FEB #" << iFeb++ << " FEBID " << std::hex << BlStruct->getFEBID()  << std::endl;
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
   //std::cout << "Number of FEB processed " << iFeb << " fragment size = " << n << std::endl;
   return;
}


void LArRodDecoder::fillCollection(const uint32_t* p, uint32_t n, LArCalibDigitContainer& coll, CaloGain::CaloGain RequestedGain)
{ // CalibDigit pointer
  LArCalibDigit * dg=0 ;
  uint32_t gain;
  CaloGain::CaloGain calogain;
  int fcNb;
  uint16_t dac, delay;
  bool ispulsed;
  std::vector<short> samples;

  //  for(int i=0;i<16;i++)
  //  std::cout << " -     " << std::hex << p[i] << std::endl;

  ATH_MSG_VERBOSE("FillCollection for LArCalibDigitContainer is called.");
  LArRodBlockStructure* BlStruct=prepareBlockStructure(p, n, RequestedGain);
  //  std::cout << "Done with LArRodBlock prepare" << std::endl;
  if (!BlStruct) return;
  // std::cout << " canSetCalibration() " << BlStruct->canSetCalibration() << std::endl;
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
	  
	  //std::cout << " in RawData calib block cId= " << cId << "  ispulsed=" << ispulsed << std::endl;
	  calogain=(CaloGain::CaloGain)gain;
	  dg = new LArCalibDigit(cId, calogain, samples, dac, delay, ispulsed);
	  samples.clear();
	  //std::cout << "Processing channel id" << iCan++ << " Gain=" << gain << std::endl;
	  //std::cout << "Feb id=" << BlStruct->getFEBID()  << " ChannelID=" << fcNb << std::endl; 
	  coll.push_back(dg);
	  //iCan++;
	  }
	//std::cout << "Found " << iCan << " Channels in this FEB." << std::endl;
      }
    while (BlStruct->nextFEB()); //Get NextFeb
  }
  else {//Not a calibration data block, try to get from database
    ATH_MSG_VERBOSE("Not LArCalibDigit data block found. Building it using DB values");
    //1st step, get Calib board config object
    const DataHandle<LArCalibParams> calibParams;
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
      
      //std::cout << "\n *** BlStruct->getFEBID() = " << std::endl ;
      //std::cout << BlStruct->getFEBID() << std::endl ;
      
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
	calibChannelIDs=&m_larCablingSvc->calibSlotLine(cId);
	//std::cout << m_larCablingSvc->isOnlineConnected(cId) << " "  << calibChannelIDs->size() << " " << fcNb << std::endl ;
      }
      while ( (!m_larCablingSvc->isOnlineConnected(cId) || calibChannelIDs->size()==0) && fcNb<128); // This is the right  conditions to exit the loop!
      
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
	  calibChannelIDs=&m_larCablingSvc->calibSlotLine(cId);
	  //if (calibChannelIDs->size()==0) 
	  //continue; //Disconnected channel
	  //For the time beeing, I assume we are in H8 and have only one calib channel per FEB channel

	  if (calibChannelIDs->size()!=0) {
	    csl_it=calibChannelIDs->begin();
	    //uint32_t calibLine = m_onlineHelper->channel(*csl_it);
	    ispulsed=calibParams->isPulsed(eventNb,*csl_it);
	      //std::cout << " in RawData CalibDigit cId= " << cId << " calibLine=" << calibLine << "  ispulsed=" << ispulsed << std::endl;
 	    
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

void LArRodDecoder::fillCollection(const uint32_t* p, uint32_t n, LArAccumulatedCalibDigitContainer& coll, CaloGain::CaloGain RequestedGain) //const
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

  LArRodBlockStructure* BlStruct=prepareBlockStructure(p, n, RequestedGain);
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
	  const std::vector<HWIdentifier>& calibChannelIDs=m_larCablingSvc->calibSlotLine(cId);
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

void LArRodDecoder::fillCollection(const uint32_t* p, uint32_t n, LArAccumulatedDigitContainer& coll, CaloGain::CaloGain RequestedGain) //const
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

  LArRodBlockStructure* BlStruct=prepareBlockStructure(p, n, RequestedGain);
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

void LArRodDecoder::fillCollection(const uint32_t* p, uint32_t n, LArFebHeaderContainer& coll,const CaloGain::CaloGain RequestedGain)
{
  LArFebHeader* larFebHeader;
  //uint32_t NWtot=0;
  HWIdentifier FEBID;

  //  for(int i=0;i<16;i++)
  //  std::cout << " -     " << std::hex << p[i] << std::endl;

  LArRodBlockStructure* BlStruct=prepareBlockStructure(p, n, RequestedGain);
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
    
    larFebHeader->SetFormatVersion(m_robFrag->rod_version());
    larFebHeader->SetSourceId(m_robFrag->rod_source_id());
    larFebHeader->SetRunNumber(m_robFrag->rod_run_no());
    larFebHeader->SetELVL1Id(m_robFrag->rod_lvl1_id());
    larFebHeader->SetBCId(m_robFrag->rod_bc_id());
    larFebHeader->SetLVL1TigType(m_robFrag->rod_lvl1_trigger_type());
    larFebHeader->SetDetEventType(m_robFrag->rod_detev_type());
  
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
      larFebHeader->SetFebELVL1Id(m_robFrag->rod_lvl1_id());
      larFebHeader->SetFebBCId(m_robFrag->rod_bc_id());
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


LArRodBlockStructure* LArRodDecoder::prepareBlockStructure(const uint32_t* p, uint32_t n, const CaloGain::CaloGain RequestedGain)
{ 
#ifndef NDEBUG
  ATH_MSG_DEBUG("Prepare LArRodBlockStructure. Got a fragement of size " << n);
#endif
  //if (n<2) //Avoid segmentation fault
  //  {msg(MSG::WARNING) << "Got empty Rod Fragment!" << endmsg;
  //   return NULL;
  //  }  
  //uint32_t blocksize=p[0]; //First word contains block size
  //if (blocksize>n)
  //  {(*m_log) << MSG::ERROR << "Got truncated ROD Fragment!" << endmsg;
  //   return NULL;
  //  }
  //Get version and blocktype form header
  eformat::helper::Version ver(m_robFrag->rod_version());
  const uint16_t rodMinorVersion=ver.minor_version();
  const uint32_t rodBlockType=m_robFrag->rod_detev_type()&0xff;
  if (rodBlockType>=m_BlStructArray.size() || m_BlStructArray[rodBlockType].size()==0)
    {msg(MSG::ERROR) << "Unknown Rod block type " <<  rodBlockType << endmsg;
     return NULL;
    }
  if (rodMinorVersion>=m_BlStructArray[rodBlockType].size() || m_BlStructArray[rodBlockType][rodMinorVersion]==NULL)
    {msg(MSG::ERROR) << "No version " << rodMinorVersion <<  " of Rod Block Type  " <<  rodBlockType << "known." << endmsg;
    return NULL;
    }
#ifndef NDEBUG
  else
    ATH_MSG_DEBUG("Found version " << rodMinorVersion << " of Rod block type " << rodBlockType);
#endif

  LArRodBlockStructure* BlStruct=m_BlStructArray[rodBlockType][rodMinorVersion];
  //BlStruct->dumpFragment(v); // For testing purpose
  if (!BlStruct->setFragment(p,n)) {
    static int nMess = 1, maxMess = 100;
    if (nMess < maxMess) {
      msg(MSG::ERROR) << "Could not set fragment (wrong number of samples in data ?) - container will not be filled" << endmsg;
      nMess++;
      if (nMess == maxMess)
        msg(MSG::ERROR) << "This message will not be repeated" << endmsg;
    }
    return NULL;
  }
#ifndef NDEBUG
  ATH_MSG_VERBOSE("Set Fragment at address "<<  (uint32_t *) &(p[0]) << " " << p[5]);
#endif

  BlStruct->setGain(RequestedGain); //Will be ignored if BlockStructure does not support fixed gains.    
  //FIXME: Very ugly hack! See explanation in LArRodDecoder.h 
  if (m_firstSample) {
    BlStruct->setFirstSample(m_firstSample);
  }
  return BlStruct;
}

