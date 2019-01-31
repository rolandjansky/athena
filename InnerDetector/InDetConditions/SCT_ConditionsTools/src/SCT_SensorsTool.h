/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_SensorsTool.h
 * header file for tool allowing one to get Vdep, crystal orientation and Mfr for sensors from a modules
 * @author shaun.roe@cern.ch
 **/

#ifndef SCT_SensorsTool_h
#define SCT_SensorsTool_h

//STL includes
#include <vector>
#include <mutex>

//Interface include
#include "SCT_ConditionsTools/ISCT_SensorsTool.h"

//Gaudi includes
#include "GaudiKernel/ContextSpecificPtr.h"

//Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsData/SCT_SensorsCondData.h"

// Read Handle Key
#include "StoreGate/ReadCondHandleKey.h"

/**
 * @class SCT_SensorsTool
 * Tool allowing one to manually get Vdep, crystal orientation and Mfr for a sensor(s)
 **/
class SCT_SensorsTool: public extends<AthAlgTool, ISCT_SensorsTool> {
 public:

  SCT_SensorsTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_SensorsTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  virtual void getSensorsData(std::vector<std::string>& userVector, const EventContext& ctx) const override;
  virtual void getSensorsData(std::vector<std::string>& userVector) const override;
  virtual const SCT_SensorCondData* getSensorsData(const unsigned int truncatedSerialNumber, const EventContext& ctx) const override;
  virtual const SCT_SensorCondData* getSensorsData(const unsigned int truncatedSerialNumber) const override;
  virtual std::string getManufacturer(unsigned int truncatedSerialNumber, const EventContext& ctx) const override;
  virtual std::string getManufacturer(unsigned int truncatedSerialNumber) const override;
  virtual void printManufacturers(const EventContext& ctx) const override;
  virtual void printManufacturers() const override;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
 private:
  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cache;
  // Pointer of SCT_SensorsCondData
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_SensorsCondData> m_condData;
  // ReadCondHandleKey
  SG::ReadCondHandleKey<SCT_SensorsCondData> m_condKey{this, "CondKey", "SCT_SensorsCondData", "SCT sensor conditions"};
  // Provides SCT_SensorsCondData pointer
  const SCT_SensorsCondData* getCondData(const EventContext& ctx) const;
};

#endif // SCT_SensorsTool_h
