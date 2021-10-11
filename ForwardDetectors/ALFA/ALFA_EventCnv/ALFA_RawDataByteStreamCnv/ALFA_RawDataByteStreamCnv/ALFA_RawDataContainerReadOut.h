/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATACONTAINERREADOUT_H
#define ALFA_RAWDATACONTAINERREADOUT_H

#include "ALFA_ReadOut.h"
#include "ALFA_RawDataReadOut_charge.h"

#include <stdint.h>
#include <vector>

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

#include "ALFA_RawEv/ALFA_RawDataContainer_charge.h"
#include "ALFA_RawEv/ALFA_RawDataCollection_charge.h"
#include "ALFA_RawEv/ALFA_RawData_charge.h"


// Decoding methods for a ROD
// S. Diglio 

class ALFA_RawDataContainerReadOut : public ALFA_ReadOut
{

 private:
 
  // Raw data word and word header
 
  uint32_t m_wordMarker;

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


  uint32_t m_timestamp;     // timestamp
  uint32_t m_BCId;     // Bunch Crossing Id
  
  
  // Data words in the ROD header
  
  // Full Event marker
  static constexpr uint32_t s_FullEVmarker = 0xaa1234aa;
  // ROB marker
  static constexpr uint32_t s_ROBmarker = 0xdd1234dd;
  // ROD marker
  static constexpr uint32_t s_RODmarker = 0xee1234ee;

  static constexpr uint32_t s_RODheadersize = 0x00000009; 
  static constexpr uint32_t s_RODversion  = 0x03010000;    

 public:
  ALFA_RawDataContainerReadOut();
 
  bool is_FullEVmarker() const {return m_wordMarker == s_FullEVmarker;}
  bool is_ROBmarker() const {return m_wordMarker == s_ROBmarker;}
  bool is_RODmarker() const {return m_wordMarker == s_RODmarker;}
  
 
  void decodeWord(uint32_t dataWord);
  void decodeHeader(std::vector<uint32_t>& vData);
  void decodeHeaderFull(std::vector<uint32_t>& );
  


  // Retrieve decoded results
  uint16_t subdetId() const  {return m_subdetId;}
  uint16_t mrodId()   const  {return m_mrodId;}
  uint32_t runNum()   const  {return m_runNum;}
  uint16_t runType() const   {return m_runType;}
  uint32_t lvl1Id()   const  {return m_lvl1Id;}
  uint16_t ecrId()   const   {return m_ecrId;}
  uint16_t bcId()    const   {return m_bcId;}
  uint16_t trigtypeId() const {return m_triggerTypeId;}
  uint32_t DetEventType()  const   {return m_DetEventType;}
  
  uint32_t TimeStamp()  const   {return m_timestamp;}
  uint32_t BCId() const    {return m_BCId;}
  
 private:
  
  // Private functions
  void setZero();
  void setZeroFull();
};   // ALFA_RODREADOUT_H

#endif
