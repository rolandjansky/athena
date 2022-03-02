/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_CSCRDOIDHASH_H
#define MUONRDO_CSCRDOIDHASH_H



#include "CSCcabling/CSCcablingSvc.h"

#include <map>
#include <vector>
#include <inttypes.h>

/**
  Hash function for CSC Raw Data Collection
  @author Ketevi A. Assamagan
*/
class CscRawDataCollectionIdHash
{
public:

  typedef uint16_t ID; 

  CscRawDataCollectionIdHash ();
  ~CscRawDataCollectionIdHash() {}

  /**Convert ID to int. return INVALID_ID if invalid ID */
  int operator() (const ID& id) const ;
  
  /**return maximum number of IDs */
  int max() const {return m_size;} 

  /** reverse conversion */
  ID identifier(int i) const;   

  /** reverse conversion for SubDetectorID and ROD ID*/
  uint16_t subDetectorId(int i) const;
  uint16_t rodId(unsigned int i) const;

  // return value definition
  enum {INVALID_ID = 0xFFFF};

private:
  /** total number of IDs */
  int m_size;

  /** lookup table 
  */
  std::map<ID,int>  m_lookup;

  /** reverse lookup 
  */ 
  std::vector<ID>  m_int2id;

  /** reverse lookup for SubDetectorID and ROD ID
   */ 
  std::vector<uint16_t> m_int2subDetectorId;
  std::vector<uint16_t> m_int2rodId;

  CSCcablingSvc * m_cabling = nullptr;

};

#endif



