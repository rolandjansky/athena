/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDSPCONFIGNTUPLE_H
#define LARDSPCONFIGNTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"


class LArDSPConfig2Ntuple : public LArCond2NtupleBase
{
 public:
   LArDSPConfig2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
   ~LArDSPConfig2Ntuple();

   //standard algorithm methods
  StatusCode initialize();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();

 private:
   std::string m_ntname; 
   std::string m_folder;

   NTuple::Item<short> m_peakSample;
   NTuple::Item<short> m_useHgIntercept;
   NTuple::Item<short> m_useMgIntercept;
   NTuple::Item<short> m_useLgIntercept;
 
};

#endif
