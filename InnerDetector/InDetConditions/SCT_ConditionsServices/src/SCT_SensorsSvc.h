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
#include <set>

//Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

//local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_SensorsSvc.h"

//forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class SCT_ID;
class StatusCode;
//class Identifier; < not fwd declared, used in a vector so need storage size


/**
 * @class SCT_ModuleVetoSvc
 * Service allowing one to manually get Vdep, crystal orientation and Mfr for a sensor(s)
**/
class SCT_SensorsSvc: virtual public ISCT_SensorsSvc, virtual public AthService{
  friend class SvcFactory<SCT_SensorsSvc>;
public:

  SCT_SensorsSvc( const std::string & name, ISvcLocator* svc);
  virtual ~SCT_SensorsSvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();

  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  virtual void getSensorsData(std::vector<std::string> & userVector);
  virtual std::string getManufacturer(unsigned int i);
  virtual void printManufacturers();

  ///Callback for fill from database
  virtual StatusCode fillSensorsData(int&  i  , std::list<std::string>& keys);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
private:
  //Declare Storegate container
  ServiceHandle<StoreGateSvc> m_detStore;
  const DataHandle<CondAttrListCollection> m_sensorsData;
  std::vector<std::string> m_sensorsValues;

  std::map<CondAttrListCollection::ChanNum, std::string> m_sensorsManufacturer;
};

inline const InterfaceID & SCT_SensorsSvc::interfaceID(){
  return ISCT_SensorsSvc::interfaceID(); 
}

#endif
