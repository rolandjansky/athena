/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFARAWDATAREADOUT_H
#define ALFARAWDATAREADOUT_H

#include "ALFA_ReadOut.h"
#include <stdint.h>

#include "StoreGate/StoreGateSvc.h"

#include "AthenaBaseComps/AthAlgTool.h"

//#include "AthenaKernel/MsgStreamMember.h"

#include "eformat/SourceIdentifier.h"
#include "eformat/Version.h"
#include "ByteStreamData/RawEvent.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#include "ALFA_RawEv/ALFA_RawDataCollection.h"
#include "ALFA_RawEv/ALFA_RawData.h"


// Decoding methods for raw data words 
// S. Diglio Sept 2009


class ALFA_RawDataReadOut : public ALFA_ReadOut {  

 private:

  // Raw data word
  uint32_t m_dataWord;
  // Data word header
  uint16_t m_wordHeader;

  // Header info: event counter, bunch crossing ID and tdc number: EOT and BOT
  uint16_t m_SlotId; // bit 28-24 BOT
  uint16_t m_LsbTdcId; // bit 25-24  EOT
  uint16_t m_ecnt_BOT; // bit 23-12  BOT(event counter, counted by TDC)
  uint16_t m_ecnt_EOT; // bit 23-12  BOT(event counter, counted by TDC)
  uint16_t m_bcId; // bit 11-0 BOT (bunch crossing Id)
  
  // Footer info: words counter
  uint16_t m_wcnt; // bit 11-0 EOT (numer of word in TDC data block counting itself)
  
  // Data info TSM: 
  
  uint16_t m_MBId;  //    bit 15-0 MB number PMF0 Word1

  uint16_t     m_bit26_27;
  uint16_t     m_bit24_27;	
 
  uint16_t m_WordId; // bit 25-24 (word number from 0 to3)
  uint16_t m_PMFId;  // bit 23-19 (PMF number from 1 to 23--> PMF0 also)
  uint16_t m_fiber;  // bit 15-0 (number of fiber)
  uint16_t m_ADC;  // bit 12-0 (ADC counter for 3rd and 4th word corresponding to PMF24)
  std::vector<bool> m_pattern;  // bit 15-12 (trigger type for 1st word corresponding to PMF24: bit 15->O2, bit14-> M2, bit 13-> O1, bit 12-> M1)
  uint16_t m_scaler;  // bit 15-0 (trigger counter (in 1 ms) for 2nd word corresponding to PMF24)
  bool     m_bit16;
  bool     m_error_bit17;
  bool     m_bit18;
 
  std::vector<uint32_t> m_MarocChan;

  // Define the data structure and the word header values

  // Word header Position and values
  // The word header occupies the 4 highest bits of each TDC data word 
  static const uint16_t s_headerPos  = 28;  // from bit 28 to 31
  static const uint16_t s_headerBits = 0xf;

  // Beginning of TDC 
  static const uint16_t s_BOTvalue  = 0xa;
    
  // End of TDC
  static const uint16_t s_EOTvalue   = 0xc;
  
  // TDC single measurement
  static const uint16_t s_TSMvalue   = 0x3;  
  
 public:
  
  // Constructor and destructor
  ALFA_RawDataReadOut();
  ~ALFA_RawDataReadOut();

  // Methods for decoding and encoding dataWords
  void decodeWord(uint32_t dataWord);
   
  //StatusCode encodeWord(uint16_t headerVal);

  // Methods to identify the word type
  // Beginning of TDC
  bool is_BOT() {return (m_wordHeader == s_BOTvalue);};
  // End of TDC
  bool is_EOT() {return (m_wordHeader == s_EOTvalue);};
  // TDC single measurement
  bool is_TDCt() {return (m_wordHeader == s_TSMvalue);};
   
  // Methods to retrieve the decoded word content (Get)
  
  uint16_t MBId()  {return m_MBId;}
  
  uint16_t SlotId() {return m_SlotId;}
  uint16_t LsbTdcId() {return m_LsbTdcId;}
  uint16_t ecnt_BOT()  {return m_ecnt_BOT;}
  uint16_t ecnt_EOT()  {return m_ecnt_EOT;}
  uint16_t bcId()  {return m_bcId;}
  uint16_t WordId()        {return m_WordId;}
  uint16_t PMFId()   {return m_PMFId;}
  uint16_t fiber()    {return m_fiber;}
  uint16_t ADC()    {return m_ADC;}
  std::vector<bool> pattern()    {return m_pattern;}
  uint16_t scaler()    {return m_scaler;}
  bool     bit16()   {return m_bit16;}
  bool     error_bit17() {return m_error_bit17;}
  bool     bit18()   {return m_bit18;}
  bool     isError() {return m_error_bit17;}
  uint16_t     bit26_27()   {return m_bit26_27;}
  uint16_t     bit24_27()   {return m_bit24_27;}

  std::vector<uint32_t> HitChan() const {return m_MarocChan;}

 private:

  void setZero();
  
  // Methods for the decoding
  //ALFA_RawDataReadOut * m_ALFA_RawDataReadOut;

};

#endif  // ALFARAWDATAREADOUT_H
