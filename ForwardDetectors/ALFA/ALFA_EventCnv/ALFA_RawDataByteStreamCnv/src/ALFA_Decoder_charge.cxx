/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawDataByteStreamCnv/ALFA_Decoder_charge.h"

#include <algorithm> 

static const InterfaceID IID_IALFA_Decoder_charge ("ALFA_Decoder_charge", 1, 0);
const InterfaceID& ALFA_Decoder_charge::interfaceID()
{
  return IID_IALFA_Decoder_charge;
}

////////////////////////
// constructor
////////////////////////

ALFA_Decoder_charge::ALFA_Decoder_charge ( const std::string& type, const std::string& name,const IInterface* parent)  
: AthAlgTool(type,name,parent),
m_robDataProvider ("ROBDataProviderSvc",name)
//,m_EvtStore(0) , m_ALFA_RawDataContainerReadOut(0), m_ALFA_RawDataCollectionReadOut(0), m_ALFA_RawDataReadOut_charge(0) 
{
declareInterface<ALFA_Decoder_charge>(this);

}



////////////////////////
// destructor  
////////////////////////
ALFA_Decoder_charge::~ALFA_Decoder_charge(){}




////////////////////////
// initialize() -
////////////////////////
StatusCode ALFA_Decoder_charge::initialize() {
  
   
  StatusCode sc;
  sc = AthAlgTool::initialize(); 
  msg(MSG::DEBUG) << " ALFA_RoDDecoder_charge::initialize" << endmsg;
  if (sc.isFailure()) return sc;

 // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_robDataProvider << endmsg;
    return StatusCode::FAILURE;
  } else
    msg(MSG::DEBUG) << "Retrieved service " << m_robDataProvider << endmsg;
 

  if(StatusCode::SUCCESS !=serviceLocator()->service("StoreGateSvc", m_EvtStore)) {
   msg(MSG::FATAL) <<"Can't get StoreGateSvc "<< endmsg;
    return StatusCode::FAILURE; 
  }

  
    m_fragment_number = 0;

  
   // Initialize decoding classes
    m_ALFA_RawDataReadOut_charge = new ALFA_RawDataReadOut_charge();
    m_ALFA_RawDataCollectionReadOut = new ALFA_RawDataCollectionReadOut();
    m_ALFA_RawDataContainerReadOut = new ALFA_RawDataContainerReadOut();

    
  return StatusCode::SUCCESS;
}






////////////////////////
// finalize() -
////////////////////////
StatusCode ALFA_Decoder_charge::finalize() { 

msg(MSG::DEBUG) << " ALFA_RoDDecoder_charge::FINALIZE" << endmsg;

msg(MSG::DEBUG) << " Bytestream summary:" << m_fragment_number << " fragments found" << endmsg;


  delete m_ALFA_RawDataReadOut_charge;
  delete m_ALFA_RawDataCollectionReadOut;
  delete m_ALFA_RawDataContainerReadOut;
    
  return StatusCode::SUCCESS;
}



