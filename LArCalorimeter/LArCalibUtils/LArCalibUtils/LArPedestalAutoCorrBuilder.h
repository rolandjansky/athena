/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARPEDESTALAUTOCORRBUILDER_H
#define LARPEDESTALAUTOCORRBUILDER_H
/********************************************************************
 
 NAME:     LArPedestalAutoCorrBuilder.h
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  W. Lampl
 CREATED:  Aug 17th, 2009, merging LArPedestalBuilder & LArAutoCorrBuilder
  
 PURPOSE:  Create LArPedestalComplete and LArAutoCorrComplete objects 
           out of pre-accumulated LArAccumulatedDigits

********************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawEvent/LArAccumulatedDigitContainer.h"
#include "LArRawConditions/LArConditionsContainer.h"

class LArOnlineID;

//-----------------------------------------------------------------------
class LArPedestalAutoCorrBuilder : public AthAlgorithm
//-----------------------------------------------------------------------
{
 public:

  // Constructor
  LArPedestalAutoCorrBuilder(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  ~LArPedestalAutoCorrBuilder();

  // Algorithm initialization   
  StatusCode initialize(); 

  // Algorithm execution
  StatusCode execute();

  // endRun - real work
  virtual StatusCode stop();

  // Algorithm finalization
  virtual StatusCode finalize(){return StatusCode::SUCCESS;}

private:
  const LArOnlineID * m_onlineHelper;
  // Container key list
  std::vector<std::string> m_keylist;
  
  // Grouping type
  std::string m_groupingType;
  
  // Cache to accumulated substeps (internal use only)
  typedef LArConditionsContainer<LArAccumulatedDigit> ACCU;
  ACCU m_accu;

  unsigned m_event_counter;

  std::string m_pedContName;
  std::string m_acContName;

  bool m_doPedestal, m_doAutoCorr;

  uint16_t m_fatalFebErrorPattern;

  int m_normalize;

  int m_sample_min;
  int m_sample_max;
};

#endif
