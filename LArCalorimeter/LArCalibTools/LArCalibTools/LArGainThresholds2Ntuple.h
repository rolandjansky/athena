//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARGAINTHRESHOLDS2NTUPLE
#define LARGAINTHRESHOLDS2NTUPLE
#include "LArCalibTools/LArCond2NtupleBase.h"
#include "LArElecCalib/ILArFEBConfigReader.h"

class LArGainThresholds2Ntuple : public LArCond2NtupleBase
{
 public:
   LArGainThresholds2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
   ~LArGainThresholds2Ntuple();

   //standard algorithm methods
  StatusCode initialize();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();
 private:
  ToolHandle<ILArFEBConfigReader> m_febConfigReader;

};

#endif
