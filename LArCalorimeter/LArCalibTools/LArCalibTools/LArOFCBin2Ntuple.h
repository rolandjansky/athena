/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LArOFCBIN2NTUPLE_H
#define LArOFCBIN2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRawConditions/LArOFCBinComplete.h"


class LArOFCBin2Ntuple : public LArCond2NtupleBase
{
 public:
  LArOFCBin2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  virtual ~LArOFCBin2Ntuple(){};

  //standard algorithm methods
  virtual StatusCode initialize() override;
  virtual StatusCode stop() override;
  virtual StatusCode finalize() override {return StatusCode::SUCCESS;}

 private:

  SG::ReadCondHandleKey<ILArOFCBin> m_binKey{this, "ContainerKey", "LArOFCBin"};
};

#endif
