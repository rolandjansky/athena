/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PixelConnCoolDbSvc.h
 *
 * Service to read/write connectivity from/to COOL
 *
 * mlimper@cern.ch
 */


#ifndef PIXEL_CONNCOOLDB_SVC_H
#define PIXEL_CONNCOOLDB_SVC_H


#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PixelCabling/IPixelConnCoolDbSvc.h"

#include <set>
#include <map>
#include <string>

class StoreGateSvc;
class PixelCablingData;
class IIOVRegistrationSvc;

class PixelConnCoolDbSvc:  public AthService,
  virtual public IPixelConnCoolDbSvc{
  
 public:
  PixelConnCoolDbSvc(const std::string& name, ISvcLocator* sl);
  virtual ~PixelConnCoolDbSvc();
  
  static const InterfaceID& interfaceID();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode record(std::string& cabling_data);
  virtual StatusCode retrieveData(std::string& dataString);

 private:
  IIOVRegistrationSvc*      m_regSvc;
  ServiceHandle< StoreGateSvc > m_detStore;
};

inline const InterfaceID& PixelConnCoolDbSvc::interfaceID(){
  static const InterfaceID IID_PixelConnCoolDbSvc("PixelConnCoolDbSvc", 1, 0);
  return IID_PixelConnCoolDbSvc;
}

#endif
