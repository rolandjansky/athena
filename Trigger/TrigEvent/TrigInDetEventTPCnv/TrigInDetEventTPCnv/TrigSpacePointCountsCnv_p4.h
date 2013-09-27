/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigSpacePointCountsCnv_p4
 *
 * @brief transient-persistent converter for TrigSpacePointCounts
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigSpacePointCountsCnv_p4.h 365974 2011-05-16 12:01:04Z salvator $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCNV_P4_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigInDetEventTPCnv/TrigHisto2DCnv_p1.h"

#include "TrigInDetEvent/TrigSpacePointCounts.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p4.h"

class MsgStream;


class TrigSpacePointCountsCnv_p4 : public T_AthenaPoolTPCnvBase<TrigSpacePointCounts, TrigSpacePointCounts_p4> {
 public:

  TrigSpacePointCountsCnv_p4() {}

  virtual void persToTrans(const TrigSpacePointCounts_p4* persObj, TrigSpacePointCounts* transObj, MsgStream &log);
  virtual void transToPers(const TrigSpacePointCounts* transObj, TrigSpacePointCounts_p4* persObj, MsgStream &log);

 private:
 
  TrigHisto2DCnv_p1 m_trigHistoCnv;
};

#endif
