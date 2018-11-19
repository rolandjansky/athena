/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFTRACKISOLATIONMULTIHYPO_H 
#define TRIG_TRIGMUONEFTRACKISOLATIONMULTIHYPO_H

#include <string>
#include <vector>
#include <deque>
// include base class
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

/**
 * \brief Hypothesis class for EF muon track isolation.
 * \author Marcus Matthias Morgenstern (marcus.matthias.morgenstern@cern.ch)
 * \details This class applies cuts on the variables calculated
 *          by the EF muon track isolation algorithm and checks that multiple muons fulfil both isolation and pT criteria.\n
 *          The cuts can be applied to the sum pT in 0.2 cone, 0.3 cone
 *          or both.  
 */
class TrigMuonEFTrackIsolationMultiHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
public:
  TrigMuonEFTrackIsolationMultiHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigMuonEFTrackIsolationMultiHypo();

  HLT::ErrorCode hltInitialize();    
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
  HLT::ErrorCode hltFinalize();

private:

  // Properties:
  
  /// should we just accept all events?
  BooleanProperty m_acceptAll;
  
  /// cut on the pt cone 0.2 variable
  FloatProperty m_ptcone02_cut;

  /// cut on the pt cone 0.3 variable
  FloatProperty m_ptcone03_cut;
  
  /// cut on absolute or sumpt/pt?
  BooleanProperty m_abscut;

  /// use offline or online isolation variables?
  BooleanProperty m_useVarIso;

  /// only consider combined muons for isolation?
  BooleanProperty m_requireCombined;

  /// monitor the variables we cut on
  std::vector<float> m_fex_ptcone02;
  std::vector<float> m_fex_ptcone03;

  std::vector<float> m_ptThresholds;

  bool checkThresholds(std::deque<float>&);
};

#endif //TRIG_TRIGMUONEFTRACKISOLATIONMULTIHYPO_H
