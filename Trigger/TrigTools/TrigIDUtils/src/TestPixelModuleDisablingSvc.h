/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * TestPixelModuleDisablingSvc
 * another implementation of IPixelTDAQSvc useful for testing with modules disabled by the RS
 * jiri.masik@manchester.ac.uk
 */


#ifndef TEST_PIXEL_DISABLING_SVC_H
#define TEST_PIXEL_DISABLING_SVC_H


#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "PixelConditionsServices/IPixelTDAQSvc.h"

#include <set>
#include <map>
#include <string>


class StoreGateSvc;
class PixelID;
class CondAttrListCollection;


class TestPixelModuleDisablingSvc:  public AthService,
  virtual public IPixelTDAQSvc,
  virtual public IIncidentListener
{

 public:
  TestPixelModuleDisablingSvc(const std::string& name, ISvcLocator* sl);
  virtual ~TestPixelModuleDisablingSvc();

  static const InterfaceID& interfaceID();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual bool tdaq_module_enabled(const Identifier& moduleId);
  virtual bool tdaq_module_enabled(const IdentifierHash & elementHash);
  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS){return StatusCode::SUCCESS;}

 public:
  typedef SimpleProperty< std::vector<uint32_t> > Uint32ArrayProperty;

 private:

  void handle(const Incident& inc);
  void initModules();

  ServiceHandle< StoreGateSvc > m_detStore;
  const PixelID* m_pixelID;
  unsigned int print;
  bool print_disabled;
  void print_disabled_modules();
  std::set<Identifier> disabled_modules;
  std::vector <unsigned int>  m_moduleList;


};

inline const InterfaceID& TestPixelModuleDisablingSvc::interfaceID(){
  static const InterfaceID IID_TestPixelModuleDisablingSvc("TestPixelModuleDisablingSvc", 1, 0);
  return IID_TestPixelModuleDisablingSvc;
}

#endif
