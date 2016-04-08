/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITS2NTUPLE_H
#define LARDIGITS2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"

class LArDigits2Ntuple : public LArCond2NtupleBase
{
 public:
  LArDigits2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArDigits2Ntuple();

  //standard algorithm methods
  virtual StatusCode initialize();
  virtual StatusCode execute();
 private:

  std::string m_contKey;

  unsigned int  m_Nsamples;

  int m_ipass;
  long m_event;
  std::vector<unsigned int> m_FTlist;

  NTuple::Item<long> m_ntNsamples;
  NTuple::Item<short> m_gain;
  NTuple::Item<long> m_IEvent;
  NTuple::Array<short>  m_samples;
};

#endif
