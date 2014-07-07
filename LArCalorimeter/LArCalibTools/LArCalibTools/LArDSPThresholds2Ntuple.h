/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDSPTHRESHOLDS2NTUPLE_H
#define LARDSPTHRESHOLDS2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"


class LArDSPThresholds2Ntuple : public LArCond2NtupleBase
{
 public:
   LArDSPThresholds2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
   ~LArDSPThresholds2Ntuple();

   //standard algorithm methods
  StatusCode initialize();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();

 private:
   std::string m_ntname; 
   std::string m_folder;
   bool m_doFlat;

  NTuple::Item<float> m_tQThr;
  NTuple::Item<float> m_samplesThr;
  NTuple::Item<float> m_trigThr;
 
};

#endif
