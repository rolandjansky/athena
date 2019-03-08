/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*

  Kin Yip : May 22, 2002

  barrel_ec in software description has changed the definition.
  
  Now, we have  +-1 for Barrel
                +-2,3 for EndCap

  "pos_neg" in the ntuple is equivalent to "barrel_ec" in software description.

  Hong Ma : Aug 30, 2002 
    Add testing of RoI. 
 */


//#include <strstream>

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/LArEM_ID.h"

#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
//#include "LArROD/LArRawChannelBuilder.h"
#include "LArROD/ReadLArRaw.h"

// access all RawChannel  
//#include "EventContainers/SelectAllObject.h" 
// access RawChannel in TrigTower 
//#include "LArRawUtils/LArRoI_Map.h"
// #include "LArRawUtils/LArRawRoISelector.h"
//#include "LArRawUtils/LArTT_Selector.h"

#include "CaloIdentifier/CaloCell_ID.h"

using namespace std ;

//const int max_chan = 2800 ;

/////////////////////////////////////////////////////////////////////////////

ReadLArRaw::ReadLArRaw(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_ChannelContainerName("LArRawChannelContainer"), 
  m_onlineID(0),
  m_larem_id(0)
{
  // Declare the properties
  declareProperty("LArRawChannelContainerName",m_ChannelContainerName);
  declareProperty("DumpFile",m_dumpFile="");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadLArRaw::initialize()
{
  ATH_MSG_INFO( "in initialize()"  );
  
  ATH_CHECK(m_cablingKey.initialize());

  ATH_CHECK( detStore()->retrieve(m_onlineID, "LArOnlineID") );

// retrieve helpers for identifier
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_larem_id   = idHelper->em_idHelper();



  if (m_dumpFile.size()) {
    m_outFile.open(m_dumpFile.c_str());
  }

  // after all these ...
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadLArRaw::execute() {

  ATH_MSG_DEBUG( "in execute()"  );
  const LArRawChannelContainer* LArRaw ;
  ATH_CHECK( evtStore()->retrieve (LArRaw, m_ChannelContainerName) );


 int emax= 0; 
 HWIdentifier maxId;

 SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
 const LArOnOffIdMapping* cabling{*cablingHdl};
 if(!cabling) {
    ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
    return StatusCode::FAILURE;
 }

 LArRawChannelContainer::const_iterator it1 = LArRaw->begin(); 
 LArRawChannelContainer::const_iterator it2 = LArRaw->end(); 
 for(; it1!=it2; ++it1){
   HWIdentifier ch_id=it1->identify();
   if (m_outFile.is_open()) {
     if(cabling->isOnlineConnected(ch_id))
       m_outFile << "LArRawChannelDump: 0x" << std::hex << ch_id.get_compact() << std::dec << " E=" << it1->energy() 
		 << " t="  << it1->time() << " g=" << (int)it1->gain()  << " Q=" << it1->quality() << std::endl;
   }

   if(emax<it1->energy() ) { 
     emax=it1->energy() ; 
     maxId =ch_id;
     //  use LArOnlineID methods
     ATH_MSG_VERBOSE( "--barrel_ec : " << m_onlineID->barrel_ec(ch_id) 
                      << " Pos_neg : " << m_onlineID->pos_neg(ch_id)
                      << " FeedThrough no. : " << m_onlineID->feedthrough(ch_id)
                      << " slot no. : " << m_onlineID->slot(ch_id)
                      << " channel no. : " << m_onlineID->channel(ch_id)
                      );
     
     ATH_MSG_DEBUG( "Energy = " << it1->energy() << "; Time = " 
                    << it1->time()
                    << "; Chi-Square = " << it1->quality()  );
   }     
   ATH_MSG_DEBUG( " Channel with max energy , maxID =  "<< emax<<" "<<maxId.getString()  );
 }

 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadLArRaw::finalize() {

  if (m_outFile.is_open()) 
    m_outFile.close();
  ATH_MSG_INFO( "in finalize()"  );

  return StatusCode::SUCCESS;
}


