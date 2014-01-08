/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTRTHTHHYPO_H 
#define TRIGTRTHTHHYPO_H 

/** @class TrigTRTHTHhypo
    @author Jiri Masik <Jiri.Masik@hep.manchester.ac.uk>
    @brief  TrigTRTHTHhypo passes depending on the number of TRT hits found by the TrigTRTHTHCounter fex
*/

#include "GaudiKernel/IToolSvc.h"
#include "TrigInterfaces/HypoAlgo.h"

#include <string>

class TrigTRTHTHhypo: public HLT::HypoAlgo {
  
 public:
  TrigTRTHTHhypo(const std::string & name, ISvcLocator* pSvcLocator);
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltBeginRun();  
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  
 private:
  
  
  /** If enabled the event is passed, ignoring the trigger decision. */
  bool m_acceptAll;

  /** actual cut values */
  unsigned int m_minTRTHTHits;
  float        m_minCaloE;
  float        m_minHTratio;
};
#endif
