/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATA_CHARGE_H
#define ALFA_RAWDATA_CHARGE_H

#include<stdint.h>
#include<vector>
#include <iostream>


/** ALFA RDO's 
    @author Sara Diglio, Sept 2009
    from MuonRDO/ALFA_RawData.h
*/

class MsgStream;

class ALFA_RawData_charge
{
 public:
  
  // Default constructor
  ALFA_RawData_charge();
  
  // Full constructor 
  ALFA_RawData_charge(uint16_t PMF_number);  
    
  ~ALFA_RawData_charge();

  // set data members

  void SetPMFId_PMF(uint16_t PMF_number);
  void SetChannelNum_PMF(uint16_t Channel_number);
  void SetMBId_PMF(uint16_t MB_number);
  void SetFiberFirmware_PMF(uint16_t FiberFirmware_number);
  void SetEventCount_PMF(uint32_t Event_number);
  void SetChargeChan_PMF(uint16_t ChargeChan_number);

  void SetChargeChanVect_PMF(const std::vector<uint16_t>& ChargeChan);
  void PushBack_PMF(uint16_t ChargeChan_number);

  void SetZero_PMF();

  // Add a data word to the vector
  void addData(uint32_t dataWord); 
  
  // Methods to retrieve the decoded word content

  uint16_t GetPMFId_PMF() const {return m_PMFId;};
  uint16_t GetChannelNum_PMF() const {return m_ChannelNumId;};
  uint16_t GetMBId_PMF() const {return m_MBId;};
  uint16_t GetFiberFirmware_PMF() const {return m_FiberFirmwareId;};
  uint32_t GetEventCount_PMF() const {return m_EventCount;};
  uint16_t GetChargeHit() const {return m_ChargeChanId;};

  // std::vector<uint16_t> GetMarocHit() {return MarocChan;};
  // uint32_t Get_Chan(uint16_t Numchan) {return MarocChan[Numchan];}; // used in SaraPkg

  bool Get_bit12() const {return m_bit12;};

  // Vector containing all 16 bits data words (undecoded)
  const std::vector<uint16_t>& ChargeChanVect_number() const {return m_ChargeChan;}

  // Vector containing all 32 bits data words (undecoded) : from the Muon BSC-> to test the structure
  const std::vector<uint32_t>* dataWords() const {return &m_dataWords;}

 private:

  // Data info TSM: 
  uint16_t m_ChannelNumId;
  uint16_t m_PMFId;  
  uint16_t m_MBId; 
  uint16_t m_FiberFirmwareId; 
  uint32_t m_EventCount;
  uint16_t m_ChargeChanId; 
  bool     m_bit12;
  std::vector<uint16_t> m_ChargeChan; 
 
  // All the datawords (no headers and footers) coming from this channel 
  // to be decoded on demand using ALFA_ReadOut methods
  std::vector<uint32_t> m_dataWords;

};
#endif // ALFA_RAWDATA_CHARGE_H


