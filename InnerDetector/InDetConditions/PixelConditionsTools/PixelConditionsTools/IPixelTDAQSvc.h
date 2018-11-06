/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * IPixelTDAQSvc.h
 *
 * Interface for PixelTDAQSvc
 * Service to get the status of pixel modules from TDAQ
 *
 * georg@cern.ch
 */


#ifndef I_PIXEL_TDAQ_SVC_H
#define I_PIXEL_TDAQ_SVC_H


#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"


class Identifier;
class IdentifierHash;


class IPixelTDAQSvc: virtual public IInterface{

 public:
  virtual ~IPixelTDAQSvc(){};
  static const InterfaceID& interfaceID();

  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS) = 0;
  virtual bool tdaq_module_enabled(const Identifier& moduleId) = 0;
  virtual bool tdaq_module_enabled(const IdentifierHash & elementHash) = 0;
};


inline const InterfaceID& IPixelTDAQSvc::interfaceID(){
  static const InterfaceID IID_IPixelTDAQSvc("IPixelTDAQSvc", 1, 0);
  return IID_IPixelTDAQSvc;
}

#endif
