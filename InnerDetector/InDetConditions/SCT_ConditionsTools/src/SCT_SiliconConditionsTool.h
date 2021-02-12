// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_SiliconConditionsTool.h
 *
 * Header file for the SCT_SiliconConditionsTool class which implements the ISiliconConditionsTool interface
 *
 * @author Carl Gwilliam <gwilliam@mail.cern.ch>
 **/

#ifndef SCT_SiliconConditionsTool_h
#define SCT_SiliconConditionsTool_h

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"

#include "SCT_ConditionsData/SCT_DCSFloatCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class SCT_ID;

/**
 * @class SCT_SiliconConditionsTool
 * Class for conditions data about the SCT silicon
 * Allows one to obtain temperature and bias + depletion voltages
 * These are currenlty static values but in future will be obtained for the DB
 **/

class SCT_SiliconConditionsTool: public extends<AthAlgTool, ISiliconConditionsTool> {
 public:

  /** Constructor */
  SCT_SiliconConditionsTool(const std::string& type, const std::string& name, const IInterface* parent);
  /** Destructor */
  virtual ~SCT_SiliconConditionsTool() = default;

  /** Initialise */
  virtual StatusCode initialize() override;
  /** Finalise */
  virtual StatusCode finalize() override;

  /** Silicon temperature by Identifier */
  virtual float temperature(const Identifier& elementId, const EventContext& ctx) const override;
  /** Silicon bias voltage by Identifier */
  virtual float biasVoltage(const Identifier& elementId, const EventContext& ctx) const override;
  /** Silicon depletion voltage by Identifier */
  virtual float depletionVoltage(const Identifier& elementId, const EventContext& ctx) const override;

  /** Silicon temperature by IdentifierHash */
  virtual float temperature(const IdentifierHash& elementHash, const EventContext& ctx) const override;
  /** Silicon bias voltage by IdentifierHash */
  virtual float biasVoltage(const IdentifierHash& elementHash, const EventContext& ctx) const override;
  /** Silicon depletion voltage by IdentifierHash */
  virtual float depletionVoltage(const IdentifierHash& elementHash, const EventContext& ctx) const override;

 private:
 
  bool setConditionsFromGeoModel();

  FloatProperty m_defaultTemperature{this, "Temperature", 10., "Default temperature (non-DB)"};
  FloatProperty m_defaultBiasVoltage{this, "BiasVoltage", 150., "Default bias voltage (non-DB)"};
  FloatProperty m_defaultDepletionVoltage{this, "DepletionVoltage", 70., "Default depletion voltage (non-DB)"};
  BooleanProperty m_useDB{this, "UseDB", true, "Whether to used the conditions DB or not"};
  BooleanProperty m_checkGeoModel{this, "CheckGeoModel", true};
  BooleanProperty m_forceUseGeoModel{this, "ForceUseGeoModel", false};

  ServiceHandle<IGeoModelSvc> m_geoModelSvc{this, "GeoModelSvc", "GeoModelSvc"};
  ServiceHandle<IRDBAccessSvc> m_rdbSvc{this, "RDBAccessSvc", "RDBAccessSvc"};

  float m_geoModelTemperature;
  float m_geoModelBiasVoltage;
  float m_geoModelDepletionVoltage;
  bool m_useGeoModel{false};

  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_condKeyHV{this, "CondKeyHV", "SCT_SiliconBiasVoltCondData", "SCT silicon bias voltage"};
  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_condKeyTemp{this, "CondKeyTemp", "SCT_SiliconTempCondData", "SCT silicon temperature"};

  const SCT_ID* m_sct_id{nullptr};

  const SCT_DCSFloatCondData* getCondDataHV(const EventContext& ctx) const;
  const SCT_DCSFloatCondData* getCondDataTemp(const EventContext& ctx) const;
  };

#endif // SCT_SiliconConditionsTool_h
