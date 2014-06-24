/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_ITemperatureSvc_H
#define MUONCONDSVC_ITemperatureSvc_H


#include <string>
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"

class StatusCode;

class ITemperatureSvc : virtual public IInterface {

 public:
  static const InterfaceID& interfaceID();

  // retrieve data from folder and channel as a string

  virtual StatusCode loadTemperature(IOVSVC_CALLBACK_ARGS)= 0;



};
inline const InterfaceID& ITemperatureSvc::interfaceID() {
  static const InterfaceID IID_ITemperatureSvc("ITemperatureSvc",1,0);
  return IID_ITemperatureSvc;
}





#endif
