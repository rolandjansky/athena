/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LArRINJ2NTUPLE_H
#define LArRINJ2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"


class LArRinj2Ntuple : public LArCond2NtupleBase
{
 public:
  LArRinj2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArRinj2Ntuple();

  //standard algorithm methods
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:

  std::string m_contKey;
};

#endif
