/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigT2HistoPrmVtx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2HistoPrmVtx
//
// AUTHOR:   Viviana Cavaliere viviana.cavaliere@cern.ch
//
// Description: HistoPrmVtx Combo algorithm.  
// 
// ********************************************************************


#ifndef TRIGT2HISTOPRMVTXCOMBO_H
#define TRIGT2HISTOPRMVTXCOMBO_H 


#include <string>

/** @class TrigT2HistoPrmVtx

    This is a ComboAlgo. It checks that the number of Trigger
    elements is equal to the number of ROIs. 
*/

/// general athena stuff
/* #include "GaudiKernel/IToolSvc.h" */
/* #include "GaudiKernel/StatusCode.h" */
/* #include "GaudiKernel/ToolHandle.h" */
/* #include "GaudiKernel/ITHistSvc.h" */

// trigger includes
#include "TrigInterfaces/ComboAlgo.h"
//#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;


class TrigT2HistoPrmVtxCombo: public HLT::ComboAlgo {


 public:
  TrigT2HistoPrmVtxCombo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigT2HistoPrmVtxCombo();

  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputs, HLT::TriggerElement* output); //!< hlt execute
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );

 private:
  //Monitoring variables
  bool m_doMonitoring;
  int m_nTeCounter;
  
  
};
 

#endif


