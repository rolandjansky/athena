/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCountsCnv_p1
 *
 * @brief transient persistent converter for TrigVertexCounts
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEXCOUNTSCNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEXCOUNTSCNV_P1_H

#include "TrigInDetEventTPCnv/TrigVertexCounts_p1.h"
#include "TrigInDetEvent/TrigVertexCounts.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigVertexCountsCnv_p1 : public T_AthenaPoolTPCnvBase<TrigVertexCounts, TrigVertexCounts_p1> {
 public:
  
  TrigVertexCountsCnv_p1() {}
  
  virtual void persToTrans(const TrigVertexCounts_p1 *persObj, TrigVertexCounts *transObj, MsgStream &log);
  virtual void transToPers(const TrigVertexCounts *transObj, TrigVertexCounts_p1 *persObj, MsgStream &log);  

};




#endif
