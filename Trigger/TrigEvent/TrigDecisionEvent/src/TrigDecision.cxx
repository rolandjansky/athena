/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT
 * @Package: TrigSteeringEvent
 * @Class  : TrigDecision
 *
 * @brief  transient representation of all three trigger level information
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
 * @author Fabrizio Salvatore <p.salvatore@cern.ch>  - University of Sussex/
 *                                                   - Royal Holloway, U. of London
 *
 * File and Version Information:
 * $Id: TrigDecision.cxx,v 1.2 2009-02-05 18:04:30 tbold Exp $
 **********************************************************************************/


#include "TrigDecisionEvent/TrigDecision.h"

//#include "TrigSteering/Sequence.h"

using namespace TrigDec;


TrigDecision::TrigDecision()
  : m_configMasterKey(0),
    m_bgCode(0),
    m_l2_result_ptr(0),
    m_ef_result_ptr(0),
    m_hlt_result_ptr(0)    
{
  //  m_detailLevel = NONE;
  //  m_status = true;
}

TrigDecision::TrigDecision( const LVL1CTP::Lvl1Result& l1Result,
			    const HLT::HLTResult& l2Result,
			    const HLT::HLTResult& efResult,
			    uint32_t masterKey ) :
  m_configMasterKey(masterKey),
  m_bgCode(0),
  m_l1_result(l1Result),
  m_l2_result(l2Result),
  m_ef_result(efResult),
  m_l2_result_ptr(0),
  m_ef_result_ptr(0),
  m_hlt_result_ptr(0)
{ }


TrigDecision::TrigDecision( const LVL1CTP::Lvl1Result& l1Result,
			    const HLT::HLTResult& hltResult,
			    uint32_t masterKey ) :
  m_configMasterKey(masterKey),
  m_bgCode(0),
  m_l1_result(l1Result),
  //  m_l2_result(0),
  //  m_ef_result(0),
  m_hlt_result(hltResult),
  m_l2_result_ptr(0),
  m_ef_result_ptr(0),
  m_hlt_result_ptr(0) { }


TrigDecision::TrigDecision( const LVL1CTP::Lvl1Result& l1Result,
                            const DataLink<HLT::HLTResult>& l2Result,
                            const DataLink<HLT::HLTResult>& efResult,
                            uint32_t masterKey,
                            char bgCode)
  : m_configMasterKey (masterKey),
    m_bgCode(bgCode),
    m_l1_result (l1Result),
    m_l2_result (l2Result),
    m_ef_result (efResult),
    m_l2_result_ptr(nullptr),
    m_ef_result_ptr(nullptr),
    m_hlt_result_ptr(nullptr)
{
}


TrigDecision::~TrigDecision()
{
  //  resetCache();
  // if pointers to HLT results pointers are set we own them (we need to handle od version)
  if (m_l2_result_ptr)
    delete m_l2_result_ptr;
  
  if (m_ef_result_ptr)
    delete m_ef_result_ptr;

  
  if (m_hlt_result_ptr)
    delete m_hlt_result_ptr;

}

namespace {
  static const HLT::HLTResult invalid_result;
}

const HLT::HLTResult&      TrigDecision::getL2Result() const {
  if (m_l2_result_ptr)        // if we have ptr we should use it (old data 13.0.X and 14.0.0)
    return *m_l2_result_ptr;  
  if (m_l2_result.isValid()) {  // add protection against invalid DataLinks
    return *m_l2_result;        // this is data link, derefencing it means whole lot different thing
  } else {
    return invalid_result; // if DataLink invalid, return dummy HLTResult
  }
}


const HLT::HLTResult&      TrigDecision::getEFResult() const {
  if (m_ef_result_ptr)        // if we have ptr we should use it (old data 13.0.X and 14.0.0)
    return *m_ef_result_ptr;  
  if (m_ef_result.isValid()) {  // add protection against invalid DataLinks
    return *m_ef_result;        // this is data link, derefencing it means whole lot different thing
  } else {
    return invalid_result; // if DataLink invalid, return dummy HLTResult
  }
}


const HLT::HLTResult&      TrigDecision::getHLTResult() const {
  if (m_hlt_result_ptr)        // if we have ptr we should use it (old data 13.0.X and 14.0.0)
    return *m_hlt_result_ptr;  
  if (m_hlt_result.isValid()) {  // add protection against invalid DataLinks
    return *m_hlt_result;        // this is data link, derefencing it means whole lot different thing
  } else {
    return invalid_result; // if DataLink invalid, return dummy HLTResult
  }
}

