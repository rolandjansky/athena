/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level container converter.
///

#include "JetEventTPCnv/JetKeyDescriptorCollectionCnv_tlp1.h"
#include "JetEvent/JetKeyDescriptorCollection.h"

///
/// Register all the converters
///

JetKeyDescriptorCollectionCnv_tlp1::JetKeyDescriptorCollectionCnv_tlp1 (void)
{
  addMainTPConverter();
  addTPConverter (&m_jetKeyDescriptorCnv_p1);
}

///
/// Connect up the storage when we are doing the reading/writing.
///
void JetKeyDescriptorCollectionCnv_tlp1::setPStorage (JetKeyDescriptorCollection_tlp1 *storage)
{
  setMainCnvPStorage(&storage->m_jetKeyDescriptorCollectionContainers);
  m_jetKeyDescriptorCnv_p1.setPStorage(&storage->m_jetKeyDescriptor_p1);
}

/// CTor for the converter.
T_TPCnv<JetKeyDescriptorCollection, JetKeyDescriptorCollection_tlp1>::T_TPCnv()
{
}

void T_TPCnv<JetKeyDescriptorCollection, JetKeyDescriptorCollection_tlp1>::persToTrans (const JetKeyDescriptorCollection_tlp1 *pers,
							      JetKeyDescriptorCollection *trans,
							      MsgStream &msg)
{
  setPStorage (const_cast<JetKeyDescriptorCollection_tlp1*> (pers));
  m_mainConverter.pstoreToTrans(0, trans, msg);
}

void T_TPCnv<JetKeyDescriptorCollection, JetKeyDescriptorCollection_tlp1>::transToPers (const JetKeyDescriptorCollection *trans,
                                                              JetKeyDescriptorCollection_tlp1 *pers,
                                                              MsgStream &msg){


  msg << MSG::DEBUG << "Creating persistent state of JetKeyDescriptorCollection (_tlp1)..."
      << endreq;

  this->setTLPersObject( pers ) ;
  m_mainConverter.virt_toPersistent(trans, msg);
  this->clearTLPersObject();

  msg << MSG::DEBUG << "Creating persistent state of JetKeyDescriptorCollection (_tlp1)[OK]"
      << endreq;

}
