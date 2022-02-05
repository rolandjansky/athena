//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/*
   * @author S. Laplace
   * 14. 12. 2005
   * Modifications:
*/

#ifndef LARAVERAGES2NTUPLE_H
#define LARAVERAGES2NTUPLE_H
#include "LArCalibTools/LArCond2NtupleBase.h"

#include <fstream>
#include <math.h>
#include <string>
#include <map>

class LArAverages2Ntuple : public LArCond2NtupleBase
{
 public:
  LArAverages2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArAverages2Ntuple();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute() ;
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:
  std::string m_ntName;
  std::string m_contKey;

  unsigned int  m_Nsamples;
  bool m_keepPulsed;
  int m_ipass;

  NTuple::Item<long> m_Nsteps;
  NTuple::Item<long> m_DAC;
  NTuple::Item<long> m_Ntrigger;
  NTuple::Item<long> m_delay;
  NTuple::Item<long> m_ntNsamples;
  NTuple::Item<long> m_isPulsed;
  
  NTuple::Item<unsigned long> m_StepIndex;
  
  NTuple::Array<long> m_Sum;
  NTuple::Array<long> m_SumSq;
  NTuple::Array<float> m_Mean;
  NTuple::Array<float> m_RMS;
};

#endif
