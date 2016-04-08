/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT
 * @Package: TrigDecisionEventTPCnv
 * @Class  : TrigDecision_p1
 *
 * @brief  persistent representation of all three trigger level information - for now
 *         this is exactly what is in TrigDecisionData.
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 * @author Nicolas Berger   <Nicolas.Berger@cern.ch>   - CERN
 * @author Till Eifert      <Till.Eifert@cern.ch>      - U. of Geneva, Switzerland
 * @author Ricardo Goncalo  <Jose.Goncalo@cern.ch>     - Royal Holloway, U. of London
 *
 * File and Version Information:
 * $Id: TrigDecision_p1.h,v 1.2 2009-04-01 22:04:16 salvator Exp $
 **********************************************************************************/

#ifndef TrigDecisionEventTPCnv_TrigDecision_p1_H
#define TrigDecisionEventTPCnv_TrigDecision_p1_H

#include "TrigSteeringEventTPCnv/HLTResult_p1.h"
#include "TrigSteeringEventTPCnv/Lvl1Result_p1.h"

#include <vector>

// needed to fix a Gaudi problem in SLC3 (should be removed eventually):
#include <stdint.h>

namespace TrigDec {

  class TrigDecision_p1
  {
    friend class TrigDecisionCnv; //!< TrigDecisionCnv will set all member variables !?
    
  public:
    
    // default constructor
    TrigDecision_p1() { }

    TrigDecision_p1(const TrigDecision_p1& other) : 
      m_l1_result(other.getL1Result()), 
      m_l2_result(other.getL2Result()), 
      m_ef_result(other.getEFResult()) { }

    virtual ~TrigDecision_p1() { }

    uint32_t configMasterKey() const { return m_configMasterKey; }

    const LVL1CTP::Lvl1Result_p1&  getL1Result() const { return m_l1_result; }
    const HLT::HLTResult_p1&       getL2Result() const { return m_l2_result; }
    const HLT::HLTResult_p1&       getEFResult() const { return m_ef_result; }

    // private:

    uint32_t m_configMasterKey = 0;

    LVL1CTP::Lvl1Result_p1  m_l1_result; //!< Light result of trigger level 1
    HLT::HLTResult_p1       m_l2_result; //!< HLTResult of trigger level 2
    HLT::HLTResult_p1       m_ef_result; //!< HLTResult of trigger level EF
  };

} // end of namespace


#endif
