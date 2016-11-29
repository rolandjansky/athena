/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonRDO/CscRawDataCollectionIdHash.h" 
#include "MuonRDO/CscRawDataCollection.h" 

// default contructor 
CscRawDataCollectionIdHash::CscRawDataCollectionIdHash()
{

  IMessageSvc*  msgSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );
  if (sc.isFailure()) std::cout << "Fail to locate Message Service" << std::endl;

  MsgStream log(msgSvc, "CscRawDataCollectionIdHash" );

  log << MSG::DEBUG << " CscRawDataCollectionIdHash Constructor "<<endmsg; 

  // initialize RPC cabling service
  sc = svcLoc->service("CSCcablingSvc", m_cabling);
  if (sc != StatusCode::SUCCESS ) {
     log << MSG::ERROR << " Cannot get CSC cabling Service " << endmsg;
  }

  m_size=0;

  // loop over all RODs
  for (uint16_t id=0; id<m_cabling->maxId(); ++id)
    {
      // map
      m_lookup[id]=m_size;
      m_int2id.push_back(id);
      // ROD ID
      m_int2rodId.push_back( id%m_cabling->nROD() );
      ++m_size;

      // SubDetectorID
      if ( id < m_cabling->nROD() ) // C-side
	m_int2subDetectorId.push_back(0x6A);
      else                                      // A-side
	m_int2subDetectorId.push_back(0x69);

    }
}


/** reverse conversion */
CscRawDataCollectionIdHash::ID CscRawDataCollectionIdHash::identifier(int index) const
{
  if (index>=0 && index < m_size)
    return m_int2id[index]; 
  
  // if invalid index 
  return INVALID_ID; 
}


/** reverse conversion : SubDetectorID */
uint16_t CscRawDataCollectionIdHash::subDetectorId(int index) const
{
  if (index>=0 && index < m_size)
    return m_int2subDetectorId[index]; 
  
  // if invalid index 
  return INVALID_ID; 
}


/** reverse conversion : ROD ID */
uint16_t CscRawDataCollectionIdHash::rodId(unsigned int index) const
{
  //if (index>=0 && index < m_size)
  //  return m_int2rodId[index]; 
  
  if(index < m_int2rodId.size())
     return m_int2rodId.at(index);
  // if invalid index 
  return INVALID_ID; 
}


/**Convert ID to int. return -1 if invalid ID */
int CscRawDataCollectionIdHash::operator() (const ID& id) const 
{
  std::map<ID,int>::const_iterator it = m_lookup.find(id); 
  if(it!=m_lookup.end())
    return it->second; 

 // if invalid ID
 return INVALID_ID; 
}

