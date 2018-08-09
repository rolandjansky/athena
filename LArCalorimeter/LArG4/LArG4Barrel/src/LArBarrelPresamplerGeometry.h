/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArBarrelPresamplerGeometry.hh

#ifndef LARG4BARREL_LARBARRELPRESAMPLERGEOMETRY_H
#define LARG4BARREL_LARBARRELPRESAMPLERGEOMETRY_H

#include "ILArBarrelPresamplerGeometry.h"
#include "AthenaBaseComps/AthService.h"

#include "LArG4Code/LArG4Identifier.h"

#include "AthenaKernel/Units.h"
#include "globals.hh"


// Forward declarations.
class LArG4Identifier;
class G4Step;

namespace LArG4 {

  namespace BarrelPresampler {

    class Geometry: public extends<AthService, ILArBarrelPresamplerGeometry> {

    public:

       //constructor
      Geometry(const std::string& name, ISvcLocator * pSvcLocator);

      virtual ~Geometry();

      virtual StatusCode initialize() override final;

      virtual LArG4Identifier CalculateIdentifier( const G4Step* ) const override final;
      virtual bool findCell(CalcData & currentCellData, G4double xloc,G4double yloc,G4double zloc) const override final;

    private:
      LArG4Identifier CalculatePSActiveIdentifier( const G4Step* , const G4int indPS ) const;
      LArG4Identifier CalculatePS_DMIdentifier( const G4Step* , const G4int indPS ) const;

      inline G4int determineZSide(const double zCoord) const {
        // in TB case, only 1 side,
        // in Atlas case, use overall z to decide side
        if (m_testbeam) { return 1; }
        if (zCoord > 0.) { return 1; }
        return -1;
      }

      // detector name, for translated geometry
      std::string m_detectorName{"LArMgr"};

#include "PresParameterDef.h"

      // end z of the various modules
      G4double m_end_module[8];
      G4double m_zminPS{3.00*Athena::Units::mm};   // FIXME this should come from the database;
      G4double m_zpres{1549.*Athena::Units::mm}; // position of mother volume inside nominal Atlas frame
      G4double m_cat_th{};
      // z of first cathode in each module
      G4double m_first_cathod[8]{};
      // tilt of electrodes
      G4double m_tilt[8]{};
      // number of gaps per cell
      G4int m_ngap_cell[8]{};
      // pitch in z of gaps
      G4double m_pitch[8]{};
      // number of cells per modules
      G4int    m_ncell_module[8]{};
      // total LAr thickness
      G4double m_halfThickLAr{0.5*13.*Athena::Units::mm}; // LAr total gap
      // is this a test beam simulation job
      bool m_testbeam{false};

    } ;

  } //end of Barrel namespace

} // end of LArG4 namespace

#endif // LARG4BARREL_LARBARRELPRESAMPLERGEOMETRY_H
