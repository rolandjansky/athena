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

#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/PixelID.h"

#include "PixelConditionsData/PixelModuleData.h"
//#include "PixelConditionsData/PixelDeadMapCondData.h"
#include "PixelConditionsData/PixelDCSStateData.h"
#include "PixelConditionsData/PixelDCSStatusData.h"
#include "PixelConditionsData/PixelTDAQData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "PixelCabling/IPixelCablingSvc.h"

class PixelConditionsSummaryTool: public AthAlgTool, public IInDetConditionsTool{
  public:
    static InterfaceID& interfaceID();

    PixelConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~PixelConditionsSummaryTool();
    virtual StatusCode initialize() override;

    virtual bool isActive(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override final;
    virtual bool isActive(const IdentifierHash & moduleHash) const override final;
    virtual bool isActive(const IdentifierHash & moduleHash, const Identifier & elementId)  const override final;
    virtual double activeFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd)  const override final;

    virtual bool isGood(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override final;
    virtual bool isGood(const IdentifierHash & moduleHash) const override final;
    virtual bool isGood(const IdentifierHash & moduleHash, const Identifier & elementId) const override final;
    virtual double goodFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd) const override final;

    virtual bool isBSActive(const IdentifierHash & moduleHash) const override final;
    virtual bool isBSError(const IdentifierHash & moduleHash) const override final;

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
    {this, "PixelTDAQCondData", "", "Pixel TDAQ conditions key"}; //Default empty - legacy option

    SG::ReadCondHandleKey<PixelModuleData> m_condDeadMapKey
    {this, "PixelModuleData", "PixelModuleData", "Pixel deadmap conditions key"};

// NEW FOR RUN3    SG::ReadCondHandleKey<PixelDeadMapCondData> m_condDeadMapKey
// NEW FOR RUN3    {this, "PixelDeadMapCondData", "PixelDeadMapCondData", "Pixel deadmap conditions key"};

    ServiceHandle<IPixelCablingSvc> m_pixelCabling
    {this,  "PixelCablingSvc", "PixelCablingSvc", "Pixel cabling service"};

    SG::ReadHandleKey<InDetBSErrContainer>  m_BSErrContReadKey
    {this, "PixelByteStreamErrs", "PixelByteStreamErrs", "PixelByteStreamErrs container key"};

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
