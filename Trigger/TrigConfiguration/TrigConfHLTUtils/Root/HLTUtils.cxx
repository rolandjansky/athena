/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <set>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <fstream>

#include "TrigConfHLTUtils/HLTUtils.h"
#include "CxxUtils/checker_macros.h"

using namespace TrigConf;

std::mutex HLTUtils::s_mutex;

std::string HLTUtils::s_newCategory ATLAS_THREAD_SAFE = std::string("##NewCategory");

//
// \brief function used to generate uniqu  ID (integer) from string
//        In fact uniqueness is not 100% guaranteed. Thread safe
// \param s string to be hashed
//
// \return key for this string (this function never fails)
//
namespace HashChecking {
  typedef std::map<HLTHash, std::string> HashMap;
  typedef std::map<std::string, HashMap> CategoryMap;
  static CategoryMap AllHashesByCategory ATLAS_THREAD_SAFE;
  void checkGeneratedHash (HLTHash hash,  const std::string& s,   const std::string& category) {
    std::lock_guard<std::mutex> lock(HLTUtils::s_mutex);
    HashMap& hashes = AllHashesByCategory[category];
    if ( hashes[hash] == "" )
      hashes[hash] = s;
    else if ( hashes[hash] != s ) {
      std::stringstream ss;
      ss << "Hashes the same for category: "<<category
            << " and elements "<< hashes[hash] << " " << s;
      throw std::domain_error( ss.str() );
    }
  }
}

HLTHash HLTUtils::string2hash( const std::string& s, const std::string& category )
{
  // hash function (based on available elswhere ELF hash function)
  // uniqueness tested in MC way; contact me for details
  // email: Tomasz.Bold@cern.ch
  HLTHash hash;
  hash = 0xd2d84a61;
  int i;

  for ( i = (int)s.size()-1; i >= 0; --i )
    hash ^= ( hash >> 5) + s[i] + ( hash << 7 );

  for ( i = 0; i < (int)s.size(); ++i )
    hash ^= ( hash >> 5) + s[i] + ( hash << 7 );

    
  HashChecking::checkGeneratedHash(hash, s, category);

  return hash;
}

const std::string HLTUtils::hash2string( HLTHash hash, const std::string& category ) {
  std::lock_guard<std::mutex> lock(s_mutex);
  HashChecking::CategoryMap::const_iterator mapForCategoryIt = HashChecking::AllHashesByCategory.find(category);
  if ( mapForCategoryIt == HashChecking::AllHashesByCategory.end() ) {
    return "UNKNOWN CATEGORY";
  }
  HashChecking::HashMap::const_iterator hashMapIt = mapForCategoryIt->second.find(hash);
  if ( hashMapIt == mapForCategoryIt->second.end() ) 
    return "UNKNOWN HASH ID";
  return hashMapIt->second;
}

void HLTUtils::hashes2file( const std::string& fileName) {
  std::lock_guard<std::mutex> lock(s_mutex);
  std::ofstream fout(fileName);
  for (const auto& categoryMapElement : HashChecking::AllHashesByCategory) {
    const std::string& categoryName = categoryMapElement.first;
    const HashChecking::HashMap& categoryMap = categoryMapElement.second;
    fout << s_newCategory << std::endl << categoryName << std::endl;
    for (const auto& mapElement : categoryMap) {
      const HLTHash hash = mapElement.first;
      std::string name = mapElement.second;
      name.erase(std::remove(name.begin(), name.end(), '\n'), name.end());
      fout << hash << std::endl << name << std::endl;
    }
  }
}

void HLTUtils::file2hashes( const std::string& fileName) {
  std::ifstream fin(fileName);
  if (!fin.is_open()) {
    return;
  }
  std::string line;
  std::string category;
  // Note: this method is a to aid with development/debugging. 
  // It won't be used in production code, hence it is light on error checking.
  while(std::getline(fin, line)) {
    if (line == s_newCategory) {
      std::getline(fin, category);
      continue;
    }
    HLTHash hash = std::stoul(line);
    std::string name;
    std::getline(fin, name);
    HLTHash check = string2hash(name, category);
    if (check != hash) {
      std::cerr << "Inconsistency in file2hashes(" << fileName << ") function,"
                   " item " << name << " has hash " << hash << " not " << check << std::endl;
    }
  }
}
