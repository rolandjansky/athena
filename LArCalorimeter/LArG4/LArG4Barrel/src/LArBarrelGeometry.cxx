/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LArBarrelGeometry.cxx
PACKAGE:  offline/LArCalorimeter/LArG4/LArG4Barrel

AUTHORS:  G. Unal, L. Carminati
CREATED:  September, 2004

PURPOSE:  'geometrical' methods used by the LArBarrelCalculator.
          These methods (previously in LArBarrelCalculator) were written
          by Gaston Parrour and adapted by Sylvain Negroni.

UPDATES:  - Calculate identifier method used by CalibrationCalculator.
          (sept 2004)
          - Cleanup  (GU March-April 2005)

********************************************************************/

// #define DEBUGHITS

#include <cmath>
#include <iostream>
#include "LArBarrelGeometry.h"

#include "LArStraightAbsorbers.h"
#include "LArStraightElectrodes.h"
#include "LArCoudeElectrodes.h"
#include "LArCoudeAbsorbers.h"

namespace LArG4 {

  namespace Barrel {

    Geometry::Geometry(const std::string& name, ISvcLocator *pSvcLocator)
      : base_class(name, pSvcLocator)
      , m_detectorName("LArMgr")
      , m_rMinAccordion(0)
      , m_rMaxAccordion(0)
      , m_zMinBarrel(0)
      , m_zMaxBarrel(0)
      , m_etaMaxBarrel(0)
      , m_NCellTot(0)
      , m_NCellMax(0)
      , m_Nbrt(0)
      , m_Nbrt1(0)
      , m_gam0(0)
      , m_rint_eleFib(0)
      , m_rc(nullptr)
      , m_phic(nullptr)
      , m_xc(nullptr)
      , m_yc(nullptr)
      , m_delta(nullptr)
      , m_parity(0)
      , m_coudeelec(nullptr)
      , m_coudeabs(nullptr)
      , m_electrode(nullptr)
      , m_absorber(nullptr)
      , m_testbeam(false)
      , m_iflSAG(false)
      , m_NRphi(0)
      , m_Rmin(0)
      , m_Rmax(0)
      , m_Rphi{0}
      , m_dR(0)
    {
      declareProperty("DetectorName",m_detectorName);
      declareProperty("TestBeam", m_testbeam);
    }

    // ====================================================================================

    StatusCode Geometry::initialize()
    {
      // initialize the geometry.
      // Access source of detector parameters.

      LArVG4DetectorParameters* parameters = LArVG4DetectorParameters::GetInstance();

      // number of straight sections (should be 14)
      m_Nbrt = (int) (parameters->GetValue("LArEMBnoOFAccZigs"));
      // Number of ZIGs + 1  i.e.  15 = number of folds
      m_Nbrt1 = m_Nbrt + 1;
      // phi of first absorber
      m_gam0 =  parameters->GetValue("LArEMBAbsPhiFirst");
      // radius of curvature of neutral fiber in the folds
      m_rint_eleFib = parameters->GetValue("LArEMBNeutFiberRadius");

      m_rc = new double[m_Nbrt1];
      m_phic = new double[m_Nbrt1];
      m_delta = new double[m_Nbrt1];
      m_xc = new double[m_Nbrt1];
      m_yc = new double[m_Nbrt1];

      // r,phi positions of the centre of the folds (nominal geometry)
      for (G4int idat = 0; idat < m_Nbrt1 ; idat++) {
        m_rc[idat] = (double) parameters->GetValue("LArEMBRadiusAtCurvature",idat);
        m_phic[idat] = (double) parameters->GetValue("LArEMBPhiAtCurvature",idat);
        m_delta[idat] = (double) parameters->GetValue("LArEMBDeltaZigAngle",idat);
        m_xc[idat] = m_rc[idat]*cos(m_phic[idat]);
        m_yc[idat] = m_rc[idat]*sin(m_phic[idat]);
      }
      // define parity of accordion waves: =0 if first wave goes up, 1 if first wave goes down in the local frame
      m_parity=0;
      if (m_phic[0]<0.) { m_parity=1; }
      //
      m_rMinAccordion  =   parameters->GetValue("LArEMBRadiusInnerAccordion");
      m_rMaxAccordion  =   parameters->GetValue("LArEMBFiducialRmax");
      m_etaMaxBarrel   =   parameters->GetValue("LArEMBMaxEtaAcceptance");
      m_zMinBarrel     =   parameters->GetValue("LArEMBfiducialMothZmin");
      m_zMaxBarrel     =   parameters->GetValue("LArEMBfiducialMothZmax");
      // === GU 11/06/2003   total number of cells in phi
      // to distinguish 1 module (testbeam case) from full Atlas
      m_NCellTot = (int) (parameters->GetValue("LArEMBnoOFPhysPhiCell"));
      // total number of cells in phi to distinguish 1 module (testbeam case) from full Atlas
      m_testbeam=false;
      if (m_NCellTot != 1024) {
        m_testbeam=true;
      }
      m_NCellMax=1024;
      // ===

      // Initialize r-phi reference map
      this->GetRphi();

      // get pointers to access G4 geometry
      m_electrode = LArStraightElectrodes::GetInstance(m_detectorName);
      m_absorber  = LArStraightAbsorbers::GetInstance(m_detectorName);
      m_coudeelec = LArCoudeElectrodes::GetInstance(m_detectorName);
      m_coudeabs  = LArCoudeAbsorbers::GetInstance(m_detectorName);

      if (m_detectorName.empty()) m_ecamName  = "LAr::EMB::ECAM";
      else                        m_ecamName  = m_detectorName+"::LAr::EMB::ECAM";


      return StatusCode::SUCCESS;
    }

    // ====================================================================================

    StatusCode Geometry::finalize()
    {
      if (m_rc) delete [] m_rc;
      if (m_phic) delete [] m_phic;
      if (m_delta) delete [] m_delta;
      if (m_xc) delete [] m_xc;
      if (m_yc) delete [] m_yc;

      return StatusCode::SUCCESS;
    }

    //======================================================================================
    //
    // Here INTRINSIC Distance_to_electrode determination (G.P.)
    //
    // This retuns an ALGEBRICDistEle value, the distance from electrode
    //neutral fiber TOWARDS the Sub_Step in LAr (measured on a local perpendicular
    //vector unit oriented upwards i.e. following increasing Phi values).
    //
    // This is done in THE INTRINSIC LOCAL Z > 0. half_barrel part ("stac_phys1")
    //
    // inputs: xhit,yhit = x,y positions in local half barrel
    //         PhiCell      = electrode number in phi (0 to 1023 for Atlas case)
    //         Num_Straight = number (0 to 13) of the straight section
    //         Num_Coude    = number (0 to 14) of closest fold
    //
    // output:  Function value = algebric distance to electrode
    //          xl = normalized lenght along electrode straight section (between +-1)

