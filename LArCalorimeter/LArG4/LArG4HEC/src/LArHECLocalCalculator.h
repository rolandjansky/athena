/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4HECLocalCalculator.hh

// Revision history:

// 17-Feb-2006 : Pavol Strizenec

#ifndef __LArG4HECLocalCalculator_H__
#define __LArG4HECLocalCalculator_H__

//#include "globals.hh"
//#include "G4ThreeVector.hh"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArCalculatorSvcImp.h"
#include <stdexcept>
#include "CLHEP/Units/SystemOfUnits.h"


// Forward declarations.
class G4Step;
class LArG4BirksLaw;

namespace LArG4 {
  namespace HEC {
    class ILocalGeometry;
  }
}

class LArHECLocalCalculator : virtual public LArCalculatorSvcImp {

public:

  LArHECLocalCalculator(const std::string& name, ISvcLocator * pSvcLocator);
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual ~LArHECLocalCalculator(){};

  virtual G4float OOTcut() const override final { return m_OOTcut; }

  virtual G4bool Process(const G4Step* a_step, std::vector<LArHitData>& hdata) const override final { return this->Process(a_step,0, 4.*CLHEP::mm, hdata);}
  virtual G4bool Process(const G4Step* a_step, int depthadd, double deadzone, std::vector<LArHitData>& hdata) const final; //FIXME not part of interface...

  // Check if the current hitTime is in-time
  virtual G4bool isInTime(G4double hitTime) const override final
  {
    return !(hitTime > m_OOTcut); //FIXME should we be checking the absolute value of hitTime here?
  }


private:

  ServiceHandle<LArG4::HEC::ILocalGeometry> m_Geometry;

  LArG4BirksLaw *m_birksLaw;
  G4bool m_isX;

  LArHECLocalCalculator (const LArHECLocalCalculator&);
  LArHECLocalCalculator operator= (const LArHECLocalCalculator&);
};

#endif
