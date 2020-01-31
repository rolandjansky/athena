/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigCompositeUtils_HLTUtils
#define TrigCompositeUtils_HLTUtils

#include <string>
#include <inttypes.h>
#include <set>
#include <vector>
#include <mutex>

namespace TrigConf {

   typedef uint32_t HLTHash; 
  
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
    /**@brief Multi-threaded protection for the static hash operations*/
    static std::mutex s_mutex;
    /**@brief In-file identifier*/
    static std::string s_newCategory;

      
   };
   
}

#endif