    double Geometry::Distance_Ele(const double & xhit,
                                  const double &yhit, const int &PhiCell, int &Num_Straight,
                                  const int &Num_Coude, double &xl) const
    {
      //
      // FrameWork is consistent with the one used to PhiCell determination
      // e.g. it assumes HERE to be the LOCAL one of "stac_phys1",
      // (mother of ACCordion volumes) from which Z> 0. and Z < 0. half_barrel
      // parts are then defined.
      //
      // One needs POINTERS to Electrode neutral fibers
      // either for straight parts or for folds
      //
      // Fold Center ccoordinates
      const G4double Xc[2] = { m_coudeelec->XCentCoude(Num_Coude, PhiCell), m_coudeelec->YCentCoude(Num_Coude, PhiCell) };
      const G4double radfold = sqrt(Xc[0]*Xc[0]+Xc[1]*Xc[1]);
      const G4double radhit = sqrt(xhit*xhit+yhit*yhit);

      // check if the assumed straight number is the correct one
      //   (this can be wrong when we are close to a fold and there is sagging)
      if (Num_Coude == Num_Straight && radhit <radfold) {
        if (Num_Straight>0) { Num_Straight = Num_Straight-1; }
        //     ATH_MSG_VERBOSE("radhit,radfold " << radhit << " " << radfold << " change straight by +1");
      }
      if (Num_Coude == (Num_Straight+1) && radhit > radfold) {
        if (Num_Straight<12) { Num_Straight = Num_Straight+1; }
        //     ATH_MSG_VERBOSE("radhit,radfold " << radhit << " " << radfold << " change straight by -1");
      }

      // u unit 2D_Vector along straight part of the electrode neutral fiber
      const double u[2] = { m_electrode->Cosu(Num_Straight, PhiCell), m_electrode->Sinu(Num_Straight, PhiCell) };
      // Middle m_coordinates of this straight part of the electrode neutral fiber
      const double Xm[2] = { m_electrode->XCentEle(Num_Straight, PhiCell), m_electrode->YCentEle(Num_Straight, PhiCell) };
      // m_Hit Vector components
      double dx = xhit - Xm[0];
      double dy = yhit - Xm[1];

      // First compute algebric distance m_hit (2D) the 2D_projection of the
      // m_Hit Vector on this electrode neutral fiber.
      const double hit = dx*u[0] + dy*u[1];

      //
      // Flat of Fold Region ?
      //
      const G4double Half_Elec(m_electrode->HalfLength(Num_Straight,PhiCell));

      if(std::fabs(hit) < Half_Elec) {
        // Flat Region
        xl=hit/Half_Elec;
        return u[0]*dy - u[1]*dx;
      }
      else {
        // Fold region
        // c_Hit Vector components and its length
        dx = xhit - Xc[0];
        dy = yhit - Xc[1];
        const double dr = sqrt( dx*dx + dy*dy);
        if (Num_Coude==Num_Straight) { xl=-1.; }
        else xl=+1;
        return (Num_Coude%2 == m_parity) ? (m_rint_eleFib-dr) : (dr - m_rint_eleFib);
      }      // end of Fold Regions
    } // end of the function Distance_Ele


    //======================================================================================
    //  Algebric distance to absorber
    //
    // inputs: xhit,yhit = x,y positions in local half barrel
    //         PhiCell      = absorber number in phi (0 to 1023 for Atlas case)
    //         Num_Straight = number (0 to 13) of the straight section
    //         Num_Coude    = number (0 to 14) of closest fold
    //
    // output:  Function value = algebric distance to electrode

    double Geometry::Distance_Abs(const double & xhit,
                                  const double &yhit, const int &PhiCell, const int &Num_Straight,
                                  const int &Num_Coude) const
    {
      //
      // FrameWork is consistent with the one used to PhiCell determination
      // e.g. it assumes HERE to be the LOCAL one of "stac_phys1",
      // (mother of ACCordion volumes) from which Z> 0. and Z < 0. half_barrel
      // parts are then defined.
      //
      // One needs POINTERS to Electrode neutral fibers
      // either for straight parts or for folds
      //
      // u unit 2D_Vector along straight part of the electrode neutral fiber
      const G4double u[2] = { m_absorber->Cosu(Num_Straight, PhiCell), m_absorber->Sinu(Num_Straight, PhiCell) };
      // Middle m_coordinates of this straight part of the electrode neutral fiber
      const G4double Xm[2] = { m_absorber->XCentAbs(Num_Straight, PhiCell), m_absorber->YCentAbs(Num_Straight, PhiCell) };
      // m_Hit Vector components
      double dx = xhit - Xm[0];  double dy = yhit - Xm[1];

      // First compute algebric distance hit (2D) the 2D_projection of the
      // m_Hit Vector on this electrode neutral fiber.
      const double hit = dx*u[0] + dy*u[1];

      //
      // Flat of Fold Region ?
      //
      if(std::fabs(hit) < m_absorber->HalfLength(Num_Straight,PhiCell)) {
        // Flat Region
        return u[0]*dy - u[1]*dx;
      }
      else {
        // Fold Center c_coordinates
        const G4double Xc[2] = { m_coudeabs->XCentCoude(Num_Coude, PhiCell), m_coudeabs->YCentCoude(Num_Coude, PhiCell) };
        // c_Hit Vector components and its length
        dx = xhit - Xc[0];
        dy = yhit - Xc[1];
        const double dr = sqrt( dx*dx + dy*dy);
        return (Num_Coude%2 == m_parity) ? (m_rint_eleFib-dr) : (dr - m_rint_eleFib);
      }      // end of Fold Regions
    } // end of the function Distance_Abs


    //=============================================================================
    //  Function SampSeg
    //
    // eta-sampling segmentation of barrel calorimeter   GU, January 2005
    //  input values: eta,radius in half-barrel frame
    //
    //  return value of function:  true=active area, false=inactive area
    //  return arguments: iregion,isampling,ieta
    //                             take into account detailed electrode drawing
    //                             with readout strips
    //                            isamp2,ieta2 do not take into account
    //                             readout strips and can be used to access current
    //                             maps.
    //
    //  iregion=0 (eta<1.4)  or 1 (eta=1.4-1.475)
    //  for region 0: isampling = 1 (strips), 2 (middle), 3 (back)
    //  for region 1: isampling = 1 or isampling = 2
    //  ieta= eta cell number
    //    region0,samp1:  ieta=1->448 (strip 0 does not exist)
    //    region0,samp2:  ieta=0->55
    //    region0,samp3:  ieta=0->26  (max eta 1.325)
    //    region1,samp1:  ieta=0->2 (deta=0.025)
    //    region1,samp2:  ieta=0 (only 1 cell)

