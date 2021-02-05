/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITS2NTUPLE_H
#define LARDIGITS2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRawEvent/LArFebHeaderContainer.h"

class LArDigits2Ntuple : public LArCond2NtupleBase
{
 public:
  LArDigits2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArDigits2Ntuple();

  // Standard algorithm methods
  virtual StatusCode initialize();
  virtual StatusCode execute();
 private:

  unsigned int  m_Nsamples;

  int m_ipass;
  long m_event;
  std::vector<unsigned int> m_FTlist;

  std::vector<std::string> m_contKeys;

  NTuple::Item<long> m_ntNsamples;
  NTuple::Item<short> m_gain;
  NTuple::Item<unsigned long long> m_IEvent;
  NTuple::Array<short>  m_samples;
  NTuple::Item<short> m_bcid;
  NTuple::Item<short> m_latomeChannel;

  // From LATOME header
  NTuple::Item<uint16_t> m_bcidLATOMEHEAD;
  NTuple::Item<uint32_t> m_latomeidLATOMEHEAD;
  NTuple::Item<uint32_t> m_l1idLATOMEHEAD;
  // DigitContainer
  NTuple::Array<unsigned short> m_bcidVec;
  NTuple::Item<uint32_t> m_latomeSourceId;
  NTuple::Array<short>  m_samples_ADC_BAS;
  NTuple::Array<unsigned short> m_bcidVec_ADC_BAS;

  NTuple::Array<int> m_energyVec_ET;
  NTuple::Array<unsigned short> m_bcidVec_ET;
  NTuple::Array<bool> m_saturVec_ET;

  NTuple::Array<int> m_energyVec_ET_ID;
  NTuple::Array<unsigned short> m_bcidVec_ET_ID;
  NTuple::Array<bool> m_saturVec_ET_ID;

  bool m_isSC;
  bool m_fillBCID;
  bool m_overwriteEventNumber;

  SG::ReadHandleKey<xAOD::EventInfo> m_evtInfoKey { this, "EventInfoKey", "EventInfo", "SG for EventInfo Key" };
  SG::ReadHandleKey<LArFebHeaderContainer> m_LArFebHeaderContainerKey { this, "LArFebHeaderKey", "LArFebHeader" };

};

#endif
