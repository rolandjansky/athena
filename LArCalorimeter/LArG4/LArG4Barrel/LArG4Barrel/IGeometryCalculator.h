/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IGeometryCalculator.h
// Common interface for Geometry Calculator classes

#ifndef __LARG4BARREL_IGEOMETRYCALCULATOR_H__
#define __LARG4BARREL_IGEOMETRYCALCULATOR_H__

#include "GaudiKernel/IService.h"

//#include "globals.hh"

#include "LArG4Code/LArG4Identifier.h"

#include <string>

class G4Step;

namespace LArG4
{
  namespace Barrel
  {
    struct LArGeomData {
      // output of computations (everything in half barrel frame except zSide)
      LArG4Identifier identifier = LArG4Identifier(); // The default result is a blank identifier.
      int cellID = 0;           // 0 if not valid cell
      int sampling = 0;       // sampling number  (1 to 3)
      int region = 0;         // region number (0 or 1)
      int etaBin = 0;         // cell number in eta
      int phiBin = 0;         // cell number in phi
      int zSide = 0;          // side (+-1 for +-z)
      int phiGap = 0;         // number (0 to 1024) of closest electrode
      int nstraight = 0;      // number of straight section (0 to 13)
      int nfold = 0;          // number of closest fold (0 to 14)
      double distElec = 0.0;  // algebric distance to electrode
      double distAbs = 0.0;   // algebric distance to absorber
      double xl = 0.0;        // normalized length along electrode
      double x0 = 0.0;        //
      double y0 = 0.0;        // coordinates in local cell frame (down absorber with phi=0)
      int sampMap = 0;        // sampling number not taking into account readout strips
      int etaMap = 0;         // eta number not taking into account readout strips
    };
  }

  class IGeometryCalculator : virtual public IService
  {
  public:
    IGeometryCalculator() {}
    virtual ~IGeometryCalculator() {}

    static const InterfaceID& interfaceID() {
      static const InterfaceID  IID_IGeometryCalculator("IGeometryCalculator",1,0);
      return IID_IGeometryCalculator;
    }

    // Given a point compute all quantities (cell number, distance to electrode, etc...)
    virtual StatusCode findCell(  LArG4::Barrel::LArGeomData& result, const double & x, const double & y, const double & z,
                                  const double & r, const double & eta, const double & phi, const bool detail) const = 0;
    // Full identifier computation from a G4 step
    virtual LArG4Identifier CalculateIdentifier( const G4Step*) const = 0;
  };
}
#endif //__LARG4BARREL_IGEOMETRYCALCULATOR_H__
