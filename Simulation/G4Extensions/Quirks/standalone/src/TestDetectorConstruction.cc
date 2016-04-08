/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>

#include "Quirks/TestDetectorConstruction.hh"

#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"

G4VPhysicalVolume* TestDetectorConstruction::Construct()
{
    G4double Bx, By, Bz, density_factor1, density_factor2;
    std::ifstream f;
    f.open("geom.txt");
    f >> Bx;                    Bx *= tesla;
    f >> By;                    By *= tesla;
    f >> Bz;                    Bz *= tesla;
    f >> density_factor1;
    f >> density_factor2;
    f.close();

    G4MagneticField* field = new G4UniformMagField(G4ThreeVector(Bx, By, Bz));
    G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldMgr->SetDetectorField(field);
    fieldMgr->CreateChordFinder(field);

    G4Material* Ar = new G4Material("ArgonGas", 18., 39.95*g/mole, 1.782e-0*mg/cm3 * density_factor1);
    G4Box* hall_box = new G4Box("hall_box", 4.*m, 4.*m, 4.*m);
    hall_log = new G4LogicalVolume(hall_box, Ar, "hall_log");
    hall_phys = new G4PVPlacement(0, G4ThreeVector(), hall_log, "hall_phys", 0, false, 0);

    G4Material* Al = new G4Material("Aluminum", 13., 26.98*g/mole, 2.7e-0*g/cm3 * density_factor2);
    G4Box* block_box = new G4Box("block_box", 1.*m, 1.*m, 1.*m);
    block_log = new G4LogicalVolume(block_box, Al, "block_log");
    block_phys = new G4PVPlacement(0, G4ThreeVector(2.*m, 0, 0), block_log, "block_phys", hall_log, false, 0);

    return hall_phys;
}
