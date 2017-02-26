/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArBarrelGeometry.hh

#ifndef LARG4BARREL_LARBARRELGEOMETRY_H
#define LARG4BARREL_LARBARRELGEOMETRY_H
#include "LArG4Barrel/IGeometryCalculator.h"
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

    class Geometry: public AthService, virtual public IGeometryCalculator {

    public:

      Geometry(const std::string& name, ISvcLocator * pSvcLocator);
      StatusCode initialize() override final;
      virtual ~Geometry();

      /** Query interface method to make athena happy */
      virtual StatusCode queryInterface(const InterfaceID&, void**) override final;

      // Full identifier computation from a G4 step
      LArG4Identifier CalculateIdentifier(const G4Step*) const override final;
      LArG4Identifier CalculateECAMIdentifier(const G4Step*, const G4int indEcam, const bool inSTAC=true, int zside=1) const;

      // Given a point compute all quantities (cell number, distance to electrode, etc...)
      StatusCode findCell( LArG4::Barrel::LArGeomData& result, const double & x, const double & y, const double & z,
                           const double & r, const double & eta, const double & phi, const bool detail) const override final; //FIXME non-const function

      bool CheckLArIdentifier(int sampling,int region, int eta,int phi) const;
      bool CheckDMIdentifier(int type, int sampling, int region, int eta, int phi) const;


    private:
      // Get geometry informations (filled with findCell)
      //   cellID = 1 valid, 0 invalid
      //   sampling = sampling number (1 to 3)
      //   region = 0 (eta<1.) or 1 (1.4<eta<1.475)
      //   etaBin = cell number in eta
      //   phiBin = cell number in phi
      //   phiGap    = gap number in phi (0 to 1023)
      //   nstraight = straight section number (0 to 13)
      //   nfold = closest fold number (0 to 14)
      //   distElec   = distance to electode (signed)
      //   distAbs    = distance to absorber (signed)
      //   x0,y0     = local coordinates in first cell frame (to be used for current maps)
      // int cellID() const  { return m_output.cellID;};
      // G4int sampling() const { return m_output.sampling;};
      // G4int region() const   { return m_output.region;};
      // G4int etaBin() const   { return m_output.etaBin;};
      // G4int phiBin() const   { return m_output.phiBin;};
      // G4int phiGap() const   { return m_output.phiGap;};
      // G4int nstraight() const {return m_output.nstraight;};
      // G4int nfold() const {return m_output.nfold;};
      // G4double distElec() const { return m_output.distElec;};
      // G4double distAbs() const { return m_output.distAbs;};
      // G4double x0() const {return m_output.x0;};
      // G4double y0() const {return m_output.y0;};
      // G4double xl() const {return m_output.xl;};
      // G4int etaMap() const {return m_output.etaMap;};
      // G4int sampMap() const {return m_output.sampMap;};

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

      std::string m_detectorName;

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
      LArCoudeElectrodes* m_coudeelec; // FIXME needed due to lazy initialization
      LArCoudeAbsorbers* m_coudeabs; // FIXME needed due to lazy initialization
      LArStraightElectrodes* m_electrode; // FIXME needed due to lazy initialization
      LArStraightAbsorbers* m_absorber; // FIXME needed due to lazy initialization
      
      // to handle small difference (mostly phi wrapping and +-z symmetry)
      // between atlas and test beam
      bool m_testbeam;


      // output of computations (everything in half barrel frame except m_zSide)
      // mutable LArGeomData m_output;
      // mutable int m_cellID;   // 0 if not valid cell
      // mutable G4int m_sampling;       // sampling number  (1 to 3)
      // mutable G4int m_region;         // region number (0 or 1)
      // mutable G4int m_etaBin;         // cell number in eta
      // mutable G4int m_phiBin;         // cell number in phi
      // mutable G4int m_zSide;          // side (+-1 for +-z)
      // mutable G4int m_phiGap;         // number (0 to 1024) of closest electrode
      // mutable G4int m_nstraight;      // number of straight section (0 to 13)
      // mutable G4int m_nfold;          // number of closest fold (0 to 14)
      // mutable G4double m_distElec;    // algebric distance to electrode
      // mutable G4double m_distAbs;     // algebric distance to absorber
      // mutable G4double m_xl;          // normalized lenght along electrode
      // mutable G4double m_x0;          //
      // mutable G4double m_y0;          // coordinates in local cell frame (down absorber with phi=0)
      // mutable G4int m_sampMap;        // sampling number not taking into account readout strips
      // mutable G4int m_etaMap;         // eta number not taking into account readout strips

      bool m_iflSAG;

      // intermediate values for phi cell computation
      //G4int m_NRphi;
      G4double m_Rmin;
      G4double m_Rmax;
      G4double m_Rphi[5000];
      G4double m_dR;
      //double m_2pi;

    protected:

    } ;

  } //end of Barrel namespace

} // end of LArG4 namespace

#endif // LARG4BARREL_LARBARRELGEOMETRY_H
