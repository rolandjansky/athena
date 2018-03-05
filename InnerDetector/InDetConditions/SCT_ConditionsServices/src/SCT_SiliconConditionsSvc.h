/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_SiliconConditionsSvc.h
 *
 * Header file for the SCT_SiliconConditionsSvc class which implements the ISiliconConditionsSvc interface
 *
 * @author Carl Gwilliam <gwilliam@mail.cern.ch>
 **/

#ifndef SCT_SiliconConditionsSvc_h
#define SCT_SiliconConditionsSvc_h

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"

#include "SCT_ConditionsData/SCT_DCSFloatCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

class IGeoModelSvc;
class IRDBAccessSvc;
class StoreGateSvc;
class SCT_ID;

/**
 * @class SCT_SiliconConditionsSvc
 * Class for conditions data about the SCT silicon
 * Allows one to obtain temperature and bias + depletion voltages
 * These are currenlty static values but in future will be obtained for the DB
 **/

class SCT_SiliconConditionsSvc: public AthService,
  virtual public ISiliconConditionsSvc {
 public:

  /** Constructor */
  SCT_SiliconConditionsSvc(const std::string& type, ISvcLocator* sl);
  /** Destructor */
  virtual ~SCT_SiliconConditionsSvc() = default;

  /** Initialise */
  virtual StatusCode initialize() override;
  /** Finalise */
  virtual StatusCode finalize() override;
  /** Service queryInterface method */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf) override;
  /** Obtain service's interface ID */
  static const InterfaceID& interfaceID();

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

  /** IOV CallBack */
  virtual StatusCode callBack(int&, std::list<std::string>&) override;
  /** Query whether a CallBack has been registered. */
  virtual bool hasCallBack() override;

 private:
 
  bool setConditionsFromGeoModel();

  float                                       m_defaultTemperature;        //!< Default temperature (non-DB)
  float                                       m_defaultBiasVoltage;        //!< Default bias voltage (non-DB)
  float                                       m_defaultDepletionVoltage;   //!< Default depletion voltage (non-DB)
  bool                                        m_useDB;                     //!< Whether to used the conditions DB or not
  bool                                        m_checkGeoModel;
  bool                                        m_forceUseGeoModel;

  ServiceHandle<StoreGateSvc>                 m_detStore;
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

inline const InterfaceID& SCT_SiliconConditionsSvc::interfaceID() {
  return ISiliconConditionsSvc::interfaceID();
}

#endif // SCT_SiliconConditionsSvc_h
