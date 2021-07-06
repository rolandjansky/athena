/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ILArBarrelGeometry.h
// Prepared in 2016 based on LArVCalculator from Bill Seligman

#ifndef LARG4BARREL_ILARBARRELGEOMETRY_H
#define LARG4BARREL_ILARBARRELGEOMETRY_H

#include "GaudiKernel/IService.h"

#include "globals.hh"

#include "LArG4Code/LArG4Identifier.h"


// Forward declaractions:
class G4Step;

namespace LArG4 {

  namespace Barrel {

    // output of computations (everything in half barrel frame except m_zSide)
    struct CalcData {
      int cellID = 0;           // 0 if not valid cell
      G4int sampling = 0;       // sampling number  (1 to 3)
      G4int region = 0;         // region number (0 or 1)
      G4int etaBin = 0;         // cell number in eta
      G4int phiBin = 0;         // cell number in phi
      G4int zSide = 0;          // side (+-1 for +-z)
      G4int phiGap = 0;         // number (0 to 1024) of closest electrode
      G4int nstraight = 0;      // number of straight section (0 to 13)
      G4int nfold = 0;          // number of closest fold (0 to 14)
      G4double distElec = 0.;   // algebric distance to electrode
      G4double distAbs = 0.;    // algebric distance to absorber
      G4double xl = 0.;         // normalized lenght along electrode
      G4double x0 = 0.;         //
      G4double y0 = 0.;         // coordinates in local cell frame (down absorber with phi=0)
      G4int sampMap = 0;        // sampling number not taking into account readout strips
      G4int etaMap = 0;         // eta number not taking into account readout strips
    };

  } //end of Barrel namespace

} // end of LArG4 namespace

class ILArBarrelGeometry: virtual public IService {
public:

  ILArBarrelGeometry() {};

  virtual ~ILArBarrelGeometry() {};

  DeclareInterfaceID(ILArBarrelGeometry,1,0);

  virtual void initializeForSDCreation() = 0;

  // Full identifier computation from a G4 step
  virtual LArG4Identifier CalculateIdentifier( const G4Step* ) const = 0;

  // Given a point compute all quantities (cell number, distance to electrode, etc...)
  virtual void findCell( LArG4::Barrel::CalcData & currentCellData, const double & x, const double & y, const double & z,
                         const double & r, const double & eta, const double & phi, const bool detail) const = 0;

};

#endif // LARG4BARREL_ILARBARRELGEOMETRY_H
