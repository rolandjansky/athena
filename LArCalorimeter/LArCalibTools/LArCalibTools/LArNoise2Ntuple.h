/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARNOISE2NTUPLE_H
#define LARNOISE2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"


class LArNoise2Ntuple : public LArCond2NtupleBase
{
 public:
  LArNoise2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArNoise2Ntuple();

  //standard algorithm methods
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:

  std::string m_contKey;
};

#endif
