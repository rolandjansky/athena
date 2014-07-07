/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARFEBTIMEOFFSET2NTUPLE_H
#define LARFEBTIMEOFFSET2NTUPLE_H

//#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArCalibTools/LArCond2NtupleBase.h"

class LArFebTimeOffset2Ntuple: public LArCond2NtupleBase {

 public:

  LArFebTimeOffset2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);

  virtual ~LArFebTimeOffset2Ntuple();

  StatusCode initialize();
  StatusCode execute()  {return StatusCode::SUCCESS;}
  StatusCode stop(); //<< Do the work here
  StatusCode finalize() {return StatusCode::SUCCESS;}


 private:
  std::string m_inputContainer;
  int m_gain;
};


#endif
