/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMAGFIELDATHENASVC
#define IMAGFIELDATHENASVC

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/IOVSvcDefs.h" 
#include "BFieldAth/MagFieldAthena.h"

static const InterfaceID IID_IMagFieldAthenaSvc(1001, 1, 0);

class IMagFieldAthenaSvc : virtual public IInterface {
  public:
  static const InterfaceID& interfaceID() { return IID_IMagFieldAthenaSvc; }
  virtual MagFieldAthena* GetMagFieldAthena() = 0;
  // retrieve through COOL callback - to be used in client callback method
  virtual MagFieldAthena* GetUpdatedMagFieldAthena() = 0;
  // check if field was really updated in callback, to be used in clients
  virtual bool WasFieldUpdated() = 0;

  virtual StatusCode initGeoModel(IOVSVC_CALLBACK_ARGS) = 0;
  virtual StatusCode updateField(IOVSVC_CALLBACK_ARGS)  = 0;

};
#endif
