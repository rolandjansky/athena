/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrackCountsCnv_p2
 *
 * @brief transient-persistent converter for TrigTrackCounts
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTrackCountsCnv_p2.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTSCNV_P2_H
#define TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTSCNV_P2_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//TrigHisto converter
#include "TrigInDetEventTPCnv/TrigHisto2DCnv_p1.h"

#include "TrigInDetEvent/TrigTrackCounts.h"
#include "TrigInDetEventTPCnv/TrigTrackCounts_p2.h"

class MsgStream;


class TrigTrackCountsCnv_p2 : public T_AthenaPoolTPCnvBase<TrigTrackCounts, TrigTrackCounts_p2> 
{

 public:

  TrigTrackCountsCnv_p2() {}

  virtual void persToTrans(const TrigTrackCounts_p2* persObj, TrigTrackCounts* transObj, MsgStream &log);
  virtual void transToPers(const TrigTrackCounts* transObj, TrigTrackCounts_p2* persObj, MsgStream &log);
 
 private:
    
  TrigHisto2DCnv_p1 m_trigHistoCnv;

};





#endif
