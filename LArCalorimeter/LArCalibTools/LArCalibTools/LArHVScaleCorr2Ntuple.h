/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArHVScaleCorr2Ntuple_H
#define LArHVScaleCorr2Ntuple_H

#include "LArCalibTools/LArCond2NtupleBase.h"
#include "LArRecConditions/LArHVCorr.h"

class LArHVScaleCorr2Ntuple : public LArCond2NtupleBase
{
 public:
  LArHVScaleCorr2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArHVScaleCorr2Ntuple()=default;

  //standard algorithm methods
  virtual StatusCode initialize();
  virtual StatusCode stop();
 private:

  SG::ReadCondHandleKey<ILArHVScaleCorr> m_contKey{this,"ContainerKey","LArHVScaleCorr","Key for HVCorr"};
};

#endif
