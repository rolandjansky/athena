/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCCRAWDATACOLLECTION_P3_H
#define MUON_CSCCRAWDATACOLLECTION_P3_H



//#include "MuonRDO/CscRawDataCollection.h"
#include "MuonEventAthenaPool/CscRawData_p3.h"

/**
Persistent represenation of the Collection of CSC Raw Hits.
*/
class CscRawDataCollection_p3
   : public std::vector<CscRawData_p3>
{
public:

  // Default constructor 
  CscRawDataCollection_p3 () :
    std::vector<CscRawData_p3>(),
    m_id(0),
    m_rodId(0),
    m_subDetectorId(0),
    m_numRPU(0),
    m_samplingPhase(false),
    m_triggerType(false),
    m_firstBitSummary(0),
    m_eventType(0),
    m_rpuID(),
    m_dataType(),
    m_spuCount(10,0),
    m_scaAddress(0)
  {}

friend class  CscRawDataCollectionCnv_p3;
  
private:

  /** Identifier of this collection */
  uint32_t m_id;

  /** online ROD Identifier */
  uint16_t m_rodId;
  uint16_t m_subDetectorId;

  /** number of RPU */
  uint16_t m_numRPU;  

  /** sampling phase */
  bool m_samplingPhase;

  /** trigger Type */ 
  bool m_triggerType;

  /** First Bit Summary */
  uint8_t m_firstBitSummary;

  /** event type/data type */
  uint32_t m_eventType;

  /** RPU ID and data type */
  std::vector<uint16_t> m_rpuID;
  std::vector<uint8_t> m_dataType;

  /** cluster counts for 10 SPU: 5 SPU in each chamber or RPU! 
      For Chamber CSS, RPU ID = 5 and for CSL RPU ID = 11
      For CSS, SPU ID goes from 0-4 and 5-9 for CSS
      The non-precision SPU are at index 4 and 9 in the list */
  std::vector<uint16_t> m_spuCount;

  /** SCA Address */
  uint32_t m_scaAddress;

};


#endif




