/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PixelRunConditionsDbSvc.h
 *
 * Service to get the run-conditions from COOL
 *
 * mlimper@cern.ch
 */


#ifndef PIXEL_RUNCONDITIONSDB_SVC_H
#define PIXEL_RUNCONDITIONSDB_SVC_H


#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PixelConditionsServices/IPixelRunConditionsDbSvc.h"

#include <set>
#include <map>
#include <string>


class StoreGateSvc;
class PixelID;
class CondAttrListCollection;

class PixelRunConditionsDbSvc:  public AthService,
                     virtual public IPixelRunConditionsDbSvc{

 public:
  PixelRunConditionsDbSvc(const std::string& name, ISvcLocator* sl);
  virtual ~PixelRunConditionsDbSvc();

  static const InterfaceID& interfaceID();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS);

  virtual std::string connTag(std::string& cTag) {
      if(cTag.length()!=0) ATH_MSG_WARNING( "Parameter cTag with value "<< cTag  <<" is not used" );
      return m_connTag;
  } ;

 private:
  ServiceHandle< StoreGateSvc > m_detStore;

  const PixelID* m_pixelID;

  unsigned int calback_calls;
  unsigned int print;

  std::string m_connTag ;

  std::string m_key;
};

inline const InterfaceID& PixelRunConditionsDbSvc::interfaceID(){
  static const InterfaceID IID_PixelRunConditionsDbSvc("PixelRunConditionsDbSvc", 1, 0);
  return IID_PixelRunConditionsDbSvc;
}

#endif
