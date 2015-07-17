/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * IPixelReadoutSpeedDb.h
 *
 * Interface for PixelReadoutSpeedDb
 * Service to read the run-conditions from COOL 
 * (writing to COOL is done from the on-line data-taking partition only!)
 *
 * mlimper@cern.ch
 */


#ifndef I_PIXEL_READOUTSPEEDDB_SVC_H
#define I_PIXEL_READOUTSPEEDDB_SVC_H


#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include <string>

class Identifier;
class IdentifierHash;


class IPixelReadoutSpeedDbSvc: virtual public IInterface{

 public:
  virtual ~IPixelReadoutSpeedDbSvc(){};
  static const InterfaceID& interfaceID();

  virtual std::string readoutspeed_per_ROD() = 0;

  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS) = 0;

};


inline const InterfaceID& IPixelReadoutSpeedDbSvc::interfaceID(){
  static const InterfaceID IID_IPixelReadoutSpeedDbSvc("IPixelReadoutSpeedDbSvc", 1, 0);
  return IID_IPixelReadoutSpeedDbSvc;
}

#endif