////////////////////////
// fillCollection() -
////////////////////////
StatusCode ALFA_Decoder_charge::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *robFrag, ALFA_RawDataContainer_charge* rdoCont, std::vector<unsigned int>* ) {
 msg(MSG::DEBUG) << " ALFA_RoDDecoder_charge::fillCollection" << endmsg;

StatusCode sc = StatusCode::SUCCESS;

ALFA_RawDataCollection_charge* collection = 0;   

 uint32_t wordPos   = 0;

 std::vector<uint32_t> VData; 

 VData.clear();


uint32_t Event_num=0;
uint32_t Event_num_max=0;
uint16_t charge_hit;

uint16_t tmpCharge=10;
uint16_t MBIdNum=0;

uint32_t EcrId=1;
uint32_t LvL1 =0;

uint32_t Count_evt=0;


uint16_t CountPMF0=0;
				
	int n=0;
 
   // Check the ROB and ROD fragment for lenght and version consistency
/*
   try 
  {
     robFrag->check();
   }

 catch (eformat::Issue &ex)
  {
  msg(MSG::DEBUG) <<ex.what ()<< endmsg; 
     return StatusCode::FAILURE;  // error in fragment - we search for no collection
   }

  // Check the ROB status word for truncation or corruption.

  uint32_t nstat = robFrag->nstatus();
  
   if (nstat)
   {
     const uint32_t *it;
     robFrag->status (it);
 
 
     if (*it)
    {
      msg(MSG::DEBUG) << " Error in ROB status word: 0x" << endmsg; 
        return StatusCode::FAILURE;
     }
  }
 */ 
  
  // set the data pointer type
   
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;
    
  robFrag->rod_data(vint);

 // get number of data words in the fragment
  uint32_t size = robFrag->rod_ndata(); 

 // get source ID
  uint32_t ROD_source_ID = robFrag->rod_source_id();


  uint32_t ROD_LVL1_ID  = robFrag->rod_lvl1_id(); 
  
msg(MSG::DEBUG) << "============================" << std::endl;
msg(MSG::DEBUG) <<"Frag Size  : " << robFrag->rod_fragment_size_word() << endmsg;
msg(MSG::DEBUG) <<"Header Size: " << robFrag->rod_header_size_word() << endmsg;
msg(MSG::DEBUG) <<"Source  ID : " << ROD_source_ID << endmsg;
msg(MSG::DEBUG) <<"Run num    : " << robFrag->rod_run_no() << endmsg;
msg(MSG::DEBUG) <<"Version    : " << robFrag->rod_version() << endmsg;
msg(MSG::DEBUG) << " ROD_LVL1_ID " <<ROD_LVL1_ID<<  endmsg; 
msg(MSG::DEBUG) << "============================" << std::endl;
 

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
	m_ALFA_RawDataReadOut_charge->decodeWord(vint[wordPos]);
	m_ALFA_RawDataContainerReadOut->decodeWord(vint[wordPos]);
	
   		int l=0;

		if(EcrId!=evt_Id) {
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
	
	msg(MSG::INFO) << " Event_num " << Event_num << " Count_evt " << Count_evt <<  endmsg; 
	


	
 	if (m_ALFA_RawDataCollectionReadOut->is_BOB()) 
   	 {
		msg(MSG::DEBUG) << " Found the beginning of buffer "<< endmsg;     
  		  // Check that Lvl1d matches the one from the ROD header. this is the number of event.it corresponds to m_ALFA_RawDataReadOut_charge->ecnt_BOT() and m_ALFA_RawDataReadOut_charge->ecnt_EOT()
		msg(MSG::DEBUG) << " Level 1 Id : " << m_ALFA_RawDataCollectionReadOut->lvl1Id()<< endmsg; 
   	  } else {
		msg(MSG::DEBUG) << " Beginning of block not found BOB "<< endmsg; 
  	  } // BOB

	
    	while (!m_ALFA_RawDataCollectionReadOut->is_EOB()){
	
	   		//Check for corrupted data
			
			while ((!m_ALFA_RawDataCollectionReadOut->is_BOL()) && (!m_ALFA_RawDataCollectionReadOut->is_EOB())) 
    			{
    	  		wordPos += 1;
    	  		if (wordPos >= size) 
    	  			{
    		    		msg(MSG::ERROR)<<" Error: data corrupted" << endmsg;
    		    		return StatusCode::FAILURE;
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
      			msg(MSG::DEBUG)<<" Error: collection not found " << endmsg;
      			return StatusCode::FAILURE;
    			} // is_EOB()
    
    		
			wordPos += 1;
			if (wordPos >= size) {
			msg(MSG::ERROR)<<" Error: data corrupted"<< endmsg;
  			return StatusCode::FAILURE;
      			}
		
		
	    			m_ALFA_RawDataCollectionReadOut->decodeWord(vint[wordPos]);
				m_ALFA_RawDataReadOut_charge->decodeWord(vint[wordPos]);
	     
	    	while (!m_ALFA_RawDataCollectionReadOut->is_TWC()) {
			
			uint16_t slotIdNum  = m_ALFA_RawDataReadOut_charge->SlotId();
	        							
	       		msg(MSG::DEBUG)<<"  Decoding data from Slot Id number : " <<  slotIdNum << endmsg;
						
			ALFA_RawData_charge* rawData_charge; 

			// Loop on the TDC data words and create the corresponding RDO's
	        				
			while (!m_ALFA_RawDataReadOut_charge->is_EOT()) 
	        		{
					wordPos += 1;
				  	if (wordPos >= size) 
	          			{
	            			msg(MSG::DEBUG)<<" Error: data corrupted" << endmsg;
	            			return StatusCode::FAILURE;
	          			}
	          		
	          		m_ALFA_RawDataReadOut_charge->decodeWord(vint[wordPos]);
	         		
				uint16_t PMFIdNum;
	         		uint16_t ChannelIdNum;
				uint16_t ChargeNum;
				
	
	if (m_ALFA_RawDataReadOut_charge->is_TDCch() ) {
							
		// if(m_ALFA_RawDataReadOut_charge->ChannelNumId() ==1) msg(MSG::DEBUG)<<" DECODER : PMFId  " <<  m_ALFA_RawDataReadOut_charge->PMFId() <<  " ChannelNumId  " << m_ALFA_RawDataReadOut_charge->ChannelNumId() << " ChargeChanId "<< m_ALFA_RawDataReadOut_charge->ChargeChanId() <<endmsg;
							
				
																	
		if(m_ALFA_RawDataReadOut_charge->PMFId() ==0){
		//  && m_ALFA_RawDataReadOut_charge->bit27_24()==1 					
			// msg(MSG::DEBUG) << " Creation of the new collection"<< endmsg;     
    			
			
			CountPMF0++;
   
	tmpCharge = m_ALFA_RawDataReadOut_charge->ChargeChanId();
	
			for (unsigned int i=0;i<8;i++){
						
				charge_hit = (char) tmpCharge & 0x00000001;
				tmpCharge>>=1;
												
				if(charge_hit==1){
					MBIdNum=i+1;
							
					if(CountPMF0==1) collection = getCollection(MBIdNum,rdoCont );
					if(collection) collection->SetMBId_POT(MBIdNum);
							
					if(CountPMF0==1) msg(MSG::DEBUG)<<"  DECODER : MBIdNum  " <<  MBIdNum << endmsg;
					
						
				}
				
						
			} // loop over MBs
					
		} // PMFId() ==0
	
				
				
				
		if(m_ALFA_RawDataReadOut_charge->PMFId() >0 && m_ALFA_RawDataReadOut_charge->PMFId() <24){
		// && m_ALFA_RawDataReadOut_charge->bit27_24()==0 
					
					
			ChargeNum=m_ALFA_RawDataReadOut_charge->ChargeChanId();							
			ChannelIdNum = m_ALFA_RawDataReadOut_charge->ChannelNumId();
			PMFIdNum = m_ALFA_RawDataReadOut_charge->PMFId();
					
			rawData_charge = new ALFA_RawData_charge(PMFIdNum);
																
			rawData_charge->SetChannelNum_PMF(ChannelIdNum);
			rawData_charge->SetPMFId_PMF(PMFIdNum);	
			rawData_charge->SetMBId_PMF(MBIdNum);
			rawData_charge->SetChargeChan_PMF(ChargeNum);
			
			
			msg(MSG::DEBUG)<<"  PMFIdNum = " << PMFIdNum <<"  ChannelIdNum = " << ChannelIdNum <<" chargeNum = " <<  ChargeNum << endmsg;
					
			std::vector<uint16_t> ChargeHitId;
			ChargeHitId.push_back(ChargeNum);
			rawData_charge->SetChargeChanVect_PMF(ChargeHitId);
					
			rawData_charge->addData(vint[wordPos]); 
			rawData_charge->PushBack_PMF(ChargeNum); 
						
			if(collection) collection->push_back(rawData_charge);
					
					
		} // PMFIdNum >0 && <24
					
					
					
					
	
		//if(m_ALFA_RawDataReadOut_charge->PMFId()==24) // PMF 24 contains charge infos
		//{	
		//} //TSM PMF24
			
  
	} // TSM_charge 
	

	} // End of loop on AMTs (EOT)
				
					 
			if (m_ALFA_RawDataReadOut_charge->is_EOT() && m_ALFA_RawDataReadOut_charge->bit24_27()!=0) {
			msg(MSG::DEBUG) << " Event Counter EOT : " << m_ALFA_RawDataReadOut_charge->ecnt_EOT()<< endmsg; 
			}
	
			wordPos += 1;
 			if (wordPos >= size) {
	            	msg(MSG::DEBUG)<<" Error: data corrupted" << endmsg;
	            	return StatusCode::FAILURE;
	          	}
					
					
		  m_ALFA_RawDataCollectionReadOut->decodeWord(vint[wordPos]);
	    	  m_ALFA_RawDataReadOut_charge->decodeWord(vint[wordPos]);	
				
	} // End of loop on TDCs (TWC)
			
		wordPos += 1;
                if (wordPos >= size) 
	        {
	        msg(MSG::DEBUG)<<" Error: data corrupted" << endmsg;
	       	return StatusCode::FAILURE;
	       	}
					
		m_ALFA_RawDataCollectionReadOut->decodeWord(vint[wordPos]);
	
	
		
	// take full event
const eformat::FullEventFragment<const uint32_t*> * event =  m_robDataProvider->getEvent();
  
msg(MSG::DEBUG) << "========FULL Event=============" << std::endl;
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
msg(MSG::DEBUG) << "============================" << std::endl;

uint32_t Time_StampID  = event->bc_time_seconds(); 
uint32_t BC_ID  = event->bc_id(); 

if(collection) collection->SetTimeStamp_POT(Time_StampID);
if(collection) collection->SetBCId_POT(BC_ID);	
	
	
	
	
	
			
	} //EOB
		
	
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

ALFA_RawDataCollection_charge* ALFA_Decoder_charge::getCollection(unsigned int MBIdNum, ALFA_RawDataContainer_charge* cont) {

msg(MSG::DEBUG) << " ALFA_RoDDecoder_charge::getCollection" << endmsg;

  ALFA_RawDataCollection_charge* coll;
  
  ALFA_RawDataContainer_charge::const_iterator cont_it;
  bool collExists = false;
  ALFA_RawDataContainer_charge::const_iterator it = cont->begin();
  ALFA_RawDataContainer_charge::const_iterator itE= cont->end();
  
  
  for (; it!=itE; ++it) {
    if ((*it)->GetMBId_POT() == MBIdNum) {
       collExists = true;
      cont_it = it;
    }
  }
  if (collExists) {
    coll = const_cast<ALFA_RawDataCollection_charge*>(&**cont_it);
    msg(MSG::DEBUG) << " Collection exists " << endmsg;
    return 0;

  } else {  // if collection does not exist create it
    coll = new ALFA_RawDataCollection_charge(MBIdNum);
    msg(MSG::DEBUG) << " create collection; MBId  " << MBIdNum <<endmsg;
    cont->push_back(coll);  // add collection to container
  }
  
  
  return coll;
}



