/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWDATACOLLECTIONREADOUT_H
#define AFP_RAWDATACOLLECTIONREADOUT_H

#include "AFP_ReadOut.h"

#include <stdint.h>

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

class AFP_RawDataCollectionReadOut : public AFP_ReadOut {
private:
  uint32_t m_dataWord;
  uint16_t m_wordHeader;
  uint16_t m_wordHeader2;
  uint32_t m_lvl1Id;
  uint32_t m_link;
  uint16_t m_flag;
  // uint16_t m_ecnt_LWC;
  uint16_t m_ToT2;
  // uint16_t m_ecnt_EOB;
  uint16_t m_bcid;
  uint16_t m_DiscConf;
  uint16_t m_row;
  uint16_t m_column;
  uint32_t m_ToT1;
  static const uint16_t headerPos = 24;
  static const uint16_t headerBits = 0xff;
  static const uint16_t headerPos2 = 20;
  static const uint16_t headerBits2 = 0xfff;
  static const uint16_t BOBvalue =
      0x3; // Header identifier, getBits(31,28) == 0011 (I think)
  static const uint16_t LWCvalue =
      0x3; // Have no idea what this is, let it be Data record id,
           // getBits(31,30) == 11. Could in princpile use BOBvalue, but need a
           // different boolean anyway, so for consistency define this again.;
  static const uint16_t BOLvalue = 0x18;
  static const uint16_t TLPvalue = 0x890;
  static const uint16_t TWCvalue = 0x8a;
  static const uint16_t EOBvalue = 0xf0;

public:
  AFP_RawDataCollectionReadOut();
  ~AFP_RawDataCollectionReadOut();

  void decodeWord(uint32_t dataWord);

  bool is_BOB() {
    return m_wordHeader == BOBvalue;
  }; // This is going to be is_Header
  bool is_LWC() { return m_wordHeader2 == LWCvalue; };
  //  bool is_BOL() {return m_wordHeader == BOLvalue;};
  //  bool is_TLP() {return m_wordHeader2 == TLPvalue;};
  //  bool is_TWC() {return m_wordHeader == TWCvalue;};
  //  bool is_EOB() {return m_wordHeader == EOBvalue;};

  uint32_t lvl1Id() { return m_lvl1Id; }
  uint32_t link() { return m_link; }
  uint16_t DiscConf() { return m_DiscConf; }
  uint16_t flag() { return m_flag; }

  //  uint16_t ecnt_LWC()   {return m_ecnt_LWC;}
  uint16_t ToT2() { return m_ToT2; }
  //  uint16_t ecnt_EOB()   {return m_ecnt_EOB;}
  uint16_t bcid() { return m_bcid; }
  uint16_t row() { return m_row; }
  uint16_t column() { return m_column; }
  uint16_t ToT1() { return m_ToT1; }

private:
  void setZero();
};

#endif
