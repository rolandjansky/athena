/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PixelReadoutSpeedDbSvc.h
 *
 * Service to get the readoutspeed per ROD from COOL
 *
 * mlimper@cern.ch
 */


#ifndef PIXEL_READOUTSPEEDDB_SVC_H
#define PIXEL_READOUTSPEEDDB_SVC_H


#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PixelCabling/IPixelReadoutSpeedDbSvc.h"

#include <set>
#include <map>
#include <string>


class StoreGateSvc;
class PixelID;
class CondAttrListCollection;

class PixelReadoutSpeedDbSvc:  public AthService,
                     virtual public IPixelReadoutSpeedDbSvc{

 public:
  PixelReadoutSpeedDbSvc(const std::string& name, ISvcLocator* sl);
  virtual ~PixelReadoutSpeedDbSvc();

  static const InterfaceID& interfaceID();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS);

  virtual std::string readoutspeed_per_ROD() {return m_dataString; } ;

 private:
  ServiceHandle< StoreGateSvc > m_detStore;

  const PixelID* m_pixelID;

  unsigned int calback_calls;
  unsigned int print;

  std::string m_dataString ;

  std::string m_key;
};

inline const InterfaceID& PixelReadoutSpeedDbSvc::interfaceID(){
  static const InterfaceID IID_PixelReadoutSpeedDbSvc("PixelReadoutSpeedDbSvc", 1, 0);
  return IID_PixelReadoutSpeedDbSvc;
}

#endif
