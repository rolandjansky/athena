/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATA_H
#define ALFA_RAWDATA_H

#include<stdint.h>
#include<vector>
#include <iostream>


/** ALFA RDO's 
    @author Sara Diglio, Sept 2009
    from MuonRDO/ALFA_RawData.h
*/

class MsgStream;

class ALFA_RawData
{
 public:
  
  // Default constructor
  ALFA_RawData();
  
  // Full constructor 
  ALFA_RawData(uint16_t PMF_number);  
 
  ~ALFA_RawData(); 
 
  // set data members

  void SetWordId_PMF(uint16_t Word_number);
  void SetPMFId_PMF(uint16_t PMF_number);
  void SetMBId_PMF(uint16_t MB_number);
  void SetEventCount_PMF(uint32_t Event_number);
  void SetMarocChan_PMF(std::vector<uint16_t> MarocChan);
  void PushBack_PMF(uint16_t fiber_hit);
  void Set_error_bit17(bool bit);

  void SetZero_PMF();
  

  // Add a data word to the vector
  void addData(uint32_t dataWord); // from MUON to test
    
  // Methods to retrieve the decoded word content

  uint16_t GetWordId_PMF() {return WordId;};
  uint16_t GetPMFId_PMF() {return PMFId;};
  uint16_t GetMBId_PMF() {return MBId;};
  uint32_t GetEventCount_PMF() {return EventCount;};
  std::vector<uint16_t> GetMarocHit() {return MarocChan;};
  uint32_t Get_Chan(uint16_t Numchan) {return MarocChan[Numchan];}; // used in SaraPkg
  uint32_t GetSize() {return MarocChan.size();};
  uint16_t Get_bit26_27() {return m_bit26_27;};
  uint16_t Get_bit24_27()   {return m_bit24_27;};	
  bool Get_bit16() {return m_bit16;};
  bool Get_bit18() {return m_bit18;};
  bool Get_error_bit17() {return m_error_bit17;};

  // Vector containing all 16 bits data words (undecoded) 
  std::vector<uint16_t> HitChan() const {return MarocChan;}

  // Vector containing all 32 bits data words (undecoded) : from the Muon BSC-> to test the structure
  std::vector<uint32_t>* dataWords() const {return p_dataWords;}

 private:
    
  // Data info TSM: 
  uint16_t WordId;
  uint16_t PMFId;  
  uint16_t MBId; 
  uint32_t EventCount;
  std::vector<uint16_t> MarocChan; // sostituisce il p_hit di prima
  bool     m_bit16;
  bool     m_bit18;
  bool     m_error_bit17;
  uint16_t     m_bit26_27;
  uint16_t     m_bit24_27;	

  // All the datawords (no headers and footers) coming from this channel 
  // to be decoded on demand using ALFA_ReadOut methods
  std::vector<uint32_t>* p_dataWords;

};
#endif // ALFA_RAWDATA_H


