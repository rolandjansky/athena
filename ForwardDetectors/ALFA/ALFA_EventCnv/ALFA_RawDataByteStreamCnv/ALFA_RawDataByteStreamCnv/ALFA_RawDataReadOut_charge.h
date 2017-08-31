/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFARAWDATAREADOUT_CHARGE_H
#define ALFARAWDATAREADOUT_CHARGE_H

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

#include "ALFA_RawEv/ALFA_RawDataContainer_charge.h"
#include "ALFA_RawEv/ALFA_RawDataCollection_charge.h"
#include "ALFA_RawEv/ALFA_RawData_charge.h"



// Decoding methods for raw data words 
// S. Diglio Sept 2009


class ALFA_RawDataReadOut_charge : public ALFA_ReadOut {  

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
  
  // Data info TSM_charge: 
  
  uint16_t m_MBId;  //    bit 7-0 MB number PMF0
  uint16_t m_FiberFirmwareId; // bit 15-8 PMF0
  
  uint16_t m_PMFId; // bit 23-19 
  uint16_t m_ChannelNumId;  // bit 18-13 PMF channel num
  uint16_t m_ChargeChanId;  // bit 11-0 
  
  bool     m_bit12;
  
  uint16_t     m_bit24_27;	
  uint16_t     m_bit27_24;	

  std::vector<uint32_t> m_ChargeChan;


  // Define the data structure and the word header values

  // PMF header Position and values
  // The word header occupies the 4 highest bits of each TDC data word 
  static const uint16_t s_headerPos  = 28;  // from bit 28 to 31
  static const uint16_t s_headerBits = 0xf;

  // Beginning of TDC 
  static const uint16_t s_BOTvalue  = 0xa;
    
  // End of TDC
  static const uint16_t s_EOTvalue   = 0xc;
  
  // TDC single measurement charge
  static const uint16_t s_TSMvalue_charge   = 0x4;
  
  
 public:
  
  // Constructor and destructor
  ALFA_RawDataReadOut_charge();
  ~ALFA_RawDataReadOut_charge();

  // Methods for decoding and encoding dataWords
  void decodeWord(uint32_t dataWord);
   
  //StatusCode encodePMF(uint16_t headerVal);

  // Methods to identify the word type
  // Beginning of TDC
  bool is_BOT() {return (m_wordHeader == s_BOTvalue);};
  // End of TDC
  bool is_EOT() {return (m_wordHeader == s_EOTvalue);};
  // TDC single measurement
  bool is_TDCch() {return (m_wordHeader == s_TSMvalue_charge);};
   
  
  // Methods to retrieve the decoded word content (Get)
  
  uint16_t SlotId() {return m_SlotId;}
  uint16_t LsbTdcId() {return m_LsbTdcId;}
  uint16_t ecnt_BOT()  {return m_ecnt_BOT;}
  uint16_t ecnt_EOT()  {return m_ecnt_EOT;}
  uint16_t bcId()  {return m_bcId;}


  uint16_t MBId()        {return m_MBId;}
  uint16_t PMFId()        {return m_PMFId;}
  uint16_t ChannelNumId()   {return m_ChannelNumId;}
  uint16_t ChargeChanId()    {return m_ChargeChanId;}
  uint16_t FiberFirmwareId() {return m_FiberFirmwareId;};
  bool     bit12()   {return m_bit12;}
 
  uint16_t     bit27_24()   {return m_bit27_24;}
  uint16_t     bit24_27()   {return m_bit24_27;}		


 private:

  void setZero();
 

};

#endif  // ALFARAWDATAREADOUT_CHARGE_H
