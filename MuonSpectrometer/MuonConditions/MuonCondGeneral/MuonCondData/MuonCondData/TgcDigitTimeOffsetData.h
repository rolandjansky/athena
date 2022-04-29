/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_TGCDIGITTIMEOFFSETDATA_H_
#define MUONCONDDATA_TGCDIGITTIMEOFFSETDATA_H_

#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <map>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  stripOffset : std::map<ChamberId, strip_timeOffset>              *
 *  wireOffset  : std::map<ChamberId, wire_timeOffset>               *
 * where                                                             *
 *  ChamberId = (station number, 41...48)<<3 + (station eta, 1...5)  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class TgcDigitTimeOffsetData
{
public:
  TgcDigitTimeOffsetData();
  virtual ~TgcDigitTimeOffsetData() = default;

  std::map<uint16_t, float> stripOffset;
  std::map<uint16_t, float> wireOffset;
};

CLASS_DEF(TgcDigitTimeOffsetData, 236463561, 1)

#include "AthenaKernel/CondCont.h"
CLASS_DEF(CondCont<TgcDigitTimeOffsetData>, 230286007, 1)

#endif  // MUONCONDDATA_TGCDIGITTIMEOFFSETDATA_H_