    G4int Geometry::SampSeg(G4double eta, G4double radius, G4double z,
                            G4int& iregion, G4int& isampling, G4int& ieta,
                            G4int& isamp2, G4int& ieta2) const
    {
      static G4double Rmax1[448];
      static G4double Rmax2[56];
      static G4double Eta_max,Eta_max_s1,Eta_max_s3,R_max_acc,Z_max_acc,R_min_acc,R_min_highz;
      static G4double Dr_s12;
      static G4double deltaz,Z_max_lowr,dzdr;
      static G4double deta;
      static G4double zmax1,zmax2,zmax3,zmax4,zmax5,zmax6,zmax7,rmax1,rmax2,rmax3,rmax4;
      static bool FILL = true;
      if (FILL) {

        LArVG4DetectorParameters* parameters = LArVG4DetectorParameters::GetInstance();

        // maximum eta barrel 1.475 (at r=1500.024)
        Eta_max = parameters->GetValue("LArEMBMaxEtaAcceptance");
        // minimum active radius 1500.024
        R_min_acc= parameters->GetValue("LArEMBRadiusInnerAccordion");
        // maximum active radius 1960.
        R_max_acc = parameters->GetValue("LArEMBFiducialRmax");
        // maximum active z (before subtracting edge for signal readout)
        //   currently 3150, should be changed in database to become 3164
        Z_max_acc = parameters->GetValue("LArEMBfiducialMothZmax");
        // minimum radius at z max for active region
        R_min_highz=1548.;     //FIXME should be taken from database

        // inactive thickness between S1 and S2 FIXME should be taken from database
        Dr_s12=1.1;

        Eta_max_s1=1.4;     // maximum eta region 0
        Eta_max_s3=1.325;   // maximum eta for S3 in region 0
        deta=0.025;         // basic granularity

        // values of the radial separations between samplings
#include "LArBarrelSampling.icc"

        // compute z edge taken by readout strips on the edge

        deltaz=7.;   // this include copper 6mm + 2*0.5mm empty space on each side

        zmax1=Z_max_acc-deltaz;
        zmax2=Z_max_acc-2.*deltaz;
        zmax3=Z_max_acc-3.*deltaz;
        zmax4=Z_max_acc-4.*deltaz;
        zmax5=Z_max_acc-5.*deltaz;
        zmax6=Z_max_acc-6.*deltaz;
        zmax7=Z_max_acc-7.*deltaz;
        rmax1=R_max_acc-deltaz;
        rmax2=R_max_acc-2.*deltaz;
        rmax3=R_max_acc-3.*deltaz;
        rmax4=R_max_acc-4.*deltaz;


        // maximum z at r=1500.024
        Z_max_lowr = sinh(Eta_max)*R_min_acc;
        // slope of z vs r edge (which is not projective in eta...)
        dzdr = (Z_max_acc-Z_max_lowr)/(R_min_highz-R_min_acc);

        //     ATH_MSG_VERBOSE("Initialization of SampSet ");
        //     ATH_MSG_VERBOSE(" Rmin/Rmax      " << R_min_acc << " " << R_max_acc);
        //     ATH_MSG_VERBOSE(" Zmax/Zmax_lowR " << Z_max_acc << " " << Z_max_lowr);
        //     ATH_MSG_VERBOSE(" Rmin_highz     " << R_min_highz);
        //     ATH_MSG_VERBOSE(" dzdr           " << dzdr);

        FILL=false;
      };

      // iactive = 1 if active region, 0 if region considered as inactive
      G4int iactive = 1;

      const G4double aeta=std::fabs(eta);

      G4double r12=-1.;
      G4double r23=-1.;

      // Not used: G4double rmax=Z_max_acc/sinh(aeta);

      G4int istrip,imid;

      // eta < 1.4

      if (aeta<Eta_max_s1) {

        // get radius for end of strips
        istrip=(int) (aeta/deta*8.);
        if (istrip<0 || istrip >=448) {
          ATH_MSG_ERROR(" Problem aeta,istrip " << aeta << " " << istrip);
          return 0;
        }
        r12=Rmax1[istrip];

        // get radius for end of middle
        imid = (int) (aeta/deta);
        if (imid <0 || imid >=56) {
          ATH_MSG_ERROR(" Problem aeta,imid " << aeta << " " << imid);
          return 0;
        }
        r23=Rmax2[imid];

        iregion=0;

        // strips
        if (radius <= r12) {
          isampling=1;
          ieta=istrip;
          if (ieta==0) iactive=0;
          isamp2=1;
          ieta2=istrip;
        }

        // region between strips and middle => not active, same identifier as strips
        else if (radius < (r12+Dr_s12)) {
          isampling=1;
          ieta=istrip;
          iactive=0;
          isamp2=1;
          ieta2=istrip;
        }

        else {

          // eta<1.325, we can be in the back
          if (aeta<Eta_max_s3) {
            // radius<r23 we are in the middle
            if (radius <= r23) {
              isampling=2;
              ieta=imid;
              isamp2=2;
              ieta2=imid;
            }
            // for radius >r23 we have to take care of the readout strips at high z
            //   and attribute some of the energy to other cells
            else {     // radius>r23
              if (z>zmax1) {
                isampling=2;
                ieta=55;
              }
              else if (z>zmax2) {
                isampling=2;
                ieta=54;
              }
              else if (z>zmax3) {
                isampling=2;
                ieta=53;
              }
              else if (z>zmax4) {
                isampling=3;
                ieta=26;
              }
              else if (aeta<1.3 && z>zmax5) {
                isampling=2;
                ieta=52;
              }
              else if (aeta<1.3 && z>zmax6) {
                isampling=2;
                ieta=51;
              }
              else if (radius>rmax4 && z<zmax5 && aeta>1.2) {
                if (radius>rmax1) {
                  isampling=2;
                  ieta=51;
                }
                else if(radius>rmax2) {
                  isampling=3;
                  ieta=25;
                }
                else if (radius>rmax3) {
                  if (z<zmax7) {
                    isampling=2;
                    ieta=50;
                  }
                  else {
                    isampling=3;
                    ieta=25;
                  }
                }
                else {
                  if (aeta<1.25) {
                    isampling=2;
                    ieta=49;
                  }
                  else {
                    isampling=3;
                    ieta=25;
                  }
                }
              }
              // normal back cell
              else {
                isampling=3;
                ieta=imid/2;
                isamp2=3;
                ieta2=ieta;
              }
              isamp2=3;
              ieta2=imid/2;
            }   // end radius>r23
            // put into middle energy deposited along readout strips across the back
            if (isampling==3 && z<zmax4 && (radius<rmax4 || aeta<1.2) ) {
              const double etastr = (imid%2==0) ? 0.025*imid : 0.025*(imid+1);
              const double delta=radius*(sinh(etastr)-sinh(aeta))/cosh(etastr);
              double deltastr;
              if (aeta<0.475) { deltastr=1.5;}
              else if (aeta<0.80) { deltastr=2.75;}
              else if (aeta<0.85) { deltastr=1.5;}
              else if (aeta<1.1)  { deltastr=2.75;}
              else { deltastr=3.25;}

              if (std::fabs(delta)<deltastr) {
                isampling=2;
                ieta=imid;
              }
            }     // end if sampling==3
          }     // end if eta<1.325
          else {
            isampling=2;
            ieta=imid;
            if (z>zmax1) {
              ieta=55;
            }
            else if (z>zmax2 && aeta<1.375) {
              ieta=54;
            }
            isamp2=2;
            ieta2=imid;
          }       // end eta>1.352
        }          // end radius selection
      }     // end eta1.4

      // eta between 1.4 and 1.475

      if (aeta>=Eta_max_s1 && aeta<Eta_max) {
        r12 = Rmax1[447];   // radius for end of sampling 1
        r23=Z_max_acc/sinh(aeta);   // radius of end of sampling 2, bounded by high z end

        const double zmax = Z_max_lowr + dzdr*(radius-R_min_acc);

        iregion=1;
        if (radius <=r12) {
          isampling=1;
          ieta=int((aeta-Eta_max_s1)/deta);
          if (z>zmax) { iactive=0; }
        }
        else if (radius < (r12+Dr_s12)) {
          isampling=1;
          ieta=int((aeta-Eta_max_s1)/deta);
          iactive=0;
        }
        else if (radius <= r23) {
          isampling=2;
          ieta=0;
          if (z>zmax) { iactive=0; }
        }
        else {
          isampling=2;
          ieta=0;
          iactive=0;
        }
        isamp2=isampling;
        ieta2=ieta;
      }
      // eta above 1.475, not fiducial region, but still returns something
      //  for calibration hits
      if (aeta>Eta_max) {
        iregion=1;
        r12 = Rmax1[447];
        if (radius <=r12) {
          isampling=1;
          ieta=2;
        }
        else {
          isampling=2;
          ieta=0;
        }
        isamp2=isampling;
        ieta2=ieta;
        iactive=0;
      }

      // cross-check of active region
      if (z>Z_max_acc || radius>R_max_acc || radius<R_min_acc || aeta > Eta_max) iactive=0;

      return iactive;
    }
    // =======================================================================
    //   function findCell
    //
    // compute cell in EM accordion for hit at position x,y,z,radius,eta,phi
    //  given in LOCAL half barrel coordinate system (Stac Geant volume)
    // It has already been checked that the hit is in the accordion sensitive volume
    //

