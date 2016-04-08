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
#include "LArG4Barrel/LArBarrelGeometry.h"

#include "LArG4Barrel/LArStraightAbsorbers.h"
#include "LArG4Barrel/LArStraightElectrodes.h"
#include "LArG4Barrel/LArCoudeElectrodes.h"
#include "LArG4Barrel/LArCoudeAbsorbers.h"

namespace LArG4 {
  
namespace Barrel {

Geometry* Geometry::m_instance = 0;

// ===================================================================

Geometry* Geometry::GetInstance()
{
  if (m_instance == 0)
    {
      m_instance = new Geometry();
    }
  return m_instance;
}

// ==========================================================================
Geometry::Geometry()
  : m_testbeam(false),
    m_cellID(0), m_sampling(0), m_region(0), m_etaBin(0), m_phiBin(0), m_zSide(0), m_phiGap(0), m_nstraight(0), m_nfold(0), m_distElec(0), m_distAbs(0), m_xl(0), m_x0(0), m_y0(0), m_sampMap(0), m_etaMap(0),
    m_iflSAG(false)
{
// Constructor initializes the geometry.
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
  for (G4int idat = 0; idat < m_Nbrt1 ; idat++) 
  {
      m_rc[idat] = (double) parameters->GetValue("LArEMBRadiusAtCurvature",idat);
      m_phic[idat] = (double) parameters->GetValue("LArEMBPhiAtCurvature",idat);
      m_delta[idat] = (double) parameters->GetValue("LArEMBDeltaZigAngle",idat);
      m_xc[idat] = m_rc[idat]*cos(m_phic[idat]);
      m_yc[idat] = m_rc[idat]*sin(m_phic[idat]);
  }
  // define parity of accordion waves: =0 if first wave goes up, 1 if first wave goes down in the local frame
  m_parity=0;
  if (m_phic[0]<0.) m_parity=1;
//
  m_rMinAccordion  =   parameters->GetValue("LArEMBRadiusInnerAccordion");
  m_rMaxAccordion  =   parameters->GetValue("LArEMBFiducialRmax");
  m_etaMaxBarrel   =   parameters->GetValue("LArEMBMaxEtaAcceptance");
  m_zMinBarrel     =   parameters->GetValue("LArEMBfiducialMothZmin");
  m_zMaxBarrel     =   parameters->GetValue("LArEMBfiducialMothZmax");  
// === GU 11/06/2003   total number of cells in phi
// to distinguish 1 module (testbeam case) from full Atlas
  m_NCellTot = (int) (parameters->GetValue("LArEMBnoOFPhysPhiCell"));
  m_NCellMax=1024;
// ===

// Initialize r-phi reference map
  GetRphi();  

  m_FIRST = true;
  m_coudeabs=NULL;
  m_absorber=NULL;
  m_coudeelec=NULL;
  m_electrode=NULL;

}

// ====================================================================================

Geometry::~Geometry() {

  if (m_rc) delete [] m_rc;
  if (m_phic) delete [] m_phic;
  if (m_delta) delete [] m_delta;
  if (m_xc) delete [] m_xc;
  if (m_yc) delete [] m_yc;

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
                    const int &Num_Coude, double &xl)
{
  double dx, dy, dr;
  double DistEle = 0.;
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
    G4double Xc[2];
    Xc[0] = m_coudeelec->XCentCoude(Num_Coude, PhiCell);
    Xc[1] = m_coudeelec->YCentCoude(Num_Coude, PhiCell);
    G4double radfold = sqrt(Xc[0]*Xc[0]+Xc[1]*Xc[1]);
    G4double radhit = sqrt(xhit*xhit+yhit*yhit);

// check if the assumed straight number is the correct one
//   (this can be wrong when we are close to a fold and there is sagging)
  if (Num_Coude == Num_Straight && radhit <radfold) {
     if (Num_Straight>0) Num_Straight = Num_Straight-1;
//     std::cout << "radhit,radfold " << radhit << " " << radfold << " change straight by +1" << std::endl;
  }
  if (Num_Coude == (Num_Straight+1) && radhit > radfold) {
     if (Num_Straight<12) Num_Straight = Num_Straight+1;
//     std::cout << "radhit,radfold " << radhit << " " << radfold << " change straight by -1" << std::endl;
  }

// u unit 2D_Vector along straight part of the electrode neutral fiber 
  double u[2];
  u[0] = m_electrode->Cosu(Num_Straight, PhiCell);
  u[1] = m_electrode->Sinu(Num_Straight, PhiCell);
// Middle m_coordinates of this straight part of the electrode neutral fiber
  double Xm[2];
  Xm[0] = m_electrode->XCentEle(Num_Straight, PhiCell);
  Xm[1] = m_electrode->YCentEle(Num_Straight, PhiCell);
// m_Hit Vector components
  dx = xhit - Xm[0];  dy = yhit - Xm[1];

// First compute algebric distance m_hit (2D) the 2D_projection of the
// m_Hit Vector on this electrode neutral fiber.
  double hit = dx*u[0] + dy*u[1];
  
//
// Flat of Fold Region ?
//
  G4double Half_Elec;
  Half_Elec = m_electrode->HalfLength(Num_Straight,PhiCell); 

  if(fabs(hit) < Half_Elec) {
// Flat Region
    DistEle = u[0]*dy - u[1]*dx;
    xl=hit/Half_Elec;
  }
  else {
// Fold region
// c_Hit Vector components and its length
    dx = xhit - Xc[0];  dy = yhit - Xc[1];  dr = sqrt( dx*dx + dy*dy);
    DistEle = (Num_Coude%2 == m_parity) ? (m_rint_eleFib-dr) : (dr - m_rint_eleFib);
    if (Num_Coude==Num_Straight) xl=-1.;
    else xl=+1;
  }      // end of Fold Regions

  return DistEle;

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
                    const int &Num_Coude)
{
  double dx, dy, dr;
  double DistAbs = 0.;
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
  G4double u[2];
  u[0] = m_absorber->Cosu(Num_Straight, PhiCell);
  u[1] = m_absorber->Sinu(Num_Straight, PhiCell);
// Middle m_coordinates of this straight part of the electrode neutral fiber
  G4double Xm[2];
  Xm[0] = m_absorber->XCentAbs(Num_Straight, PhiCell);
  Xm[1] = m_absorber->YCentAbs(Num_Straight, PhiCell);
// m_Hit Vector components
  dx = xhit - Xm[0];  dy = yhit - Xm[1];

// First compute algebric distance hit (2D) the 2D_projection of the
// m_Hit Vector on this electrode neutral fiber.
  double hit = dx*u[0] + dy*u[1];
  
//
// Flat of Fold Region ?
//
  G4double Half_Abs;
  Half_Abs = m_absorber->HalfLength(Num_Straight,PhiCell); 

  if(fabs(hit) < Half_Abs) {
// Flat Region
    DistAbs = u[0]*dy - u[1]*dx;
  }
  else {
// Fold Center c_coordinates
    G4double Xc[2];
    Xc[0] = m_coudeabs->XCentCoude(Num_Coude, PhiCell);
    Xc[1] = m_coudeabs->YCentCoude(Num_Coude, PhiCell);
// c_Hit Vector components and its length
    dx = xhit - Xc[0];  dy = yhit - Xc[1];  dr = sqrt( dx*dx + dy*dy);
    DistAbs = (Num_Coude%2 == m_parity) ? (m_rint_eleFib-dr) : (dr - m_rint_eleFib);

  }      // end of Fold Regions

  return DistAbs;

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
                        G4int& isamp2, G4int& ieta2)
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

//     std::cout << "Initialization of SampSet " << std::endl;
//     std::cout << " Rmin/Rmax      " << R_min_acc << " " << R_max_acc << std::endl;
//     std::cout << " Zmax/Zmax_lowR " << Z_max_acc << " " << Z_max_lowr << std::endl;
//     std::cout << " Rmin_highz     " << R_min_highz << std::endl;
//     std::cout << " dzdr           " << dzdr << std::endl;

