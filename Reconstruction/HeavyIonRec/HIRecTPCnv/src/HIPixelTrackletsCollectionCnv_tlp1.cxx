/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIRecTPCnv/HIPixelTrackletsCollectionCnv_tlp1.h"

HIPixelTrackletsCollectionCnv_tlp1::HIPixelTrackletsCollectionCnv_tlp1() {
  addMainTPConverter();
  
  addTPConverter( &m_hiPixelTrackletsCnv);
}

void HIPixelTrackletsCollectionCnv_tlp1::setPStorage( HIPixelTrackletsCollection_tlp1 *storage) {
  setMainCnvPStorage( &storage->m_trackletsCollections );
  
  m_hiPixelTrackletsCnv.setPStorage(&storage->m_tracklets);
}

T_TPCnv<HIPixelTrackletsCollection, HIPixelTrackletsCollection_tlp1 >::T_TPCnv()
{ }

void T_TPCnv<HIPixelTrackletsCollection, HIPixelTrackletsCollection_tlp1 >::persToTrans(const HIPixelTrackletsCollection_tlp1* pers, HIPixelTrackletsCollection* trans, MsgStream& msg) {
  setPStorage( const_cast<HIPixelTrackletsCollection_tlp1*> (pers));
  m_mainConverter.pstoreToTrans(0,trans,msg); 
}
