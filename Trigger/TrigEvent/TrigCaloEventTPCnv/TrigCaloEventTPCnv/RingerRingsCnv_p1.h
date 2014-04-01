/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRingsCnv_p1
 *
 * @brief transient persistent converter for RingerRings
 *
 * @author Danilo Enoque Ferreira de Lima  <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id: RingerRingsCnv_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_RINGERRINGSCNV_P1_H
#define TRIGCALOEVENTTPCNV_RINGERRINGSCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEvent/RingerRings.h"
#include "TrigCaloEventTPCnv/RingerRings_p1.h"

class MsgStream;

class RingerRingsCnv_p1  : public T_AthenaPoolTPCnvBase<RingerRings, RingerRings_p1>  {

 public:
  RingerRingsCnv_p1() { }

  void persToTrans(const RingerRings_p1 *persObj,  RingerRings *transObj, MsgStream &log);
  void transToPers(const RingerRings *transObj, RingerRings_p1 *persObj, MsgStream &log);

};

#endif

