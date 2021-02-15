/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "LArElecCalib/ILArAutoCorr.h"

class LArAutoCorr2Ntuple : public LArCond2NtupleBase
{
 public:
  LArAutoCorr2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArAutoCorr2Ntuple();

  //standard algorithm methods
  virtual StatusCode stop();
  virtual StatusCode initialize();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 
 private:

  // Key of the AutocorrComplete object
  SG::ReadCondHandleKey<ILArAutoCorr> m_objKey{this,"ContainerKey",""};

  // Number of samples
  unsigned  m_nsamples;

};

#endif
