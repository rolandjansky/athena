/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGTAUTRACKSINFOCOLLECTION_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGTAUTRACKSINFOCOLLECTION_CNV_H
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigInDetEvent/TrigTauTracksInfoCollection.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollection_p2.h"

typedef TrigTauTracksInfoCollection_p2   TrigTauTracksInfoCollection_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigTauTracksInfoCollection, TrigTauTracksInfoCollection_PERS> TrigTauTracksInfoCollectionCnvBase;
 
class TrigTauTracksInfoCollectionCnv : public TrigTauTracksInfoCollectionCnvBase {
  
  friend class CnvFactory<TrigTauTracksInfoCollectionCnv>;
  
 protected:

  TrigTauTracksInfoCollectionCnv( ISvcLocator *svcloc ) : TrigTauTracksInfoCollectionCnvBase(svcloc){}

  virtual TrigTauTracksInfoCollection_PERS *createPersistent( TrigTauTracksInfoCollection *transObj);
  virtual TrigTauTracksInfoCollection      *createTransient();
  
};//end of class definitions


#endif //TRIGTAUTRACKSINFOCOLLECTION_CNV_H
