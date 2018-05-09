/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELSILICONCONDITIONStOOL_H
#define PIXELSILICONCONDITIONStOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"

#include "PixelConditionsData/PixelDCSCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

class PixelSiliconConditionsTool: public extends<AthAlgTool, ISiliconConditionsTool> {
  public:

    PixelSiliconConditionsTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~PixelSiliconConditionsTool() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual float temperature(const Identifier& elementId) const override;
    virtual float biasVoltage(const Identifier& elementId) const override;
    virtual float depletionVoltage(const Identifier& elementId) const override;

    virtual float temperature(const IdentifierHash& elementHash) const override;
    virtual float biasVoltage(const IdentifierHash& elementHash) const override;
    virtual float depletionVoltage(const IdentifierHash& elementHash) const override;

  private:
    const InDetDD::PixelDetectorManager* m_pixman;
    const PixelID* m_pixid;

    bool m_useDB;
    float m_defaultTemperature;
    float m_defaultBiasVoltage;
    float m_defaultDepletionVoltage;

    SG::ReadCondHandleKey<PixelDCSCondData> m_condKeyHV;
    SG::ReadCondHandleKey<PixelDCSCondData> m_condKeyTemp;

    const PixelDCSCondData* getCondDataHV() const;
    const PixelDCSCondData* getCondDataTemp() const;
};

#endif // PIXELSILICONCONDITIONStOOL_H
