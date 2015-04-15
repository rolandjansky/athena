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

  void SetChargeChanVect_PMF(std::vector<uint16_t> ChargeChan);
  void PushBack_PMF(uint16_t ChargeChan_number);

  void SetZero_PMF();

  // Add a data word to the vector
  void addData(uint32_t dataWord); 
  
  // Methods to retrieve the decoded word content

  uint16_t GetPMFId_PMF() {return PMFId;};
  uint16_t GetChannelNum_PMF() {return ChannelNumId;};
  uint16_t GetMBId_PMF() {return MBId;};
  uint16_t GetFiberFirmware_PMF() {return FiberFirmwareId;};
  uint32_t GetEventCount_PMF() {return EventCount;};
  uint16_t GetChargeHit() {return ChargeChanId;};

  // std::vector<uint16_t> GetMarocHit() {return MarocChan;};
  // uint32_t Get_Chan(uint16_t Numchan) {return MarocChan[Numchan];}; // used in SaraPkg

  bool Get_bit12() {return m_bit12;};

  // Vector containing all 16 bits data words (undecoded)
  std::vector<uint16_t> ChargeChanVect_number() const {return ChargeChan;}

  // Vector containing all 32 bits data words (undecoded) : from the Muon BSC-> to test the structure
  std::vector<uint32_t>* dataWords() const {return p_dataWords;}

 private:

  // Data info TSM: 
  uint16_t ChannelNumId;
  uint16_t PMFId;  
  uint16_t MBId; 
  uint16_t  FiberFirmwareId; 
  uint32_t EventCount;
  uint16_t ChargeChanId; 
  bool     m_bit12;
  std::vector<uint16_t> ChargeChan; 
 
  // All the datawords (no headers and footers) coming from this channel 
  // to be decoded on demand using ALFA_ReadOut methods
  std::vector<uint32_t>* p_dataWords;

};
#endif // ALFA_RAWDATA_CHARGE_H


