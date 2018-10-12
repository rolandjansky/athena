/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef I_PIXEL_DCS_SVC_H
#define I_PIXEL_DCS_SVC_H

// Includes for Gaudi 
#include "GaudiKernel/IInterface.h" 
#include "AthenaKernel/IOVSvcDefs.h" 

class Identifier;
class IdentifierHash;

class IPixelDCSSvc: virtual public IInterface{

 public:

  virtual ~IPixelDCSSvc(){};

  static const InterfaceID& interfaceID();

  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS) = 0; 

  virtual double getTemperature(const Identifier& module_id) const = 0;
  virtual double getHV(const Identifier& module_id) const = 0;
  virtual std::string getFSMStatus(const Identifier& module_id) const = 0;
  virtual std::string getFSMState(const Identifier& module_id) const = 0;

  virtual double getTemperature(const IdentifierHash& id_hash) const = 0;
  virtual double getHV(const IdentifierHash& id_hash) const = 0;
  virtual std::string getFSMStatus(const IdentifierHash& id_hash) const = 0;
  virtual std::string getFSMState(const IdentifierHash& id_hash) const = 0;

  virtual StatusCode printData() const = 0;

}; 

inline const InterfaceID& IPixelDCSSvc::interfaceID(){
  static const InterfaceID IID_IPixelDCSSvc("IPixelDCSSvc", 1, 0); 
  return IID_IPixelDCSSvc;
}


#endif // I_PIXEL_DCS_SVC_H
