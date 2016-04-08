/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATACONTAINER_H
#define ALFA_RAWDATACONTAINER_H

#include <vector>
#include <string>
#include "ALFA_RawEv/ALFA_RawDataCollection.h"
#include "SGTools/CLASS_DEF.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
//#include "EventContainers/IdentifiableContainer.h" 

/** This container provides acces to the PMF RDOs
    @author Sara Diglio, Sept 2009
*/


//class ALFA_RawDataContainer : public IdentifiableContainer<ALFA_RawDataCollection> {
class ALFA_RawDataContainer : public DataVector<ALFA_RawDataCollection> {

 private:

  // Raw data word and word header
 
  uint32_t m_wordMarker;
 
  // I added runNum, runType, DetEventType
  // modification based on page19 MROD data format document and on ALFA dumped data file
  //uint32_t m_dataWord;

  // Data members
  uint16_t m_subdetId;   // Sub-detector Id 
  uint16_t m_mrodId;     // MROD Id
  uint32_t m_lvl1Id;     // Lvl1 Id
  uint16_t m_ecrId;      // ECR Id
  uint16_t m_bcId;       // Bunch crossing Id
  uint32_t m_runNum;     // Run sequence number
  uint16_t m_runType;       // Run Type
  uint16_t m_triggerTypeId; // Trigger type Id
  uint32_t m_DetEventType;     // Detector Event Type

  // Data words in the ROD header
  
  // Full Event marker
  static const uint32_t s_FullEVmarker = 0xaa1234aa;
  // ROB marker
  static const uint32_t s_ROBmarker = 0xdd1234dd;
  // ROD marker
  static const uint32_t s_RODmarker = 0xee1234ee;

  static const uint32_t s_RODheadersize = 0x00000009; 
  static const uint32_t s_RODversion  = 0x03010000;      

  //Time Stamp
  uint32_t m_TimeStamp;
  //Time Stamp ns
  uint32_t m_TimeStampns;
  //Bunch Crossing ID
  uint32_t m_BCId;
  //LumiBlock ID
  uint32_t m_LumiBlock;
  //Lvl1 trigger pattern (fired items in lvl1 trigger)
  std::vector<bool> m_lvl1Pattern;
  //Lvl2 trigger pattern (fired items in lvl2 trigger)
  std::vector<bool> m_lvl2Pattern;
  //EF pattern (fired items in EF)
  std::vector<bool> m_efPattern;

 public:  

  ALFA_RawDataContainer() ; 
  ~ALFA_RawDataContainer() ; 

  typedef ALFA_RawDataCollection::size_type size_type ; 
 
  size_type digit_size() const ; 

  bool is_FullEVmarker() const {return m_wordMarker == s_FullEVmarker;};
  bool is_ROBmarker() const {return m_wordMarker == s_ROBmarker;};  	
  bool is_RODmarker() const {return m_wordMarker == s_RODmarker;};  	
  

  // Retrieve decoded results
  uint16_t subdetId()  const {return m_subdetId;}
  uint16_t mrodId()    const {return m_mrodId;}
  uint32_t runNum()    const {return m_runNum;}
  uint16_t runType()   const {return m_runType;}
  uint32_t lvl1Id()    const {return m_lvl1Id;}
  uint16_t ecrId()     const {return m_ecrId;}
  uint16_t bcId()      const {return m_bcId;}
  uint16_t trigtypeId() const {return m_triggerTypeId;}
  uint32_t DetEventType() const    {return m_DetEventType;}
  

  void SetLvl1Pattern(const std::vector<bool>& tmplvl1_pattern);
  void SetLvl2Pattern(const std::vector<bool>& tmplvl2_pattern);
  void SetEFPattern(const std::vector<bool>& tmpef_pattern);
  void SetTimeStamp(uint32_t tmpTimeStamp);
  void SetTimeStampns(uint32_t tmpTimeStampns);
  void SetLumiBlock(uint32_t tmpLumiBlock);
  void SetBCId(uint32_t tmpBCId);

  const std::vector<bool>& GetLvl1Pattern_POT() const {return m_lvl1Pattern;};
  const std::vector<bool>& GetLvl2Pattern_POT() const {return m_lvl2Pattern;};
  const std::vector<bool>& GetEFPattern_POT() const {return m_efPattern;};
  uint32_t GetTimeStamp() const {return m_TimeStamp;};	
  uint32_t GetTimeStampns() const {return m_TimeStampns;};	
  uint32_t GetLumiBlock() const {return m_LumiBlock;};
  uint32_t GetBCId() const {return m_BCId;};	

 private:
  
  // Private functions
  void setZero();
    
};

CLASS_DEF( ALFA_RawDataContainer , 1083669809 , 0 ) // da rimettere a posto prima di sottomettere in SVN


#endif     // ALFA_RAWDATACONTAINER_H
