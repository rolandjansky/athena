/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/LArCalculatorSvcImp.h"

#include "CLHEP/Units/SystemOfUnits.h"

LArCalculatorSvcImp::LArCalculatorSvcImp(const std::string& name, ISvcLocator *pSvcLocator)
  : base_class(name, pSvcLocator)
  , m_BirksLaw(true)
  , m_Birksk(0.05832)// value updated for G4 10.6.p03 - 1.2 times the previous value of 0.0486 used in all campaigns before MC21.
  , m_OOTcut(300*CLHEP::ns)
{
   declareProperty("BirksLaw",m_BirksLaw);
   declareProperty("Birksk",m_Birksk);
   declareProperty("OOTcut",m_OOTcut);
}
