/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HltOnlHelper_H
#define HltOnlHelper_H
#include <string>
#include <utility>
#include <map>
#include <stdint.h>

// Helper class for easy access of error codes defined in enumerations
namespace hltonl {
  template <class Type> class MapEnumeration {
    public:
      typedef std::pair<std::string, int>  EnumMapEntry; 
      typedef std::map<Type, EnumMapEntry> EnumMap; 

      MapEnumeration();
      virtual ~MapEnumeration() {};
      virtual std::string codeToName( Type );
      virtual int         codeToHash( Type );
      virtual typename EnumMap::const_iterator begin();
      virtual typename EnumMap::const_iterator end();
       
    protected:
      virtual void add( Type, std::string );  
      virtual void invalidCode(std::string, int );

    private:
      EnumMap      m_enumMap;
      int          m_hash_index;
      EnumMapEntry m_invalid_code;
  };
} // end name space hltonl

template <class Type> hltonl::MapEnumeration<Type>::MapEnumeration(): 
  m_hash_index(0), m_invalid_code("UNDEFINED",-1) {}

template <class Type> std::string hltonl::MapEnumeration<Type>::codeToName(Type code) {
  typename EnumMap::const_iterator map_it = m_enumMap.find(code) ;
  if(map_it != m_enumMap.end()) { 
    return ((*map_it).second).first ;
  } else {
    return m_invalid_code.first ;
  }
}

template <class Type> int hltonl::MapEnumeration<Type>::codeToHash(Type code) {
  typename EnumMap::const_iterator map_it = m_enumMap.find(code) ;
  if(map_it != m_enumMap.end()) { 
    return ((*map_it).second).second ;
  } else {
    return m_invalid_code.second ;
  }
}

template <class Type> typename hltonl::MapEnumeration<Type>::EnumMap::const_iterator hltonl::MapEnumeration<Type>::begin() {
  return m_enumMap.begin();
}

template <class Type> typename hltonl::MapEnumeration<Type>::EnumMap::const_iterator hltonl::MapEnumeration<Type>::end() {
  return m_enumMap.end();
}

template <class Type> void hltonl::MapEnumeration<Type>::add(Type code, std::string name) {
  m_hash_index++;
  m_enumMap[code] = EnumMapEntry(name, m_hash_index);
}

template <class Type> void hltonl::MapEnumeration<Type>::invalidCode(std::string inv_name, int inv_code) {
  m_invalid_code = EnumMapEntry(inv_name, inv_code);
}

#endif
