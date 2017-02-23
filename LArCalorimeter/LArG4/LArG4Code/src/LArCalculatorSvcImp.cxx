/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/LArCalculatorSvcImp.h"

#include "CLHEP/Units/SystemOfUnits.h"

LArCalculatorSvcImp::LArCalculatorSvcImp(const std::string& name, ISvcLocator* pSvcLocator)
  : AthService(name, pSvcLocator)
  , m_BirksLaw(true)
  , m_Birksk(0.0486)
  , m_OOTcut(300*CLHEP::ns)
{
  declareProperty("BirksLaw", m_BirksLaw);
  declareProperty("Birksk", m_Birksk);
  declareProperty("OOTcut", m_OOTcut);
}

StatusCode LArCalculatorSvcImp::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( ILArCalculatorSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ILArCalculatorSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
