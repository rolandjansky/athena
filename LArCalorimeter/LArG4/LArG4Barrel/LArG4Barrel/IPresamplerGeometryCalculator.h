/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IPresamplerGeometryCalculator.h
// Common interface for Geometry Calculator classes

#ifndef __LARG4BARREL_IPRESAMPLERGEOMETRYCALCULATOR_H__
#define __LARG4BARREL_IPRESAMPLERGEOMETRYCALCULATOR_H__

#include "GaudiKernel/IService.h"

//#include "globals.hh"

#include "LArG4Code/LArG4Identifier.h"

#include <string>

class G4Step;

namespace LArG4 {

  namespace BarrelPresampler {

    struct LArGeomData {
      // output of computations
      //LArG4Identifier identifier = LArG4Identifier(); // The default result is a blank identifier.
      int zSide = 0;          // side (+-1 for +-z)
      int sampling = 0;       // sampling number  (1 to 3)
      int region = 0;         // region number (0 or 1)
      int etaBin = 0;         // cell number in eta
      int phiBin = 0;         // cell number in phi
      int gap = 0;            //
      int module = 0;         //
      double distElec = 0.0;  // signed distance to electrode in electrode reference frame
      double xElec = 0.0;     // projection along electrode axis in electrode reference frame
      double dist = 0.0;      //
      //int cellID = 0;           // 0 if not valid cell
      // int phiGap = 0;         // number (0 to 1024) of closest electrode
      // int nstraight = 0;      // number of straight section (0 to 13)
      // int nfold = 0;          // number of closest fold (0 to 14)
      // double distAbs = 0.0;   // algebric distance to absorber
      // double xl = 0.0;        // normalized length along electrode
      // double x0 = 0.0;        //
      // double y0 = 0.0;        // coordinates in local cell frame (down absorber with phi=0)
      // int sampMap = 0;        // sampling number not taking into account readout strips
      // int etaMap = 0;         // eta number not taking into account readout strips
    };
  } //end of BarrelPresampler namespace

  class IPresamplerGeometryCalculator : virtual public IService
  {
  public:
    IPresamplerGeometryCalculator() {}
    virtual ~IPresamplerGeometryCalculator() {}

    static const InterfaceID& interfaceID() {
      static const InterfaceID  IID_IPresamplerGeometryCalculator("IPresamplerGeometryCalculator",1,0);
      return IID_IPresamplerGeometryCalculator;
    }

    // Full identifier computation from a G4 step
    virtual LArG4Identifier CalculateIdentifier( const G4Step* ) const = 0;
    // Given a point compute all quantities (cell number, distance to electrode, etc...)
    virtual bool findCell(LArG4::BarrelPresampler::LArGeomData& result,double,double,double) const = 0;

  };
} // end of LArG4 namespace

#endif //__LARG4BARREL_IPRESAMPLERGEOMETRYCALCULATOR_H__
