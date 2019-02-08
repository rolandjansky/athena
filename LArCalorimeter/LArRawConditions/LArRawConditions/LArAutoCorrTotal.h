/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARAUTOCORRTOTAL
#define LARRAWCONDITIONS_LARAUTOCORRTOTAL

#include "CaloIdentifier/CaloGain.h"
#include "LArIdentifier/LArOnlineID_Base.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"

#include <vector>
#include <array>

class LArOnlineID_Base;

class LArAutoCorrTotal {

public:
  LArAutoCorrTotal() = delete;
  LArAutoCorrTotal(const LArOnlineID_Base *onlineID, const LArOnOffIdMapping *cabling, const size_t nGains);
  ~LArAutoCorrTotal();

  // FIXME: Tool implementation indexes float vector by an unsigned int id32
  // derived from HWIdentifier
  // here, IdentifierHash is used, which is certainly indexed from 0. This makes
  // it possible to be used
  // as the index for vecAutoCorrTotal.
  // is id32 indexed from 0? if id32 is used, we may need to change the data
  // structure.
  // and modify the accessors.

  bool set(const IdentifierHash &hid, const int gain,
           std::vector<float> &autocorrtotal);

  const std::vector<double> computeAutoCorr(const std::vector<float> terms,
                                            float Nminbias) const;

  const std::vector<double> autoCorrTotal(const IdentifierHash &hid, int gain,
                                          float Nminbias) const;
  const std::vector<double> autoCorrTotal(const HWIdentifier &hwid, int gain,
                                          float Nminbias) const;
  const std::vector<double> autoCorrTotal(const Identifier &id, int gain,
                                          float Nminbias) const;

  const std::vector<double> computeRMS(const std::vector<float> terms,
                                       float Nminbias) const;

  const std::vector<double> samplRMS(const IdentifierHash &hid, int gain,
                                     float Nminbias) const;
  const std::vector<double> samplRMS(const HWIdentifier &hwid, int gain,
                                     float Nminbias) const;
  const std::vector<double> samplRMS(const Identifier &id, int gain,
                                     float Nminbias) const;

private:
  // dimensions n x m
  // n: number of IDs
  // m: number of possible non-trivial N(N-1)/2 terms of the autocorr matrix
  typedef std::vector<std::vector<float> > vecAutoCorrTotal;

  // dimensions k x (n x m)
  // k: 3 (value of enum LARNGAIN)
  std::array<vecAutoCorrTotal, CaloGain::LARNGAIN> m_AutoCorrTotal;

  const LArOnlineID_Base *m_onlineID;
  const LArOnOffIdMapping* m_cabling;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(LArAutoCorrTotal, 204702932, 1)
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(LArAutoCorrTotal, 194641932);

#endif
