/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMINBIAS2NTUPLE_H
#define LARMINBIAS2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"


class LArMinBias2Ntuple : public LArCond2NtupleBase
{
 public:
  LArMinBias2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArMinBias2Ntuple();

  //standard algorithm methods
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:

  std::string m_contKey;
  bool m_isPileup;
};

#endif
