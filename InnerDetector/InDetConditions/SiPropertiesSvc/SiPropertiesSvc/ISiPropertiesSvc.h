/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISiPropertiesSvc.h
 * @author Grant.Gorfine@cern.ch
**/
#ifndef ISiPropertiesSvc_h
#define ISiPropertiesSvc_h
//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "SiPropertiesSvc/SiliconProperties.h"

//forward declarations
class IdentifierHash;

namespace Trk 
{
  class LocalPosition;
}



/**
 * @class ISiPropertiesSvc
 * Interface class for service providing silicon properties (mobility, etc).
 * for each detector element. These depend on conditions such as temperature.
**/
class ISiPropertiesSvc: virtual public IInterface
{
public:
  virtual ~ISiPropertiesSvc(){}

  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface

  /// Get Silicon properties for a given detector element.
  virtual const InDet::SiliconProperties & getSiProperties(const IdentifierHash & elementHash) = 0;

  /// IOV CallBack
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS) = 0;

};

inline const InterfaceID & ISiPropertiesSvc::interfaceID(){
  static const InterfaceID IID_ISiPropertiesSvc("ISiPropertiesSvc",1,0);
  return IID_ISiPropertiesSvc;
}
#endif
