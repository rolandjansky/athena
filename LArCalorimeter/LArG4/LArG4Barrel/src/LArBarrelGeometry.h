/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArBarrelGeometry.hh

#ifndef LARG4BARREL_LARBARRELGEOMETRY_H
#define LARG4BARREL_LARBARRELGEOMETRY_H

#include "ILArBarrelGeometry.h"
#include "AthenaBaseComps/AthService.h"

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4TouchableHistory.hh"

#include <string>

// Forward declarations.
class LArG4Identifier;
class G4Step;
class LArCoudeElectrodes;
class LArCoudeAbsorbers;
class LArStraightAbsorbers;
class LArStraightElectrodes;

namespace LArG4 {

  namespace Barrel {

    class Geometry: public AthService, virtual public ILArBarrelGeometry {

    public:

      //constructor
      Geometry(const std::string& name, ISvcLocator * pSvcLocator);

      virtual ~Geometry() { };

      /** Query interface method to make athena happy */
      virtual StatusCode queryInterface(const InterfaceID&, void**) override final;

      virtual StatusCode initialize() override final;
      virtual StatusCode finalize() override final;

      // Full identifier computation from a G4 step
      virtual LArG4Identifier CalculateIdentifier( const G4Step* ) const override final;

      // Given a point compute all quantities (cell number, distance to electrode, etc...)
      virtual void findCell( CalcData & currentCellData, const double & x, const double & y, const double & z,
                             const double & r, const double & eta, const double & phi, const bool detail) const override final;

    private:

      LArG4Identifier CalculateECAMIdentifier( const G4Step* , const G4int indEcam, const bool inSTAC=true,int zside=1) const;
      bool CheckLArIdentifier(int sampling,int region, int eta,int phi) const;
      bool CheckDMIdentifier(int type, int sampling, int region, int eta, int phi) const;

      // detector name, for translated geometry
      std::string m_detectorName;
      G4String m_ecamName;

      // global EMBarrel dimensions
      double m_rMinAccordion;
      double m_rMaxAccordion;
      double m_zMinBarrel;
      double m_zMaxBarrel;
      double m_etaMaxBarrel;

      // GU 11/06/2003  total number of cells in phi
      int m_NCellTot;    // either 64 or 1024 for TestBeam or Atlas
      int m_NCellMax;    // 1024

      // Accordion parameters
      int m_Nbrt;         //   number of straight sections (=14)
      int m_Nbrt1;        //   number of folds (=15)

      // Accordion parameters, refering to the neutral fibre
      double m_gam0 ;         //phi position for the first absorber  neutral fiber
      double m_rint_eleFib ; //2.78
      double *m_rc, *m_phic, *m_xc, *m_yc, *m_delta;// double m_rc[15] ;     // R and
      int m_parity;
      //double m_phic[15] ;   // phi positions of center of fold for first absorber
      // double m_xc[15];       // corresponding x,y values
      // double m_yc[15];
      //double m_delta[15];   // zig-zag angles
      // to access G4 geometry
      mutable LArCoudeElectrodes* m_coudeelec; // FIXME needed due to lazy initialization
      mutable LArCoudeAbsorbers* m_coudeabs; // FIXME needed due to lazy initialization
      mutable LArStraightElectrodes* m_electrode; // FIXME needed due to lazy initialization
      mutable LArStraightAbsorbers* m_absorber; // FIXME needed due to lazy initialization

      // to handle small difference (mostly phi wrapping and +-z symmetry)
      // between atlas and test beam
      bool m_testbeam;

      bool m_iflSAG;

      // intermediate values for phi cell computation
      G4int m_NRphi;
      G4double m_Rmin;
      G4double m_Rmax;
      G4double m_Rphi[5000];
      G4double m_dR;

      // function to compute distance to electrode
      double Distance_Ele(const double &x, const double &y,
                          const int &PhiC, int &Num_Straight, const int &Num_Coude,
                          double &xl) const;
      // function to compute distance to absorber
      double Distance_Abs(const double &x, const double &y,
                          const int &nabs, const int &Num_Straight, const int &Num_Coude) const;

      // longitudinal and eta segmentation of electrodes
      G4int SampSeg(G4double,G4double,G4double,G4int&,G4int&,G4int&,G4int&,G4int&) const;

      /// phi vs r of first absorber in nominal geometry

      /// Initialize r-phi reference map (called from constructor)
      void GetRphi();
      ///
      G4double Phi0(G4double) const;
      ///
      G4int PhiGap(const double &, const double &, const double &) const;

    protected:

      Geometry();


    } ;

  } //end of Barrel namespace

} // end of LArG4 namespace

#endif // LARG4BARREL_LARBARRELGEOMETRY_H
