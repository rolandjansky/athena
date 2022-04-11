/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITS2NTUPLE_H
#define LARDIGITS2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "LArRawEvent/LArFebHeaderContainer.h"


class LArDigits2Ntuple : public LArCond2NtupleBase
{
 public:
  LArDigits2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArDigits2Ntuple();

  // Standard algorithm methods
  virtual StatusCode initialize();
  virtual StatusCode execute();

 protected:

  int m_ipass;
  long m_event;

  Gaudi::Property< unsigned int >  m_Nsamples{this, "NSamples", 32, "number of samples to store"};
  Gaudi::Property< std::vector<unsigned int> > m_FTlist{this, "FTlist", {}, "which FT to dump"};
  Gaudi::Property< bool > m_fillBCID{this, "FillBCID", false, "if to fill BCID"};

  NTuple::Item<long> m_ntNsamples;
  NTuple::Item<short> m_gain;
  NTuple::Item<short> m_ELVL1Id;
  NTuple::Item<unsigned long long> m_IEvent;
  NTuple::Array<short>  m_samples;
  NTuple::Item<short> m_bcid;

  SG::ReadHandleKey<LArDigitContainer> m_contKey{this, "ContainerKey", "FREE", "key for LArDigitContainer"};
  SG::ReadHandleKey<xAOD::EventInfo> m_evtInfoKey { this, "EventInfoKey", "EventInfo", "SG for EventInfo Key" };
  SG::ReadHandleKey<LArFebHeaderContainer> m_LArFebHeaderContainerKey { this, "LArFebHeaderKey", "LArFebHeader" };
};

#endif
