/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1RESULTBYTESTREAM_IL1TRIGGERBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_IL1TRIGGERBYTESTREAMTOOL_H

#include "AthenaKernel/SlotSpecificObj.h"
#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "eformat/Status.h"

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
   *
   * The caller should set LVL1 ID and TriggerType in all ROBs created by this function after it returns,
   * because it already has a handle on the FullEventFragment (RawEvent). The LVL1 ID and TriggerType set
   * for the ROBs inside this function should not matter.
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
  virtual const std::vector<uint32_t>& robIds() const = 0;

protected:
  /// Helper to clear the ByteStream data cache for a given event slot
  inline void clearCache(const EventContext& eventContext) {
    m_cache.get(eventContext)->clear();
  }
  /// Allocate new array of raw ROD words for output ByteStream data
  inline uint32_t* newRodData(const EventContext& eventContext, const size_t size) {
    Cache* cache = m_cache.get(eventContext);
    cache->rodData.push_back(std::make_unique<uint32_t[]>(size));
    return cache->rodData.back().get();
  }
  /// Allocate new ROBFragment for output ByteStream data
  inline OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment* newRobFragment(
    const EventContext& eventContext,
    uint32_t source_id,
    uint32_t ndata,
    const uint32_t* data,
    uint32_t detev_type = 0,
    uint32_t status_position = eformat::STATUS_BACK) {

    Cache* cache = m_cache.get(eventContext);
    const EventIDBase& eid = eventContext.eventID();
    cache->robFragments.push_back(std::make_unique<OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment>(
      source_id,
      eid.run_number(),
      0, // lvl1_id will be overwritten downstream from full event fragment
      eid.bunch_crossing_id(),
      0, // lvl1_type will be overwritten downstream from full event fragment
      detev_type,
      ndata,
      data,
      status_position
    ));
    return cache->robFragments.back().get();
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
  SG::SlotSpecificObj<Cache> m_cache; // one cache per event slot
};

#endif // TRIGT1RESULTBYTESTREAM_IL1TRIGGERBYTESTREAMTOOL_H
