/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARSC2NTUPLE_H
#define LARSC2NTUPLE_H

#include "LArCalibTools/LArDigits2Ntuple.h"

class LArSC2Ntuple : public LArDigits2Ntuple
{
 public:
  LArSC2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  virtual ~LArSC2Ntuple() = default;

  // Standard algorithm methods
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
 private:

  long m_event;

  Gaudi::Property< std::vector<std::string> > m_contKeys{ this, "SCContainerKeys", {},"which containers to dump"};
  Gaudi::Property< bool > m_overwriteEventNumber{this, "OverwriteEventNumber", false, "overwrite the event number from EventInfo ?"};

  NTuple::Item<short> m_ELVL1Id;
  NTuple::Item<short> m_latomeChannel;

  NTuple::Item<float>  m_raw_energy;

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

  bool m_hasRawChan = false;
};

#endif
