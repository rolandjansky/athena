/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FlatBM.h"

FlatBM::FlatBM(const std::string& name, ISvcLocator* svc)
  : AthService(name,svc)
{
}

StatusCode FlatBM::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IBeamIntensity::interfaceID().versionMatch(riid) )
    {
      *ppvInterface = (IBeamIntensity*)this;
      addRef();
      return StatusCode::SUCCESS;
    }
  // Interface is not directly available: try out the base class
  return AthService::queryInterface(riid, ppvInterface);
}
