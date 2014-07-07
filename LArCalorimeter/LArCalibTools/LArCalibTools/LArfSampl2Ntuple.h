/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARFSAMPL2NTUPLE_H
#define LARFSAMPL2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"


class LArfSampl2Ntuple : public LArCond2NtupleBase
{
 public:
  LArfSampl2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArfSampl2Ntuple();

  //standard algorithm methods
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:

  std::string m_contKey;
};

#endif
