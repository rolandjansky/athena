/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CalibrationHit.h
// Definition of a calibration hit in the LArG4 simulation.
// 08-Jan-2004 Bill Seligman

#include "LArG4Code/CalibrationHit.h"
#include "LArG4Code/LArG4Identifier.h"

#include "G4Allocator.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "AthenaKernel/Units.h"

namespace LArG4 {

  G4Allocator<CalibrationHit> CalibrationHitAllocator;

  void CalibrationHit::Draw() {;}

  void CalibrationHit::Print()
  {
    G4cout << "CalibrationHit: ";
    m_identifier.show();
    G4cout << ", energy(0,1,2,3)=("
	   << m_energy0/Athena::Units::MeV << ","
	   << m_energy1/Athena::Units::MeV << ","
	   << m_energy2/Athena::Units::MeV << ","
	   << m_energy3/Athena::Units::MeV << ")" << G4endl;
  }

} // namespace LArG4
