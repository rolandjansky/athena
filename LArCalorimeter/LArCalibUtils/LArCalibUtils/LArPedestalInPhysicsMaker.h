/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARPEDESTALINPHYSICSMAKER_H
#define LARPEDESTALINPHYSICSMAKER_H
/********************************************************************
 
 NAME:     LArPedestalInPhysicsMaker.h
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  Remi Lafaye
 CREATED:  Feb. 16, 2005
  
 PURPOSE:  Computes the pedestal and rms for each cell
           at each gain and records them in TDS

********************************************************************/

// Include files
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "LArRawEvent/LArPedestal.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawConditions/LArPedestalComplete.h"

#include <stdio.h>
#include <vector>


typedef std::vector<LArPedestal> LArPedestalVect;

//-----------------------------------------------------------------------
class LArPedestalInPhysicsMaker : public AthAlgorithm
//-----------------------------------------------------------------------
{
 public:

  // Constructor
  LArPedestalInPhysicsMaker(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  ~LArPedestalInPhysicsMaker();

  // Algorithm initialization   
  StatusCode initialize(); 

  // Algorithm execution
  StatusCode execute();

  // Algorithm finalization
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  
  StatusCode fillDB();

 private:
  // Container key list
  std::vector<std::string> m_keylist;//, m_keylistproperty;
  std::string m_keyoutput;
  
  // Grouping type
  std::string m_groupingType;

  // Number of events used to define pedestal
  int m_nref; 
  
  //IOV limits;
  int m_run, m_event, m_pevent;

  // Sample number for pedestal 
  int m_which_sample;
  int m_accept, m_reject, m_S1cut;
  int m_count;
  int m_record;
  std::vector<std::string> m_scint_names;
 
 // Vector of LArPedestal
  typedef std::map<HWIdentifier, LArPedestal >  LARPEDMAP;
  std::vector<LARPEDMAP> m_pedestal;
  //LArPedestalDB* vPedestal;
  
  // Vector of pedestal and rms
  std::vector<float> m_mean;
  std::vector<float> m_rms;
  
};

#endif

