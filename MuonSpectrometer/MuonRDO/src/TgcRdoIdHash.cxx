/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcRdoIdHash.h" 
#include "MuonRDO/TgcRdo.h" 
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"

// default contructor 
TgcRdoIdHash::TgcRdoIdHash()
{
  m_size=0;

  static bool isAtlas = true;
  static bool isFirstTime = true;
  if(isFirstTime) {
    ISvcLocator* svcLoc = Gaudi::svcLocator();
    if(!svcLoc) return;

    const ITGCcablingServerSvc* tgcCabGet = 0;
    StatusCode sc = svcLoc->service("TGCcablingServerSvc", tgcCabGet, true);
    if(!sc.isSuccess() || !tgcCabGet || !tgcCabGet->isConfigured()) {
      IMessageSvc* msgSvc =0 ;
      if(!(svcLoc->service("MessageSvc", msgSvc).isSuccess()) || !msgSvc) return;

      MsgStream log(msgSvc, "TgcRdoIdHash::TgcRdoIdHash");
      log << MSG::ERROR << "Could not get TGCcablingServerSvc! "
          << (!sc.isSuccess() ? "service(\"TGCcablingServerSvc\", tgcCabGet, true) is failed" : "")
          << (!tgcCabGet ? "TGCcablingServerSvc pointer is NULL" : "")
          << (tgcCabGet && !tgcCabGet->isConfigured() ? "TGCcablingServerSvc is NOT configured yet" : "")
          << endmsg;
      return;
    }
    
    isAtlas = tgcCabGet->isAtlas();
    isFirstTime = false;
  }
  
  if(isAtlas){
  
      // loop over all RODs
      for (uint16_t id=0; id<24; ++id)
      {
          // map
          m_lookup[id]=m_size;
          m_int2id.push_back(id);
          ++m_size;

          // SubDetectorID
          if (id < 12) // A-side
	      m_int2subDetectorId.push_back(0x67);
          else
	      m_int2subDetectorId.push_back(0x68);

          // ROD ID
          m_int2rodId.push_back( (id % 12) + 1);
      }
  
  } else {
      
     // throw GaudiException("Code not prepared for working with the old TGccabling",
     //                      "TgcRdoIdHash::TgcRdoIdHash()", StatusCode::FAILURE);
      // loop over all RODs
      for (uint16_t id=0; id<16; ++id)
      {
          // map
          m_lookup[id]=m_size;
          m_int2id.push_back(id);
          ++m_size;

          // SubDetectorID
          if (id < 8) // A-side
	      m_int2subDetectorId.push_back(0x67);
          else
	      m_int2subDetectorId.push_back(0x68);

          // ROD ID
          m_int2rodId.push_back(id % 8);
      }
  }
    
}


/** reverse conversion */
TgcRdoIdHash::ID TgcRdoIdHash::identifier(int index) const
{
  if (index>=0 && index < m_size)
    return m_int2id[index]; 
  
  // if invalid index 
  return INVALID_ID; 
}


/** reverse conversion : SubDetectorID */
uint16_t TgcRdoIdHash::subDetectorId(int index) const
{
  if (index>=0 && index < m_size)
    return m_int2subDetectorId[index]; 
  
  // if invalid index 
  return INVALID_ID; 
}


/** reverse conversion : ROD ID */
uint16_t TgcRdoIdHash::rodId(int index) const
{
  if (index>=0 && index < m_size)
    return m_int2rodId[index]; 
  
  // if invalid index 
  return INVALID_ID; 
}


/**Convert ID to int. return -1 if invalid ID */
int TgcRdoIdHash::operator() (const ID& id) const 
{
  std::map<ID,int>::const_iterator it = m_lookup.find(id); 
  if(it!=m_lookup.end())
    return (*it).second; 

 // if invalid ID
 return INVALID_ID; 
}
