/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloCellGroup2Ntuple_H
#define CaloCellGroup2Ntuple_H

#include "LArCalibTools/LArCond2NtupleBase.h"
#include "CaloIdentifier/CaloCellGroup.h"

class CaloCellGroupList;

class CaloCellGroup2Ntuple : public LArCond2NtupleBase
{
 public:
  CaloCellGroup2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~CaloCellGroup2Ntuple();

  //standard algorithm methods
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();

 private:
  std::vector<std::string> m_groupInit;
  CaloCellGroupList m_cellGroupList;
};

#endif
