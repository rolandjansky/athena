/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level container converter.
///

#include "JetEventTPCnv/JetCollectionCnv_tlp2.h"
#include "JetEvent/JetCollection.h"

///
/// Register all the converters
///

JetCollectionCnv_tlp2::JetCollectionCnv_tlp2 (void)
{
  addMainTPConverter();
  addTPConverter (&m_jetCnv_p4);
  addTPConverter (&m_jetAssCnv_p1);
}

///
/// Connect up the storage when we are doing the reading/writing.
///
void JetCollectionCnv_tlp2::setPStorage (JetCollection_tlp2 *storage)
{
  setMainCnvPStorage(&storage->m_jetCollectionContainers);
  m_jetCnv_p4.setPStorage(&storage->m_jet_p4);
  m_jetAssCnv_p1.setPStorage(&storage->m_jetAssociationBase_p1);
}

/// CTor for the converter.
T_TPCnv<JetCollection, JetCollection_tlp2>::T_TPCnv()
{
}

void T_TPCnv<JetCollection, JetCollection_tlp2>::persToTrans (const JetCollection_tlp2 *pers,
							      JetCollection *trans,
							      MsgStream &msg)
{
  setPStorage (const_cast<JetCollection_tlp2*> (pers));
  m_mainConverter.pstoreToTrans(0, trans, msg);
}

void T_TPCnv<JetCollection, JetCollection_tlp2>::transToPers (const JetCollection *trans,
							      JetCollection_tlp2 *pers,
							      MsgStream &msg){

  this->setTLPersObject( pers ) ;
  m_mainConverter.virt_toPersistent(trans, msg);
  this->clearTLPersObject();
}
