/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATACONTAINER_CHARGE_H
#define ALFA_RAWDATACONTAINER_CHARGE_H

#include <vector>
#include <string>
#include "ALFA_RawEv/ALFA_RawDataCollection_charge.h"
#include "SGTools/CLASS_DEF.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
//#include "EventContainers/IdentifiableContainer.h" 

/** This container provides acces to the PMF RDOs
    @author Sara Diglio, Sept 2009
*/


//class ALFA_RawDataContainer_charge : public IdentifiableContainer<ALFA_RawDataCollection_charge> {
class ALFA_RawDataContainer_charge : public DataVector<ALFA_RawDataCollection_charge> {

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

 public:  

  ALFA_RawDataContainer_charge() ; 
  ~ALFA_RawDataContainer_charge() ; 

  typedef ALFA_RawDataCollection_charge::size_type size_type ; 
 
  size_type digit_size() const ; 

  bool is_FullEVmarker() const {return m_wordMarker == s_FullEVmarker;};
  bool is_ROBmarker()    const {return m_wordMarker == s_ROBmarker;};  	
  bool is_RODmarker()    const {return m_wordMarker == s_RODmarker;};  	

  // Retrieve decoded results
  uint16_t subdetId()   const {return m_subdetId;}
  uint16_t mrodId()     const {return m_mrodId;}
  uint32_t runNum()     const {return m_runNum;}
  uint16_t runType()    const {return m_runType;}
  uint32_t lvl1Id()     const {return m_lvl1Id;}
  uint16_t ecrId()      const {return m_ecrId;}
  uint16_t bcId()       const {return m_bcId;}
  uint16_t trigtypeId() const {return m_triggerTypeId;}
  uint32_t DetEventType() const {return m_DetEventType;}
  
 private:
  
  // Private functions
  void setZero();
    
};

CLASS_DEF( ALFA_RawDataContainer_charge , 1146048907, 0 ) 


#endif     // ALFA_RAWDATACONTAINER_CHARGE_H
