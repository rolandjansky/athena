/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4Hit
// Definition of a hit in the ATLAS Liquid-Argon Calorimeter.
// 09-Nov-2000 Bill Seligman

// 09-Jan-2002 WGS: Renamed to LArG4Hit (from LArEMHit) since
// this hit class can be used with any of the LAr calorimeters.

// Based on exampleN04 in Geant 4.2.0ref2.

#include "LArG4Code/LArG4Hit.h"
#include "LArG4Code/LArG4Identifier.h"

#include "G4Allocator.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "AthenaKernel/Units.h" 

G4Allocator<LArG4Hit> LArG4HitAllocator;

// Default constructor.  Prevent undefined values.
LArG4Hit::LArG4Hit()
  : m_identifier(LArG4Identifier())
  , m_sumEnergy(0.)
  , m_sumEnergyTime(0.)
{}

LArG4Hit::LArG4Hit(LArG4Identifier a_ident,
		   G4double a_energy, 
		   G4double a_time)
  : m_identifier(a_ident)
  , m_sumEnergy(a_energy)
  , m_sumEnergyTime(a_energy*a_time)
{;}

LArG4Hit::~LArG4Hit() {;}

void LArG4Hit::Draw() {;}

void LArG4Hit::Print()
{
  G4cout << "LArG4Hit: ";
  m_identifier.show();
  G4cout << ", "
	 << energy()/Athena::Units::MeV << " [MeV],"
	 << time()/Athena::Units::ns << " [ns])" << G4endl;
}
