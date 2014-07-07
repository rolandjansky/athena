/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARPHYSTCALIDIFFALG_H
#define LARPHYSTCALIDIFFALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArCalibTools/LArCond2NtupleBase.h"

class LArPhysCaliTDiffAlg: public LArCond2NtupleBase {

 public:

  LArPhysCaliTDiffAlg(const std::string & name, ISvcLocator * pSvcLocator);

  virtual ~LArPhysCaliTDiffAlg();

  StatusCode initialize();
  StatusCode execute();// {return StatusCode::SUCCESS;}
  //  StatusCode stop(); //<< Do the work here
  StatusCode finalize() {return StatusCode::SUCCESS;}


 private:
  std::string m_inputContainer;
  std::string m_outputContainer;
  std::string m_fileName;


};


#endif
