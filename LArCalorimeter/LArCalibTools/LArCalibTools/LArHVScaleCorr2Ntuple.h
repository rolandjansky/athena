/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArHVScaleCorr2Ntuple_H
#define LArHVScaleCorr2Ntuple_H

#include "LArCalibTools/LArCond2NtupleBase.h"


class LArHVScaleCorr2Ntuple : public LArCond2NtupleBase
{
 public:
  LArHVScaleCorr2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArHVScaleCorr2Ntuple();

  //standard algorithm methods
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:

  std::string m_contKey;
};

#endif
