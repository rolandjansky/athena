/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1RESULTBYTESTREAM_IL1TRIGGERBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_IL1TRIGGERBYTESTREAMTOOL_H

#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

/**
 * @class IL1TriggerByteStreamTool
 * @brief Interface for AlgTools converting ByteStream to xAOD RoI objects and vice versa
 **/
class IL1TriggerByteStreamTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IL1TriggerByteStreamTool, 1, 0);
  virtual ~IL1TriggerByteStreamTool() override {}

  /**
   * @brief Convert BS -> xAOD
   *
   * The implementation should create an xAOD RoI object from the raw data and record it in the event store
   * using a WriteHandle it declares.
   **/
  virtual StatusCode convertFromBS(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                                   const EventContext& eventContext) const = 0;

  /**
   * @brief Convert xAOD -> BS
   *
   * The implementation should take the xAOD RoI object from the event store using a ReadHandle it declares,
   * convert it to raw data, and fill the vrobf vector. The function is not const, as it needs to rely on
   * the internal cache to track data allocated for BS representation. The provided helpers clearCache,
   * newRodData, newRobFragment should be used to allocate memory for the BS representation.
   **/
  virtual StatusCode convertToBS(std::vector<OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment*>& vrobf,
                                 const EventContext& eventContext) = 0;

  /**
   * @brief List of IDs of ROBs which the convert methods expect in the vrobf input/output parameter
   *
   * The implementation has to hold a Gaudi::Property<vector<uint32_t>> to declare the ROB IDs it requires/provides
   * and this method has to return the value of this property. There is no easy way to declare a Gaudi::Property here
   * in the interface, so it is delegated to the implementation.
   **/
  virtual const std::vector<uint32_t> robIds() const = 0;

protected:
  /// Helper to clear the ByteStream data cache for a given event slot
  inline void clearCache(const EventContext& eventContext) {
    m_cache[eventContext.slot()].clear();
  }
  /// Allocate new array of raw ROD words for output ByteStream data
  inline uint32_t* newRodData(const EventContext& eventContext, const size_t size) {
    m_cache[eventContext.slot()].rodData.push_back(std::make_unique<uint32_t[]>(size));
    return m_cache[eventContext.slot()].rodData.back().get();
  }
  /// Allocate new ROBFragment for output ByteStream data
  template<typename ...Ts> inline OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment* newRobFragment(const EventContext& eventContext, Ts... args) {
    m_cache[eventContext.slot()].robFragments.push_back(std::make_unique<OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment>(args...));
    return m_cache[eventContext.slot()].robFragments.back().get();
  }
private:
  /**
   * @brief Cache which tracks memory allocated for ByteStream data representation in the convertToBS method.
   *
   * The raw ROD data and the ROBFragment object need to remain valid until they are packed into the output
   * full event at the very end of event processing. Due to this requirement, the cache is cleared only on the
   * next event by calling the clearCache method.
   **/
  struct Cache {
    std::vector<std::unique_ptr<uint32_t[]>> rodData;
    std::vector<std::unique_ptr<OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment>> robFragments;
    ~Cache() {clear();}
    void clear() {
      for (auto& ptr : rodData) ptr.reset();
      rodData.clear();
      for (auto& ptr : robFragments) ptr.reset();
      robFragments.clear();
    }
  };
  std::unordered_map<EventContext::ContextID_t, Cache> m_cache; // one cache per event slot
};

#endif // TRIGT1RESULTBYTESTREAM_IL1TRIGGERBYTESTREAMTOOL_H
