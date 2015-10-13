/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-


#ifndef ATHCONTAINERS_UDSLABELHASHTABLE_H
#define ATHCONTAINERS_UDSLABELHASHTABLE_H

// Don't use this header outside of Athena:
#ifdef XAOD_STANDALONE
#error "UDSLabelHashTable.h is not to be used outside of Athena!"
#endif // XAOD_STANDALONE


#include "SGTools/crc64.h"
#include <map>
#include <string>
#include "GaudiKernel/MsgStream.h"

#include <stdint.h>


/** @class UDSLabelHashTable
 * @brief Helper class for UserDataStore
 * Provides the hash function and keeps a map<hash,label>
 */

class UDSLabelHashTable {

 public:
  typedef uint32_t hash_t;

  enum {INVALID=0};

  hash_t hash(const std::string& label) const;
  const std::string& getLabel(const hash_t number) const;
  hash_t addLabel(const std::string& label);
  void addLabel(const std::string& label, hash_t hash);

  std::vector<const std::string*> getAllLabels() const;

  size_t size() const {return m_hashLabels.size();}

  typedef std::map<hash_t, std::string>::const_iterator const_iterator;
  const_iterator begin() const { return m_hashLabels.begin(); }
  const_iterator end()   const { return m_hashLabels.end();   }

 private:
  std::map<hash_t, std::string> m_hashLabels;
  std::string m_empty;
  
  
};
  
inline
UDSLabelHashTable::hash_t UDSLabelHashTable::hash(const std::string& label) const {
  const uint64_t hash64=SG::crc64(label);
  return (hash_t)(hash64 & 0xFFFFFFFF);
}

#endif // not ATHCONTAINERS_UDSLABELHASHTABLE_H
