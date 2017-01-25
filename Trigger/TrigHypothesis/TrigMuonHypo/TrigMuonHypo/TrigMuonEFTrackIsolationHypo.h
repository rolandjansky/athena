/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFTRACKISOLATIONHYPO_H 
#define TRIG_TRIGMUONEFTRACKISOLATIONHYPO_H

// std lib includes
#include <string>
#include <vector>
// include base class
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

/**
 * \brief Hypothesis class for EF muon track isolation.
 * \author Mark Owen (markowen@cern.ch)
 * \details This class applies cuts on the variables calculated
 *          by the EF muon track isolation algorithm.\n
 *          The cuts can be applied to the sum pT in 0.2 cone, 0.3 cone
 *          or both.  
 */
class TrigMuonEFTrackIsolationHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
 public:
  /// Standard constructor
  TrigMuonEFTrackIsolationHypo(const std::string & name, ISvcLocator* pSvcLocator);
  /// Destructor
  ~TrigMuonEFTrackIsolationHypo();

  /// Initialize the algorithm
  HLT::ErrorCode hltInitialize();    
  /// Execute the algorithm
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
  /// Finalize the algorithm
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

};//class TrigMuonEFTrackIsolation

#endif //TRIG_TRIGMUONEFTRACKISOLATIONHYPO_H
