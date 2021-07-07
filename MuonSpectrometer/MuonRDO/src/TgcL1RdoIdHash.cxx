/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcL1RdoIdHash.h" 
#include "MuonRDO/TgcL1Rdo.h" 
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"

// default contructor 
TgcL1RdoIdHash::TgcL1RdoIdHash()
{
  m_size=0;

  static const bool isAtlas = getIsAtlas();
  
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

bool TgcL1RdoIdHash::getIsAtlas() const
{
  const char* name = "TgcL1RdoIdHash::TgcL1RdoIdHash";
  ServiceHandle<ITGCcablingServerSvc> tgcCabGet ("TGCcablingServerSvc", name);
  if (tgcCabGet.retrieve().isFailure()) {
    REPORT_ERROR_WITH_CONTEXT (StatusCode::FAILURE, name)
      << "Could not get TGCcablingServerSvc! ";
    return false;
  }
    
  return tgcCabGet->isAtlas();
}

/** reverse conversion */
TgcL1RdoIdHash::ID TgcL1RdoIdHash::identifier(int index) const
{
  if (index>=0 && index < m_size)
    return m_int2id[index]; 
  
  // if invalid index 
  return INVALID_ID; 
}


/** reverse conversion : SubDetectorID */
uint16_t TgcL1RdoIdHash::subDetectorId(int index) const
{
  if (index>=0 && index < m_size)
    return m_int2subDetectorId[index]; 
  
  // if invalid index 
  return INVALID_ID; 
}


/** reverse conversion : ROD ID */
uint16_t TgcL1RdoIdHash::rodId(int index) const
{
  if (index>=0 && index < m_size)
    return m_int2rodId[index]; 
  
  // if invalid index 
  return INVALID_ID; 
}


/**Convert ID to int. return -1 if invalid ID */
int TgcL1RdoIdHash::operator() (const ID& id) const 
{
  std::map<ID,int>::const_iterator it = m_lookup.find(id); 
  if(it!=m_lookup.end())
    return (*it).second; 

 // if invalid ID
 return INVALID_ID; 
}
