/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigSpacePointCountsCnv_p2
 *
 * @brief transient-persistent converter for TrigSpacePointCounts
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigSpacePointCountsCnv_p2.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCNV_P2_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTSCNV_P2_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//TrigHisto converter
#include "TrigInDetEventTPCnv/TrigHisto2DCnv_p1.h"

#include "TrigInDetEvent/TrigSpacePointCounts.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p2.h"


class MsgStream;


class TrigSpacePointCountsCnv_p2 : public T_AthenaPoolTPCnvBase<TrigSpacePointCounts, TrigSpacePointCounts_p2> 
{

 public:

  TrigSpacePointCountsCnv_p2() {}

  virtual void persToTrans(const TrigSpacePointCounts_p2* persObj, TrigSpacePointCounts* transObj, MsgStream &log);
  virtual void transToPers(const TrigSpacePointCounts* transObj, TrigSpacePointCounts_p2* persObj, MsgStream &log);

 private:
 
  TrigHisto2DCnv_p1 m_trigHistoCnv;
 
};





#endif
