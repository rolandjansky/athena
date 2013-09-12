/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT
 * @Package: TrigDecisionEventTPCnv
 * @Class  : TrigDecisionCnv_p4
 *
 * @brief  The converter for the TrigDecision_p4 persistent format.
 *         Called by TrigDecisionCnv
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UST-AGH Krakow
 *
 * File and Version Information:
 * $Id: TrigDecisionCnv_p4.h,v 1.2 2009-04-01 22:04:16 salvator Exp $
 **********************************************************************************/

#ifndef TrigDecisionEventTPCnv_TrigDecisionCnv_p4_H
#define TrigDecisionEventTPCnv_TrigDecisionCnv_p4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigSteeringEventTPCnv/HLTResultCnv_p1.h"
#include "TrigSteeringEventTPCnv/Lvl1ResultCnv_p2.h"

class MsgStream;

namespace TrigDec {
  class TrigDecision;
  class TrigDecision_p4;
}

namespace TrigDec {

  class TrigDecisionCnv_p4  : public T_AthenaPoolTPCnvBase<TrigDecision, TrigDecision_p4>  {

  public:
    TrigDecisionCnv_p4() {}
    void persToTrans(const TrigDecision_p4* persObj,  TrigDecision* transObj,   MsgStream& log);
    void transToPers(const TrigDecision*    transObj, TrigDecision_p4* persObj, MsgStream& log);

  private:

    //converters
    HLT::HLTResultCnv_p1 m_hltResultCnv;
    LVL1CTP::Lvl1ResultCnv_p2 m_lvl1ResultCnv;

  };

}

#endif
