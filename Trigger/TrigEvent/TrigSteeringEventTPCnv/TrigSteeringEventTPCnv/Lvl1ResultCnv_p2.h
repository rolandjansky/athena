/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @class  : Lvl1ResultCnv_p1
 *
 * @brief  The converter for the Lvl1Result_p1 persistent format.
 *
 * @author Andrew Hamilton <Andrew.Hamilton@cern.ch> - U. of Geneva, Switzerland
 * @author Francesca Bucci <f.bucci@cern.ch>         - U. of Geneva, Switzerland
 *
 * File and Version Information:
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

  class Lvl1ResultCnv_p2  : public T_AthenaPoolTPCnvConstBase<Lvl1Result, Lvl1Result_p2>  {

  public:
    using base_class::persToTrans;
    using base_class::transToPers;

    Lvl1ResultCnv_p2() {}
    virtual void persToTrans(const Lvl1Result_p2* persObj,  Lvl1Result* transObj,   MsgStream& log) const override;
    virtual void transToPers(const Lvl1Result*    transObj, Lvl1Result_p2* persObj, MsgStream& log) const override;
  };

}

#endif
