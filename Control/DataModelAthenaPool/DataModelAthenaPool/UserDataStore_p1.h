/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef USERDATASTOREOBJ_P1
#define USERDATASTOREOBJ_P1



#include "DataModelAthenaPool/UDSLabelHashTable_p1.h"

/** @class UserDataStoreObj_p1
 *  @brief T/P converter class for UserDataStoreObj
 */

class UserDataStore_p1: public UDSLabelHashTable_p1 {
 public:

  uint64_t m_eventABC;
  
  //Values in mapmap
  std::vector<uint64_t> m_barcode;
  std::vector<uint32_t> m_hash;
  std::vector<uint16_t> m_valuesPerABC;
  std::vector<uint16_t> m_vecSize;
  std::vector<uint32_t> m_values;
  std::vector<uint8_t>  m_type;

};


#endif
