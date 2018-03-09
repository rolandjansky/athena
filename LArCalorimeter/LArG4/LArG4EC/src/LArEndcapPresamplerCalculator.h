/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArEndcapPresamplerCalculator.hh
// Prepared 27-Dec-2002 Bill Seligman

// This is a separate "calculator" class for the endcap presampler.
// For more information about what a "calculator" class does, see the
// documentation: LArG4/doc/LArG4.ps.

#ifndef LARG4EC_LARENDCAPPRESAMPLERCALCULATOR_H
#define LARG4EC_LARENDCAPPRESAMPLERCALCULATOR_H

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArCalculatorSvcImp.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include <stdexcept>
// Forward declarations.
class G4Step;
class LArG4BirksLaw;

namespace LArG4 {
  class IECPresamplerGeometry;
}

class LArEndcapPresamplerCalculator : public LArCalculatorSvcImp {
public:

  LArEndcapPresamplerCalculator(const std::string& name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  /////////////////////////////////////////////

  virtual G4float OOTcut() const override final { return m_OOTcut; }

  // Check if the current hitTime is in-time
  virtual G4bool isInTime(G4double hitTime) const override final
  {
    return !(hitTime > m_OOTcut); //FIXME should we be checking the absolute value of hitTime here?
  }

  virtual G4bool Process(const G4Step*, std::vector<LArHitData>&) const override final;


private:

  // Pointer to geometry routine.
  ServiceHandle<LArG4::IECPresamplerGeometry> m_geometry;

  LArG4BirksLaw *m_birksLaw;

  LArEndcapPresamplerCalculator (const LArEndcapPresamplerCalculator&);
  LArEndcapPresamplerCalculator& operator= (const LArEndcapPresamplerCalculator&);
};

#endif
