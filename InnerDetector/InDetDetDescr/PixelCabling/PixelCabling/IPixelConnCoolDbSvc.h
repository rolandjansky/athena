/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * IPixelConnCoolDbSvc.h
 *
 * Interface for IPixelConnCoolDbSvc
 * Service to read/write connectivity from/to COOL
 *
 * mlimper@cern.ch
 */


#ifndef I_PIXEL_CONNCOOLDB_SVC_H
#define I_PIXEL_CONNCOOLDB_SVC_H


#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"


class Identifier;
class IdentifierHash;
class PixelCablingData;

class IPixelConnCoolDbSvc: virtual public IInterface{

 public:
  virtual ~IPixelConnCoolDbSvc(){};
  static const InterfaceID& interfaceID();

  virtual StatusCode record(std::string& cabling_data) = 0;
  virtual StatusCode retrieveData(std::string& dataString) = 0;
};


inline const InterfaceID& IPixelConnCoolDbSvc::interfaceID(){
  static const InterfaceID IID_IPixelConnCoolDbSvc("IPixelConnCoolDbSvc", 1, 0);
  return IID_IPixelConnCoolDbSvc;
}

#endif
