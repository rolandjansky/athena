/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGMUONEFCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGMUONEFCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainer_p2.h"

typedef TrigMuonEFContainer_p2   TrigMuonEFContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigMuonEFContainer, TrigMuonEFContainer_PERS> TrigMuonEFContainerCnvBase;
 
class TrigMuonEFContainerCnv : public TrigMuonEFContainerCnvBase {
  
  friend class CnvFactory<TrigMuonEFContainerCnv>;
  
 protected:
  
  TrigMuonEFContainerCnv( ISvcLocator *svcloc ): TrigMuonEFContainerCnvBase(svcloc) {}
  
  virtual TrigMuonEFContainer_PERS *createPersistent( TrigMuonEFContainer *transObj);
  virtual TrigMuonEFContainer      *createTransient();
  
};//end of class definitions


#endif //TRIGMUONEFCONTAINER_CNV_H
