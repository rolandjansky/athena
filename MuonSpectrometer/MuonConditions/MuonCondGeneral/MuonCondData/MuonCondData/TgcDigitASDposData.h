/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDIGITASDPOSDATA_H
#define TGCDIGITASDPOSDATA_H

#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <map>
#include <vector>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  stripAsdPos : std::map<ChamberId, std::vector<strip_ASDpos>>             *
 *  wireAsdPos  : std::map<ChamberId, std::vector<wire_ASDpos>>              *
 * where                                                                     *
 *  ChamberId = (station number)<<8 + (station eta)<<5 + (station phi)       *
 *  station number: 41...48                                                  *
 *  station eta: 1...5                                                       *
 *  station phi: 0x1f for BW, 1...21 for EI, 1...24 for FI                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class TgcDigitASDposData
{
 public:
  TgcDigitASDposData();
  virtual ~TgcDigitASDposData() = default;

  enum {
    N_WIREASDPOS=8,
    N_STRIPASDPOS=2,
    N_CHANNELINPUT_TOASD=16
  };

  std::map<uint16_t, std::vector<float>> stripAsdPos;
  std::map<uint16_t, std::vector<float>> wireAsdPos;
};
CLASS_DEF(TgcDigitASDposData, 54799429, 1)

#include "AthenaKernel/CondCont.h"
CLASS_DEF(CondCont<TgcDigitASDposData>, 50515203, 1)

#endif // TGCDIGITASDPOSDATA_H
