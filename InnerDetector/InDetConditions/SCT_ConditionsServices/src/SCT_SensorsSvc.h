/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_SensorsSvc.h
 * header file for service allowing one to get Vdep, crystal orientation and Mfr for sensors from a modules
 * @author shaun.roe@cern.ch
 **/

#ifndef SCT_SensorsSvc_h
#define SCT_SensorsSvc_h

//STL includes
#include <string>
#include <vector>
#include <mutex>

//Interface include
#include "SCT_ConditionsServices/ISCT_SensorsSvc.h"

//Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

//Athena includes
#include "AthenaBaseComps/AthService.h"
#include "SCT_ConditionsData/SCT_SensorsCondData.h"

// Read Handle Key
#include "StoreGate/ReadCondHandleKey.h"

//forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class StatusCode;

/**
 * @class SCT_ModuleVetoSvc
 * Service allowing one to manually get Vdep, crystal orientation and Mfr for a sensor(s)
 **/
class SCT_SensorsSvc: virtual public ISCT_SensorsSvc, virtual public AthService {
  friend class SvcFactory<SCT_SensorsSvc>;
 public:

  SCT_SensorsSvc(const std::string & name, ISvcLocator* svc);
  virtual ~SCT_SensorsSvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  static const InterfaceID& interfaceID();
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  virtual void getSensorsData(std::vector<std::string>& userVector);
  virtual const SCT_SensorCondData* getSensorsData(const unsigned int truncatedSerialNumber) const;
  virtual std::string getManufacturer(unsigned int truncatedSerialNumber);
  virtual void printManufacturers();

  ///Callback for fill from database
  virtual StatusCode fillSensorsData(int& i, std::list<std::string>& keys);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
 private:
  std::vector<std::string> m_sensorsValues;

  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cache;
  // Pointer of SCT_TdaqEnabledCondData
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_SensorsCondData> m_condData;
  // ReadCondHandleKey
  SG::ReadCondHandleKey<SCT_SensorsCondData> m_condKey;
  // Provides SCT_SensorsCondData pointer
  const SCT_SensorsCondData* getCondData(const EventContext& ctx) const;
};

inline const InterfaceID& SCT_SensorsSvc::interfaceID() {
  return ISCT_SensorsSvc::interfaceID();
}

#endif // SCT_SensorsSvc_h
