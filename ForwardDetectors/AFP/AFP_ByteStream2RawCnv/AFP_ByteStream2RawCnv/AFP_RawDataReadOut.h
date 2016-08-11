/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPRAWDATAREADOUT_H
#define AFPRAWDATAREADOUT_H

#include "AFP_ReadOut.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

class AFP_RawDataReadOut : public AFP_ReadOut {

private:
  uint32_t m_dataWord;
  uint16_t m_wordHeader;
  uint16_t m_SlotId;
  uint16_t m_LsbTdcId;
  uint16_t m_ecnt_BOT;
  uint16_t m_ecnt_EOT;
  uint16_t m_bcId;
  uint16_t m_wcnt;
  uint16_t m_Column;
  uint16_t m_bit26_27;
  uint16_t m_bit24_27;

  uint16_t m_HitDiscConfig;
  uint16_t m_Link;
  uint16_t m_fiber;
  uint16_t m_ADC;
  std::vector<bool> m_pattern;
  uint16_t m_BCId;
  bool m_bit16;
  bool m_error_bit17;
  bool m_bit18;
  std::vector<uint32_t> ToT;

  static const uint16_t headerPos = 28;
  static const uint16_t headerBits = 0xf;
  static const uint16_t BOTvalue = 0xa;
  static const uint16_t EOTvalue = 0xc;
  static const uint16_t TSMvalue = 0x3;

public:
  AFP_RawDataReadOut();
  ~AFP_RawDataReadOut();

  void decodeWord(uint32_t dataWord);

  bool is_BOT() { return (m_wordHeader == BOTvalue); };

  bool is_EOT() { return (m_wordHeader == EOTvalue); };

  bool is_TDCt() { return (m_wordHeader == TSMvalue); };

  uint16_t Column() { return m_Column; }
  uint16_t SlotId() { return m_SlotId; }
  uint16_t LsbTdcId() { return m_LsbTdcId; }
  uint16_t ecnt_BOT() { return m_ecnt_BOT; }
  uint16_t ecnt_EOT() { return m_ecnt_EOT; }
  uint16_t bcId() { return m_bcId; }
  uint16_t HitDiscConfig() { return m_HitDiscConfig; }
  uint16_t Link() { return m_Link; }
  uint16_t fiber() { return m_fiber; }
  uint16_t ADC() { return m_ADC; }
  std::vector<bool> pattern() { return m_pattern; }
  uint16_t BCId() { return m_BCId; }
  bool bit16() { return m_bit16; }
  bool error_bit17() { return m_error_bit17; }
  bool bit18() { return m_bit18; }
  bool isError() { return m_error_bit17; }
  uint16_t bit26_27() { return m_bit26_27; }
  uint16_t bit24_27() { return m_bit24_27; }

  std::vector<uint32_t> HitChan() const { return ToT; }

private:
  void setZero();
};

#endif
