/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellRec/LArTimeVetoAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "LArRecEvent/LArEventBitInfo.h"
#include "GaudiKernel/Property.h"
#include "xAODEventInfo/EventInfo.h"

using xAOD::EventInfo;

//Constructor
LArTimeVetoAlg:: LArTimeVetoAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_folderName("/LAR/BadChannelsOfl/EventVeto")
  {
    declareProperty("folderName",m_folderName,"Folder name for DB access");
    m_nevt=0;
    m_nevtMasked=0;
  }
  
//__________________________________________________________________________
//Destructor
  LArTimeVetoAlg::~LArTimeVetoAlg()
  {
    ATH_MSG_DEBUG("LArTimeVetoAlg destructor called");
  }

//__________________________________________________________________________
StatusCode LArTimeVetoAlg::initialize() 
  {
    ATH_MSG_INFO("LArTimeVetoAlg initialize()"  );
    m_nevt=0;
    m_nevtMasked=0;

    ATH_CHECK( detStore()->regHandle(m_dd_atrList,m_folderName) );
    return StatusCode::SUCCESS; 

  }

//__________________________________________________________________________
StatusCode LArTimeVetoAlg::finalize()
  {
    ATH_MSG_DEBUG( "LArTimeVetoAlg finalize()"  );
    ATH_MSG_INFO( " Number of events processed " << m_nevt << "   Number of events in LAr bad time interval " << m_nevtMasked  );
    return StatusCode::SUCCESS; 
  }
  
//__________________________________________________________________________
StatusCode LArTimeVetoAlg::execute()
{
  ATH_MSG_DEBUG("LArTimeVetoAlg execute()");

  m_nevt++;

  const uint32_t vetoWord = (*m_dd_atrList)["EventVeto"].data<uint32_t>();

  //std::cout << " vetoWord " << vetoWord << std::endl;

  if (vetoWord != 0) {

    m_nevtMasked++;

    // retrieve EventInfo
    const EventInfo* eventInfo_c=0;
    ATH_CHECK( evtStore()->retrieve(eventInfo_c) );
    EventInfo* eventInfo=0;
    if (eventInfo_c) {
     eventInfo = const_cast<EventInfo*>(eventInfo_c);
    }
    if (eventInfo) {
      if (!eventInfo->setErrorState(EventInfo::LAr,EventInfo::Error)) {
	ATH_MSG_WARNING( " cannot set error state for LAr "  );
      }
      if (vetoWord & 0x1FFFF) {
	ATH_MSG_DEBUG("Event flagged as Noise Burst!");
	if (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::NOISEBURSTVETO)) {
	  ATH_MSG_WARNING( " cannot set flag bit for LAr "  );
	} 
	
        if (vetoWord & 0x10000) {
           ATH_MSG_DEBUG("Event flagged as Mini-Noise Burst!");
           if (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::MININOISEBURSTTIGHT)) {
             ATH_MSG_WARNING( " cannot set flag bit for LAr "  );
           } 
        }
             
	//std::cout << " after first event flag " << std::hex << eventInfo->eventFlags(EventInfo::LAr) << std::dec << std::endl;
	//Record location (partition) of noise burst
	for (int i=0;i<8;i++) {
	  if (vetoWord & (1<<i)) {
	    if (!eventInfo->setEventFlagBit(EventInfo::LAr,20+i)) {
	      ATH_MSG_WARNING( " cannot set flag bit for LAr documenting noise burst location "  );
	    }
	  }
	}
      }//end if one of the first 16 bits set
      if (vetoWord & 0xFFF80000) {
	ATH_MSG_DEBUG("Event flagged as DataCorruption!");
	if (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::DATACORRUPTEDVETO)) {
	  ATH_MSG_WARNING( " cannot set flag bit for LAr "  );
	} 
      }//end if one of the second 16 bits set
    }  //if eventInfo

  }   //if vetoWord

  //std::cout << " Lar bit in error word " << eventInfo->errorState(EventInfo::LAr) << std::endl;
  //std::cout << " Lar word              " << std::hex << eventInfo->eventFlags(EventInfo::LAr) << std::dec << std::endl;


  return StatusCode::SUCCESS;
}
