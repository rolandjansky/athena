/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArBarrelGeometry.hh

#ifndef LARG4BARREL_LARBARRELGEOMETRY_H
#define LARG4BARREL_LARBARRELGEOMETRY_H

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
      G4double distElec = 0;    // algebric distance to electrode
      G4double distAbs = 0;     // algebric distance to absorber
      G4double xl = 0;          // normalized lenght along electrode
      G4double x0 = 0;          //
      G4double y0 = 0;          // coordinates in local cell frame (down absorber with phi=0)
      G4int sampMap = 0;        // sampling number not taking into account readout strips
      G4int etaMap = 0;         // eta number not taking into account readout strips
    };

    class Geometry {

    public:

      // Accessor method for singleton pattern.
      static Geometry* GetInstance();

      virtual ~Geometry();

      // Full identifier computation from a G4 step
      LArG4Identifier CalculateIdentifier( const G4Step* ,std::string strDetector="") const;
      LArG4Identifier CalculateECAMIdentifier( const G4Step* , const G4int indEcam, std::string strDetector="",const bool inSTAC=true,int zside=1) const;

      // Given a point compute all quantities (cell number, distance to electrode, etc...)
      void findCell( CalcData & currentCellData, const double & x, const double & y, const double & z,
                     const double & r, const double & eta, const double & phi, const bool detail,
                     std::string strDetector="") const; //FIXME non-const function

      bool CheckLArIdentifier(int sampling,int region, int eta,int phi) const;
      bool CheckDMIdentifier(int type, int sampling, int region, int eta, int phi) const;


    private:

      //copy constructor
      Geometry(const Geometry& );//coverity issue fix. Declared, but not implemented
      Geometry& operator=(const Geometry&);//coverity issue fix. Declared, but not implemented

      static Geometry* m_instance;

      mutable bool m_FIRST; // FIXME needed due to lazy initialization

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
      double m_2pi;

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
