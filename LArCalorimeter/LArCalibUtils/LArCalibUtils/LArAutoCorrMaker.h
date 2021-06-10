/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//dear emacs, this is -*-c++-*-

#ifndef LARAUTOCORRMAKER_H
#define LARAUTOCORRMAKER_H
/********************************************************************
 
 NAME:     LArAutoCorrMaker.h
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  M. AHARROUCHE
 CREATED:  Dec. 16, 2003
  
 PURPOSE:  Selects the good events and computes the autocorrelation 
           matrix for each cell. It processes all the gains
           simultaneously.
           In fact only the last (m_nsamples-1) elements of the 
           first line (or column) of autocorrelation matrix are
           computed and stored in TDS, for these reasons:
           - symetrie of autocorrelation matrix
           - equivalence of autocorrelation elements: 
             B(n,n+i)\eq B(m,m+i) (eg B12 \eq B23).
                      
********************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRawEvent/LArAutoCorr.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include "LumiBlockData/BunchCrossingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

//-----------------------------------------------------------------------
class LArAutoCorrMaker : public AthAlgorithm
//-----------------------------------------------------------------------
{
 public:

  // Constructor
  LArAutoCorrMaker(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  ~LArAutoCorrMaker();

  // Algorithm initialization   
  StatusCode initialize(); 

  // Algorithm execution
  StatusCode execute();

  // Algorithm finalization
  StatusCode stop();
  StatusCode finalize(){ return StatusCode::SUCCESS;}
  
 private:

  SG::ReadCondHandleKey<BunchCrossingCondData> m_bcDataKey {this, "BunchCrossingCondDataKey", "BunchCrossingData" ,"SG Key of BunchCrossing CDO"};

  int m_bunchCrossingsFromFront;

  // Container key list
  std::vector<std::string> m_keylist, m_keylistproperty;
  std::string m_keyoutput;
  
  // Grouping type
  std::string m_groupingType;
  
  // Number of events used to define window
  int m_nref; 

  // Number of sigma
  int m_rms_cut;
  int m_nsamples;

  //MGV Decide whether or not to normalize autocorr elements
  int m_normalize;

  // flag save full N(N+1)/2 matrix   (relevant for pileup)
   int m_physics;
  
  typedef LArConditionsContainer<LArAutoCorr>  LARACMAP;
  LARACMAP m_autocorr;


  //counter for accepted events
  unsigned m_nEvents;
};

#endif

