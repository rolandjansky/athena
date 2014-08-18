/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//dear emacs, this is -*-c++-*-
#ifndef USDLABELHASHTABLE_P1_H
#define USDLABELHASHTABLE_P1_H

#include <vector>
#include <string>


/** @class UDSLabelHashTable_p1
 *  @brief Persistent representation of the UDSLabelHashTable class used by UserDataStore
 **/
 
class UDSLabelHashTable_p1 {
 public:
  typedef uint32_t hash_t;
  //Hash-to-label association
  std::vector<std::string> m_label;
  std::vector<hash_t> m_labelHash;

};


#endif
