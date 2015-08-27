/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetEventTPCnv/JetKeyDescriptorCnv_p1.h"

void JetKeyDescriptorCnv_p1::persToTrans( const JetKeyDescriptor_p1* persObj, 
                            JetKeyDescriptor* transObj, 
                            MsgStream& msg )
  
{
  msg << MSG::DEBUG << "JetKeyDescriptorCnv persToTrans Begin " << endreq;
  
  msg << MSG::DEBUG << " Size : " << (persObj->m_catStore).size() << endreq;
  if(msg.level() <= MSG::VERBOSE  ){
    for ( std::vector<std::string>::const_iterator it = (persObj->m_catStore).begin();
	  it != (persObj->m_catStore).end(); it++ )
      {
	msg << MSG::VERBOSE << "  " << *it << endreq;
      }
  }
  if( (persObj->m_keyStoreLength).size() != (persObj->m_catStore).size() )
    {
      msg << MSG::ERROR << "Length of KeyStore and CategoryStore do not match !"
          << endreq;
      return;
    }

  JetKeyDescriptorInstance* instance = JetKeyDescriptorInstance::instance();

  transObj->m_keyStore.clear();
  transObj->m_catStore.clear();
  // if(bool( instance->m_Stores ) ) delete instance->m_Stores; // to avoid memory link
  instance->m_Stores = transObj; // global instance points to the store being read. 

  
  unsigned int index(0);
  for( unsigned int n = 0; n < (persObj->m_keyStoreLength).size(); n++ )
    {
      if(msg.level() <= MSG::VERBOSE  ) msg << MSG::VERBOSE << "   "
					    << (persObj->m_keyStoreLength)[n] << "  "
					    << (persObj->m_catStore)[n] << endreq;
      //	  (persObj->m_keyStore).resize((persObj->m_keyStoreLength)[n]);
      for( unsigned int m = 0; m < (persObj->m_keyStoreLength)[n]; m++ )
        {
          size_t index2 = instance->getIndex( (persObj->m_catStore)[n],
                                              (persObj->m_keyStore)[index]);
          if(msg.level() <= MSG::VERBOSE  ) msg << MSG::VERBOSE << "    "
						<< m << "  "
						<< (persObj->m_keyStore)[index]
						<< " stored at index "
						<< index2
						<< endreq;
          index++;
        }
    }
  msg << MSG::DEBUG << "JetKeyDescriptorCnv persToTrans End" << endreq;
}



void JetKeyDescriptorCnv_p1::transToPers( const JetKeyDescriptor* transObj,
                                          JetKeyDescriptor_p1* persObj, 
                                          MsgStream& msg )

{
  msg << MSG::DEBUG << "JetKeyDescriptorCnv transToPers Begin " << endreq;
  
  persObj->m_keyStoreLength.resize((transObj->m_catStore).size(),0);
  msg << MSG::DEBUG << " Size : " << (transObj->m_catStore).size()
      << endreq;
  
  for ( JetKeyDescriptor::catlist_t::const_iterator it = (transObj->m_catStore).begin();
        it != (transObj->m_catStore).end(); it++ )
    {
      if(msg.level() <= MSG::VERBOSE  ) msg << MSG::VERBOSE << "  " << *it << endreq;
      persObj->m_catStore.push_back(*it);
    }
  
  unsigned int n(0);
  for ( JetKeyDescriptor::storelist_t::const_iterator iti = (transObj->m_keyStore).begin();
        iti != (transObj->m_keyStore).end(); iti++ )
    {
      for ( JetKeyDescriptor::keystore_t::const_iterator ito = (*iti).begin();
            ito != (*iti).end(); ito++ )
        {
          if(msg.level() <= MSG::VERBOSE  ) msg << MSG::VERBOSE << "   " << *ito << " : " << n << endreq;
          persObj->m_keyStore.push_back(*ito);
          persObj->m_keyStoreLength[n]++;
        }
      n++;
    }
  msg << MSG::DEBUG << "JetKeyDescriptorCnv transToPers End" << endreq;
}



