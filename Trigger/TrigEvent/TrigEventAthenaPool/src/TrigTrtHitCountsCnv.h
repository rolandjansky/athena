/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigTrtHitCountsCnv
 *
 * @brief transient persistent converter for TrigTrtHitCounts
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGTRTHITCOUNTSCNV_H
#define TRIGEVENTATHENAPOOL_TRIGTRTHITCOUNTSCNV_H

#include "TrigInDetEvent/TrigTrtHitCounts.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TrigTrtHitCounts_p1;

class MsgStream;

// typedef to the latest persistent version
typedef TrigTrtHitCounts_p1  TrigTrtHitCounts_PERS;

typedef T_AthenaPoolCustomCnv<TrigTrtHitCounts, TrigTrtHitCounts_PERS > TrigTrtHitCountsCnvBase; 

class TrigTrtHitCountsCnv : public TrigTrtHitCountsCnvBase {
  friend class CnvFactory<TrigTrtHitCountsCnv>;
  
 protected:
  TrigTrtHitCountsCnv(ISvcLocator* svcloc);
  ~TrigTrtHitCountsCnv();
  
  TrigTrtHitCounts_PERS* createPersistent(TrigTrtHitCounts* transObj);
  TrigTrtHitCounts* createTransient();
};


#endif
