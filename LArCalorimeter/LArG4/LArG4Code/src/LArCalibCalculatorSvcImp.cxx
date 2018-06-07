/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/LArCalibCalculatorSvcImp.h"

LArCalibCalculatorSvcImp::LArCalibCalculatorSvcImp(const std::string& name, ISvcLocator *pSvcLocator)
  : AthService(name, pSvcLocator)
{
}

StatusCode LArCalibCalculatorSvcImp::queryInterface( const InterfaceID & riid,  void** ppvInterface )
{
  if ( ILArCalibCalculatorSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ILArCalibCalculatorSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
