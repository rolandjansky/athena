/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfHLTUtils_HLTUtils
#define TrigConfHLTUtils_HLTUtils

#include <string>
#include <inttypes.h>
#include <set>
#include <vector>
#include <mutex>

#include <tbb/concurrent_hash_map.h>

namespace TrigConf {

  struct stringHashCompare {
    static size_t hash(const std::string& s);
    static bool equal(const std::string& x, const std::string& y);
  };

  inline size_t stringHashCompare::hash(const std::string& s) {
    return std::hash<std::string>{}(s);
  }

  inline bool stringHashCompare::equal(const std::string& x, const std::string& y) {
    return (x == y);
  }

  typedef uint32_t HLTHash; 

  struct HLTHashCompare {
    static size_t hash(const HLTHash& s);
    static bool equal(const HLTHash& x, const HLTHash& y);
  };

  inline size_t HLTHashCompare::hash(const HLTHash& h) {
    return h;
  }

  inline bool HLTHashCompare::equal(const HLTHash& x, const HLTHash& y) {
    return (x == y);
  }

  
  class HLTUtils {

    public:

    /**@brief hash function translating TE names into identifiers*/
    static HLTHash string2hash( const std::string&, const std::string& category="TE" );
    /**@brief hash function translating identifiers into names (via internal dictionary)*/
    static const std::string hash2string( HLTHash, const std::string& category="TE" );
    /**@brief debugging output of internal dictionary*/
    static void hashes2file( const std::string& fileName="hashes2string.txt" );
    /**@brief debugging output of internal dictionary*/
    static void file2hashes( const std::string& fileName="hashes2string.txt" );

    /**@brief In-file identifier*/
    static std::string s_newCategory;

    typedef tbb::concurrent_hash_map<HLTHash, std::string, HLTHashCompare> HashMap_t;
    typedef tbb::concurrent_hash_map<std::string, HashMap_t, stringHashCompare> CategoryMap_t;

    /**@brief Nested concurrent hash-maps to store (key=hash, value=string) pairs for different hash categories*/
    static CategoryMap_t s_allHashesByCategory;
      
  };
 
}

#endif
