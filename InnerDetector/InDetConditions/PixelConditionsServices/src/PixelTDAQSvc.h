/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PixelTDAQSvc.h
 *
 * Service to get the status of pixel modules from TDAQ
 *
 * georg@cern.ch
 */


#ifndef PIXEL_TDAQ_SVC_H
#define PIXEL_TDAQ_SVC_H


#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PixelConditionsServices/IPixelTDAQSvc.h"
#include "PixelCabling/IPixelCablingSvc.h"
//#include "PixelCablingSvc.h"

#include <set>
#include <map>
#include <string>


class StoreGateSvc;
class PixelID;
class CondAttrListCollection;


class PixelTDAQSvc:  public AthService,
                     virtual public IPixelTDAQSvc{

 public:
  PixelTDAQSvc(const std::string& name, ISvcLocator* sl);
  virtual ~PixelTDAQSvc();

  static const InterfaceID& interfaceID();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS);
  virtual bool tdaq_module_enabled(const Identifier& moduleId);
  virtual bool tdaq_module_enabled(const IdentifierHash & elementHash);


 private:
  ServiceHandle< StoreGateSvc > m_detStore;
  ServiceHandle< IPixelCablingSvc > m_pc;

  const PixelID* m_pixelID;

  unsigned int calback_calls;
  unsigned int print;

  std::string m_modulelistkey;
  const CondAttrListCollection* m_atrlistcol;

  std::set<Identifier> all_modules;
  std::set<Identifier> disabled_modules;
  std::set<std::string> disabled_modules_first_lumiblock;
  std::set<std::string> disabled_modules_last_lumiblock;
  bool first_lumiblock_filled;

  bool print_disabled;
  void print_disabled_modules();

  std::string always_disabled_module_name;//("L2_B11_S1_A6_M4A");
  static const int always_disabled_module_offlineid = 52903936;
};

inline const InterfaceID& PixelTDAQSvc::interfaceID(){
  static const InterfaceID IID_PixelTDAQSvc("PixelTDAQSvc", 1, 0);
  return IID_PixelTDAQSvc;
}

#endif