    void Geometry::findCell(CalcData & currentCellData,
                            const double &xPosition,
                            const double &yPosition,
                            const double &zPosition,
                            const double &aRadius,
                            const double &anEta,
                            const double &/*aPhi*/,
                            const bool MapDetail) const
    {

      currentCellData.cellID = 0;

      if (aRadius < m_rc[0] || aRadius >= m_rc[m_Nbrt1-1]) {
#ifdef DEBUGHITS
        ATH_MSG_VERBOSE(" Outside Accordion " << aRadius << " " << m_rc[0] << " " << m_rc[m_Nbrt1-1]);
#endif
        return;    // outside accordion
      }

      // set the straight section number
      currentCellData.nstraight=0;
      for (int i=1;i<m_Nbrt1;i++) {
        if (m_rc[i] > aRadius) { break; }
        currentCellData.nstraight++;
      }
      if (currentCellData.nstraight <0 || currentCellData.nstraight >= m_Nbrt) {
        ATH_MSG_ERROR("Invalid straight number " << currentCellData.nstraight << " " << aRadius);
        return;
      }

      // get the closest fold number
      currentCellData.nfold=currentCellData.nstraight;
      if (std::fabs(aRadius-m_rc[currentCellData.nfold]) > std::fabs(aRadius-m_rc[currentCellData.nfold+1]) ) {
        currentCellData.nfold +=1;
      }
      if (currentCellData.nfold <0 || currentCellData.nfold >= m_Nbrt1) {
        ATH_MSG_ERROR("Invalid fold number " << currentCellData.nfold);
        return;
      }


#ifdef DEBUGHITS
      ATH_MSG_VERBOSE("  BarrelGeometry: radius,eta,phi " << aRadius << " " << anEta << " ");
      ATH_MSG_VERBOSE("  Straight/Fold numbers " << currentCellData.nstraight << " " << currentCellData.nfold);
#endif

      // eta and longitudinal segmentations
      G4int ireg,isamp,ieta,isamp2,ieta2;
      currentCellData.cellID = this->SampSeg(anEta,aRadius,zPosition,ireg,isamp,ieta,isamp2,ieta2);

      currentCellData.etaBin   = ieta;
      currentCellData.sampling = isamp;
      currentCellData.region   = ireg;
      currentCellData.etaMap   = ieta2;
      currentCellData.sampMap  = isamp2;

      // compute electrode number in phi
      int phicell = this->PhiGap(aRadius,xPosition,yPosition);
      if (phicell<0) phicell=0;
      // for test beam, some protection
      if (m_NCellTot !=1024) {
        if (phicell>=m_NCellTot) {
          if (phicell<512) { phicell=m_NCellTot-1; }
          else { phicell=0; }
          currentCellData.cellID=0;
        }
      }

#ifdef DEBUGHITS
      ATH_MSG_VERBOSE(" phigap " << phicell);
#endif

      // compute readout cell number
      int sampling_phi_nGaps=4;
      if (currentCellData.region==0 && currentCellData.sampling==1) { sampling_phi_nGaps=16; }

      if (currentCellData.cellID==0) {
        currentCellData.phiBin = (G4int) ( phicell/sampling_phi_nGaps );
        currentCellData.distElec=9999.;
        return;
      }

      // compute distance to electrode
      G4double xl;
      G4int nstr = currentCellData.nstraight;
      const G4double distElec = this->Distance_Ele(xPosition,yPosition,phicell,nstr,currentCellData.nfold,xl);

#ifdef DEBUGHITS
      ATH_MSG_VERBOSE(" distElec " << distElec);
#endif

      // if distance is < 2.5mm we are sure to be in the correct gap
      if (std::fabs(distElec) > 2.5) {
        // try +-2 electrode in phi to get minimum distance
        double dElecMin=distElec;
        double xlmin=xl;
        int phicellmin=phicell;
        for (int ii=-2;ii<3;ii++) {
          if (ii==0) { continue; }
          int phicellnew = phicell+ii;
          //  for test beam no phi wrapping
          if (m_NCellTot != 1024 && ( phicellnew<0 || phicellnew >= m_NCellTot)) { continue; }
          if (phicellnew < 0) { phicellnew += m_NCellTot; }
          if (phicellnew >= m_NCellTot) { phicellnew -= m_NCellTot; }
          double xln;
          int nstr2=currentCellData.nstraight;
          double dElec = Distance_Ele(xPosition,yPosition,phicellnew,nstr2,currentCellData.nfold,xln);
          if (std::fabs(dElec)<std::fabs(dElecMin)) {
            phicellmin=phicellnew;
            xlmin=xln;
            dElecMin = dElec;
            nstr=nstr2;
          }
        }
        currentCellData.phiGap = phicellmin;
        currentCellData.distElec = dElecMin;
        currentCellData.xl = xlmin;
        currentCellData.nstraight = nstr;
      }       // end distance >2.5mm
      else {
        currentCellData.phiGap=phicell;
        currentCellData.distElec=distElec;
        currentCellData.xl=xl;
        currentCellData.nstraight=nstr;
      }

#ifdef DEBUGHITS
      ATH_MSG_VERBOSE("  final phiGap,distElec,xl " << currentCellData.phiGap << " " << currentCellData.distElec << " "
                      << currentCellData.xl);
#endif

      // compute distance to absorber

      G4int nabs;
      if (currentCellData.distElec<0) nabs=currentCellData.phiGap;
      else nabs=currentCellData.phiGap+1;
      if (nabs >= m_NCellMax) nabs -= m_NCellMax;
      currentCellData.distAbs = Distance_Abs(xPosition,yPosition,nabs,currentCellData.nstraight,currentCellData.nfold);
#ifdef DEBUGHITS
      ATH_MSG_VERBOSE("  nabs,distAbs " << nabs << " " << currentCellData.distAbs);
#endif

      // in some rare cases near fold, the closest distance could give the wrong gap
      //  in such case, the signs of distAbs and distElec are not opposite as they should
      if ((currentCellData.distAbs>0. && currentCellData.distElec>0) ||
          (currentCellData.distAbs<0. && currentCellData.distElec<0) ) {
        //    ATH_MSG_VERBOSE("distElec and distAbs same sign " << currentCellData.distElec << " " << currentCellData.distAbs);
        //    ATH_MSG_VERBOSE(" currentCellData.phiGap " << currentCellData.phiGap);
        if (std::fabs(currentCellData.distElec)>std::fabs(currentCellData.distAbs)) {
          if (currentCellData.distAbs>0) { currentCellData.phiGap += 1; }
          if (currentCellData.distAbs<0) { currentCellData.phiGap -= 1; }
          if (m_NCellTot != 1024) {
            if (currentCellData.phiGap <0) { currentCellData.phiGap=0; }
            if (currentCellData.phiGap >= m_NCellTot) { currentCellData.phiGap = m_NCellTot-1; }
          }
          else {
            if (currentCellData.phiGap < 0) { currentCellData.phiGap += m_NCellTot; }
            if (currentCellData.phiGap >= m_NCellTot) { currentCellData.phiGap -= m_NCellTot; }
          }
          currentCellData.distElec = Distance_Ele(xPosition,yPosition,currentCellData.phiGap,currentCellData.nstraight,currentCellData.nfold,currentCellData.xl);
          //        ATH_MSG_VERBOSE(" new phiGap,distElec " << currentCellData.phiGap << " " << currentCellData.distElec);
        }
      }

      currentCellData.phiBin = (G4int) ( currentCellData.phiGap/sampling_phi_nGaps );

      if (MapDetail) {
        // compute x0,y0 coordinates in local electrode frame, using closest fold
        // as reference
        const G4double alpha = m_coudeelec->PhiRot(currentCellData.nfold,currentCellData.phiGap);
        const G4double dx=xPosition-m_coudeelec->XCentCoude(currentCellData.nfold,currentCellData.phiGap);
        const G4double dy=yPosition-m_coudeelec->YCentCoude(currentCellData.nfold,currentCellData.phiGap);
        const G4double dx1=dx*cos(alpha)-dy*sin(alpha);
        const G4double dy1=dx*sin(alpha)+dy*cos(alpha);
        currentCellData.x0 = dx1 + m_xc[currentCellData.nfold];
        currentCellData.y0 = dy1 + m_yc[currentCellData.nfold];
        if (m_parity==1) { currentCellData.y0 = -1*currentCellData.y0; }
      }


    } // end of findCell method

