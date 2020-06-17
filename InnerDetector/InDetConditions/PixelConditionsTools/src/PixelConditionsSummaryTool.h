/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsTools/PixelConditionsSummaryTool.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Return pixel module/FE status.
 */

#ifndef PIXELCONDITIONSSERVICES_PIXELCONDITIONSSUMMARYTOOL_H
#define PIXELCONDITIONSSERVICES_PIXELCONDITIONSSUMMARYTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "AthenaKernel/SlotSpecificObj.h"

// STSTST #include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/PixelID.h"

#include "PixelConditionsData/PixelModuleData.h"
//#include "PixelConditionsData/PixelDeadMapCondData.h"
#include "PixelConditionsData/PixelDCSStateData.h"
#include "PixelConditionsData/PixelDCSStatusData.h"
#include "PixelConditionsData/PixelTDAQData.h"
#include "PixelConditionsData/PixelByteStreamErrors.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "PixelCabling/IPixelCablingSvc.h"

class PixelConditionsSummaryTool: public AthAlgTool, public IInDetConditionsTool{
  public:
    static InterfaceID& interfaceID();

    PixelConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~PixelConditionsSummaryTool();
    virtual StatusCode initialize() override;

    virtual bool isActive(const Identifier& elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override final;
    virtual bool isActive(const IdentifierHash& moduleHash) const override final;
    virtual bool isActive(const IdentifierHash& moduleHash, const Identifier& elementId)  const override final;
    virtual double activeFraction(const IdentifierHash& moduleHash, const Identifier & idStart, const Identifier & idEnd)  const override final;

    virtual bool isActive(const Identifier& elementId, const InDetConditions::Hierarchy h, const EventContext& ctx) const override final;
    virtual bool isActive(const IdentifierHash& moduleHash, const EventContext& ctx) const override final;
    virtual bool isActive(const IdentifierHash& moduleHash, const Identifier& elementId, const EventContext& ctx)  const override final;

    virtual bool isGood(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override final;
    virtual bool isGood(const IdentifierHash & moduleHash) const override final;
    virtual bool isGood(const IdentifierHash & moduleHash, const Identifier & elementId) const override final;
    virtual double goodFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd) const override final;

    virtual bool isGood(const Identifier& elementId, const InDetConditions::Hierarchy h, const EventContext& ctx) const override final;
    virtual bool isGood(const IdentifierHash& moduleHash, const EventContext& ctx) const override final;
    virtual bool isGood(const IdentifierHash& moduleHash, const Identifier& elementId, const EventContext& ctx) const override final;

    virtual bool isBSActive(const IdentifierHash& moduleHash, const EventContext& ctx) const override final;
    virtual bool hasBSError(const IdentifierHash& moduleHash, const EventContext& ctx) const override final;
    virtual uint64_t getBSErrorWord(const IdentifierHash& moduleHash, const EventContext& ctx) const override final;

    bool checkChipStatus(IdentifierHash moduleHash, Identifier pixid) const;

  private:
    const PixelID* m_pixelID;

    std::vector<std::string> m_isActiveStatus;
    std::vector<std::string> m_isActiveStates;
    std::vector<int> m_activeState;
    std::vector<int> m_activeStatus;

    bool m_useByteStream;

    SG::ReadCondHandleKey<PixelDCSStateData> m_condDCSStateKey
    {this, "PixelDCSStateCondData", "PixelDCSStateCondData", "Pixel FSM state key"};

    SG::ReadCondHandleKey<PixelDCSStatusData> m_condDCSStatusKey
    {this, "PixelDCSStatusCondData", "PixelDCSStatusCondData", "Pixel FSM status key"};

    SG::ReadCondHandleKey<PixelTDAQData> m_condTDAQKey
    {this, "PixelTDAQCondData", "PixelTDAQCondData", "Pixel TDAQ conditions key"};

    SG::ReadCondHandleKey<PixelModuleData> m_condDeadMapKey
    {this, "PixelModuleData", "PixelModuleData", "Pixel deadmap conditions key"};

// NEW FOR RUN3    SG::ReadCondHandleKey<PixelDeadMapCondData> m_condDeadMapKey
// NEW FOR RUN3    {this, "PixelDeadMapCondData", "PixelDeadMapCondData", "Pixel deadmap conditions key"};

    ServiceHandle<IPixelCablingSvc> m_pixelCabling
    {this,  "PixelCablingSvc", "PixelCablingSvc", "Pixel cabling service"};

// STSTST    SG::ReadHandleKey<InDetBSErrContainer>  m_BSErrContReadKey
// STSTST    {this, "PixelByteStreamErrs", "PixelByteStreamErrs", "PixelByteStreamErrs container key"};

    SG::ReadHandleKey<IDCInDetBSErrContainer>  m_BSErrContReadKey
    {this, "PixelByteStreamErrs", "PixBSErr", "PixelByteStreamErrs container key"};


    mutable std::mutex m_cacheMutex{};

    struct IDCCacheEntry {
      EventContext::ContextEvt_t eventId = EventContext::INVALID_CONTEXT_EVT; // invalid event ID for the start
      const IDCInDetBSErrContainer_Cache* IDCCache = nullptr;
      // infomations in granularity of Chips
      // misisng value mean that the map need updating
      // 0 as the value denotes no error
      std::map<Identifier, unsigned int> tempMaskedChips;
      std::map<Identifier, unsigned int> abcdErrorChips;

      void reset( EventContext::ContextEvt_t evtId, const IDCInDetBSErrContainer_Cache* cache) {
        eventId = evtId;
        IDCCache   = cache;
        tempMaskedChips.clear();
        abcdErrorChips.clear();
      }

      bool needsUpdate( const EventContext& ctx) const {
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
//  StatusCode fillData(const EventContext& ctx) const;

//  bool isGoodChip(const Identifier& stripId, const EventContext& ctx) const;
//  int getChip(const Identifier& stripId, const EventContext& ctx) const;

  // For isRODSimulatedData, HVisOn and isCondensedReadout
//  const InDetDD::SiDetectorElement* getDetectorElement(const IdentifierHash& waferHash, const EventContext& ctx) const;

  /**
   * Method that returns BS Error code from the map passed @rag where-Expected
   * If the information is initially missing, the cache update is triggered
   **/
//  std::pair<StatusCode, unsigned int> getErrorCodeWithCacheUpdate( const Identifier& id, const EventContext& ctx, std::map<Identifier, unsigned int>& whereExected ) const;
















};

inline InterfaceID& PixelConditionsSummaryTool::interfaceID(){
  static InterfaceID IID_PixelConditionsSummaryTool("PixelConditionsSummaryTool", 1, 0);
  return IID_PixelConditionsSummaryTool;
}

inline bool PixelConditionsSummaryTool::checkChipStatus(IdentifierHash moduleHash, Identifier pixid) const {
  std::bitset<16> chipStatus(SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey)->getChipStatus(moduleHash));
  if (chipStatus.any()) {
    Identifier moduleID = m_pixelID->wafer_id(pixid);
    std::bitset<16> circ; circ.set(m_pixelCabling->getFE(&pixid,moduleID));
    if ((chipStatus&circ).any()) { return false; }
  }
  return true;
}

#endif
