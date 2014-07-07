/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArAutoCorr2Ntuple.h
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  M. AHARROUCHE
 CREATED:  Mar. 21, 2004
  
 PURPOSE:  Create a column-wise ntuple
             
********************************************************************/
#ifndef LARAUTOCORR2NTUPLE_H
#define LARAUTOCORR2NTUPLE_H
#include "LArCalibTools/LArCond2NtupleBase.h"


class LArAutoCorr2Ntuple : public LArCond2NtupleBase
{
 public:
  LArAutoCorr2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArAutoCorr2Ntuple();

  //standard algorithm methods
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 
 private:

  // Key of the AutocorrComplete object
  std::string m_objKey;

  // Number of samples
  unsigned  m_nsamples;

};

#endif
