/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISiPropertiesSvc.h
 * @author Grant.Gorfine@cern.ch
**/
#ifndef SiPropertiesSvc_h
#define SiPropertiesSvc_h

#include "SiPropertiesSvc/ISiPropertiesSvc.h"

//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthService.h"

//forward declarations
class IdentifierHash;
class ISiliconConditionsSvc;
class StoreGateSvc;

namespace Trk 
{
  class LocalPosition;
}

namespace InDetDD {
  class SiDetectorManager;
}  

/**
 * @class SiPropertiesSvc
 * Concrete class for service providing silicon properties (mobility, etc).
 * for each detector element. These depend on conditions such as temperature.
**/
class SiPropertiesSvc:  public AthService, virtual public ISiPropertiesSvc 
{
public:
  SiPropertiesSvc(const std::string& name, ISvcLocator* sl);
  virtual ~SiPropertiesSvc();
 
  virtual StatusCode initialize();          //!< Service init
  virtual StatusCode finalize();            //!< Service finalize
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  static const InterfaceID & interfaceID();


  /// Get properties for the detector element.
  virtual const InDet::SiliconProperties & getSiProperties(const IdentifierHash & elementHash);

  /// IOV CallBack
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS);

private:

  void updateCache(const IdentifierHash & elementHash);
  void invalidateCache();
  bool valid(const IdentifierHash & elementHash);
  
private:

  // Properties
  double m_temperatureMin;
  double m_temperatureMax;
  double m_electronSaturationVelocity;
  double m_holeSaturationVelocity;
  std::string m_detectorName;
  ServiceHandle<ISiliconConditionsSvc> m_siConditionsSvc;
  ServiceHandle<StoreGateSvc> m_detStore;

  bool m_conditionsSvcValid;

  std::vector<InDet::SiliconProperties> m_propertiesCache;
  std::vector<bool> m_cacheValid;
  const InDetDD::SiDetectorManager * m_detManager;

  std::vector<bool> m_outOfRangeWarning;

};

inline const InterfaceID & SiPropertiesSvc::interfaceID(){
  return ISiPropertiesSvc::interfaceID();
}

#endif
