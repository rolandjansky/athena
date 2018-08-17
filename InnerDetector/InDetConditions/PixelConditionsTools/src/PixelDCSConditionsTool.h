/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELSILICONCONDITIONStOOL_H
#define PIXELSILICONCONDITIONStOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PixelConditionsTools/IPixelDCSConditionsTool.h"
#include "PixelConditionsData/PixelDCSConditionsData.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

class PixelDCSConditionsTool: public extends<AthAlgTool, IPixelDCSConditionsTool> {
  public:

    PixelDCSConditionsTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~PixelDCSConditionsTool() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual float temperature(const Identifier& elementId) const override;
    virtual float biasVoltage(const Identifier& elementId) const override;
    virtual float depletionVoltage(const Identifier& elementId) const override;

    virtual float temperature(const IdentifierHash& elementHash) const override;
    virtual float biasVoltage(const IdentifierHash& elementHash) const override;
    virtual float depletionVoltage(const IdentifierHash& elementHash) const override;

    virtual std::string PixelFSMState(const Identifier& elementId) const override;
    virtual std::string PixelFSMState(const IdentifierHash& elementHash) const override;

    virtual std::string PixelFSMStatus(const Identifier& elementId) const override;
    virtual std::string PixelFSMStatus(const IdentifierHash& elementHash) const override;

  private:
    const PixelID* m_pixid;

    bool m_useDB;
    float m_defaultTemperature;
    float m_defaultBiasVoltage;
    float m_defaultDepletionVoltage;

    SG::ReadCondHandleKey<PixelDCSConditionsData> m_condKeyHV{this, "CondKeyHV", "PixelDCSHVCondData", "Pixel bias voltage"};
    SG::ReadCondHandleKey<PixelDCSConditionsData> m_condKeyTemp{this, "CondKeyTemp", "PixelDCSTempCondData", "Pixel temperature"};

    SG::ReadCondHandleKey<PixelDCSConditionsData> m_condKeyState{this, "CondKeyState", "PixelDCSStateCondData", "Pixel FSM state"};
    SG::ReadCondHandleKey<PixelDCSConditionsData> m_condKeyStatus{this, "CondKeyStatus", "PixelDCSStatusCondData", "Pixel FSM status"};

    const PixelDCSConditionsData* getCondDataHV() const;
    const PixelDCSConditionsData* getCondDataTemp() const;

    const PixelDCSConditionsData* getCondDataState() const;
    const PixelDCSConditionsData* getCondDataStatus() const;
};

#endif // PIXELSILICONCONDITIONStOOL_H
