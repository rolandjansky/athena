/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ILArBarrelPresamplerGeometry.h
// Prepared in 2016 based on LArVCalculator from Bill Seligman

#ifndef LARG4BARREL_ILARBARRELPRESAMPLERGEOMETRY_H
#define LARG4BARREL_ILARBARRELPRESAMPLERGEOMETRY_H

#include "GaudiKernel/IService.h"

#include "globals.hh"

#include "LArG4Code/LArG4Identifier.h"


// Forward declaractions:
class G4Step;

namespace LArG4 {

  namespace BarrelPresampler {

    // output of computations
    struct CalcData {
      G4int sampling = 0;       // sampling number  (1 to 3)
      G4int region = 0;         // region number (0 or 1)
      G4int etaBin = 0;         // cell number in eta
      G4int phiBin = 0;         // cell number in phi
      G4int gap = 0;
      G4int module = 0;
      G4double distElec = 0.;    // algebric distance to electrode
      G4double xElec = 0.;       // projection along electrode axis
      G4double dist = 0.;
    };

  } //end of BarrelPresampler namespace

} // end of LArG4 namespace

class ILArBarrelPresamplerGeometry: virtual public IService {
public:

  ILArBarrelPresamplerGeometry() {};

  virtual ~ILArBarrelPresamplerGeometry() {};

  DeclareInterfaceID(ILArBarrelPresamplerGeometry,1,0);

  // Full identifier computation from a G4 step
  virtual LArG4Identifier CalculateIdentifier( const G4Step*) const = 0;

  // Given a point compute all quantities (cell number, distance to electrode, etc...)
  virtual bool findCell(LArG4::BarrelPresampler::CalcData & currentCellData, G4double xloc,G4double yloc,G4double zloc) const = 0;
};

#endif // LARG4BARREL_ILARBARRELPRESAMPLERGEOMETRY_H
