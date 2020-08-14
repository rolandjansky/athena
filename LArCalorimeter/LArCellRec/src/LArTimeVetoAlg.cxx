/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTimeVetoAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "LArRecEvent/LArEventBitInfo.h"
#include "Gaudi/Property.h"
#include "xAODEventInfo/EventInfo.h"

//Constructor
LArTimeVetoAlg:: LArTimeVetoAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthReentrantAlgorithm(name,pSvcLocator)
  {
    //m_nevt=0;
    //m_nevtMasked=0;
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
    //m_nevt=0;
    //m_nevtMasked=0;

    ATH_CHECK( m_eventInfoKey.initialize() );
    ATH_CHECK( m_eventVetoKey.initialize() );
    return StatusCode::SUCCESS; 

  }

//__________________________________________________________________________
StatusCode LArTimeVetoAlg::finalize()
  {
    ATH_MSG_DEBUG( "LArTimeVetoAlg finalize()"  );
    //ATH_MSG_INFO( " Number of events processed " << m_nevt << "   Number of events in LAr bad time interval " << m_nevtMasked  );
    return StatusCode::SUCCESS; 
  }
  
//__________________________________________________________________________
StatusCode LArTimeVetoAlg::execute( const EventContext& ctx ) const
{
  ATH_MSG_DEBUG("LArTimeVetoAlg execute()");

  //m_nevt++;
  SG::ReadCondHandle<AthenaAttributeList> ev_handle{m_eventVetoKey, ctx};
  const AthenaAttributeList* dd_atrList{*ev_handle}; 
  if (dd_atrList==nullptr) {
     ATH_MSG_FATAL("Null pointer to the read conditions object");
     return StatusCode::FAILURE;
  }

  const uint32_t vetoWord = (*dd_atrList)["EventVeto"].data<uint32_t>();

  if (vetoWord != 0) {

    //m_nevtMasked++;

    // retrieve EventInfo
    SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey, ctx);
    if (!eventInfo->updateErrorState(xAOD::EventInfo::LAr,xAOD::EventInfo::Error)) {
      ATH_MSG_WARNING( " cannot set error state for LAr "  );
    }
    if (vetoWord & 0x1FFFF) {
      ATH_MSG_DEBUG("Event flagged as Noise Burst!");
      if (!eventInfo->updateEventFlagBit(xAOD::EventInfo::LAr,LArEventBitInfo::NOISEBURSTVETO)) {
        ATH_MSG_WARNING( " cannot set flag bit for LAr "  );
      } 
      
      if (vetoWord & 0x10000) {
         ATH_MSG_DEBUG("Event flagged as Mini-Noise Burst!");
         if (!eventInfo->updateEventFlagBit(xAOD::EventInfo::LAr,LArEventBitInfo::MININOISEBURSTTIGHT)) {
           ATH_MSG_WARNING( " cannot set flag bit for LAr "  );
         } 
      }
           
      //Record location (partition) of noise burst
      for (int i=0;i<8;i++) {
        if (vetoWord & (1<<i)) {
          if (!eventInfo->updateEventFlagBit(xAOD::EventInfo::LAr,20+i)) {
            ATH_MSG_WARNING( " cannot set flag bit for LAr documenting noise burst location "  );
          }
        }
      }
    }//end if one of the first 16 bits set
    if (vetoWord & 0xFFF80000) {
      ATH_MSG_DEBUG("Event flagged as DataCorruption!");
      if (!eventInfo->updateEventFlagBit(xAOD::EventInfo::LAr,LArEventBitInfo::DATACORRUPTEDVETO)) {
        ATH_MSG_WARNING( " cannot set flag bit for LAr "  );
      } 
    }//end if one of the second 16 bits set

    ATH_MSG_VERBOSE( "LAr bit in error word " << eventInfo->errorState(xAOD::EventInfo::LAr) );
    ATH_MSG_VERBOSE( "LAr word              " << std::hex << eventInfo->eventFlags(xAOD::EventInfo::LAr) << std::dec );

  }   //if vetoWord


  return StatusCode::SUCCESS;
}
