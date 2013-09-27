/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrtHitCountsCnv_p2
 *
 * @brief transient persistent converter for TrigTrtHitCounts
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCNV_P2_H
#define TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTSCNV_P2_H

#include "TrigInDetEventTPCnv/TrigTrtHitCounts_p2.h"
#include "TrigInDetEvent/TrigTrtHitCounts.h"
#include "TrigInDetEventTPCnv/TrigHisto1DCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigTrtHitCountsCnv_p2 : public T_AthenaPoolTPCnvBase<TrigTrtHitCounts, TrigTrtHitCounts_p2> {
 public:
  
  TrigTrtHitCountsCnv_p2() {}
  
  virtual void persToTrans( const TrigTrtHitCounts_p2 *persObj,
			    TrigTrtHitCounts    *transObj,
			    MsgStream            &log );
  virtual void transToPers( const TrigTrtHitCounts    *transObj,
			    TrigTrtHitCounts_p2 *persObj,
			    MsgStream            &log );

 private:
  TrigHisto1DCnv_p1 m_trigHistoCnv;
};

#endif
