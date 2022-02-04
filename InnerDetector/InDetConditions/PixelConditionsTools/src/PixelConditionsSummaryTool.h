/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "Gaudi/Property.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "AthenaKernel/SlotSpecificObj.h"

#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/PixelID.h"

#include "PixelConditionsData/PixelDeadMapCondData.h"
#include "PixelConditionsData/PixelDCSStateData.h"
#include "PixelConditionsData/PixelDCSStatusData.h"
#include "PixelConditionsData/PixelTDAQData.h"
#include "PixelConditionsData/PixelByteStreamErrors.h"
#include "PixelReadoutGeometry/IPixelReadoutManager.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

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
    virtual double activeFraction(const IdentifierHash& moduleHash, const Identifier & idStart, const Identifier & idEnd, const EventContext& ctx)  const override final;

    virtual bool isGood(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override final;
    virtual bool isGood(const IdentifierHash & moduleHash) const override final;
    virtual bool isGood(const IdentifierHash & moduleHash, const Identifier & elementId) const override final;
    virtual double goodFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd) const override final;

    virtual bool isGood(const Identifier& elementId, const InDetConditions::Hierarchy h, const EventContext& ctx) const override final;
    virtual bool isGood(const IdentifierHash& moduleHash, const EventContext& ctx) const override final;
    virtual bool isGood(const IdentifierHash& moduleHash, const Identifier& elementId, const EventContext& ctx) const override final;
    virtual double goodFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd, const EventContext& ctx) const override final;

    virtual bool hasBSError(const IdentifierHash& moduleHash) const override final;
    virtual bool hasBSError(const IdentifierHash& moduleHash, Identifier pixid) const override final;
    virtual bool hasBSError(const IdentifierHash& moduleHash, const EventContext& ctx) const override final;
    virtual bool hasBSError(const IdentifierHash& moduleHash, Identifier pixid, const EventContext& ctx) const override final;
    virtual bool hasBSError(const IdentifierHash& moduleHash, const EventContext& ctx, Identifier pixid) const override final;
    virtual uint64_t getBSErrorWord(const IdentifierHash& moduleHash, const EventContext& ctx) const override final;
    virtual uint64_t getBSErrorWord(const IdentifierHash& moduleHash, const int index, const EventContext& ctx) const override final;

    bool checkChipStatus(IdentifierHash moduleHash, Identifier pixid) const;
    bool checkChipStatus(IdentifierHash moduleHash, Identifier pixid, const EventContext& ctx) const;

  private:
    const PixelID* m_pixelID;

    std::vector<std::string> m_isActiveStatus;
    std::vector<std::string> m_isActiveStates;
    std::vector<int> m_activeState;
    std::vector<int> m_activeStatus;

    Gaudi::Property<bool> m_useByteStreamFEI4
    {this, "UseByteStreamFEI4", false, "Switch of the ByteStream error for FEI4"};

    Gaudi::Property<bool> m_useByteStreamFEI3
    {this, "UseByteStreamFEI3", false, "Switch of the ByteStream error for FEI3"};

    Gaudi::Property<bool> m_useByteStreamRD53
    {this, "UseByteStreamRD53", false, "Switch of the ByteStream error for RD53"};

    SG::ReadCondHandleKey<PixelDCSStateData> m_condDCSStateKey
    {this, "PixelDCSStateCondData", "PixelDCSStateCondData", "Pixel FSM state key"};

    SG::ReadCondHandleKey<PixelDCSStatusData> m_condDCSStatusKey
    {this, "PixelDCSStatusCondData", "PixelDCSStatusCondData", "Pixel FSM status key"};

    SG::ReadCondHandleKey<PixelTDAQData> m_condTDAQKey
    {this, "PixelTDAQCondData", "", "Pixel TDAQ conditions key"}; //Default empty - legacy option

    SG::ReadCondHandleKey<PixelDeadMapCondData> m_condDeadMapKey
    {this, "PixelDeadMapCondData", "PixelDeadMapCondData", "Pixel deadmap conditions key"};

    ServiceHandle<InDetDD::IPixelReadoutManager> m_pixelReadout
    {this, "PixelReadoutManager", "PixelReadoutManager", "Pixel readout manager" };

    SG::ReadHandleKey<IDCInDetBSErrContainer>  m_BSErrContReadKey
    {this, "PixelByteStreamErrs", "PixelByteStreamErrs", "PixelByteStreamErrs container key"};

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey
    {this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

    const uint64_t m_missingErrorInfo{std::numeric_limits<uint64_t>::max()-3000000000};

    mutable SG::SlotSpecificObj<std::mutex> m_cacheMutex ATLAS_THREAD_SAFE;

    struct IDCCacheEntry {
      EventContext::ContextEvt_t eventId = EventContext::INVALID_CONTEXT_EVT; // invalid event ID for the start
      const IDCInDetBSErrContainer_Cache* IDCCache = nullptr;

      void reset( EventContext::ContextEvt_t evtId, const IDCInDetBSErrContainer_Cache* cache) {
        eventId = evtId;
        IDCCache   = cache;
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

    /**
     * Return cache for the current event
     * If, for current slot, the cache is outdated it is retrieved from the IDC collection.
     * If the IDC is missing nullptr is returned.
     **/
    [[nodiscard]] IDCCacheEntry* getCacheEntry(const EventContext& ctx) const;

};

inline InterfaceID& PixelConditionsSummaryTool::interfaceID(){
  static InterfaceID IID_PixelConditionsSummaryTool("PixelConditionsSummaryTool", 1, 0);
  return IID_PixelConditionsSummaryTool;
}

inline bool PixelConditionsSummaryTool::checkChipStatus(IdentifierHash moduleHash, Identifier pixid, const EventContext& ctx) const {
  std::bitset<16> chipStatus(SG::ReadCondHandle<PixelDeadMapCondData>(m_condDeadMapKey, ctx)->getChipStatus(moduleHash));
  if (chipStatus.any()) {
    Identifier moduleID = m_pixelID->wafer_id(pixid);
    std::bitset<16> circ; circ.set(m_pixelReadout->getFE(pixid,moduleID));
    if ((chipStatus&circ).any()) { return false; }
  }
  return true;
}

#endif
