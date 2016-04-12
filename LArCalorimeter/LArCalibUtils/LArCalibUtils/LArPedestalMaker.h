/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARPEDESTALMAKER_H
#define LARPEDESTALMAKER_H
/********************************************************************
 
 NAME:     LArPedestalMaker.h
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  M. AHARROUCHE
 CREATED:  Jan. 12, 2004
 UPDATED:  Mar. 10, 2004 by Remi Lafaye
  	   Oct, 4, 2007 by M. Delmastro
           May, 5, 2008 W.Lampl (use LArConditionsContainer instead of std::map)
  
 PURPOSE:  Computes the pedestal and rms for each cell
           at each gain and records them in TDS

********************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"

#include "LArRawEvent/LArPedestal.h"
#include "LArRawEvent/LArDigitContainer.h"

#include "LArRawConditions/LArPedestalComplete.h"

#include <vector>


//-----------------------------------------------------------------------
class LArPedestalMaker : public AthAlgorithm
//-----------------------------------------------------------------------
{
 public:

  // Constructor
  LArPedestalMaker(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  ~LArPedestalMaker();

  // Algorithm initialization   
  StatusCode initialize(); 

  // Algorithm execution
  StatusCode execute();

  // Algorithm finalization
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  
 private:
  // Container key list
  std::vector<std::string> m_keylist;
  std::string m_keyoutput;
  
  // Grouping type
  std::string m_groupingType;
  
  // Number of events used to define window
  int m_nref; 

  // Number of sigma
  double m_rms_cut;
 
  // Sample number for pedestal 
  int m_which_sample;

  int m_sample_min;
  int m_sample_max;
 
 // Vector of LArPedestal
  //typedef std::map<HWIdentifier, LArPedestal >  LARPEDMAP;
  typedef LArConditionsContainer<LArPedestal>  LARPEDMAP;
  LARPEDMAP m_pedestal;
  
  // Vector of pedestal and rms
  std::vector<float> m_mean;
  std::vector<float> m_rms;
  
};

#endif
