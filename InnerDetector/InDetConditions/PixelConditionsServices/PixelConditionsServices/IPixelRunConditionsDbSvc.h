/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * IPixelRunConditionsDb.h
 *
 * Interface for PixelRunConditionsDb
 * Service to read the run-conditions from COOL 
 * (writing to COOL is done from the on-line data-taking partition only!)
 *
 * mlimper@cern.ch
 */


#ifndef I_PIXEL_RUNCONDITIONSDB_SVC_H
#define I_PIXEL_RUNCONDITIONSDB_SVC_H


#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include <string>

class Identifier;
class IdentifierHash;


class IPixelRunConditionsDbSvc: virtual public IInterface{

 public:
  virtual ~IPixelRunConditionsDbSvc(){};
  static const InterfaceID& interfaceID();

  virtual std::string connTag(std::string& cTag) = 0;

  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS) = 0;

};


inline const InterfaceID& IPixelRunConditionsDbSvc::interfaceID(){
  static const InterfaceID IID_IPixelRunConditionsDbSvc("IPixelRunConditionsDbSvc", 1, 0);
  return IID_IPixelRunConditionsDbSvc;
}

#endif
