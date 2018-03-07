/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

class IGeoModelSvc;
class IRDBAccessSvc;
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
  virtual float temperature(const Identifier& elementId) override;
  /** Silicon bias voltage by Identifier */
  virtual float biasVoltage(const Identifier& elementId) override;
  /** Silicon depletion voltage by Identifier */
  virtual float depletionVoltage(const Identifier& elementId) override;

  /** Silicon temperature by IdentifierHash */
  virtual float temperature(const IdentifierHash& elementHash) override;
  /** Silicon bias voltage by IdentifierHash */
  virtual float biasVoltage(const IdentifierHash& elementHash) override;
  /** Silicon depletion voltage by IdentifierHash */
  virtual float depletionVoltage(const IdentifierHash& elementHash) override;

 private:
 
  bool setConditionsFromGeoModel();

  float                                       m_defaultTemperature;        //!< Default temperature (non-DB)
  float                                       m_defaultBiasVoltage;        //!< Default bias voltage (non-DB)
  float                                       m_defaultDepletionVoltage;   //!< Default depletion voltage (non-DB)
  bool                                        m_useDB;                     //!< Whether to used the conditions DB or not
  bool                                        m_checkGeoModel;
  bool                                        m_forceUseGeoModel;

  ServiceHandle<IGeoModelSvc>                 m_geoModelSvc;
  ServiceHandle<IRDBAccessSvc>                m_rdbSvc;

  float                                       m_geoModelTemperature;
  float                                       m_geoModelBiasVoltage;
  float                                       m_geoModelDepletionVoltage;
  bool                                        m_useGeoModel;

  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_condKeyHV;
  SG::ReadCondHandleKey<SCT_DCSFloatCondData> m_condKeyTemp;

  const SCT_ID* m_sct_id;

  const SCT_DCSFloatCondData* getCondDataHV() const;
  const SCT_DCSFloatCondData* getCondDataTemp() const;
  };

#endif // SCT_SiliconConditionsTool_h
