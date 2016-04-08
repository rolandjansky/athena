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
  void SetMarocChan_PMF(const std::vector<uint16_t>& MarocChan);
  void PushBack_PMF(uint16_t fiber_hit);
  void Set_error_bit17(bool bit);

  void SetZero_PMF();
  

  // Add a data word to the vector
  void addData(uint32_t dataWord); // from MUON to test
    
  // Methods to retrieve the decoded word content

  uint16_t GetWordId_PMF() const {return m_WordId;};
  uint16_t GetPMFId_PMF() const {return m_PMFId;};
  uint16_t GetMBId_PMF() const {return m_MBId;};
  uint32_t GetEventCount_PMF() const {return m_EventCount;};
  const std::vector<uint16_t>& GetMarocHit() const {return m_MarocChan;};
  uint32_t Get_Chan(uint16_t Numchan) const {return m_MarocChan[Numchan];}; // used in SaraPkg
  uint32_t GetSize() const {return m_MarocChan.size();};
  uint16_t Get_bit26_27() const {return m_bit26_27;};
  uint16_t Get_bit24_27() const  {return m_bit24_27;};	
  bool Get_bit16() const {return m_bit16;};
  bool Get_bit18() const {return m_bit18;};
  bool Get_error_bit17() const {return m_error_bit17;};

  // Vector containing all 16 bits data words (undecoded) 
  const std::vector<uint16_t>& HitChan() const {return m_MarocChan;}

  // Vector containing all 32 bits data words (undecoded) : from the Muon BSC-> to test the structure
  const std::vector<uint32_t>* dataWords() const {return &m_dataWords;}

 private:
    
  // Data info TSM: 
  uint16_t m_WordId;
  uint16_t m_PMFId;  
  uint16_t m_MBId; 
  uint32_t m_EventCount;
  std::vector<uint16_t> m_MarocChan; // sostituisce il p_hit di prima
  bool     m_bit16;
  bool     m_bit18;
  bool     m_error_bit17;
  uint16_t     m_bit26_27;
  uint16_t     m_bit24_27;	

  // All the datawords (no headers and footers) coming from this channel 
  // to be decoded on demand using ALFA_ReadOut methods
  std::vector<uint32_t> m_dataWords;

};
#endif // ALFA_RAWDATA_H