    // =============================================================================
    //  initialize phi0 vs radius of first absorber (for gam=0)
    void Geometry::GetRphi()
    {
      const G4double dl=0.001;
      const G4double inv_dl = 1. / dl;
      G4double cenx[15],ceny[15];
      G4double xl,xl2;
      G4double sum1[5000],sumx[5000];
      xl=0;
      xl2=0.;
      m_NRphi=5000;
      m_Rmin=1500.;
      m_dR=0.10;
      m_Rmax=0.;

      const G4double rint= m_rint_eleFib;
      const G4double inv_rint = 1. / rint;
      const G4double dt=dl * inv_rint;
      const G4double inv_dt = 1. / dt;

      for (G4int i=0;i<m_NRphi;i++) {
        sum1[i]=0.;
        sumx[i]=0.;
      }
      for (G4int i=0;i<15;i++) {
        cenx[i]=m_rc[i]*cos(m_phic[i]);
        ceny[i]=m_rc[i]*sin(m_phic[i]);
      }

      for (G4int i=0; i<15; i++) {

        // fold
        G4double phi0,phi1;
        if (i==0) {
          // first fold goes up
          if (m_parity==0) {
            phi0=-CLHEP::pi/2.;
            phi1=-m_delta[0];
          }
          // first fold goes down
          else {
            phi0=m_delta[0];
            phi1=CLHEP::pi/2;
          }
        }
        else if (i==14) {
          if (m_parity==0) {
            phi0=-CLHEP::pi+m_delta[13];
            phi1=-CLHEP::pi/2.;
          }
          else {
            phi0=CLHEP::pi/2;
            phi1=CLHEP::pi - m_delta[13];
          }
        }
        else {
          if (i%2==(1-m_parity)) {
            phi0=m_delta[i];
            phi1=CLHEP::pi-m_delta[i-1];
          }
          else {
            phi0=-CLHEP::pi+m_delta[i-1];
            phi1=-m_delta[i];
          }
        }
        xl2+=rint*std::fabs(phi1-phi0);
        const G4int nstep=int((phi1-phi0)*inv_dt)+1;
        for (int ii=0;ii<nstep;ii++) {
          xl+=dl;
          const G4double phi=phi0+dt*((G4double)ii);
          const G4double x=cenx[i]+rint*cos(phi);
          const G4double y=ceny[i]+rint*sin(phi);
          const G4double radius=sqrt(x*x+y*y);
          if (radius>m_Rmax) { m_Rmax=radius; }
          const G4double phid=atan(y/x);
          const G4int ir=((int) ((radius-m_Rmin)/m_dR) );
          if (ir>=0 && ir < m_NRphi) {
            sum1[ir]+=1.;
            sumx[ir]+=phid;
          }
        }

        // straight section
        if (i<14) {
          const G4double dx=cenx[i+1]-cenx[i];
          const G4double dy=ceny[i+1]-ceny[i];
          const G4double along=std::sqrt(dx*dx+dy*dy-4.*rint*rint);
          const G4double x0=0.5*(cenx[i+1]+cenx[i]);
          const G4double y0=0.5*(ceny[i+1]+ceny[i]);
          const G4double phi = (i%2==m_parity) ? CLHEP::pi/2-m_delta[i] : -CLHEP::pi/2.+m_delta[i];
          const G4double x1=x0-0.5*along*cos(phi);
          const G4double y1=y0-0.5*along*sin(phi);
          xl2+=along;
          const int nstep=int(along*inv_dl)+1;
          for (int ii=0;ii<nstep;ii++) {
            xl+=dl;
            const G4double x=x1+dl*((G4double)ii)*cos(phi);
            const G4double y=y1+dl*((G4double)ii)*sin(phi);
            const G4double radius=sqrt(x*x+y*y);
            if (radius>m_Rmax) { m_Rmax=radius; }
            const G4double phid=atan(y/x);
            const G4int ir=((int) ((radius-m_Rmin)/m_dR) );
            if (ir>=0 && ir < m_NRphi) {
              sum1[ir]+=1.;
              sumx[ir]+=phid;
            }
          }
        }
      }
      //  ATH_MSG_VERBOSE("total electrode lenght " << xl << " " << xl2);
      //  ATH_MSG_VERBOSE("rmax in accordion " << m_Rmax);
      for (int i=0; i<m_NRphi; i++) {
        if (sum1[i]>0) {
          m_Rphi[i]=sumx[i]/sum1[i];
          // Not used:
          //G4double radius = m_Rmin + ((G4double(i))+0.5)*m_dR;
          //ATH_MSG_VERBOSE(" GUTEST  r,phi0 " << radius << " " << m_Rphi[i]);
        }
        else { m_Rphi[i]=0.; }
      }
    }

