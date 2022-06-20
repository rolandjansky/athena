/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_TGCDIGITCROSSTALKDATA_H_
#define MUONCONDDATA_TGCDIGITCROSSTALKDATA_H_

#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <array>
#include <map>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  stripProb : std::map<LayerId, strip_prob>                                      *
 *  wireProb  : std::map<LayerId, wire_prob>                                       *
 * where                                                                           *
 *  LayerId = (station number, 41..48)<<5 + (station eta, 1..5)<<2 + (layer, 1..3) *
 *  strip(wire)_prob = std::array<float, 4> (prob10, prob11, prob20, prob21)       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class TgcDigitCrosstalkData
{
 public:
  TgcDigitCrosstalkData();
  virtual ~TgcDigitCrosstalkData() = default;

  static constexpr unsigned int N_PROB = 4;

  void setStripProbability(const uint16_t layer_id, const std::array<float, N_PROB> prob);
  void setWireProbability(const uint16_t layer_id, const std::array<float, N_PROB> prob);

  float getStripProbability(const uint16_t layer_id, const unsigned int index_prob) const;
  float getWireProbability(const uint16_t layer_id, const unsigned int index_prob) const;

 private:
  std::map<uint16_t, std::array<float, N_PROB>> m_stripProb;
  std::map<uint16_t, std::array<float, N_PROB>> m_wireProb;
};

CLASS_DEF(TgcDigitCrosstalkData, 13719396, 1)

#include "AthenaKernel/CondCont.h"
CLASS_DEF(CondCont<TgcDigitCrosstalkData>, 247967934, 1)

#endif  // MUONCONDDATA_TGCDIGITCROSSTALKDATA_H_

