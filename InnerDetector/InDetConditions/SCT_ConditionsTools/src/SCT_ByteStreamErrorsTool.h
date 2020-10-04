// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamErrorsTool.h
 * header file for tool that keeps track of errors in the bytestream.
 * @author Susumu.Oda@cern.ch
 **/

#ifndef SCT_ByteStreamErrorsTool_h
#define SCT_ByteStreamErrorsTool_h

///Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "Identifier/IdContext.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

///Gaudi includes
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/EventIDBase.h"

///STL includes
#include <array>
#include <atomic>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <vector>

/** forward declarations */
class SCT_ID;

/**
 * @class SCT_ByteStreamErrorsTool
 * Tool that keeps track of modules that give rise to errors in the bytestram.
 *
 * The API offers two levels of access, 1) by identifier 2) for "all event"
 * In 1st case the information from entire event is used via the cache if IDC
 * container that keeps an information about errors for so far decoded SCT data.
 * That access pattern is identical for HLT and offline use-case. Missing data in the cache is sign of error.
 *
 * The 2nd patter is to request error information for the whole event.
 * In case of trigger these methods can in fact return information for the fragment of the detector data.
 * This depends on the IDC container that the tool is configured to read.
 *
 * There are two levels of caching involved in data access.
 * First the IDC stores actual BS error codes in the event-wide container called cache. Even if the IDC container may be different, this cache remains the same for the whole event.
 * Secondly the tool itself has a cache that is essentially needed to avoid going to the event store for the IDC container for each call.
 * Inside this local cache object derived information is stored to speed up calls like isGood.
 **/

class SCT_ByteStreamErrorsTool: public extends<AthAlgTool, ISCT_ByteStreamErrorsTool> {

public:
  //@name Tool methods
  //@{
  SCT_ByteStreamErrorsTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_ByteStreamErrorsTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //@}
  
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;
  
  ///Is the detector element good?
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const IdentifierHash& elementIdHash) const override;
  virtual bool isGood(const IdentifierHash& elementIdHash, const EventContext& ctx) const override;
  
  virtual std::set<IdentifierHash> getErrorSet(int errorType, const EventContext& ctx) const override; // Used by SCTRawDataProviderTool and others
  virtual std::set<IdentifierHash> getErrorSet(int errorType) const override; // Used by SCTRawDataProviderTool and others

  virtual unsigned int tempMaskedChips(const Identifier& moduleId, const EventContext& ctx) const override; // Internally used
  virtual unsigned int tempMaskedChips(const Identifier& moduleId) const override;
  virtual unsigned int abcdErrorChips(const Identifier& moduleId) const override; // Internally used
  virtual unsigned int abcdErrorChips(const Identifier& moduleId, const EventContext& ctx) const override; // Internally used

private:
  enum N_ELEMENTS { N_CHIPS_PER_SIDE = 6, N_SIDES = 2, N_STRIPS_PER_CHIP = 128, N_STRIPS_PER_SIDE = N_STRIPS_PER_CHIP*N_CHIPS_PER_SIDE };

  ToolHandle<ISCT_ConfigurationConditionsTool> m_config{this, "ConfigTool",
      "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};

  SG::ReadHandleKey<IDCInDetBSErrContainer> m_bsIDCErrContainerName{this, "IDCByteStreamErrContainer", "SCT_ByteStreamErrs", "SCT BS error key for IDC variant"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  const SCT_ID* m_sct_id{nullptr};
  IdContext m_cntx_sct;

  mutable std::mutex m_cacheMutex{};
  struct IDCCacheEntry {
    EventContext::ContextEvt_t eventId{EventContext::INVALID_CONTEXT_EVT}; // invalid event ID for the start
    const IDCInDetBSErrContainer_Cache* IDCCache{nullptr};
    // infomations in granularity of Chips
    // misisng value mean that the map need updating
    // 0 as the value denotes no error
    // error encoding is as follows: pattern for a module has length of 6 bits, side 0 is encoded in bits 0-5, side 1 in bits 6-11
    // so bit 0 is for chip 0 on side 0, bit 1 is for chip 1 on side 0, ..., and bit 11 is for chip 5 on side 1
    std::unordered_map<Identifier, unsigned int> tempMaskedChips;
    std::unordered_map<Identifier, unsigned int> abcdErrorChips;

    void reset(EventContext::ContextEvt_t evtId, const IDCInDetBSErrContainer_Cache* cache) {
      eventId = evtId;
      IDCCache = cache;
      tempMaskedChips.clear();
      abcdErrorChips.clear();
    }

    bool needsUpdate(const EventContext& ctx) const {
      return eventId != ctx.evt() or eventId == EventContext::INVALID_CONTEXT_EVT;
    }
    
  };
  mutable SG::SlotSpecificObj<IDCCacheEntry> m_eventCache ATLAS_THREAD_SAFE; // Guarded by m_cacheMutex

  /**
   * Obtains container form the SG, if it is missing it will complain (hard-coded 3 times per job) and return nullptr
   **/
  [[nodiscard]] const IDCInDetBSErrContainer* getContainer(const EventContext& ctx) const;
  mutable std::atomic_uint m_nRetrievalFailure{0};

  /**
   * Return cache for the current event
   * If, for current slot, the cache is outdated it is retrieved from the IDC collection.
   * If the IDC is missing nullptr is returned.
   **/
  [[nodiscard]] IDCCacheEntry* getCacheEntry(const EventContext& ctx) const;

  /**
   * Updates information per module & ABCD chip
   **/
  StatusCode fillData(const EventContext& ctx) const;

  bool isGoodChip(const Identifier& stripId, const EventContext& ctx) const;
  int getChip(const Identifier& stripId, const EventContext& ctx) const;

  // For isRODSimulatedData, HVisOn and isCondensedReadout
  const InDetDD::SiDetectorElement* getDetectorElement(const IdentifierHash& waferHash, const EventContext& ctx) const;

  /**
   * Method that returns BS Error code from the map passed @rag where-Expected
   * If the information is initially missing, the cache update is triggered
   **/
  std::pair<StatusCode, unsigned int> getErrorCodeWithCacheUpdate(const Identifier& id, const EventContext& ctx, std::unordered_map<Identifier, unsigned int>& whereExected) const;

};

#endif // SCT_ByteStreamErrorsTool_h
