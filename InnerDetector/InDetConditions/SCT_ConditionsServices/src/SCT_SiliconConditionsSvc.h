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
#include "StoreGate/StoreGateSvc.h"
#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"
#include "SCT_ConditionsServices/ISCT_DCSConditionsSvc.h"

class ISvcLocator;
class StatusCode;
class InterfaceID; 
class IGeoModelSvc;
class IRDBAccessSvc;
class StoreGateSvc;
class ISCT_DCSConditionsSvc;
class IBLParameterSvc;

/**
 * @class SCT_SiliconConditionsSvc
 * Class for conditions data about the SCT silicon
 * Allows one to obtain temperature and bias + depletion voltages
 * These are currenlty static values but in future will be obtained for the DB
**/

class SCT_SiliconConditionsSvc: public AthService,
  virtual public ISiliconConditionsSvc{
public:

  /** Constructor */
  SCT_SiliconConditionsSvc(const std::string& type, ISvcLocator* sl);
  /** Destructor */
  virtual ~SCT_SiliconConditionsSvc();

  /** Initialise */
  virtual StatusCode initialize();
  /** Finalise */
  virtual StatusCode finalize();
  /** Service queryInterface method */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf); 
  /** Obtain service's interface ID */
  static const InterfaceID & interfaceID();

  /** Silicon temperature by Identifier */
  virtual float temperature(const Identifier& elementId);
  /** Silicon bias voltage by Identifier */
  virtual float biasVoltage(const Identifier& elementId);
  /** Silicon depletion voltage by Identifier */
  virtual float depletionVoltage(const Identifier & elementId);

  /** Silicon temperature by IdentifierHash */
  virtual float temperature(const IdentifierHash& elementHash);
  /** Silicon bias voltage by IdentifierHash */
  virtual float biasVoltage(const IdentifierHash& elementHash);
  /** Silicon depletion voltage by IdentifierHash */
  virtual float depletionVoltage(const IdentifierHash& elementHash);

  /** IOV CallBack */
  virtual StatusCode callBack(int&, std::list<std::string>&);
  /** Query whether a CallBack has been registered. */
  virtual bool hasCallBack();

private:
 
  bool setConditionsFromGeoModel();

  float                                       m_defaultTemperature;        //!< Default temperature (non-DB)
  float                                       m_defaultBiasVoltage;        //!< Default bias voltage (non-DB)
  float                                       m_defaultDepletionVoltage;   //!< Default depletion voltage (non-DB)
  bool                                        m_useDB;                     //!< Whether to used the conditions DB or not
  bool                                        m_checkGeoModel;
  bool                                        m_forceUseGeoModel;

  ServiceHandle< StoreGateSvc >               m_detStore;
  ServiceHandle<ISCT_DCSConditionsSvc >       m_sctDCSSvc;
  ServiceHandle<IGeoModelSvc>                 m_geoModelSvc;
  ServiceHandle<IRDBAccessSvc>                m_rdbSvc;

  float                                       m_geoModelTemperature;
  float                                       m_geoModelBiasVoltage;
  float                                       m_geoModelDepletionVoltage;
  bool                                        m_useGeoModel;


};

inline const InterfaceID & SCT_SiliconConditionsSvc::interfaceID(){
  return ISiliconConditionsSvc::interfaceID();
}

#endif