    // ======================================================================================
    // phi of first absorber as function of radius for nominal accordion geometry
    //  (before sagging)
    G4double Geometry::Phi0(G4double radius) const
    {
      // TODO This function could be simplified.
      G4int ir;
      if (radius < m_Rmin) { ir=0; }
      else {
        if (radius > m_Rmax) radius=m_Rmax-0.0001;
        ir=((int) ((radius-m_Rmin)/m_dR) );
      }
      return m_Rphi[ir];
    }

    // ======================================================================================
    // compute number (0 to 1023) of closest electrode according to nominal
    // accordion geometry
    G4int Geometry::PhiGap(const double & radius, const double & xhit, const double &yhit) const
    {
      const G4double phi0=Phi0(radius)+m_gam0;   // from -pi to pi
      const G4double phi_hit=atan2(yhit,xhit);  // from -pi to pi
      G4double dphi=phi_hit-phi0;
      // bring back to 0-2pi
      if (dphi<0) dphi=dphi+2*M_PI;
      if (dphi>=2*M_PI) dphi=dphi-2*M_PI;
      dphi=dphi/(2*M_PI)*1024;
      const G4int ngap=((int) dphi);
#ifdef DEBUGHITS
      ATH_MSG_VERBOSE(" phi0 " << phi0 << " dphi, ngap " << dphi << " " << ngap);
#endif
      return ngap;
    }

    //===================================================================================
    // full cell id computation starting from an arbitrary G4 step

    LArG4Identifier Geometry::CalculateIdentifier(const G4Step* a_step) const
    {

      // The default result is a blank identifier.
      LArG4Identifier result = LArG4Identifier();

      // Get all the required information from the current step
      const G4NavigationHistory* g4navigation = a_step->GetPreStepPoint()->GetTouchable()->GetHistory();
      const G4int ndep = g4navigation->GetDepth();
      bool inSTAC = false;
      int zside=1;
      G4int indECAM = -999;

      // Now navigate through the volumes hierarchy
      for (G4int ii=0;ii<=ndep;ii++) {
        const G4String& vname = g4navigation->GetVolume(ii)->GetName();
        // FIXME Need to find a way to avoid these string-comparisons
        if ( indECAM<0 && vname == m_ecamName ) indECAM=ii;
        if ( !inSTAC && vname.find("STAC") !=std::string::npos) inSTAC=true;
        if ( vname.find("NegPhysical") != std::string::npos) zside=-1;
      }
      if (indECAM>=0)
        result = this->CalculateECAMIdentifier( a_step , indECAM, inSTAC, zside) ;
      else
        ATH_MSG_ERROR("LArBarrel::Geometry::CalculateIdentifier  ECAM volume not found in hierarchy");

      return result;
    }

    //======================================================================================
    //
    // The following method computes the identifiers in the ECAM volume:
    //  (including dead material identifier)
    //
    // 1) Tranformation into LOCAL half barrel frame
    //
    // 2) Check if the hit is in the fiducial region (EM accordion, no presampler)
    //    fiducial range:   1500.24 < r < 1960.00 mm
    //                        |eta| < 1.475
    //                            4 < z < 3164 mm      (in local half barrel coordinates)
    //
    // 3) If the hit is in the fiducial region standard identifier are filled
    //
    // 4) If the hit is outside the fiduacial region calibration hits are filled
    //
    //    CaloDM_ID identifier for the barrel:
    //
    //        detector_system/subdet/type/sampling/region/eta/phi
    //
    //    * For hits with radius < 1500.24
    //      ******************************
    //
    //        detector system = 10   -> Calorimeters
    //        subdet          = +/-4 -> LAr dead materials
    //        type            = 1    -> dead materials outside accordion and active presampler layers
    //        sampling        = 1    -> dead materials in front and in active LAr calorimeters (starting from front warm
    //                                  cryostat walls)
    //        regions:        = 3    -> all materials from the active layer of the barrel presampler to the active layer
    //                                  of accordion, 0 < |eta| < 1.5
    //
    //        ---> Granularity : deta       0.1          granularity within region
    //                           dphi       pi/32 ~ 0.1  granularity within region
    //
    //    * For hits with radius > 1960.00
    //      ******************************
    //
    //        detector system = 10   -> Calorimeters
    //        subdet          = +/-4 -> LAr dead materials
    //        type            = 1    -> dead materials outside accordion and active presampler layers
    //        sampling        = 2    -> dead materials between active LAr calorimeters and Tile calorimeters or HEC-2 wheels
    //        regions:        = 0    -> all materials behind the active layer of accordion in front the Tile barrel
    //                                  for |eta| < 1.0
    //                        =2     -> all materials in front of the scintillator and behind the active layer of accordion
    //                                  for 1.0 < |eta| < 1.5
    //
    //        ---> Granularity : deta       0.1          granularity within region
    //                           dphi       pi/32 ~ 0.1  granularity within region
    //
    //======================================================================================

