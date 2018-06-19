/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISiPropertiesSvc.h
 * @author Grant.Gorfine@cern.ch
**/
#ifndef SiPropertiesCHSvc_h
#define SiPropertiesCHSvc_h

#include "SiPropertiesSvc/ISiPropertiesSvc.h"

//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthService.h"
#include "SiPropertiesSvc/SiliconPropertiesVector.h"
#include "StoreGate/ReadCondHandleKey.h"

//forward declarations
class IdentifierHash;
class ISiliconConditionsSvc;
class StoreGateSvc;

namespace Trk 
{
  class LocalPosition;
}

/**
 * @class SiPropertiesCHSvc
 * Concrete class for service providing silicon properties (mobility, etc).
 * for each detector element. These depend on conditions such as temperature.
**/
class SiPropertiesCHSvc:  public AthService, virtual public ISiPropertiesSvc 
{
public:
  SiPropertiesCHSvc(const std::string& name, ISvcLocator* sl);
  virtual ~SiPropertiesCHSvc();
 
  virtual StatusCode initialize();          //!< Service init
  virtual StatusCode finalize();            //!< Service finalize
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  static const InterfaceID& interfaceID();

  /// Get properties for the detector element.
  virtual const InDet::SiliconProperties& getSiProperties(const IdentifierHash& elementHash);

  /// IOV CallBack
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS);

private:
  // Properties
  std::string m_detectorName;
  SG::ReadCondHandleKey<InDet::SiliconPropertiesVector> m_propertiesVector;

  InDet::SiliconProperties m_defaultProperties;
};

inline const InterfaceID& SiPropertiesCHSvc::interfaceID(){
  return ISiPropertiesSvc::interfaceID();
}

#endif // SiPropertiesCHSvc_h
