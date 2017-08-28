/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawDataByteStreamCnv/ALFAEventBitInfo.h"
#include "ALFA_RawDataByteStreamCnv/ALFA_Decoder.h"
#include "eformat/Issue.h"
#include "xAODEventInfo/EventInfo.h"
#include <algorithm>

using xAOD::EventInfo;

static const InterfaceID IID_IALFA_Decoder ("ALFA_Decoder", 1, 0);
const InterfaceID& ALFA_Decoder::interfaceID()
{
  return IID_IALFA_Decoder;
}

////////////////////////
// constructor
////////////////////////

ALFA_Decoder::ALFA_Decoder ( const std::string& type, const std::string& name,const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_robDataProvider ("ROBDataProviderSvc",name)
    //,m_EvtStore(0) , m_ALFA_RawDataContainerReadOut(0), m_ALFA_RawDataCollectionReadOut(0), m_ALFA_RawDataReadOut(0)
{
  declareInterface<ALFA_Decoder>(this);

}



////////////////////////
// destructor
////////////////////////
ALFA_Decoder::~ALFA_Decoder()
{
}


////////////////////////
// initialize() -
////////////////////////
StatusCode ALFA_Decoder::initialize()
{


  StatusCode sc;
  sc = AthAlgTool::initialize();
  msg(MSG::DEBUG) << " ALFA_RoDDecoder::initialize" << endmsg;
  if (sc.isFailure()) return sc;

  // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure())
    {
      msg(MSG::FATAL) << "Failed to retrieve service " << m_robDataProvider << endmsg;
      return StatusCode::FAILURE;
    } else
    msg(MSG::DEBUG) << "Retrieved service " << m_robDataProvider << endmsg;


  if(StatusCode::SUCCESS !=serviceLocator()->service("StoreGateSvc", m_EvtStore))
    {
      msg(MSG::FATAL) <<"Can't get StoreGateSvc "<< endmsg;
      return StatusCode::FAILURE;
    }


  m_fragment_number = 0;


  // Initialize decoding classes
  m_ALFA_RawDataReadOut = new ALFA_RawDataReadOut();
  m_ALFA_RawDataCollectionReadOut = new ALFA_RawDataCollectionReadOut();
  m_ALFA_RawDataContainerReadOut = new ALFA_RawDataContainerReadOut();


  return StatusCode::SUCCESS;
}






////////////////////////
// finalize() -
////////////////////////
StatusCode ALFA_Decoder::finalize()
{

  msg(MSG::DEBUG) << " ALFA_RoDDecoder::FINALIZE" << endmsg;

  msg(MSG::DEBUG) << " Bytestream summary:" << m_fragment_number << " fragments found" << endmsg;


  delete m_ALFA_RawDataReadOut;
  delete m_ALFA_RawDataCollectionReadOut;
  delete m_ALFA_RawDataContainerReadOut;

  return StatusCode::SUCCESS;
}



////////////////////////
// fillCollection() -
////////////////////////
StatusCode ALFA_Decoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *robFrag, ALFA_RawDataContainer* rdoCont, std::vector<unsigned int>* )
{
  msg(MSG::DEBUG) << " ALFA_RoDDecoder::fillCollection" << endmsg;

  ALFA_RawDataCollection* collection = 0;

  uint32_t wordPos   = 0;

  std::vector<uint32_t> VData;

  VData.clear();


  uint32_t Event_num=0;
  uint32_t Event_num_max=0;
  uint16_t fiber_hit;

  uint16_t tmpFiber;
  uint16_t thisMB;
  uint16_t tmpMB;
  uint16_t MBIdNum=0;

  uint16_t tmpADC1=10000;
  uint16_t tmpADC2=10000;
  std::vector<bool> tmpPattern;
  uint16_t tmpScaler=100;

  std::vector<bool> tmpLvl1Pattern;
  std::vector<bool> tmpLvl2Pattern;
  std::vector<bool> tmpEFPattern;
  uint32_t wo=0;

  uint32_t EcrId=1;
  uint32_t LvL1 =0;

  uint32_t Count_evt=0;

  int n=0;


  // retrieve EventInfo
  const EventInfo* eventInfo_c=0;
  StatusCode sc = evtStore()->retrieve(eventInfo_c);
  if (sc.isFailure()) {
    ATH_MSG_WARNING (" cannot retrieve EventInfo, will not set ALFA bit information ");
  }
  
  EventInfo* eventInfo=0;
  if (eventInfo_c)
    eventInfo = const_cast<EventInfo*>(eventInfo_c);

  // Check the ROB and ROD fragment for lenght and version consistency

  try
    {
      robFrag->check();
    }

  catch (eformat::Issue &ex)
    {
      msg(MSG::WARNING) <<ex.what ()<< endmsg;
      if (!eventInfo->setErrorState(EventInfo::ForwardDet, EventInfo::Error))
	msg(MSG::WARNING) <<"Cannot set ALFA error state"<< endmsg;
      return StatusCode::SUCCESS;  // error in fragment - we search for no collection
    }

  // Check the ROB status word for truncation or corruption.

  uint32_t nstat = robFrag->nstatus();

  if (nstat)
    {
      const uint32_t *it;
      robFrag->status (it);


      if (*it)
	{
	  msg(MSG::WARNING) << " Error in ROB status word: 0x" << endmsg;
	  if (!eventInfo->setErrorState(EventInfo::ForwardDet, EventInfo::Error))
	    msg(MSG::WARNING) <<"Cannot set ALFA error state"<< endmsg;
	  if (!eventInfo->setEventFlagBit(EventInfo::ForwardDet, ALFAEventBitInfo::ROB_ERROR))
	    msg(MSG::WARNING) <<"Cannot set event bit info for ALFA"<< endmsg;

	  return StatusCode::SUCCESS;
	}
    }

  // set the data pointer type

  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;

  robFrag->rod_data(vint);

  // get number of data words in the fragment
  uint32_t size = robFrag->rod_ndata();

  // get source ID
  uint32_t ROD_source_ID = robFrag->rod_source_id();


  uint32_t ROD_LVL1_ID  = robFrag->rod_lvl1_id();

  msg(MSG::DEBUG) << "============================" << std::endl;
  msg(MSG::DEBUG) << "Frag Size  : " << robFrag->rod_fragment_size_word() << endmsg;
  msg(MSG::DEBUG) << "Header Size: " << robFrag->rod_header_size_word() << endmsg;
  msg(MSG::DEBUG) << "Source  ID : " << ROD_source_ID << endmsg;
  msg(MSG::DEBUG) << "Run num    : " << robFrag->rod_run_no() << endmsg;
  msg(MSG::DEBUG) << "Version    : " << robFrag->rod_version() << endmsg;
  msg(MSG::DEBUG) << " ROD_LVL1_ID " << ROD_LVL1_ID <<  endmsg;
  msg(MSG::DEBUG) << "============================" << std::endl;

  // msg(MSG::INFO) << "ROD Time_StampID " <<robFrag->bc_time_seconds() <<  endmsg;


  uint32_t evt_Id = ((robFrag->rod_lvl1_id()) & 0xFF000000) >> 24;
  uint32_t lvl1_Id = ((robFrag->rod_lvl1_id()) & 0x00FFFFFF) ;


  msg(MSG::DEBUG) <<" evt_Id    : " <<  evt_Id << " lvl1_Id    : " <<  lvl1_Id <<endmsg;

  /*msg(MSG::DEBUG) << " **********Decoder dumping the words******** "<< endmsg;

    if (size > 0) {
    msg(MSG::DEBUG) << " The size of this ROD-read is "<< endmsg;
    for (unsigned int i=0; i < size; i++)
    msg(MSG::DEBUG) << " word " << i << " = " << MSG::hex << vint[i] << MSG::dec<< endmsg;
    } else {
    msg(MSG::DEBUG) << " Buffer size 0 ! "<< endmsg;
    return StatusCode::FAILURE;
    }*/


  m_ALFA_RawDataCollectionReadOut->decodeWord(vint[wordPos]);
  m_ALFA_RawDataReadOut->decodeWord(vint[wordPos]);
  m_ALFA_RawDataContainerReadOut->decodeWord(vint[wordPos]);

  int l=0;

  if(EcrId!=evt_Id)
    {
      n++;
      l=1;
    }

  if (n==0) Event_num = LvL1;
  else Event_num =  ((robFrag->rod_lvl1_id()) & 0x00FFFFFF) + Event_num;

  LvL1 = ((robFrag->rod_lvl1_id()) & 0x00FFFFFF)+1 ;
  EcrId = ((robFrag->rod_lvl1_id()) & 0xFF000000) >> 24;

  if(l==1) Event_num_max=Event_num;

  Event_num =  ((robFrag->rod_lvl1_id()) & 0x00FFFFFF)+1 + Event_num_max;

  Count_evt++;
  //if (Count_evt == maxEvt) break;

  msg(MSG::DEBUG) << " Event_num " << Event_num << " Count_evt " << Count_evt <<  endmsg;




  if (m_ALFA_RawDataCollectionReadOut->is_BOB())
    {
      msg(MSG::DEBUG) << " Found the beginning of buffer "<< endmsg;
      // Check that Lvl1d matches the one from the ROD header. this is the number of event.it corresponds to m_ALFA_RawDataReadOut->ecnt_BOT() and m_ALFA_RawDataReadOut->ecnt_EOT()
      msg(MSG::DEBUG) << " Level 1 Id : " << m_ALFA_RawDataCollectionReadOut->lvl1Id()<< endmsg;
    }
  else
    {
      msg(MSG::DEBUG) << " Beginning of block not found BOB "<< endmsg;
    } // BOB


  while (!m_ALFA_RawDataCollectionReadOut->is_EOB())
    {

      //Check for corrupted data

      while ((!m_ALFA_RawDataCollectionReadOut->is_BOL()) && (!m_ALFA_RawDataCollectionReadOut->is_EOB()))
	{
	  wordPos += 1;
	  if (wordPos >= size)
	    {
	      msg(MSG::WARNING)<<" Error: data corrupted" << endmsg;
	      if (!eventInfo->setErrorState(EventInfo::ForwardDet, EventInfo::Error))
		msg(MSG::WARNING) <<"Cannot set ALFA error state"<< endmsg;
	      if (!eventInfo->setEventFlagBit(EventInfo::ForwardDet, ALFAEventBitInfo::CORRUPTION))
		msg(MSG::WARNING) <<"Cannot set event bit info for ALFA"<< endmsg;
	      
	      return StatusCode::SUCCESS;
	    }
	  m_ALFA_RawDataCollectionReadOut->decodeWord(vint[wordPos]);
	}


      if (m_ALFA_RawDataCollectionReadOut->is_BOL())
	{
	  msg(MSG::DEBUG) << " Found the beginning of Link BOL"<< endmsg;
	  msg(MSG::DEBUG) << " MrodNum BOL : " << m_ALFA_RawDataCollectionReadOut->MrodNum()<< " RODinput BOL : " <<m_ALFA_RawDataCollectionReadOut->RODinput()<< endmsg;

	} // is_BOL()

      else if (m_ALFA_RawDataCollectionReadOut->is_EOB())
	{
	  msg(MSG::WARNING)<<" Error: collection not found " << endmsg;
	  if (!eventInfo->setErrorState(EventInfo::ForwardDet, EventInfo::Error))
	    msg(MSG::WARNING) <<"Cannot set ALFA error state"<< endmsg;
	  if (!eventInfo->setEventFlagBit(EventInfo::ForwardDet, ALFAEventBitInfo::COLL_NOT_FOUND))
	    msg(MSG::WARNING) <<"Cannot set event bit info for ALFA"<< endmsg;
	      
	  return StatusCode::SUCCESS;
	} // is_EOB()


      wordPos += 1;
      if (wordPos >= size)
	{
	  msg(MSG::WARNING)<<" Error: data corrupted"<< endmsg;
	  if (!eventInfo->setErrorState(EventInfo::ForwardDet, EventInfo::Error))
	    msg(MSG::WARNING) <<"Cannot set ALFA error state"<< endmsg;
	  if (!eventInfo->setEventFlagBit(EventInfo::ForwardDet, ALFAEventBitInfo::CORRUPTION))
	    msg(MSG::WARNING) <<"Cannot set event bit info for ALFA"<< endmsg;
	  return StatusCode::SUCCESS;
	}


      m_ALFA_RawDataCollectionReadOut->decodeWord(vint[wordPos]);
      m_ALFA_RawDataReadOut->decodeWord(vint[wordPos]);

      while (!m_ALFA_RawDataCollectionReadOut->is_TWC())
	{

	  uint16_t slotIdNum  = m_ALFA_RawDataReadOut->SlotId();

	  msg(MSG::DEBUG)<<"  Decoding data from Slot Id number : " <<  slotIdNum << endmsg;

	  ALFA_RawData* rawData;

	  // Loop on the TDC data words and create the corresponding RDO's

	  while (!m_ALFA_RawDataReadOut->is_EOT())
	    {
	      wordPos += 1;
	      if (wordPos >= size)
		{
		  msg(MSG::WARNING)<<" Error: data corrupted" << endmsg;
		  if (!eventInfo->setErrorState(EventInfo::ForwardDet, EventInfo::Error))
		    msg(MSG::WARNING) <<"Cannot set ALFA error state"<< endmsg;
		  if (!eventInfo->setEventFlagBit(EventInfo::ForwardDet, ALFAEventBitInfo::CORRUPTION))
		    msg(MSG::WARNING) <<"Cannot set event bit info for ALFA"<< endmsg;

		  return StatusCode::SUCCESS;
		}

	      m_ALFA_RawDataReadOut->decodeWord(vint[wordPos]);

	      uint16_t PMFIdNum;
	      uint16_t WordIdNum;
	      uint16_t FiberNum;

	      if (m_ALFA_RawDataReadOut->is_TDCt() && m_ALFA_RawDataReadOut->bit26_27()==0 &&  m_ALFA_RawDataReadOut->bit16()==false && m_ALFA_RawDataReadOut->bit18()==false )
		{

		  //msg(MSG::DEBUG)<<" DECODER : PMFId  " <<  m_ALFA_RawDataReadOut->PMFId() <<  " MBId  " << m_ALFA_RawDataReadOut->MBId() << endmsg;


		  if(m_ALFA_RawDataReadOut->PMFId() ==0 && m_ALFA_RawDataReadOut->WordId() ==0)  //PMF0 contains the information on wich MB is readen
		    {

		      msg(MSG::DEBUG) << " Creation of the new collection"<< endmsg;

		      tmpMB = m_ALFA_RawDataReadOut->fiber();


		      for (unsigned int i=0;i<8;i++)
			{

			  thisMB = (char) tmpMB & 0x00000001;
			  tmpMB>>=1;

			  if(thisMB==1)
			    {
			      MBIdNum=i+1;

			      collection = getCollection(MBIdNum,rdoCont );
			      if(collection) collection->SetMBId_POT(MBIdNum);

			      msg(MSG::DEBUG)<<"  DECODER : MBIdNum  " <<  MBIdNum << endmsg;


			    }


			} // loop over 8 Mother Boards

		    } // PMFId() ==0 & WordId() ==0




		  if(m_ALFA_RawDataReadOut->PMFId() >0 && m_ALFA_RawDataReadOut->PMFId() <24)
		    {


		      tmpFiber = m_ALFA_RawDataReadOut->fiber();

		      WordIdNum = m_ALFA_RawDataReadOut->WordId();
		      PMFIdNum = m_ALFA_RawDataReadOut->PMFId();

		      rawData = new ALFA_RawData(PMFIdNum);

		      rawData->SetWordId_PMF(WordIdNum);

		      rawData->SetPMFId_PMF(PMFIdNum);
		      rawData->SetMBId_PMF(MBIdNum);

		      rawData->Set_error_bit17(m_ALFA_RawDataReadOut->error_bit17());

		      std::vector<uint16_t> hitId;

		      for (unsigned int i=0;i<16;i++)
			{

			  FiberNum=100;
			  fiber_hit = (char) tmpFiber & 0x00000001;
			  tmpFiber>>=1;

			  if(fiber_hit==1)
			    {
			      FiberNum=m_ALFA_RawDataReadOut->WordId()*16+i;
			      hitId.push_back(FiberNum);
			      //msg(MSG::DEBUG)<<" fiberNum = " <<  FiberNum <<",  fiber_hit = " << fiber_hit << "  PMFIdNum = " << PMFIdNum <<"  WordIdNum = " << WordIdNum <<endmsg;
			    }
			  else 	hitId.push_back(100);

			} // loop over 16 fibers


		      rawData->SetMarocChan_PMF(hitId);

		      rawData->addData(vint[wordPos]);
		      rawData->PushBack_PMF(FiberNum);
		      if(collection) collection->push_back(rawData);
		      else delete rawData;

		    } // PMFIdNum >0 && <24





		  if(m_ALFA_RawDataReadOut->PMFId()==24) // PMF 24 contains trigger and charge infos
		    {


		      if(m_ALFA_RawDataReadOut->WordId()==0)
			{
			  tmpPattern=m_ALFA_RawDataReadOut->pattern();
			  if(collection) collection->Set_pattern_POT(tmpPattern);

			  // 			  // TEST of the readed trigger pattern
			  // 			  std::vector<bool> testpattern = m_ALFA_RawDataReadOut->pattern();
			  // 			  //uint16_t testMBid = m_ALFA_RawDataReadOut->MBId();
			  // 			  uint16_t testPMFid = m_ALFA_RawDataReadOut->PMFId();
			  // 			  msg() << MSG::DEBUG << "MB :  " << MBIdNum << endmsg;
			  // 			  msg(MSG::DEBUG) << "PMF :  " << testPMFid << endmsg;
			  // 			  for (unsigned int i = 0; i < (unsigned int) testpattern.size(); i++)
			  // 			    {
			  // 			      msg(MSG::DEBUG) << "Trig pattern " << i << " :  " << testpattern[i] << endmsg;
			  // 			    }
			  // 			  // end of TEST of the readed trigger pattern


			} // m_ALFA_RawDataReadOut->WordId()==0


		      else
			if(m_ALFA_RawDataReadOut->WordId()==1)
			  {
			    tmpScaler=m_ALFA_RawDataReadOut->scaler();
			    if(collection) collection->Set_scaler_POT(tmpScaler);
			  }
			else
			  if(m_ALFA_RawDataReadOut->WordId()==2)
			    {
			      tmpADC1=m_ALFA_RawDataReadOut->ADC();
			      if(collection) collection->Set_ADC1_POT(tmpADC1);
			      if(collection) msg(MSG::DEBUG) <<" tmpADC1 = " <<  tmpADC1 << endmsg;
			    }
			  else
			    if(m_ALFA_RawDataReadOut->WordId()==3)
			      {
				tmpADC2=m_ALFA_RawDataReadOut->ADC();
				if(collection) collection->Set_ADC2_POT(tmpADC2);
				if(collection) msg(MSG::DEBUG)<<" tmpADC2 = " <<  tmpADC2 << endmsg;
			      }

		      if(collection) collection->SetTrigSyncErr(m_ALFA_RawDataReadOut->error_bit17());

		    } //TSM PMF24


		} // TSM


	    } // End of loop on AMTs (EOT)


	  if (m_ALFA_RawDataReadOut->is_EOT() && m_ALFA_RawDataReadOut->bit24_27()!=0)
	    {
	      msg(MSG::DEBUG) << " Event Counter EOT : " << m_ALFA_RawDataReadOut->ecnt_EOT()<< endmsg;
	    }

	  wordPos += 1;
	  if (wordPos >= size)
	    {
	      msg(MSG::WARNING)<<" Error: data corrupted" << endmsg;
	      if (!eventInfo->setErrorState(EventInfo::ForwardDet, EventInfo::Error))
		msg(MSG::WARNING) <<"Cannot set ALFA error state"<< endmsg;
	      if (!eventInfo->setEventFlagBit(EventInfo::ForwardDet, ALFAEventBitInfo::CORRUPTION))
		msg(MSG::WARNING) <<"Cannot set event bit info for ALFA"<< endmsg;

	      return StatusCode::SUCCESS;
	    }


	  m_ALFA_RawDataCollectionReadOut->decodeWord(vint[wordPos]);
	  m_ALFA_RawDataReadOut->decodeWord(vint[wordPos]);

	} // End of loop on TDCs (TWC)

      wordPos += 1;
      if (wordPos >= size)
	{
	  msg(MSG::WARNING)<<" Error: data corrupted" << endmsg;
	  if (!eventInfo->setErrorState(EventInfo::ForwardDet, EventInfo::Error))
	    msg(MSG::WARNING) <<"Cannot set ALFA error state"<< endmsg;
	  if (!eventInfo->setEventFlagBit(EventInfo::ForwardDet, ALFAEventBitInfo::CORRUPTION))
	    msg(MSG::WARNING) <<"Cannot set event bit info for ALFA"<< endmsg;

	  return StatusCode::SUCCESS;
	}

      m_ALFA_RawDataCollectionReadOut->decodeWord(vint[wordPos]);

    } //EOB


      // take full event
  const eformat::FullEventFragment<const uint32_t*> * event =  m_robDataProvider->getEvent();

  msg(MSG::DEBUG) << "========FULL=============" << std::endl;
  msg(MSG::DEBUG) << "Event time (sec): " << (uint32_t)event->bc_time_seconds() << std::endl;
  msg(MSG::DEBUG) << "Event time (ns):  " << (uint32_t)event->bc_time_nanoseconds() << std::endl;
  msg(MSG::DEBUG) << "Global ID:        " << (uint32_t)event->global_id() << std::endl;
  msg(MSG::DEBUG) << "Run Type:         " << (uint32_t)event->run_type() << std::endl;
  msg(MSG::DEBUG) << "Run Number:       " << (uint32_t)event->run_no() << std::endl;
  msg(MSG::DEBUG) << "Lumi Block:       " << (uint32_t)event->lumi_block() << std::endl;
  msg(MSG::DEBUG) << "Level1 ID:        " << (uint32_t)event->lvl1_id() << std::endl;
  msg(MSG::DEBUG) << "BCID:             " << (uint32_t)event->bc_id() << std::endl;
  msg(MSG::DEBUG) << "Level1 trig type: " << (uint32_t)event->lvl1_trigger_type() << std::endl;
  msg(MSG::DEBUG) << "Level1 Nwords:    " << (uint32_t)event->nlvl1_trigger_info() << std::endl;
  msg(MSG::DEBUG) << "Level2 Nwords:    " << (uint32_t)event->nlvl2_trigger_info() << std::endl;
  msg(MSG::DEBUG) << "EF Nwords:    		" << (uint32_t)event->nevent_filter_info() << std::endl;
  msg(MSG::DEBUG) << "============================" << std::endl;

  uint32_t Time_StampID  = event->bc_time_seconds();
  uint32_t Time_StampnsID  = event->bc_time_nanoseconds();
  uint32_t BC_ID  = event->bc_id();
  uint32_t LumiBlock_ID = event->lumi_block();

  if(rdoCont) rdoCont->SetTimeStamp(Time_StampID);
  if(rdoCont) rdoCont->SetTimeStampns(Time_StampnsID);
  if(rdoCont) rdoCont->SetBCId(BC_ID);
  if(rdoCont) rdoCont->SetLumiBlock(LumiBlock_ID);

  /*
    For the LVL1 trigger there are three consecutive blocks of 256 bits (8 ints):
    256 bits for the TBP (trigger before prescale)
    256 bits for the TAP (trigger after prescale
    256 bits for the TAV (trigger after veto)
  */
  const uint32_t *point_lvl1 = event->lvl1_trigger_info();
  uint32_t point_lvl1_end = event->nlvl1_trigger_info();
  for(uint32_t i = 0; i <= point_lvl1_end; ++i)
    {
      wo = *(point_lvl1 + i);
      for(uint32_t j = 0; j < 32; ++j)
	{
	  tmpLvl1Pattern.push_back((wo & 0x1));
	  wo >>= 1;
	}
    }
  if(rdoCont) rdoCont->SetLvl1Pattern(tmpLvl1Pattern);

  wo=0;

  //It's not defined how many LVL2 and EF words are there
  const uint32_t *point_lvl2 = event->lvl2_trigger_info();
  uint32_t point_lvl2_end = event->nlvl2_trigger_info();
  for(uint32_t i = 0; i <= point_lvl2_end; ++i)
    {
      wo = *(point_lvl2 + i);
      for(uint32_t j = 0; j < 32; ++j)
	{
	  tmpLvl2Pattern.push_back((wo & 0x1));
	  wo >>= 1;
	}
    }
  //std::cout << std::dec << std::endl;
  if(rdoCont) rdoCont->SetLvl2Pattern(tmpLvl2Pattern);

  wo=0;

  //std::cout << "Event num  " << event->global_id() << std::endl;
  //std::cout << "EF pat:" << std::endl;
  const uint32_t *point_ef = event->event_filter_info();
  uint32_t point_ef_end = event->nevent_filter_info();
  for(uint32_t i = 0; i <= point_ef_end; ++i)
    {
      wo = *(point_ef + i);
      //std::cout << std::dec << i << ": ";
      //std::cout << std::hex << wo << "  ";
      for(uint32_t j = 0; j < 32; ++j)
	{
	  tmpEFPattern.push_back((wo & 0x1));
	  wo >>= 1;
	}
    }
  //std::cout << std::dec << std::endl;
  if(rdoCont) rdoCont->SetEFPattern(tmpEFPattern);



  if (m_ALFA_RawDataCollectionReadOut->is_EOB())
    {
      msg(MSG::DEBUG) << " Found the end of block EOB ---------------------- "<<  endmsg;
    }

  wordPos += 1;
  m_ALFA_RawDataCollectionReadOut->decodeWord(vint[wordPos]);

  return StatusCode::SUCCESS;

}


