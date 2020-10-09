/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT
 * @Package: TrigDecisionEvent
 * @class  : TrigDecision
 *
 * @brief transient trigger decision class
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
 * @author Fabrizio Salvatore <p.salvatore@cern.ch>  - University of Sussex/Royal Holloway, 
 *                                                     U. of London
 *
 * File and Version Information:
 * $Id: TrigDecision.h,v 1.3 2009-02-11 13:43:06 tbold Exp $
 **********************************************************************************/

#ifndef TrigDecisionEvent_TrigDecision_H
#define TrigDecisionEvent_TrigDecision_H

#include "AthLinks/DataLink.h"
#include "TrigSteeringEvent/Enums.h"


#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteeringEvent/HLTResult.h"

#include "xAODCore/CLASS_DEF.h"
#include <string>
#include <stdint.h>



namespace TrigDec {
  class TrigDecisionCnv_p1;

  /**
   * @brief The TrigDecision is an object which merges trigger informations from various levels.
   *
   **/
  class TrigDecision
  {
  public:

    // constructors
    TrigDecision();

    TrigDecision( const LVL1CTP::Lvl1Result& l1Result,
                  const HLT::HLTResult& l2Result,
                  const HLT::HLTResult& efResult,
		      uint32_t masterKey = 0 );

    TrigDecision( const LVL1CTP::Lvl1Result& l1Result,
                  const HLT::HLTResult& HltResult,                 
		      uint32_t masterKey = 0 ); 

    TrigDecision( const LVL1CTP::Lvl1Result& l1Result,
                  const DataLink<HLT::HLTResult>& l2Result,
                  const DataLink<HLT::HLTResult>& efResult,
                  uint32_t masterKey = 0,
                  char bgCode = 0);

    virtual ~TrigDecision();


    uint32_t masterKey() const { return m_configMasterKey; }
    char     BGCode()    const { return m_bgCode; }

    const LVL1CTP::Lvl1Result&   getL1Result() const { return m_l1_result; }
    const HLT::HLTResult&        getL2Result() const;
    const DataLink<HLT::HLTResult>& getL2ResultLink() const { return m_l2_result; }
    const HLT::HLTResult&        getEFResult() const;
    const DataLink<HLT::HLTResult>& getEFResultLink() const { return m_ef_result; }
    const HLT::HLTResult&        getHLTResult() const;
    const DataLink<HLT::HLTResult>& getHLTResultLink() const { return m_hlt_result; }

    friend class TrigDecisionTool;
    friend class TrigDecisionMaker;

  private:
    friend class TrigDecisionCnv_p1;

    uint32_t m_configMasterKey;
    char     m_bgCode;

    // The packed data
    // ----------------

    LVL1CTP::Lvl1Result           m_l1_result;  //!< Lvl1Result
    DataLink<HLT::HLTResult>      m_l2_result;  //!< HLTResult of trigger level 2
    DataLink<HLT::HLTResult>      m_ef_result;  //!< HLTResult of trigger level EF
    DataLink<HLT::HLTResult>      m_hlt_result;  //!< HLTResult of merged L2EF 

    HLT::HLTResult* m_l2_result_ptr; // this is for backward compatibility // remove in the next non-cache release
    HLT::HLTResult* m_ef_result_ptr; // this is for backward compatibility
    HLT::HLTResult* m_hlt_result_ptr; // this is for backward compatibility 
  };

} // end of namespace

CLASS_DEF( TrigDec::TrigDecision, 4356087 , 1 )

#endif
