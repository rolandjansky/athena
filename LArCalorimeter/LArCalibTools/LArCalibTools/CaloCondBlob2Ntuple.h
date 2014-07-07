/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloCondBlob2Ntuple_H
#define CaloCondBlob2Ntuple_H

#include "LArCalibTools/LArCond2NtupleBase.h"

class CaloCondBlob2Ntuple : public LArCond2NtupleBase
{
 public:
  CaloCondBlob2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~CaloCondBlob2Ntuple();

  //standard algorithm methods
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();

 private:
  std::string m_folder;
};

#endif
