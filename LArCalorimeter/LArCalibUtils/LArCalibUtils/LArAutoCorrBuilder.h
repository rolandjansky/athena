/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARAUTOCORRBUILDER_H
#define LARAUTOCORRBUILDER_H
/********************************************************************
 
 NAME:     LArAutoCorrBuilder.h
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  R. Lafaye
 CREATED:  Jun. 12, 2008 from LArAutoCorrMaker
    
 PURPOSE:  Get the autocorrelation for each cell from LArAccumulatedDigits
           at each gain and records them in TDS

           In fact only the last (m_nsamples-1) elements of the 
           first line (or column) of autocorrelation matrix are
           recovered and stored in TDS, for these reasons:
           - symetry of autocorrelation matrix
           - equivalence of autocorrelation elements: 
             B(n,n+i)\eq B(m,m+i) (eg B12 \eq B23).
HISTORY:
          Walter Lampl, 26 Aug 2009:
          Derive from LArPedestalBuilder 
                      
********************************************************************/

// Include files
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArCalibUtils/LArPedestalBuilder.h"

#include "LArRawEvent/LArAccumulatedDigitContainer.h"

#include "LArRawConditions/LArAutoCorrComplete.h"

//-----------------------------------------------------------------------
class LArAutoCorrBuilder : public LArPedestalBuilder
//-----------------------------------------------------------------------
{
 public:

  // Constructor
  LArAutoCorrBuilder(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  ~LArAutoCorrBuilder();

  // Algorithm initialization   
  //StatusCode initialize();  Inherited from LArPedestalBuilder

  // Algorithm execution
  //StatusCode execute(); Inherited from LArPedestal Builder

  // Algorithm finalization
  StatusCode stop(); 
  StatusCode finalize(){return StatusCode::SUCCESS;}
  
 private:
  //MGV Decide whether or not to normalize autocorr elements
  int m_normalize;
  std::string m_acContName;
};

#endif

