/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_IRPC_TriggerSvc_test_H
#define MUONCONDSVC_IRPC_TriggerSvc_test_H
// ICoolStrFileSvc.h - interact with COOL conditions database for simple
// XML file/string data


#include <string>
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"

class StatusCode;

class IRPC_TriggerSvc_test : virtual public IInterface {

 public:
  static const InterfaceID& interfaceID();

  // retrieve data from folder and channel as a string

  virtual StatusCode loadParameterInfo(IOVSVC_CALLBACK_ARGS)= 0;



};
inline const InterfaceID& IRPC_TriggerSvc_test::interfaceID() {
  static const InterfaceID IID_IRPC_TriggerSvc_test("IRPC_TriggerSvc_test",1,0);
  return IID_IRPC_TriggerSvc_test;
}





#endif
