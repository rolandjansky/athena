/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"

#ifndef SIMULATIONBASE
#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#endif

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/PixelID.h"

#include "PixelConditionsData/PixelModuleData.h"
//#include "PixelConditionsData/PixelDeadMapCondData.h"
#include "PixelConditionsData/PixelDCSStateData.h"
#include "PixelConditionsData/PixelDCSStatusData.h"
#include "PixelConditionsData/PixelTDAQData.h"
#include "StoreGate/ReadCondHandleKey.h"

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

//    SG::ReadCondHandleKey<PixelDeadMapCondData> m_condDeadMapKey
//    {this, "PixelDeadMapCondData", "PixelDeadMapCondData", "Pixel deadmap conditions key"};

#ifndef SIMULATIONBASE
    SG::ReadHandleKey<InDetBSErrContainer>  m_BSErrContReadKey
    {this, "PixelByteStreamErrs", "PixelByteStreamErrs", "PixelByteStreamErrs container key"};
#endif

};

inline InterfaceID& PixelConditionsSummaryTool::interfaceID(){
  static InterfaceID IID_PixelConditionsSummaryTool("PixelConditionsSummaryTool", 1, 0);
  return IID_PixelConditionsSummaryTool;
}

#endif
