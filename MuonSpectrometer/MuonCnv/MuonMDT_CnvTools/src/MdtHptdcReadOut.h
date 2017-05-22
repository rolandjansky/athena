/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTHPTDCREADOUT_H
#define MUONBYTESTREAM_MDTHPTDCREADOUT_H

#include "MdtReadOut.h"
#include <stdint.h>

// Decoding methods for MDT Hptdc raw data words
// S. Rosati, J. Meyer Apr 2017

class MdtHptdcReadOut : public MdtReadOut {

 private:

  // Raw data word
  uint32_t m_dataWord;
  // Data word header
  uint16_t m_wordHeader;

  // Header info: event counter, bunch crossing ID and tdc number
  uint16_t m_tdcId;
  uint16_t m_ecnt;
  uint16_t m_bcId;

  // Footer info: words counter
  uint16_t m_wcnt;

  // Data info:
  uint16_t m_channel;
  uint16_t m_coarse;
  uint16_t m_fine;
  uint16_t m_width;
  bool     m_errflag;

  bool     m_leading;

  // Define the data structure and the word header values

  // Word header Position and values
  // The word header occupies the 4 highest bits of each TDC data word
  static const uint16_t headerPos  = 28;
  static const uint16_t headerBits = 0xf;

  // Beginning of TDC: two possible header values
  // for channels 00-15 and 16-17 respectively
  static const uint16_t BOTvalue1  = 0xa;
  static const uint16_t BOTvalue2  = 0xb;

  // End of TDC
  static const uint16_t EOTvalue   = 0xc;

  // TDC single measurement
  static const uint16_t TSMvalue_lead   = 0x4;
  static const uint16_t TSMvalue_tail   = 0x5;

  // TDC combined measurement
  static const uint16_t TCMvalue   = 0x9;

  // TDC error status
  static const uint16_t TESvalue   = 0x6;

 public:

  // Constructor and destructor
  MdtHptdcReadOut();
  ~MdtHptdcReadOut();

  // Methods for decoding and encoding dataWords
  void decodeWord(uint32_t dataWord);
  void encodeWord(uint16_t headerVal);

  // Methods to identify the word type
  // Beginning of TDC
  bool is_BOT() {return ((m_wordHeader == BOTvalue1) ||
                         (m_wordHeader == BOTvalue2));};
  // End of TDC
  bool is_EOT() {return (m_wordHeader == EOTvalue);};
  // TDC single measurement
  bool is_TSM() {return ( (m_wordHeader == TSMvalue_lead) ||
                          (m_wordHeader == TSMvalue_tail) );};
  // TDC combined measurement
  bool is_TCM() {return (m_wordHeader == TCMvalue);};
  // TDC error status
  bool is_TES() {return (m_wordHeader == TESvalue);};

  // Methods to retrieve the decoded word content
  uint16_t tdcId() {return m_tdcId;}
  uint16_t ecnt()  {return m_ecnt;}
  uint16_t bcId()  {return m_bcId;}

  uint16_t channel()   {return m_channel;}
  uint16_t coarse()    {return m_coarse;}
  uint16_t fine()      {return m_fine;}
  uint16_t width()     {return m_width;}
  bool     errflag()   {return m_errflag;}
  bool     isLeading() {return m_leading;}

  // Methods to encode data in 32 bits words

  uint32_t makeBOT(uint16_t tdcId, uint16_t ecnt, uint16_t bcid);
  uint32_t makeEOT(uint16_t tdcId, uint16_t ecnt, uint16_t wcnt);
  uint32_t makeTSM(uint16_t tdcId, uint16_t channel, bool leading,
		   uint16_t coarse, uint16_t fine);

  uint32_t makeTCM(uint16_t tdcId, uint16_t channel, uint16_t width,
		   uint16_t coarse, uint16_t fine);

 private:

  void setZero();

};

#endif  // MUONBYTESTREAM_MDTHPTDCREADOUT_H