    LArG4Identifier Geometry::CalculateECAMIdentifier(const G4Step* a_step, const G4int indECAM, const bool inSTAC, int zside) const
    {

      LArG4Identifier result = LArG4Identifier();;

      // Get all the information about the step

      const G4StepPoint *thisStepPoint = a_step->GetPreStepPoint();
      const G4StepPoint *thisStepBackPoint = a_step->GetPostStepPoint();
      const G4ThreeVector startPoint = thisStepPoint->GetPosition();
      const G4ThreeVector endPoint = thisStepBackPoint->GetPosition();
      const G4ThreeVector p = (thisStepPoint->GetPosition() + thisStepBackPoint->GetPosition()) * 0.5;

#ifdef  DEBUGHITS
      ATH_MSG_VERBOSE("Position of the step in the ATLAS frame (x,y,z) --> " << p.x() << " " << p.y() << " " << p.z());
      ATH_MSG_VERBOSE("Eta and Phi in the ATLAS frame                  --> " << p.eta() << " " << p.phi());
#endif

      // BACK directly into the LOCAL half_Barrel. All the variables in this LOCAL framework get the SUFFIX  Zpos

      const G4NavigationHistory* g4navigation = thisStepPoint->GetTouchable()->GetHistory();
      const G4AffineTransform transformation = g4navigation->GetTransform(indECAM);
      const G4ThreeVector startPointinLocal = transformation.TransformPoint(startPoint);
      const G4ThreeVector endPointinLocal = transformation.TransformPoint  (endPoint);
      const G4ThreeVector midinLocal = (startPointinLocal+endPointinLocal)*0.5;

#ifdef  DEBUGHITS
      ATH_MSG_VERBOSE("Position of the step in the LOCAL frame (x,y,z) --> " << midinLocal.x() << " " << midinLocal.y() << " " << midinLocal.z());
      ATH_MSG_VERBOSE("Eta and Phi of the step in LOCAL frame          --> " << midinLocal.eta() << " " << midinLocal.phi());
#endif

      // coordinates in the local frame

      const G4double xZpos   = midinLocal.x();
      const G4double yZpos   = midinLocal.y();
      const G4double zZpos   = midinLocal.z();
      const G4double etaZpos = midinLocal.pseudoRapidity();
      const G4double phiZpos = (midinLocal.phi()<0.) ? midinLocal.phi() + 2.*M_PI : midinLocal.phi();
      const G4double radius2Zpos = xZpos*xZpos + yZpos*yZpos;
      const G4double radiusZpos = sqrt(radius2Zpos);

      CalcData currentCellData;
      if (m_testbeam) {
        currentCellData.zSide = 1;
      }
      else {
        currentCellData.zSide = zside;
      }

      // Check if the hit is in the fiducial range and in the STAC volume
      //  if yes this is active or inactive material

      if (inSTAC && radiusZpos >=m_rMinAccordion && radiusZpos <= m_rMaxAccordion &&
          zZpos <= m_zMaxBarrel && zZpos >= m_zMinBarrel && etaZpos <= m_etaMaxBarrel) {

#ifdef  DEBUGHITS
        ATH_MSG_VERBOSE("This hit is in the STAC volume !!!!! ");
#endif

        //   DETERMINATION of currentCellData.cellID,
        //   currentCellData.zSide, currentCellData.sampling,
        //   currentCellData.phiBin, currentCellData.etaBin,
        //   m_stackNumID
        const bool MapDetail(false);
        this->findCell( currentCellData, xZpos, yZpos, zZpos, radiusZpos, etaZpos, phiZpos, MapDetail );

        // adjust phi in the negative half barrel frame

        if( currentCellData.zSide == -1 )
          {
            if( currentCellData.sampling == 1 && currentCellData.region ==0 )
              {
                currentCellData.phiBin = 31 - currentCellData.phiBin;
                if(currentCellData.phiBin < 0 ) currentCellData.phiBin += 64;
              }
            if( currentCellData.sampling == 1 && currentCellData.region ==1 )
              {
                currentCellData.phiBin = 127 - currentCellData.phiBin;
                if(currentCellData.phiBin < 0 ) currentCellData.phiBin += 256;
              }
            if( currentCellData.sampling >= 2 )
              {
                currentCellData.phiBin = 127 - currentCellData.phiBin;
                if(currentCellData.phiBin < 0 ) currentCellData.phiBin += 256;
              }
          }

        // there are few hundred microns between the 4mm nominal beginning of the active region
        //  and the real beginning of the first strip at eta=0.025/8
        //  to avoid inactive energy with strip=0 assign this to strip=1
        if (currentCellData.sampling==1 && currentCellData.region==0 && currentCellData.etaBin==0) {
          currentCellData.etaBin=1;
        }

        result << 4          // LArCalorimeter
               << 1          // LArEM
               << currentCellData.zSide
               << currentCellData.sampling
               << currentCellData.region
               << currentCellData.etaBin
               << currentCellData.phiBin;

#ifdef  DEBUGHITS
        ATH_MSG_VERBOSE("Here the identifier for the barrel ACTIVE ----> ");
        ATH_MSG_VERBOSE("eta in local frame --> " << etaZpos);
        ATH_MSG_VERBOSE("currentCellData.zSide  ----> " << currentCellData.zSide);
        ATH_MSG_VERBOSE("currentCellData.sampling  ----> " << currentCellData.sampling);
        ATH_MSG_VERBOSE("currentCellData.region  ----> " <<  currentCellData.region);
        ATH_MSG_VERBOSE("currentCellData.etaBin  ----> " << currentCellData.etaBin);
        ATH_MSG_VERBOSE("currentCellData.phiBin  ----> " << currentCellData.phiBin);
        ATH_MSG_VERBOSE("And also etafirst ----> " << thisStepPoint->GetPosition().pseudoRapidity());
#endif

        //    if (!Geometry::CheckLArIdentifier(currentCellData.sampling,currentCellData.region,currentCellData.etaBin,currentCellData.phiBin)) {
        //      ATH_MSG_ERROR(" **  Bad LAr identifier " << currentCellData.sampling << " " << currentCellData.region << " "
        //                << currentCellData.etaBin << " " << currentCellData.phiBin);
        //      ATH_MSG_ERROR(" x,y,z,eta,phi " <<  xZpos << " " << yZpos << " " << zZpos
        //                << " " << radiusZpos << " " << etaZpos << " " << phiZpos);
        //    }


      }
      // hits in dead material part
      else {

        G4int sampling=0;
        G4int region=0;
        const G4int numDeadPhiBins = 64;
        double abs_eta = std::fabs(etaZpos);
        const double DM1EtaWidth = 0.1 ;
        const double DM1PhiWidth = 2.*M_PI / numDeadPhiBins ;
        currentCellData.etaBin = (G4int) ( abs_eta * (1./DM1EtaWidth) ) ;
        currentCellData.phiBin = (G4int) (phiZpos/ DM1PhiWidth );
        G4int type=1;
        // protect against rounding error for phi ~2pi
        if (currentCellData.phiBin==numDeadPhiBins) currentCellData.phiBin=currentCellData.phiBin-1;

        // adjust phi for negative half barrel

        if ( currentCellData.zSide == -1 ) {
          currentCellData.phiBin = 31 - currentCellData.phiBin;
          if (currentCellData.phiBin < 0 ) currentCellData.phiBin +=64 ;
        }

        // material in front of the active accordion
        if ( radiusZpos < m_rMinAccordion ) {
          sampling =1 ;
          region = 3 ;
          if (currentCellData.etaBin > 14) currentCellData.etaBin=14;

#ifdef  DEBUGHITS
          ATH_MSG_VERBOSE("This hit is in the ECAM volume in front of the accordion (DEAD MATERIAL) !!!!! ");
#endif

        } else if (radiusZpos >= m_rMaxAccordion){  // material behind the active accordion
          sampling = 2;

          if (abs_eta < 1.0 ) {
            region = 0 ;
#ifdef  DEBUGHITS
            ATH_MSG_VERBOSE("This hit is in the ECAM volume behind accordion (DEAD MATERIAL 0)  !!!!! ");
#endif
          } else if ( abs_eta >= 1.0 && abs_eta < 1.5) {
            region = 2;
            currentCellData.etaBin = currentCellData.etaBin - 10;    // to have etabin between 0 and 4
#ifdef  DEBUGHITS
            ATH_MSG_VERBOSE("This hit is in the ECAM volume behind accordion (DEAD MATERIAL 2) !!!!! ");
#endif
          } else {
            ATH_MSG_ERROR(" LArBarrelGeometry: hit behind accordion at eta>1.5 !!! ");
            region = 2;
            currentCellData.etaBin = 4;
          }

        } else if (zZpos <= m_zMinBarrel) {   // inactive matter between two EMB halves
          type=2;
          region=0;
          const G4int phisave=currentCellData.phiBin;
          const G4bool MapDetail(false);
          this->findCell( currentCellData, xZpos, yZpos, zZpos, radiusZpos, etaZpos, phiZpos, MapDetail );
          sampling = currentCellData.sampling; // sampling as in normal definition
          currentCellData.etaBin=0;
          currentCellData.phiBin=phisave;

        } else if (zZpos >= m_zMaxBarrel || abs_eta >= 1.40) { // inactive matter between EMB and scintillator
          if (abs_eta >1.0 && abs_eta < 1.5) {
            sampling=2;
            region=2;
            currentCellData.etaBin = currentCellData.etaBin - 10;    // to have etabin between 0 and 4
          } else if (abs_eta < 1.6) {
            sampling=1;
            region=4;
            currentCellData.etaBin=0;
          } else {
            ATH_MSG_ERROR(" LArBarrelGeometry: hit at eta>1.6 !!! ");
            sampling=1;
            region=4;
            currentCellData.etaBin=0;
          }
        } else {
          if (!m_testbeam) {
            ATH_MSG_ERROR("LArBarrelGeometry: cannot find region for DM hit...");
            ATH_MSG_ERROR("r,z,eta,phi " << radiusZpos << " " << zZpos << " " << etaZpos << " " << phiZpos);
            ATH_MSG_ERROR("x,y,z (Atlas) " << p.x() << " " << p.y() << " " << p.z());
            ATH_MSG_ERROR(" inSTAC " << inSTAC);
            const G4double thisStepEnergyDeposit = a_step->GetTotalEnergyDeposit() * a_step->GetTrack()->GetWeight();
            ATH_MSG_ERROR(" eDeposited " << thisStepEnergyDeposit);
            const G4VPhysicalVolume* vol = thisStepPoint->GetPhysicalVolume();
            const G4String volName = vol->GetName();
            ATH_MSG_ERROR(" volName " << volName);
            const G4int ndep = g4navigation->GetDepth();
            for (G4int ii=0;ii<=ndep;ii++) {
              const G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
              const G4String vname = v1->GetName();
              ATH_MSG_ERROR("vname " << vname);
            }

          }
          // in test beam case, we can get there for leakage on the side (in phi) of the module
          // in this case, we attribute an identifier like inactive material
          else
            {
              G4bool MapDetail=false;
              this->findCell( currentCellData, xZpos, yZpos, zZpos, radiusZpos, etaZpos, phiZpos, MapDetail );
              //         ATH_MSG_ERROR(" Lateral lakage r,eta,phi " << radiusZpos << " " << etaZpos << " "
              //                   << phiZpos << "  sampling/region/eta/phi " << currentCellData.sampling << " " <<
              //              currentCellData.region << " " << currentCellData.etaBin << " " << currentCellData.phiBin);
              // protect against small space between z=4m and real beginning of ieta=1 in strips
              if (currentCellData.sampling==1 && currentCellData.region==0 && currentCellData.etaBin==0) {
                currentCellData.etaBin=1;
                //            ATH_MSG_ERROR("S1R0 etabin 0 found  r,z,phi local " << radiusZpos << " "
                //                   << " " << zZpos << " " << phiZpos);
              }
              result << 4          // LArCalorimeter
                     << 1          // LArEM
                     << currentCellData.zSide
                     << currentCellData.sampling
                     << currentCellData.region
                     << currentCellData.etaBin
                     << currentCellData.phiBin;
              return result;
            }
        }

        result << 10             // LArCalorimeter
               << currentCellData.zSide * 4     // LArBarrel
               << type
               << sampling
               << region
               << currentCellData.etaBin
               << currentCellData.phiBin;

#ifdef  DEBUGHITS
        ATH_MSG_VERBOSE("Here the identifier for the barrel DEAD materials ---->");
        ATH_MSG_VERBOSE("Type     ----> " << type);
        ATH_MSG_VERBOSE("Sampling ----> " << sampling);
        ATH_MSG_VERBOSE("Region   ----> " << region);
        ATH_MSG_VERBOSE("zSide  ----> "   << currentCellData.zSide*4);
        ATH_MSG_VERBOSE("etaBin   ----> " << currentCellData.etaBin);
        ATH_MSG_VERBOSE("phiBin   ----> " << currentCellData.phiBin);
#endif

        //    if (!Geometry::CheckDMIdentifier(type,sampling,region,currentCellData.etaBin,currentCellData.phiBin)) {
        //        ATH_MSG_ERROR(" **  Bad DM identifier " << type << " " << sampling << " " << region << " "
        //                  << currentCellData.etaBin << " " << currentCellData.phiBin);
        //        ATH_MSG_ERROR("x,y,z,r,eta,phi" << xZpos << " " << yZpos << " " << zZpos <<
        //         " " << radiusZpos << " " << etaZpos << " " << phiZpos);
        //    }

      }

      return result;

    }

