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

std::string HLTUtils::s_newCategory ATLAS_THREAD_SAFE = std::string("##NewCategory");
HLTUtils::CategoryMap_t HLTUtils::s_allHashesByCategory ATLAS_THREAD_SAFE = HLTUtils::CategoryMap_t();

namespace HashChecking {
  //
  // \brief Function to check for hash collisions. To minimize the chance of collision,
  // the hash can additionally be assigned to a category. 
  // Thread-safe implementation. 
  // \param hash The hash of the string
  // \param s The string
  // \param category The category this string is placed under
  // \throws std::domain_error Upon a hash-collision within a category
  void checkGeneratedHash(HLTHash hash, const std::string& s, const std::string& category)
  {

    // Attempt a read-only check. 
    // This is the most common use-case and it avoids locking.
    // Hence it is suitable for many threads to use at once.
    {
      HLTUtils::CategoryMap_t::const_accessor cacc_category;
      const bool categoryExists = HLTUtils::s_allHashesByCategory.find(cacc_category, category);
      if (categoryExists) {
        HLTUtils::HashMap_t::const_accessor cacc_hash;
        const bool hashExists = cacc_category->second.find(cacc_hash, hash);
        if (hashExists) {
          if (s != cacc_hash->second) {
            // Stored string disagrees with test string for given hash. Flag a hash collision!
            std::stringstream ss;
            ss << "Hashes the same for category: " << category
               << " and elements "<< cacc_hash->second << " " << s;
            throw std::domain_error( ss.str() );
          } else {
            // Stored hash's string in agreement with test string. No collisions. Finished.
            return;
          }
        }
      }
    }

    // If we failed to find either the category-map itself, or the hash within its category-map,
    // then we need to obtain exclusive mutable access to add the category and/or hash.
    // This is rarer, and will temporarily block other threads trying to read from within the respective hash-buckets.
    // The difference is the use of accessor rather than const_accessor and insert() instead of find()   
    //
    // We don't mind if this is a new category or an existing one (we ignore return boolean from s_allHashesByCategory.insert)
    // We use the default constructor in the case that the category-map is newly created.
    HLTUtils::CategoryMap_t::accessor acc_category;
    HLTUtils::s_allHashesByCategory.insert(acc_category, category);
    HLTUtils::HashMap_t::accessor acc_hash;
    const bool newEntryInMap = acc_category->second.insert(acc_hash, hash);
    if (newEntryInMap) {
      // We are the first thread to write this hash into this category (as expected). Set the payload value
      acc_hash->second = s;
    } else {
      // Rarer, but possible that two-or-more threads are all trying to concurrently insert the same hash into the same category.
      // Getting here means that we weren't the first thread.
      // While this does mean that the other thread did the job for us, we must check once again that what this other thread inserted
      // doesn't collide with what this thread wanted to insert.
      if (s != acc_hash->second) {
        // Stored string disagrees with test string for given hash. Flag a hash collision!
        std::stringstream ss;
        ss << "Hashes the same for category: " << category
           << " and elements "<< acc_hash->second << " " << s;
        throw std::domain_error( ss.str() );
      }
    }
  }
}


HLTHash HLTUtils::string2hash( const std::string& s, const std::string& category )
{
  // hash function (based on available elsewhere ELF hash function)
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

  HLTUtils::CategoryMap_t::const_accessor cacc_category;
  const bool categoryExists = HLTUtils::s_allHashesByCategory.find(cacc_category, category);
  if (!categoryExists) {
    return "UNKNOWN CATEGORY";
  }

  HLTUtils::HashMap_t::const_accessor cacc_hash;
  const bool hashExists = cacc_category->second.find(cacc_hash, hash);
  if (!hashExists) {
    return "UNKNOWN HASH ID";
  }

  return cacc_hash->second;
}

void HLTUtils::hashes2file( const std::string& fileName) {
  std::ofstream fout(fileName);

  HLTUtils::CategoryMap_t::const_iterator cat_begin = HLTUtils::s_allHashesByCategory.begin(); 
  HLTUtils::CategoryMap_t::const_iterator cat_end = HLTUtils::s_allHashesByCategory.end();

  for (HLTUtils::CategoryMap_t::const_iterator cat_it = cat_begin; cat_it != cat_end; ++cat_it) {
    const std::string category = cat_it->first;
    fout << s_newCategory << std::endl << category << std::endl;
    HLTUtils::HashMap_t::const_iterator hash_begin = cat_it->second.begin(); 
    HLTUtils::HashMap_t::const_iterator hash_end = cat_it->second.end();
    for (HLTUtils::HashMap_t::const_iterator hash_it = hash_begin; hash_it != hash_end; ++hash_it) {
      const HLTHash hash = hash_it->first;
      std::string name = hash_it->second;
      name.erase(std::remove(name.begin(), name.end(), '\n'), name.end()); // Remove any line breaks
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
