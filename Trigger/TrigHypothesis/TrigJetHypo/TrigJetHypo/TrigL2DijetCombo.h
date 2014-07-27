/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2DijetCombo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   Simon Owen, Ignacio Aracena, Teresa Fonseca Martin
//
// Description: Level2 Jet Combo algorithm.  
// 
// ********************************************************************

//Not sure put the 'usual' name
#ifndef TRIGJETHYPO_TRIGL2DIJETCOMBO_H
#define TRIGJETHYPO_TRIGL2DIJETCOMBO_H


#include <string>
//#include "TrigT2CaloCommon/T2CaloBase.h"

/** @class TrigL2DijetCombo

    This is a ComboAlgo. It checks the number of Trigger
    elements is 2. Used by 
    TrigHypothesis/TrigJetHypo/TrigJetHypo/TrigL2JetDphiHypo.h
    which in an assymetric dijet trigger using a deltaPhi
    threshold.
    
    @author  Simon Owen <simon.owen@sheffield.ac.uk>
*/

/// general athena stuff
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// trigger includes
#include "TrigInterfaces/ComboAlgo.h"
//#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;

//bruxa namespace PESA {
class TrigL2DijetCombo: public HLT::ComboAlgo {


 public:
  TrigL2DijetCombo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2DijetCombo();

  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputs, HLT::TriggerElement* output); //!< hlt execute
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );

 private:
  //Monitoring variables
  bool m_doMonitoring;
  int m_nTe2jCounter;
  
  // No timing internal to alg. Should be done automatically.
  // Timing:

  //ITrigTimerSvc*            m_timersvc;
  //std::vector<TrigTimer*>   m_timers;

  // Not sure... 
  //virtual bool reset() { m_usemHTCachedResult = false; m_mhtcachedTE=0; ComboAlgo::reset(); return true;}

  // Not sure...
  //std::string m_featureLabel;
  //bool m_usemHTCachedResult;
  //HLT::TriggerElement* m_mhtcachedTE;
  
};
 
//bruxa}
#endif
