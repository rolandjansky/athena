/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrackCountsCnv_p1
 *
 * @brief transient-persistent converter for TrigTrackCounts
 *
 * @author Regina Kwee      <Regina.Kwee@cern.ch>      - CERN
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTSCNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTSCNV_P1_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//TrigTrackCounts persistent class
#include "TrigInDetEventTPCnv/TrigTrackCounts_p1.h"


class MsgStream;

class TrigTrackCountsCnv_p1 : public T_AthenaPoolTPCnvBase<TrigTrackCounts, TrigTrackCounts_p1> 
{

 public:

  TrigTrackCountsCnv_p1() {}

  virtual void persToTrans(const TrigTrackCounts_p1* persObj, TrigTrackCounts* transObj, MsgStream &log);
  virtual void transToPers(const TrigTrackCounts* transObj, TrigTrackCounts_p1* persObj, MsgStream &log);
  
};


#endif
