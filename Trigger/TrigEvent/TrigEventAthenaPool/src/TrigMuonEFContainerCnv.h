/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGMUONEFCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGMUONEFCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainerCnv_p2.h"

typedef TrigMuonEFContainer_p2   TrigMuonEFContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigMuonEFContainer, TrigMuonEFContainer_PERS> TrigMuonEFContainerCnvBase;
 
class TrigMuonEFContainerCnv : public TrigMuonEFContainerCnvBase {
  
  friend class CnvFactory<TrigMuonEFContainerCnv>;
  
 protected:
  
public:
  TrigMuonEFContainerCnv( ISvcLocator *svcloc ): TrigMuonEFContainerCnvBase(svcloc) {}
protected:
  
  virtual TrigMuonEFContainer_PERS *createPersistent( TrigMuonEFContainer *transObj);
  virtual TrigMuonEFContainer      *createTransient();

private:
  TrigMuonEFContainerCnv_tlp1 m_converter_tlp1;
  TrigMuonEFContainerCnv_p2   m_converter;
  
};//end of class definitions


#endif //TRIGMUONEFCONTAINER_CNV_H
