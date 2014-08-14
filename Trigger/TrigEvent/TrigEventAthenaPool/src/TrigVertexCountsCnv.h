/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigVertexCountsCnv
 *
 * @brief transient persistent converter for TrigVertexCounts
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGVERTEXCOUNTSCNV_H
#define TRIGEVENTATHENAPOOL_TRIGVERTEXCOUNTSCNV_H

#include "TrigInDetEvent/TrigVertexCounts.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class MsgStream;

class TrigVertexCounts_p1;
// typedef to the latest persistent version

typedef TrigVertexCounts_p1 TrigVertexCounts_PERS;
typedef T_AthenaPoolCustomCnv<TrigVertexCounts, TrigVertexCounts_PERS > TrigVertexCountsCnvBase; 

class TrigVertexCountsCnv : public TrigVertexCountsCnvBase {
  friend class CnvFactory<TrigVertexCountsCnv>;
  
 protected:
  TrigVertexCountsCnv(ISvcLocator* svcloc);
  ~TrigVertexCountsCnv();
  
  TrigVertexCounts_PERS* createPersistent(TrigVertexCounts* transObj);
  TrigVertexCounts* createTransient();
};


#endif