////////////////////////
// getCollection() - return collection corresponding to a particular channel
//                   if it exists in container, or create it if it doesn't
////////////////////////

ALFA_RawDataCollection* ALFA_Decoder::getCollection(unsigned int MBIdNum, ALFA_RawDataContainer* cont)
{

  msg(MSG::DEBUG) << " ALFA_RoDDecoder::getCollection" << endmsg;

  ALFA_RawDataCollection* coll;

  ALFA_RawDataContainer::const_iterator cont_it;
  bool collExists = false;
  ALFA_RawDataContainer::const_iterator it = cont->begin();
  ALFA_RawDataContainer::const_iterator itE= cont->end();


  for (; it!=itE; ++it)
    {
      if ((*it)->GetMBId_POT() == MBIdNum)
	{
	  collExists = true;
	  cont_it = it;
	}
    }
  if (collExists)
    {
      coll = const_cast<ALFA_RawDataCollection*>(&**cont_it);
      msg(MSG::DEBUG) << " Collection exists " << endmsg;
      return 0;

    }
  else // if collection does not exist create it
    {
      coll = new ALFA_RawDataCollection(MBIdNum);
      msg(MSG::DEBUG) << " create collection; MBId  " << MBIdNum <<endmsg;
      cont->push_back(coll);  // add collection to container


    }


  return coll;
}
