/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT
 * @Package: TrigDecisionEventTPCnv
 * @class  : TrigDecisionCnv_p3
 *
 * @brief  The converter for the TrigDecision_p3 persistent format.
 *         Called by TrigDecisionCnv
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UST-AGH Krakow
 *
 * File and Version Information:
 **********************************************************************************/

#ifndef TrigDecisionEventTPCnv_TrigDecisionCnv_p3_H
#define TrigDecisionEventTPCnv_TrigDecisionCnv_p3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigSteeringEventTPCnv/Lvl1ResultCnv_p2.h"

class MsgStream;

namespace TrigDec {
  class TrigDecision;
  class TrigDecision_p3;
}

namespace TrigDec {

  class TrigDecisionCnv_p3  : public T_AthenaPoolTPCnvConstBase<TrigDecision, TrigDecision_p3>  {

  public:
    using base_class::persToTrans;
    using base_class::transToPers;

    TrigDecisionCnv_p3() {}
    virtual void persToTrans(const TrigDecision_p3* persObj,  TrigDecision* transObj,   MsgStream& log) const override;
    virtual void transToPers(const TrigDecision*    transObj, TrigDecision_p3* persObj, MsgStream& log) const override;

  private:
    LVL1CTP::Lvl1ResultCnv_p2 m_lvl1ResultCnv;

  };

}

#endif
