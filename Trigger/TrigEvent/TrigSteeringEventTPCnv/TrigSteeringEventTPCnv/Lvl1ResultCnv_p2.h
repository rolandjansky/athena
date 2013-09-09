/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : Lvl1ResultCnv_p1
 *
 * @brief  The converter for the Lvl1Result_p1 persistent format.
 *
 * @author Andrew Hamilton <Andrew.Hamilton@cern.ch> - U. of Geneva, Switzerland
 * @author Francesca Bucci <f.bucci@cern.ch>         - U. of Geneva, Switzerland
 *
 * File and Version Information:
 * $Id: Lvl1ResultCnv_p2.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/

#ifndef TrigSteeringEventTPCnv_Lvl1ResultCnv_p2_H
#define TrigSteeringEventTPCnv_Lvl1ResultCnv_p2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

namespace LVL1CTP {
  class Lvl1Result;
  class Lvl1Result_p2;
}

namespace LVL1CTP {

  class Lvl1ResultCnv_p2  : public T_AthenaPoolTPCnvBase<Lvl1Result, Lvl1Result_p2>  {

  public:
    Lvl1ResultCnv_p2() {}
    void persToTrans(const Lvl1Result_p2* persObj,  Lvl1Result* transObj,   MsgStream& log);
    void transToPers(const Lvl1Result*    transObj, Lvl1Result_p2* persObj, MsgStream& log);
  };

}

#endif