     FILL=false;
 };

// iactive = 1 if active region, 0 if region considered as inactive
  G4int iactive = 1;

  G4double aeta=fabs(eta);

  G4double r12=-1.;
  G4double r23=-1.;

  // Not used: G4double rmax=Z_max_acc/sinh(aeta);

  G4int istrip,imid;

// eta < 1.4

  if (aeta<Eta_max_s1) {

// get radius for end of strips
    istrip=(int) (aeta/deta*8.);
    if (istrip<0 || istrip >=448) {
      std::cout << " Problem aeta,istrip " << aeta << " " << istrip << std::endl;
      return 0;
    }
    r12=Rmax1[istrip];

// get radius for end of middle
    imid = (int) (aeta/deta);
    if (imid <0 || imid >=56) {
      std::cout << " Problem aeta,imid " << aeta << " " << imid << std::endl;
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
            double etastr;
            if (imid%2==0) etastr=0.025*imid;
            else           etastr=0.025*(imid+1);
            double delta=radius*(sinh(etastr)-sinh(aeta))/cosh(etastr);
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

     double zmax = Z_max_lowr + dzdr*(radius-R_min_acc);

     iregion=1; 
     if (radius <=r12) {
        isampling=1;
        ieta=int((aeta-Eta_max_s1)/deta);
        if (z>zmax) iactive=0;
     }
     else if (radius < (r12+Dr_s12)) {
        isampling=1;
        ieta=int((aeta-Eta_max_s1)/deta);
        iactive=0;
     }
     else if (radius <= r23) {
        isampling=2;
        ieta=0;
        if (z>zmax) iactive=0;
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

void Geometry::findCell(const double &xPosition,
	  	 	const double &yPosition,
		 	const double &zPosition,
			const double &aRadius, 
			const double &anEta, 
			const double &/*aPhi*/,
		        const bool MapDetail,
                        std::string strDetector)
{

  if (m_FIRST) {
// get pointers to access G4 geometry
    m_electrode = LArStraightElectrodes::GetInstance(strDetector);
    m_absorber  = LArStraightAbsorbers::GetInstance(strDetector);
    m_coudeelec = LArCoudeElectrodes::GetInstance(strDetector);
    m_coudeabs  = LArCoudeAbsorbers::GetInstance(strDetector);
    m_FIRST = false;
  }

  m_cellID = 0;

  if (aRadius < m_rc[0] || aRadius >= m_rc[m_Nbrt1-1]) {
#ifdef DEBUGHITS
    std::cout << " Outside Accordion " << aRadius << " " << m_rc[0] << " " << m_rc[m_Nbrt1-1] << std::endl;
#endif
    return;    // outside accordion
  }

// set the straight section number
  m_nstraight=0;
  for (int i=1;i<m_Nbrt1;i++) {
    if (m_rc[i] > aRadius) break;
    m_nstraight++; 
  }
  if (m_nstraight <0 || m_nstraight >= m_Nbrt) {
    std::cout << "Invalid straight number " << m_nstraight << " " << aRadius << std::endl;
    return;
  }

// get the closest fold number
  m_nfold=m_nstraight;
  if (fabs(aRadius-m_rc[m_nfold]) > fabs(aRadius-m_rc[m_nfold+1]) ) m_nfold +=1;
  if (m_nfold <0 || m_nfold >= m_Nbrt1) {
    std::cout << "Invalid fold number " << m_nfold << std::endl;
    return;
  }
  

#ifdef DEBUGHITS
  std::cout << "  BarrelGeometry: radius,eta,phi " << aRadius << " " << anEta << " " << aPhi << std::endl;
  std::cout << "  Straight/Fold numbers " << m_nstraight << " " << m_nfold << std::endl;
#endif
     
// eta and longitudinal segmentations
  G4int ireg,isamp,ieta,isamp2,ieta2;
  m_cellID = SampSeg(anEta,aRadius,zPosition,ireg,isamp,ieta,isamp2,ieta2); 

  m_etaBin   = ieta;
  m_sampling = isamp;
  m_region   = ireg;
  m_etaMap   = ieta2;
  m_sampMap  = isamp2;

// compute electrode number in phi 
  int phicell = PhiGap(aRadius,xPosition,yPosition);
  if (phicell<0) phicell=0;
// for test beam, some protection
  if (m_NCellTot !=1024) {
    if (phicell>=m_NCellTot) {
      if (phicell<512) phicell=m_NCellTot-1;
      else phicell=0;
      m_cellID=0;
    }
  }

#ifdef DEBUGHITS
  std::cout << " phigap " << phicell << std::endl;
#endif

// compute readout cell number
  int sampling_phi_nGaps=4;
  if (m_region==0 && m_sampling==1) sampling_phi_nGaps=16;

  if (m_cellID==0) {
      m_phiBin = (G4int) ( phicell/sampling_phi_nGaps );
      m_distElec=9999.;
      return;
  }

// compute distance to electrode
  G4double xl;
  G4int nstr = m_nstraight;
  G4double distElec = Distance_Ele(xPosition,yPosition,phicell,nstr,m_nfold,xl);

#ifdef DEBUGHITS
  std::cout << " distElec " << distElec << std::endl;
#endif

// if distance is < 2.5mm we are sure to be in the correct gap
  if (std::fabs(distElec) > 2.5) {
// try +-2 electrode in phi to get minimum distance
      double dElecMin=distElec;
      double xlmin=xl;
      int phicellmin=phicell;
      for (int ii=-2;ii<3;ii++) {
        if (ii==0) continue;
        int phicellnew = phicell+ii;
//  for test beam no phi wrapping
        if (m_NCellTot != 1024 && ( phicellnew<0 || phicellnew >= m_NCellTot)) continue;
        if (phicellnew < 0) phicellnew += m_NCellTot;
        if (phicellnew >= m_NCellTot) phicellnew -= m_NCellTot;
        double xln;
        int nstr2=m_nstraight;
        double dElec = Distance_Ele(xPosition,yPosition,phicellnew,nstr2,m_nfold,xln);
        if (fabs(dElec)<fabs(dElecMin)) {
            phicellmin=phicellnew;
            xlmin=xln;
            dElecMin = dElec;
            nstr=nstr2;
        }
      }
      m_phiGap = phicellmin;
      m_distElec = dElecMin;
      m_xl = xlmin;
      m_nstraight = nstr;
  }       // end distance >2.5mm
  else {
      m_phiGap=phicell;
      m_distElec=distElec;
      m_xl=xl;
      m_nstraight=nstr;
  }

#ifdef DEBUGHITS
  std::cout << "  final phiGap,distElec,xl " << m_phiGap << " " << m_distElec << " "
            << m_xl << std::endl;
#endif
  
// compute distance to absorber

  G4int nabs;
  if (m_distElec<0) nabs=m_phiGap;
  else nabs=m_phiGap+1;
  if (nabs >= m_NCellMax) nabs -= m_NCellMax;
  m_distAbs = Distance_Abs(xPosition,yPosition,nabs,m_nstraight,m_nfold);
#ifdef DEBUGHITS
  std::cout << "  nabs,distAbs " << nabs << " " << m_distAbs << std::endl;
#endif

// in some rare cases near fold, the closest distance could give the wrong gap
//  in such case, the signs of distAbs and distElec are not opposite as they should
  if ((m_distAbs>0. && m_distElec>0) ||
      (m_distAbs<0. && m_distElec<0) ) {
//    std::cout << "distElec and distAbs same sign " << m_distElec << " " << m_distAbs << std::endl;
//    std::cout << " m_phiGap " << m_phiGap << std::endl;
    if (std::fabs(m_distElec)>std::fabs(m_distAbs)) {
        if (m_distAbs>0) m_phiGap += 1;
        if (m_distAbs<0) m_phiGap -= 1;
        if (m_NCellTot != 1024) {
          if (m_phiGap <0) m_phiGap=0;
          if (m_phiGap >= m_NCellTot) m_phiGap = m_NCellTot-1;
        } else {
          if (m_phiGap < 0) m_phiGap += m_NCellTot;
          if (m_phiGap >= m_NCellTot) m_phiGap -= m_NCellTot;
        }
        m_distElec = Distance_Ele(xPosition,yPosition,m_phiGap,m_nstraight,m_nfold,m_xl);
//        std::cout << " new phiGap,distElec " << m_phiGap << " " << m_distElec << std::endl;
    }
  }

  m_phiBin = (G4int) ( m_phiGap/sampling_phi_nGaps );

  if (MapDetail) {
// compute x0,y0 coordinates in local electrode frame, using closest fold
// as reference
    G4double alpha = m_coudeelec->PhiRot(m_nfold,m_phiGap);
    G4double dx=xPosition-m_coudeelec->XCentCoude(m_nfold,m_phiGap);
    G4double dy=yPosition-m_coudeelec->YCentCoude(m_nfold,m_phiGap);
    G4double dx1=dx*cos(alpha)-dy*sin(alpha);
    G4double dy1=dx*sin(alpha)+dy*cos(alpha);
    m_x0 = dx1 + m_xc[m_nfold];
    m_y0 = dy1 + m_yc[m_nfold];
    if (m_parity==1) m_y0 = -1*m_y0;
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

  m_2pi = 2.*M_PI;

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
     xl2+=rint*fabs(phi1-phi0);
     G4int nstep=int((phi1-phi0)*inv_dt)+1;
     for (int ii=0;ii<nstep;ii++) {
        xl+=dl;
        G4double phi=phi0+dt*((G4double)ii);
        G4double x=cenx[i]+rint*cos(phi);
        G4double y=ceny[i]+rint*sin(phi);
        G4double radius=sqrt(x*x+y*y);
        if (radius>m_Rmax) m_Rmax=radius;
        G4double phid=atan(y/x);
        G4int ir=((int) ((radius-m_Rmin)/m_dR) );
        if (ir>=0 && ir < m_NRphi) {
           sum1[ir]+=1.;
           sumx[ir]+=phid;
        }
     }

// straight section
     if (i<14) {
        G4double dx=cenx[i+1]-cenx[i];
        G4double dy=ceny[i+1]-ceny[i];
        G4double along=dx*dx+dy*dy-4.*rint*rint;
        along=sqrt(along);
        G4double x0=0.5*(cenx[i+1]+cenx[i]);
        G4double y0=0.5*(ceny[i+1]+ceny[i]);
        G4double phi;
        if (i%2==m_parity) phi=CLHEP::pi/2-m_delta[i];
        else               phi=-CLHEP::pi/2.+m_delta[i];
        G4double x1=x0-0.5*along*cos(phi);
        G4double y1=y0-0.5*along*sin(phi);
        xl2+=along;
        int nstep=int(along*inv_dl)+1;
        for (int ii=0;ii<nstep;ii++) {
           xl+=dl;
           G4double x=x1+dl*((G4double)ii)*cos(phi);
           G4double y=y1+dl*((G4double)ii)*sin(phi);
           G4double radius=sqrt(x*x+y*y);
           if (radius>m_Rmax) m_Rmax=radius;
           G4double phid=atan(y/x);
           G4int ir=((int) ((radius-m_Rmin)/m_dR) );
           if (ir>=0 && ir < m_NRphi) {
              sum1[ir]+=1.;
              sumx[ir]+=phid;
           }
        }
     }
  }
//  std::cout << "total electrode lenght " << xl << " " << xl2 << std::endl;
//  std::cout << "rmax in accordion " << m_Rmax << std::endl;
  for (int i=0; i<m_NRphi; i++) {
    if (sum1[i]>0) {
     m_Rphi[i]=sumx[i]/sum1[i];
     // Not used: 
      //G4double radius = m_Rmin + ((G4double(i))+0.5)*m_dR;
      //std::cout << " GUTEST  r,phi0 " << radius << " " << m_Rphi[i] << std::endl;
    } 
    else m_Rphi[i]=0.;
  }
}

// ======================================================================================
// phi of first absorber as function of radius for nominal accordion geometry
//  (before sagging)
G4double Geometry::Phi0(G4double radius)
{
 G4int ir;
 if (radius < m_Rmin) ir=0;
 else {
   if (radius > m_Rmax) radius=m_Rmax-0.0001;
   ir=((int) ((radius-m_Rmin)/m_dR) );
 }
 return m_Rphi[ir];
}

// ======================================================================================
// compute number (0 to 1023) of closest electrode according to nominal
// accordion geometry
G4int Geometry::PhiGap(const double & radius, const double & xhit, const double &yhit)
{
  G4double phi0=Phi0(radius)+m_gam0;   // from -pi to pi
  G4double phi_hit=atan2(yhit,xhit);  // from -pi to pi
  G4double dphi=phi_hit-phi0;
// bring back to 0-2pi
  if (dphi<0) dphi=dphi+m_2pi;
  if (dphi>=m_2pi) dphi=dphi-m_2pi;
  dphi=dphi/(m_2pi)*1024;
  G4int ngap=((int) dphi);
#ifdef DEBUGHITS
  std::cout << " phi0 " << phi0 << " dphi, ngap " << dphi << " " << ngap << std::endl;
#endif
  return ngap;
}

//===================================================================================
// full cell id computation starting from an arbitrary G4 step

LArG4Identifier Geometry::CalculateIdentifier(const G4Step* a_step,std::string strDetector)
{

// total number of cells in phi to distinguish 1 module (testbeam case) from full Atlas
  m_testbeam=false;
  if (m_NCellTot != 1024) {
    m_testbeam=true;
  }
  
// The default result is a blank identifier.
  LArG4Identifier result = LArG4Identifier();
  
// Get all the required information from the current step

  G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
  G4StepPoint* post_step_point = a_step->GetPostStepPoint();
  G4ThreeVector startPoint = pre_step_point->GetPosition();
  G4ThreeVector endPoint   = post_step_point->GetPosition();
  G4ThreeVector p = (startPoint + endPoint) * 0.5;
  
  const G4NavigationHistory* g4navigation = pre_step_point->GetTouchable()->GetHistory(); 
  G4int ndep = g4navigation->GetDepth();
  G4int indECAM = -999;

// Now navigate through the volumes hierarchy 

  G4String ecamName;
  if (strDetector=="") ecamName  = "LAr::EMB::ECAM";
  else                 ecamName  = strDetector+"::LAr::EMB::ECAM";

  bool inSTAC = false;
  int zside=1;
  for (G4int ii=0;ii<=ndep;ii++) {
    G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
    G4String vname = v1->GetName();
    if ( vname == ecamName ) indECAM=ii;
    if ( vname.find("STAC") !=std::string::npos) inSTAC=true;
    if ( vname.find("NegPhysical") != std::string::npos) zside=-1;
  }
  if (indECAM>=0)
     result = CalculateECAMIdentifier( a_step , indECAM, strDetector, inSTAC, zside) ;     
  else
     std::cout << "LArBarrel::Geometry::CalculateIdentifier  ECAM volume not found in hierarchy" << std::endl;

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
 
LArG4Identifier Geometry::CalculateECAMIdentifier(const G4Step* a_step, const G4int indECAM, std::string strDetector, const bool inSTAC, int zside)
{

  LArG4Identifier result = LArG4Identifier();;

  // Get all the information about the step

  G4StepPoint *thisStepPoint = a_step->GetPreStepPoint();
  G4StepPoint *thisStepBackPoint = a_step->GetPostStepPoint();
  G4ThreeVector startPoint = thisStepPoint->GetPosition();
  G4ThreeVector endPoint = thisStepBackPoint->GetPosition();
  G4ThreeVector p = (thisStepPoint->GetPosition() + thisStepBackPoint->GetPosition()) * 0.5;

#ifdef  DEBUGHITS
  std::cout << "Position of the step in the ATLAS frame (x,y,z) --> " << p.x() << " " << p.y() << " " << p.z() << std::endl;
  std::cout << "Eta and Phi in the ATLAS frame                  --> " << p.eta() << " " << p.phi() << std::endl;
#endif
  
  // BACK directly into the LOCAL half_Barrel. All the variables in this LOCAL framework get the SUFFIX  Zpos

  const G4NavigationHistory* g4navigation = thisStepPoint->GetTouchable()->GetHistory();
  const G4AffineTransform transformation = g4navigation->GetTransform(indECAM);  
  G4ThreeVector startPointinLocal = transformation.TransformPoint(startPoint);
  G4ThreeVector endPointinLocal = transformation.TransformPoint  (endPoint);
  G4ThreeVector midinLocal = (startPointinLocal+endPointinLocal)*0.5;
  
#ifdef  DEBUGHITS
  std::cout << "Position of the step in the LOCAL frame (x,y,z) --> " << midinLocal.x() << " " << midinLocal.y() << " " << midinLocal.z() << std::endl;
  std::cout << "Eta and Phi of the step in LOCAL frame          --> " << midinLocal.eta() << " " << midinLocal.phi() << std::endl;
#endif

  // coordinates in the local frame

  G4double xZpos   = midinLocal.x();
  G4double yZpos   = midinLocal.y();
  G4double zZpos   = midinLocal.z();
  G4double etaZpos = midinLocal.pseudoRapidity();
  G4double phiZpos = midinLocal.phi();
  if(phiZpos<0.) phiZpos = phiZpos + 2.*M_PI;
  G4double radius2Zpos = xZpos*xZpos + yZpos*yZpos;
  G4double radiusZpos = sqrt(radius2Zpos);

  if (m_testbeam) 
    m_zSide = 1;
  else
    m_zSide = zside;

    
// Check if the hit is in the fiducial range and in the STAC volume
//  if yes this is active or inactive material
  
  if (inSTAC && radiusZpos >=m_rMinAccordion && radiusZpos <= m_rMaxAccordion &&
      zZpos <= m_zMaxBarrel && zZpos >= m_zMinBarrel && etaZpos <= m_etaMaxBarrel) {

#ifdef  DEBUGHITS   
    std::cout << "This hit is in the STAC volume !!!!! " << std::endl;
#endif

//   DETERMINATION of m_cellID, m_zSide, m_sampling, m_phiBin, m_etaBin, m_stackNumID
    bool MapDetail=false; 
    Geometry::findCell( xZpos, yZpos, zZpos, radiusZpos, etaZpos, phiZpos, MapDetail, strDetector );

    // adjust phi in the negative half barrel frame

    if( m_zSide == -1 )
      {
	if( m_sampling == 1 && m_region ==0 )
	  {
	    m_phiBin = 31 - m_phiBin;
	    if(m_phiBin < 0 ) m_phiBin += 64;
	  }
	if( m_sampling == 1 && m_region ==1 )
	  {
	    m_phiBin = 127 - m_phiBin;
	    if(m_phiBin < 0 ) m_phiBin += 256;
	  }
	if( m_sampling >= 2 )
	  {
	    m_phiBin = 127 - m_phiBin;
	    if(m_phiBin < 0 ) m_phiBin += 256;
	  }  
      }

// there are few hundred microns between the 4mm nominal beginning of the active region
//  and the real beginning of the first strip at eta=0.025/8
//  to avoid inactive energy with strip=0 assign this to strip=1
    if (m_sampling==1 && m_region==0 && m_etaBin==0) {
       m_etaBin=1;
    }
    
    result << 4          // LArCalorimeter
	   << 1          // LArEM
	   << m_zSide
	   << m_sampling
	   << m_region
	   << m_etaBin
	   << m_phiBin;
    
#ifdef  DEBUGHITS 
    std::cout << "Here the identifier for the barrel ACTIVE ----> " << std::endl;
    std::cout << "eta in local frame --> " << etaZpos << std::endl;
    std::cout << "m_zSide  ----> " << m_zSide << std::endl;
    std::cout << "m_sampling  ----> " << m_sampling << std::endl;
    std::cout << "m_region  ----> " <<  m_region << std::endl;
    std::cout << "m_etaBin  ----> " << m_etaBin << std::endl;
    std::cout << "m_phiBin  ----> " << m_phiBin << std::endl;
    G4double firsteta = thisStepPoint->GetPosition().pseudoRapidity();  
    std::cout << "And also etafirst ----> " << firsteta << std::endl;
#endif

//    if (!Geometry::CheckLArIdentifier(m_sampling,m_region,m_etaBin,m_phiBin)) {
//      std::cout << " **  Bad LAr identifier " << m_sampling << " " << m_region << " "
//                << m_etaBin << " " << m_phiBin << std::endl;
//      std::cout << " x,y,z,eta,phi " <<  xZpos << " " << yZpos << " " << zZpos
//                << " " << radiusZpos << " " << etaZpos << " " << phiZpos << std::endl;
//    }

    
  } 
// hits in dead material part
  else {
    
    G4int sampling=0;
    G4int region=0;        
    G4int numDeadPhiBins = 64;
    double abs_eta = fabs(etaZpos);
    double DM1EtaWidth = 0.1 ;
    double DM1PhiWidth = 2.*M_PI / numDeadPhiBins ; 
    m_etaBin = (G4int) ( abs_eta * (1./DM1EtaWidth) ) ;
    m_phiBin = (G4int) (phiZpos/ DM1PhiWidth );
    G4int type=1;
    // protect against rounding error for phi ~2pi
    if (m_phiBin==numDeadPhiBins) m_phiBin=m_phiBin-1;
    
    // adjust phi for negative half barrel
    
    if ( m_zSide == -1 ) {
      m_phiBin = 31 - m_phiBin;
      if (m_phiBin < 0 ) m_phiBin +=64 ;
    }

// material in front of the active accordion
    if ( radiusZpos < m_rMinAccordion ) {    
      sampling =1 ;
      region = 3 ;
      if (m_etaBin > 14) m_etaBin=14;

#ifdef  DEBUGHITS 
    std::cout << "This hit is in the ECAM volume in front of the accordion (DEAD MATERIAL) !!!!! " << std::endl;
#endif      

    } else if (radiusZpos >= m_rMaxAccordion){  // material behind the active accordion
      sampling = 2;
      
      if (abs_eta < 1.0 ) {
	region = 0 ;	
#ifdef  DEBUGHITS 
    std::cout << "This hit is in the ECAM volume behind accordion (DEAD MATERIAL 0)  !!!!! " << std::endl;
#endif 
      } else if ( abs_eta >= 1.0 && abs_eta < 1.5) {  
	region = 2;
        m_etaBin = m_etaBin - 10;    // to have etabin between 0 and 4
#ifdef  DEBUGHITS 
    std::cout << "This hit is in the ECAM volume behind accordion (DEAD MATERIAL 2) !!!!! " << std::endl;
#endif 	
      } else {
       std::cout << " LArBarrelGeometry: hit behind accordion at eta>1.5 !!! " << std::endl,
       region = 2;
       m_etaBin = 4;
      }

    } else if (zZpos <= m_zMinBarrel) {   // inactive matter between two EMB halves
        type=2;
        region=0;
        G4int phisave=m_phiBin;
        G4bool MapDetail=false;
        Geometry::findCell( xZpos, yZpos, zZpos, radiusZpos, etaZpos, phiZpos, MapDetail ,strDetector );
        sampling = m_sampling; // sampling as in normal definition
        m_etaBin=0;
        m_phiBin=phisave;

    } else if (zZpos >= m_zMaxBarrel || abs_eta >= 1.40) { // inactive matter between EMB and scintillator
      if (abs_eta >1.0 && abs_eta < 1.5) {
         sampling=2;
         region=2;
         m_etaBin = m_etaBin - 10;    // to have etabin between 0 and 4
      } else if (abs_eta < 1.6) {
         sampling=1;
         region=4;
         m_etaBin=0;    
      } else {
       std::cout << " LArBarrelGeometry: hit at eta>1.6 !!! " << std::endl;
       sampling=1;
       region=4;
       m_etaBin=0;
      }
    } else {
      if (!m_testbeam) {
        std::cout << "LArBarrelGeometry: cannot find region for DM hit..." << std::endl;
        std::cout << "r,z,eta,phi " << radiusZpos << " " << zZpos << " " << etaZpos << " " << phiZpos << std::endl;
        std::cout << "x,y,z (Atlas) " << p.x() << " " << p.y() << " " << p.z() << std::endl;
        std::cout << " inSTAC " << inSTAC << std::endl;
        G4double thisStepEnergyDeposit = a_step->GetTotalEnergyDeposit();
        std::cout << " eDeposited " << thisStepEnergyDeposit << std::endl;
        G4VPhysicalVolume* vol = thisStepPoint->GetPhysicalVolume();
        G4String volName = vol->GetName();
        std::cout << " volName " << volName << std::endl;
        G4int ndep = g4navigation->GetDepth();
        for (G4int ii=0;ii<=ndep;ii++) {
          G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
          G4String vname = v1->GetName();
          std::cout << "vname " << vname << std::endl;
        }

      }
// in test beam case, we can get there for leakage on the side (in phi) of the module
// in this case, we attribute an identifier like inactive material
      else
      {
         G4bool MapDetail=false;
         Geometry::findCell( xZpos, yZpos, zZpos, radiusZpos, etaZpos, phiZpos, MapDetail ,strDetector );
//         std::cout << " Lateral lakage r,eta,phi " << radiusZpos << " " << etaZpos << " "
//                   << phiZpos << "  sampling/region/eta/phi " << m_sampling << " " <<
//              m_region << " " << m_etaBin << " " << m_phiBin << std::endl;
// protect against small space between z=4m and real beginning of ieta=1 in strips
         if (m_sampling==1 && m_region==0 && m_etaBin==0) {
              m_etaBin=1;
//            std::cout << "S1R0 etabin 0 found  r,z,phi local " << radiusZpos << " " 
//                   << " " << zZpos << " " << phiZpos << std::endl;
         }
         result << 4          // LArCalorimeter
                << 1          // LArEM
                << m_zSide
                << m_sampling
                << m_region
                << m_etaBin
                << m_phiBin;
         return result;
      }
    }
    
    result << 10             // LArCalorimeter
	   << m_zSide * 4     // LArBarrel
	   << type
	   << sampling
	   << region
	   << m_etaBin
	   << m_phiBin;

#ifdef  DEBUGHITS     
    std::cout << "Here the identifier for the barrel DEAD materials ---->" << std::endl;
    std::cout << "Type     ----> " << type << std::endl;
    std::cout << "Sampling ----> " << sampling << std::endl;
    std::cout << "Region   ----> " << region << std::endl;
    std::cout << "m_zSide  ----> " << m_zSide*4 << std::endl;
    std::cout << "etaBin   ----> " << m_etaBin << std::endl;
    std::cout << "phiBin   ----> " << m_phiBin << std::endl;
#endif

//    if (!Geometry::CheckDMIdentifier(type,sampling,region,m_etaBin,m_phiBin)) {
//        std::cout << " **  Bad DM identifier " << type << " " << sampling << " " << region << " " 
//                  << m_etaBin << " " << m_phiBin << std::endl;
//        std::cout << "x,y,z,r,eta,phi" << xZpos << " " << yZpos << " " << zZpos <<
//         " " << radiusZpos << " " << etaZpos << " " << phiZpos << std::endl;
//    }

  } 
  
  return result;
  
}

bool  Geometry::CheckDMIdentifier(int type, int sampling, int region, int eta, int phi)
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


bool Geometry::CheckLArIdentifier(int sampling, int region, int eta, int phi)
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
