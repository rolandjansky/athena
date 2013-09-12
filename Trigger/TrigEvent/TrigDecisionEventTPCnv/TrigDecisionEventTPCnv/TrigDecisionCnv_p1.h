/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT
 * @Package: TrigDecisionEventTPCnv
 * @Class  : TrigDecisionCnv_p1
 *
 * @brief  The converter for the TrigDecision_p1 persistent format.
 *         Called by TrigDecisionCnv
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
 *
 * File and Version Information:
 * $Id: TrigDecisionCnv_p1.h,v 1.2 2009-04-01 22:04:16 salvator Exp $
 **********************************************************************************/

#ifndef TrigDecisionEventTPCnv_TrigDecisionCnv_p1_H
#define TrigDecisionEventTPCnv_TrigDecisionCnv_p1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigSteeringEventTPCnv/HLTResultCnv_p1.h"
#include "TrigSteeringEventTPCnv/Lvl1ResultCnv_p1.h"

class MsgStream;

namespace TrigDec {
  class TrigDecision;
  class TrigDecision_p1;
}

namespace TrigDec {

  class TrigDecisionCnv_p1  : public T_AthenaPoolTPCnvBase<TrigDecision, TrigDecision_p1>  {
    
  public:
    TrigDecisionCnv_p1() {}
    void persToTrans(const TrigDecision_p1* persObj,  TrigDecision* transObj,   MsgStream& log);
    void transToPers(const TrigDecision*    transObj, TrigDecision_p1* persObj, MsgStream& log);

  private:

    //converters
    HLT::HLTResultCnv_p1 m_hltResultCnv;
    LVL1CTP::Lvl1ResultCnv_p1 m_lvl1ResultCnv;

  };

}

#endif
