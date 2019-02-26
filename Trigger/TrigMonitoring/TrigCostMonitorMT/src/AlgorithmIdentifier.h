/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTMONITORMT_TRIGCOSTALGORITHMIDENTIFIER_H
#define TRIGCOSTMONITORMT_TRIGCOSTALGORITHMIDENTIFIER_H 1

#include "GaudiKernel/EventContext.h"

#include "TrigConfHLTData/HLTUtils.h"

#include <functional>
#include <string>

/** 
 * @class AlgorithmIdentifier
 * @brief Small structure to hold an algorithm's name and store, plus some details on its EventView.
 * The AI also keeps a pointer to the EventContext and MsgService, this is for convenience such that 
 * methods which use the AI don't need to be additionally passed these 
 */
struct AlgorithmIdentifier {
  AlgorithmIdentifier();
  AlgorithmIdentifier(const EventContext& context, const std::string& caller, const std::string& storeName,  MsgStream& msg, const int16_t viewID = -1);
  ~AlgorithmIdentifier() = default;

  TrigConf::HLTHash callerHash() const;
  TrigConf::HLTHash storeHash() const;
  StatusCode isValid() const;

  const EventContext* m_context; //!< Convenience reference to the context.
  MsgStream* m_msg;  //!< Convenience reference to the msg service.
  const std::string m_caller; //!< Name of the algorithm
  const std::string m_store; //!< Name of the algorithm's store. e.g. '0_StoreGateSvc_Impl' for the main store.
  const int16_t m_viewID; //!< If not within an event view, then the m_iewID = s_noView = -1.
  const size_t m_hash; //!< Hash of algorithm + store. Event-unique quantity. Cached for speed.

  static constexpr int16_t s_noView = -1; //!< Constant value used to express an Algorithm which is not running in a View
};

/** 
 * @class AlgorithmIdentifierMaker
 * @brief Simple factory class to construct an AlgorithmIdentifier from an algorithm name and context
 */
struct AlgorithmIdentifierMaker {
  /** 
   * @brief Construct an AlgorithmIdentifier
   * @param[in] context The event context
   * @param[in] caller The name of the algorithm
   * @param[in] msg a reference to the msgstream for printing
   * @returns Constructed AlgorithmIdentifier on the stack
   */
  static AlgorithmIdentifier make(const EventContext& context, const std::string& caller, MsgStream& msg);
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
  static size_t hash(const AlgorithmIdentifier& ai);

  /** 
   * @brief Equality function for AlgorithmIdentifier to check for uniqueness within hash bucket
   * @param[in] x First AlgorithmIdentifier to check equality of
   * @param[in] y Second AlgorithmIdentifier to check equality of
   * @returns True if both the algorithm name and algorithm store strings are identical
   */
  static bool equal(const AlgorithmIdentifier& x, const AlgorithmIdentifier& y);
};

inline size_t AlgorithmIdentifierHashCompare::hash(const AlgorithmIdentifier& ai) {
  return ai.m_hash;
}

inline bool AlgorithmIdentifierHashCompare::equal(const AlgorithmIdentifier& x, const AlgorithmIdentifier& y) {
  return (x.m_caller == y.m_caller && x.m_store == y.m_store);
}

#endif // TRIGCOSTMONITORMT_TRIGCOSTALGORITHMIDENTIFIER_H