    bool Geometry::CheckDMIdentifier(int type, int sampling, int region, int eta, int phi) const
    {

      if (type <1 || type > 2) return false;
      if (type==1) {
        if (sampling<1 || sampling>2) return false;
        if (sampling==1) {
          if (region!=3 && region !=4) return false;
          if (phi<0 || phi>63) return false;
          if (region==3) {
            if (eta<0 || eta>14) return false;
          }
          if (region==4) {
            if (eta !=0) return false;
          }
        }
        if (sampling==2) {
          if (region !=0 && region !=2) return false;
          if (phi<0 || phi>63) return false;
          if (region==0){
            if (eta<0 || eta>9) return false;
          }
          if (region==2) {
            if (eta<0 || eta>4) return false;
          }
        }
      }
      if (type==2) {
        if (sampling<1 || sampling >3) return false;
        if (region !=0) return false;
        if (eta!=0) return false;
        if (phi<0 || phi>63) return false;
      }
      return true;
    }


    bool Geometry::CheckLArIdentifier(int sampling, int region, int eta, int phi) const
    {
      if (sampling<0 || sampling >3) return false;
      if (sampling==0) {
        if (region!=0) return false;
        if (eta<0 || eta>60) return false;
        if (phi<0 || phi>63) return false;
      }
      if (sampling==1) {
        if (region<0 || region >1) return false;
        if (region==0) {
          if (eta<1 || eta>447) return false;
          if (phi<0 || phi>63) return false;
        }
        if (region==1) {
          if (eta<0 || eta>2) return false;
          if (phi<0 || phi>255) return false;
        }
      }
      if (sampling==2) {
        if (region<0 || region >1) return false;
        if (region==0) {
          if (eta<0 || eta>55) return false;
          if (phi<0 || phi>255) return false;
        }
        if (region==1) {
          if (eta!=0) return false;
          if (phi<0 || phi>255) return false;
        }
      }
      if (sampling==3) {
        if (region !=0) return false;
        if (eta<0 || eta>26) return false;
        if (phi<0 || phi>255) return false;
      }
      return true;
    }

  } //end of Barrel namespace

} // end of LArG4 namespace
