/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRingsCnv_p2
 *
 * @brief transient persistent converter for RingerRings
 *
 * @author Danilo Enoque Ferreira de Lima  <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id: RingerRingsCnv_p2.h,v 1.2 2011-05-31 22:01:37 damazio Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_RINGERRINGSCNV_P2_H
#define TRIGCALOEVENTTPCNV_RINGERRINGSCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEvent/RingerRings.h"
#include "TrigCaloEventTPCnv/RingerRings_p2.h"

class MsgStream;

class RingerRingsCnv_p2  : public T_AthenaPoolTPCnvBase<RingerRings, RingerRings_p2>  {

 public:
  RingerRingsCnv_p2() { }

  void persToTrans(const RingerRings_p2 *persObj,  RingerRings *transObj, MsgStream &log);
  void transToPers(const RingerRings *transObj, RingerRings_p2 *persObj, MsgStream &log);

};

#endif

