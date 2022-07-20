/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARFEBTIMEOFFSET2NTUPLE_H
#define LARFEBTIMEOFFSET2NTUPLE_H

#include "CxxUtils/checker_macros.h"
#include "LArCalibTools/LArCond2NtupleBase.h"

class LArFebTimeOffset2Ntuple: public LArCond2NtupleBase {

 public:

  LArFebTimeOffset2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);

  virtual ~LArFebTimeOffset2Ntuple();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override {return StatusCode::SUCCESS;}
  virtual StatusCode stop ATLAS_NOT_THREAD_SAFE() override; //<< Do the work here


 private:
  std::string m_inputContainer;
  int m_gain;
};


#endif
