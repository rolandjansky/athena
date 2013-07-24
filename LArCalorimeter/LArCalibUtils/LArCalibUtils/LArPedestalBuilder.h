/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARPEDESTALBUILDER_H
#define LARPEDESTALBUILDER_H
/********************************************************************
 
 NAME:     LArPedestalBuilder.h
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  R. Lafaye
 CREATED:  Jun. 12, 2008 from LArPedestalMaker
  
 PURPOSE:  Get the pedestal and rms for each cell from LArAccumulatedDigits
           at each gain and records them in TDS

********************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawEvent/LArAccumulatedDigitContainer.h"
#include "LArRawConditions/LArConditionsContainer.h"

class LArOnlineID;

//-----------------------------------------------------------------------
class LArPedestalBuilder : public AthAlgorithm
//-----------------------------------------------------------------------
{
 public:

  // Constructor
  LArPedestalBuilder(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  ~LArPedestalBuilder();

  // Algorithm initialization   
  StatusCode initialize(); 

  // Algorithm execution
  StatusCode execute();

  // Algorithm finalization
  virtual StatusCode stop();
  virtual StatusCode finalize(){return StatusCode::SUCCESS;}

 protected:

  const LArOnlineID * m_onlineHelper;
  // Container key list
  std::vector<std::string> m_keylist;
  
  // Grouping type
  std::string m_groupingType;
  
  // Cache to accumulated substeps
  typedef LArConditionsContainer<LArAccumulatedDigit> ACCU;
  ACCU m_accu;

 private:
  unsigned m_event_counter;
  std::string m_pedContName;
  uint16_t m_fatalFebErrorPattern;
 
};

#endif
