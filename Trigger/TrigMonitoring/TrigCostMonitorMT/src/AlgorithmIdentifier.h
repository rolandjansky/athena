/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTMONITORMT_TRIGCOSTALGORITHMIDENTIFIER_H
#define TRIGCOSTMONITORMT_TRIGCOSTALGORITHMIDENTIFIER_H 1

#include <functional>
#include <string>

/** 
 * @class AlgorithmIdentifier
 * @brief Small structure to hold an algorithm's name and store, plus some details on its EventView.
 */
struct AlgorithmIdentifier {
  AlgorithmIdentifier(const std::string& caller, const std::string& storeName, const int16_t viewID = -1) :
    m_caller(caller), 
    m_store(storeName),
    m_viewID(viewID),
    m_hash(std::hash<std::string>{}(m_caller + m_store))
    {}

  const std::string m_caller; //!< Name of the algorithm
  const std::string m_store; //!< Name of the algorithm's store. '0_StoreGateSvc_Impl' for main store.
  const int16_t m_viewID; //!< If within an event view, then the ViewID. -1 if the main store.
  const size_t m_hash; //!< Hash of algorithm + store. Event-unique quantity.
};

/** 
 * @class AlgorithmIdentifierHashCompare
 * @brief Static hash and equal members as required by tbb::concurrent_hash_map
 */
struct AlgorithmIdentifierHashCompare {
  /** 
   * @brief Hash function for AlgorithmIdentifier to assign bucket in hash map
   * @param[in] ai AlgorithmIdentifier to calculate hash for
   * @returns Cached std::hash<std::string> of algorithm name combined with algorithm store
   */
  static size_t hash(const AlgorithmIdentifier& ai) {
    return ai.m_hash;
  }

  /** 
   * @brief Equality function for AlgorithmIdentifier to check for uniqueness within hash bucket
   * @param[in] x First AlgorithmIdentifier to check equality of
   * @param[in] y Second AlgorithmIdentifier to check equality of
   * @returns True if both the algorithm name and algorithm store strings are identical
   */
  static bool equal(const AlgorithmIdentifier& x, const AlgorithmIdentifier& y) {
    return (x.m_caller == y.m_caller && x.m_store == y.m_store);
  }
};

#endif // TRIGCOSTMONITORMT_TRIGCOSTALGORITHMIDENTIFIER_H
