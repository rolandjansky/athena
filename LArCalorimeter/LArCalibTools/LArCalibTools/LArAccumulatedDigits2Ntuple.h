//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*
   * @author R. Lafaye
   * 12. 06. 2008
   * Modifications:
   * 25.7.2014 P. Strizenec, moving to LArCond2NtupleBase
*/

#ifndef LARACCUMULATEDDIGITS2NTUPLE_H
#define LARACCUMULATEDDIGITS2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"

class LArAccumulatedDigits2Ntuple : public LArCond2NtupleBase
{
 public:
  LArAccumulatedDigits2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArAccumulatedDigits2Ntuple();

  //standard algorithm methods
  virtual StatusCode initialize();
  virtual StatusCode execute();
 private:

  std::string m_contKey;

  unsigned int  m_Nsamples;
  int m_normalize;

  int m_ipass;
  long m_event;
  /*
  NTuple::Item<long> region, layer, eta, phi;
  NTuple::Item<long> pos_neg, barrel_ec, FT, slot, channel;
  NTuple::Item<long> calibLine;
  NTuple::Item<long> m_isConnected;
  */
  NTuple::Item<long> m_Ntrigger;
  NTuple::Item<long> m_IEvent;
  NTuple::Item<long> m_ntNsamples;
  NTuple::Array<long>  m_sum;
  NTuple::Array<long>  m_sumsq;
  NTuple::Item<float>  m_mean;
  NTuple::Item<float>  m_rms;
  NTuple::Array<float> m_covr;
  
};

#endif
