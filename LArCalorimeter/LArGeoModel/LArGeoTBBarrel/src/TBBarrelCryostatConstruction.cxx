/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// TBBarrelCryostatConstruction

//  25-Jan-2006  G.Unal   creation from hardcoded G4 geometry

#include "LArGeoTBBarrel/TBBarrelCryostatConstruction.h"
#include "LArGeoBarrel/BarrelConstruction.h"
#include "LArGeoBarrel/BarrelPresamplerConstruction.h"


#include "GeoModelKernel/GeoElement.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/Units.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/StoredAlignX.h"


#include <string>
#include <cmath>
#include <map>
#include <climits>

#define DEBUG_GEO

#define BUILD_TBCRYO1
#define BUILD_TBCRYO2
#define BUILD_ENDCRYO
#define BUILD_LARMODULE
#define BUILD_PRESAMPLER
#define BUILD_LARFOAM  
#define BUILD_SUPPORTRING  


LArGeo::TBBarrelCryostatConstruction::TBBarrelCryostatConstruction():
 _barrelSagging(0),
 _barrelVisLimit(-1),
 cryoMotherPhysical(NULL)
{}


LArGeo::TBBarrelCryostatConstruction::~TBBarrelCryostatConstruction() {}

GeoFullPhysVol* LArGeo::TBBarrelCryostatConstruction::GetEnvelope()
{

  if (cryoMotherPhysical) return cryoMotherPhysical;

// Get access to the matarial manager

  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in EMBConstruction, cannot access MessageSvc");
  }

  MsgStream log(msgSvc, "LAr::DetectorFactory"); 

  log <<MSG::INFO <<  "++++++++++++++++++++++++++++++++++++++++++++++++++++i++" <<  endmsg;
  log <<MSG::INFO <<  "+                                                     +" <<  endmsg;
  log <<MSG::INFO <<  "+   HELLO from LArGeo::TBBarrelCryostatConstruction   +" << endmsg;
  log <<MSG::INFO <<  "+                                                     +" <<  endmsg;
  log <<MSG::INFO <<  "+++++++++++++++++++++++++++++++++++++++++++++++++++++++" <<  endmsg;


  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in LArDetectorFactory, cannot access DetectorStore");
  }

// Get the materials from the material manager:---------------------------------------------------
//                                                                                                

  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;

  const GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) {
    throw std::runtime_error("Error in TBBarrelCryostatConstruction, std::Air is not found.");
  }

  const GeoMaterial *Aluminium  = materialManager->getMaterial("std::Aluminium");
  if (!Aluminium) {
    throw std::runtime_error("Error in TBBarrelCryostatConstruction, std::Aluminium is not found.");
  }

  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) {
    throw std::runtime_error("Error in TBBarrelCryostatConstruction, std::LiquidArgon is not found.");
  }

  const GeoMaterial* Iron = materialManager->getMaterial("std::Iron");
  if (!Iron) {
   throw std::runtime_error("Error in TBBarrelCryostatConstruction, std::Iron is not found.");
  }

  const GeoElement* Hydrogen = materialManager->getElement("Hydrogen");
  if (!Hydrogen) {
    throw std::runtime_error("Error in TBBarrelCryostatConstruction, hydrogen not found.");
  }

  const GeoElement* Carbon = materialManager->getElement("Carbon");
  if (!Carbon) {
    throw std::runtime_error("Error in TBBarrelCryostatConstruction, carbon not found.");
  }

  const GeoElement* Oxygen = materialManager->getElement("Oxygen");
  if (!Oxygen) {
    throw std::runtime_error("Error in TBBarrelCryostatConstruction, oxygen not found.");
  }

  GeoMaterial *Vacuum = new GeoMaterial("Vacuum",Gaudi::Units::universe_mean_density );
  Vacuum->add(Hydrogen,1.);
  Vacuum->lock();

// define material for FOAM (C5H8O2)
// latest density value from P.Puzo (october 2003)
//
  double density = 0.058*GeoModelKernelUnits::g/Gaudi::Units::cm3;
    GeoMaterial* Foam = new GeoMaterial("Foam", density);
    double fraction=8*1.01/(5*12.01+8*1.01+2.*16.0);
    Foam->add(Hydrogen,fraction);
    fraction=5.*12.01/(5*12.01+8*1.01+2.*16.0);
    Foam->add(Carbon,fraction);
    fraction=2.*16.0/(5*12.01+8*1.01+2.*16.0);
    Foam->add(Oxygen,fraction);
    Foam->lock();
#ifdef DEBUGGEO
    std::cout << " Foam density,rad length from GeoModel " << density << " " << Foam->getRadLength() << std::endl;
#endif


    std::string baseName = "LAr::TBBarrel::Cryostat::";

//////////////////////////////////////////////////////////////////
// Define geometry
//////////////////////////////////////////////////////////////////

// Big mother volume to be included in transparent way in Calo Ctb envelope

    double zp[3]    ={-1050.,0.,3810.};
    double ri[3]   ={965.,965.,965.};
    double ro[3]   ={2270.,2270.,2270.};
#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " Pcone volume for overall envelope " << std::endl;
    std::cout << " phi0=-25deg, Dphi=50deg " << std::endl;
    for (int i=0;i<3;i++) {
     std::cout << " Plane zp/ri/ro " << zp[i] << " " << ri[i] << " " << ro[i] << std::endl;
    }
#endif
    GeoPcon* Em_pcone = new GeoPcon(-25.*Gaudi::Units::deg,50.*Gaudi::Units::deg);
    for (int i=0; i < 3; i++) Em_pcone->addPlane(zp[i],ri[i],ro[i]);

    const GeoLogVol* cryoMotherLogical = 
       new GeoLogVol(baseName+"Envelope",Em_pcone, Air);

    cryoMotherPhysical = new GeoFullPhysVol(cryoMotherLogical);


// Cryostat geometry

    double  Cryo_Distz = 483.5*Gaudi::Units::cm;   // total size in z
    double  Cryo_z0 = 103.0*Gaudi::Units::cm;       // eta=0 position wrt cryosta edge at z<0

    double  DeltaR_cold = 4.1*Gaudi::Units::cm;    // thickness cold vessel before calo
    double  DeltaRout_cold = 5.0*Gaudi::Units::cm;    // thickness cold vessel after calo

    double  DeltaR_warm= 3.86*Gaudi::Units::cm;    // thickness warm vessel before calo
    double  DeltaRout_warm = 4.0*Gaudi::Units::cm;    // thickness warm vessel after calo

    double  DeltaRout_vac = 3.0*Gaudi::Units::cm;  // vacuum space cryo after calo

    double Dz_end_warm = 7.0*Gaudi::Units::cm;   // thickness of end plate at high z
    double Dz_end_vac  = 8.0*Gaudi::Units::cm;   
    double Dz_end_cold = 7.0*Gaudi::Units::cm;

    double Dz_end_tot = Dz_end_warm + Dz_end_vac + Dz_end_cold;

// position of center of curvature of cryostat before calo
    double  Cryo_Xcent = 3363.;

    double  Cryo_Rmax_C = 2326.;
    double  Cryo_Rmin_C  = Cryo_Rmax_C - DeltaR_cold;     

    double  Cryo_Rmax_W = 2396.;
    double  Cryo_Rmin_W =  Cryo_Rmax_W - DeltaR_warm;

// recompute vacuum space cryoastat before calo
//    double  DeltaR_vac = Cryo_Rmin_W - Cryo_Rmax_C;     

    double  Rmin_mother = Cryo_Xcent-Cryo_Rmax_W;
    double  Rmax_mother = 2270.;

    double Phi_Min = -5.0 * Gaudi::Units::deg;
    double Phi_Span = 32.5 * Gaudi::Units::deg;
// GU 10/09/2004
// For cryostat mother volume, sligthly larger phi range
//  to avoid clash with front cryostat
    double Phi_Min_Moth=-9.0*Gaudi::Units::deg;
    double Phi_Span_Moth=38.5*Gaudi::Units::deg;

// -----------------------------------------------------------------
// Mother volume for Cryostat, filled with foam
// ------------------------------------------------------------------

#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << "** Mother Volume for TB cryostat (Tubs) " << std::endl;
    std::cout << "   (matter = foam) " << std::endl;
    std::cout << " Rmin/Rmax " << Rmin_mother << " " << Rmax_mother << std::endl;
    std::cout << " Dz/2 " << Cryo_Distz/2. << std::endl;
    std::cout << " PhiMin, Span " << Phi_Min_Moth*(1./Gaudi::Units::deg) << " "
              << Phi_Span_Moth*(1./Gaudi::Units::deg) << std::endl;
#endif

    GeoTubs* Cent_tube = new GeoTubs(Rmin_mother,
                                     Rmax_mother,
                                     Cryo_Distz/2.,
                                     Phi_Min_Moth,
                                     Phi_Span_Moth);
    GeoLogVol* Cent_log = new GeoLogVol(baseName+"Mother",Cent_tube,Foam);
//  position in Pcon mother envelope (which has Atlas reference frame)
     double zpos = Cryo_Distz/2.-Cryo_z0;
     double phi  = -1.*360.*Gaudi::Units::deg/16/2.;   // to have x axis in middle of volume

    GeoPhysVol* Cent_phys  = new GeoPhysVol(Cent_log);
    cryoMotherPhysical->add(new GeoTransform(GeoTrf::RotateZ3D(phi)));
    cryoMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zpos)));
    cryoMotherPhysical->add(Cent_phys);

// ----------------------------------------------------------------------
// Cryostat before calo
// ----------------------------------------------------------------------
#ifdef BUILD_TBCRYO1

// Warm vessel

#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " ** Cryostat before LAr (shape=Tubs)" << std::endl;
    std::cout << " center in x = " << Cryo_Xcent  << std::endl;
    std::cout << " angle 180-11 deg, span = 14 deg" << std::endl;
    std::cout << " R warm vessel " << Cryo_Rmin_W << " "
                                   << Cryo_Rmax_W << std::endl;
    std::cout << " R vacuum      " << Cryo_Rmax_C << " "
                                   << Cryo_Rmin_W << std::endl;
    std::cout << " R cold vessel " << Cryo_Rmin_C << " "
                                   << Cryo_Rmax_C << std::endl;
    std::cout << " Half size in z " << (Cryo_Distz-Dz_end_tot)/2. << std::endl;
    std::cout << " position in z in mother " <<  -Dz_end_tot/2. << std::endl;
#endif

    GeoTubs* CryoW_tube = new GeoTubs(
                                    Cryo_Rmin_W,
                                    Cryo_Rmax_W,
                                    (Cryo_Distz-Dz_end_tot)/2.,
                                    (180.-11.)*Gaudi::Units::deg,
                                    14.*Gaudi::Units::deg);

    GeoLogVol* CryoW_log = new GeoLogVol(baseName+"WarmTube",
                                                     CryoW_tube,
                                                     Aluminium);

    GeoPhysVol* CryoW_phys = new GeoPhysVol(CryoW_log);
    Cent_phys->add(new GeoTransform(GeoTrf::Translate3D(Cryo_Xcent, 0., -Dz_end_tot/2.)));
    Cent_phys->add(CryoW_phys);

// Waccum between warm and cold vessels

    GeoTubs* CryoV_tube = new GeoTubs(
                                    Cryo_Rmax_C,
                                    Cryo_Rmin_W,
                                    (Cryo_Distz-Dz_end_tot)/2.,
                                    (180.-11.)*Gaudi::Units::deg,
                                    14.*Gaudi::Units::deg);

    GeoLogVol *CryoV_log = new GeoLogVol(baseName+"VacTube",
                                               CryoV_tube,
                                               Vacuum);

    GeoPhysVol* CryoV_phys = new GeoPhysVol(CryoV_log);
    Cent_phys->add(new GeoTransform(GeoTrf::Translate3D(Cryo_Xcent, 0., -Dz_end_tot/2.)));
    Cent_phys->add(CryoV_phys);

// Cold vessel

    GeoTubs* CryoC_tube = new GeoTubs(
                                    Cryo_Rmin_C,
                                    Cryo_Rmax_C,
                                    (Cryo_Distz-Dz_end_tot)/2.,
                                    (180.-11.)*Gaudi::Units::deg,
                                    14.*Gaudi::Units::deg);

    GeoLogVol *CryoC_log = new GeoLogVol(baseName+"ColdTube",
                                               CryoC_tube,
                                               Aluminium);

    GeoPhysVol* CryoC_phys = new GeoPhysVol(CryoC_log);
    Cent_phys->add(new GeoTransform(GeoTrf::Translate3D(Cryo_Xcent, 0., -Dz_end_tot/2.)));
    Cent_phys->add(CryoC_phys);

#endif

// ----------------------------------------------------------------------
// ACCB  Mother volume for all the LAr 
// including PS + Accordion
// the LAr start at r=1410mm (end of foam) and go to the beginning of the
// cold vessel of the cryostat after calo
//-----------------------------------------------------------------------

    double LAr_inner_radius=141.00*Gaudi::Units::cm;   // min radius of PS
    double LAr_outer_radius=Rmax_mother-DeltaRout_warm-DeltaRout_cold
                                         -DeltaRout_vac;

    double LAr_z_max = Cryo_Distz-Dz_end_tot;

#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " *** LAr volume (tubs put in foam)" << std::endl;
    std::cout << "Rmin/Rmax " << LAr_inner_radius << " "
                              << LAr_outer_radius << std::endl;
    std::cout << "PhiMin,Span " << Phi_Min*(1./Gaudi::Units::deg) << " "
                                << Phi_Span*(1./Gaudi::Units::deg) << std::endl;
    std::cout << "DeltaZ/2 " << LAr_z_max/2. << std::endl;
    std::cout << "Position in z in mother " << (LAr_z_max-Cryo_Distz)/2. << std::endl;
#endif

// ACCB mother is a tube

     GeoTubs* moth_tube = new GeoTubs( LAr_inner_radius,
                                       LAr_outer_radius,
                                       LAr_z_max/2.,
                                       Phi_Min,
                                       Phi_Span);

     GeoLogVol* moth_log = new GeoLogVol(baseName+"LAr",moth_tube,LAr);

     GeoPhysVol* moth_phys = new GeoPhysVol(moth_log);

     Cent_phys->add(new GeoTransform(GeoTrf::TranslateZ3D((LAr_z_max-Cryo_Distz)/2.)));
     Cent_phys->add(moth_phys);

// Adjust LAR volumes at the end of foam, such as to have 
//   18.5mm before PS at top
//   12.5mm before PS at mid top
//   14.5mm before PS at mid
//    7.5mm before PS at mid bottom
//    9.5mm before PS at bottom
// takes into account that for 1 PS sector r at the middle is 1412 and
//  at the edges is 1419.  (middle of PS sectors=mid top and mid bottom)
//  therefore compared to r=1410mm (LAr above) to get the correct Ar space
//  one needs to add  ~9.5mm at the top, 10mm at mid top, 5.5mm at mid,
//    5.5mm at mid bottom and 0.5 mm at bottom

// modified 17-Aug-05
// the measured thickness are at 0, 22.5/4, 22.5/2, 3*22.5/4 and 22.5 deg
// so define the regions in phi to be better centered on the measurements
// and to cover:
//   -  0 to 22.5/8 deg  = bottom thickness => need 9.5 - 9mm = 0.5mm Ar
//   - 22.5/8 to 3*22.5/8 deg = mid bottom => need 7.5-2mm    = 5.5mm Ar
//   - 3*22.5/8 to 5*22.5/8 deg = mid => need 14.5-9 = 5.5 mm Ar
//   - 5*22.5/8 to 7*22.5/8 deg = mid top => need 12.5-2mm = 10.5mm Ar
//   - 7*22.5/8 to 22.5/8 deg   = top     => need 18.5-9 = 9.5 mm Ar

#ifdef BUILD_LARFOAM

    double delta_LAr[5]={0.5*Gaudi::Units::mm,5.5*Gaudi::Units::mm,5.5*Gaudi::Units::mm,10.5*Gaudi::Units::mm,9.5*Gaudi::Units::mm};
    double Phi1[5]={0.*Gaudi::Units::deg,22.5/8.*Gaudi::Units::deg,3.*22.5/8*Gaudi::Units::deg,5.*22.5/8*Gaudi::Units::deg,7.*22.5/8*Gaudi::Units::deg};
    double Delta_phi[5]={22.5/8*Gaudi::Units::deg, 2.*22.5/8.*Gaudi::Units::deg,2.*22.5/8.*Gaudi::Units::deg,2.*22.5/8.*Gaudi::Units::deg,22.5/8.*Gaudi::Units::deg};

// GU 08-dec-2005
// additionnal LAr fudged before presampler to get better agreement
//  waiting for Rhoacell measurement to know if this is reasonnable or not
//  this should be now considered as a systematics
//  25mm LAr ~ 0.18 X0
//    double fudge_lar_gap = 25.*mm;
// GU 28--feb-2006   removed this fudge 25mm, not supported by measurements of Rohacell

    for (int ilar=0;ilar<5;ilar++) {
      double r1=LAr_inner_radius-delta_LAr[ilar];
      double r2=LAr_inner_radius;

#ifdef DEBUG_GEO
      std::cout << " Ar additionnal volume before PS " << r1 << " "
                << r2 << " " << Phi1[ilar]*(1./Gaudi::Units::deg) << " " << Delta_phi[ilar]*(1./Gaudi::Units::deg) << std::endl;
#endif

      GeoTubs* lar_tube = new GeoTubs(r1,
                                      r2,
                                      LAr_z_max/2.,
                                      Phi1[ilar],
                                      Delta_phi[ilar]);

      GeoLogVol* lar_log = new GeoLogVol(baseName+"LAr2",lar_tube,LAr);

      GeoPhysVol* lar_phys = new GeoPhysVol(lar_log);
      Cent_phys->add(new GeoTransform(GeoTrf::TranslateZ3D((LAr_z_max-Cryo_Distz)/2.)));
      Cent_phys->add(lar_phys);

    }
#endif

// Outer support rings: 6 steel rings, starting just
//   after Barrel volume (G10 bars) (r=2003.6)
//   DZ=80mm for DR=12mm, then DZ=10mm for DR=757mm then DZ=80mm for DR=12mm
//   at locations z=397,805,1255,1750,2316,2868 mm
#ifdef BUILD_SUPPORTRING

     double R_ring = 2003.6; 
     double DeltaR1_ring = 12.0; 
     double DeltaZ1_ring = 80.0; 
     double DeltaR2_ring = 75.7; 
     double DeltaZ2_ring = 10.0; 
     double DeltaR3_ring = 12.0; 
     double DeltaZ3_ring = 80.0; 

#ifdef DEBUG_GEO
     std::cout << " " << std::endl;
     std::cout << " *** Support Ring1: R/DR/DZ " << R_ring << " "
               << DeltaR1_ring << " " << DeltaZ1_ring << std::endl;
     std::cout << "             Ring2: R/DR/DZ " << R_ring+DeltaR1_ring << " "
               << DeltaR2_ring << " " << DeltaZ2_ring << std::endl;
     std::cout << "             Ring3: R/DR/DZ " << R_ring+DeltaR1_ring+DeltaR2_ring << " "
               << DeltaR3_ring << " " << DeltaZ3_ring << std::endl;
#endif


     GeoTubs* ring1_shape = new GeoTubs(
                                 R_ring,
                                 R_ring+DeltaR1_ring,
                                 DeltaZ1_ring/2.,
                                 Phi_Min,
                                 Phi_Span);
     GeoLogVol* ring1_log = new GeoLogVol(baseName+"Ring1",ring1_shape,Iron);
     GeoPhysVol* ring1_phys = new GeoPhysVol(ring1_log);
     
     GeoTubs* ring2_shape = new GeoTubs(
                                 R_ring+DeltaR1_ring,
                                 R_ring+DeltaR1_ring+DeltaR2_ring,
                                 DeltaZ2_ring/2.,
                                 Phi_Min,
                                 Phi_Span);
     GeoLogVol* ring2_log = new GeoLogVol(baseName+"Ring2",ring2_shape,Iron);
     GeoPhysVol* ring2_phys = new GeoPhysVol(ring2_log);

     GeoTubs* ring3_shape = new GeoTubs(
                                 R_ring+DeltaR1_ring+DeltaR2_ring,
                                 R_ring+DeltaR1_ring+DeltaR2_ring+DeltaR3_ring,
                                 DeltaZ3_ring/2.,
                                 Phi_Min,
                                 Phi_Span);
     GeoLogVol* ring3_log = new GeoLogVol(baseName+"Ring3",ring3_shape,Iron);
     GeoPhysVol* ring3_phys = new GeoPhysVol(ring3_log);

     static double zring[6] = {397.,805.,1255.,1750.,2316.,2868.};
     for (int iring=0; iring < 6; iring++)
     {
       double Zcd =  zring[iring]-LAr_z_max/2.+Cryo_z0;
#ifdef DEBUG_GEO
       std::cout << " Position ring in LAr mother volume at z = "
                 << Zcd << " (z atlas= " << zring[iring] << std::endl;
#endif
       moth_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(Zcd)));
       moth_phys->add(ring1_phys);
       moth_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(Zcd)));
       moth_phys->add(ring2_phys);
       moth_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(Zcd)));
       moth_phys->add(ring3_phys);
     }
#endif

// -----------------------------------------------------------------
//  Cryostat after LAr
// -----------------------------------------------------------------
#ifdef BUILD_TBCRYO2
     double rmin,rmax;

// Cold vessel
     rmin = LAr_outer_radius;
     rmax = LAr_outer_radius + DeltaRout_cold;
#ifdef DEBUG_GEO
     std::cout << " " << std::endl;
     std::cout << "** Cryostat after calo " << std::endl;
     std::cout << "cold vessel from " << rmin << " to " << rmax << std::endl;
     std::cout << " position in mother in z " << -Dz_end_tot/2. << std::endl;
#endif
     GeoTubs * CryoC2_tube = new GeoTubs(
                                     rmin,
                                     rmax,
                                     (Cryo_Distz-Dz_end_tot)/2.,
                                     Phi_Min,
                                     Phi_Span);
     GeoLogVol* CryoC2_log = new GeoLogVol(baseName+"ColdTube2",CryoC2_tube,Aluminium);
     GeoPhysVol* CryoC2_phys = new GeoPhysVol(CryoC2_log);
     Cent_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(-Dz_end_tot/2.)));
     Cent_phys->add(CryoC2_phys);

// vacuum between warn and cold vessel
     rmin = rmax;
     rmax = rmin + DeltaRout_vac;
#ifdef DEBUG_GEO
    std::cout << "vacuum from " << rmin << " to " << rmax << std::endl;
#endif
     GeoTubs* CryoV2_tube = new GeoTubs(
                                     rmin,
                                     rmax,
                                     (Cryo_Distz-Dz_end_tot)/2.,
                                     Phi_Min,
                                     Phi_Span);
     GeoLogVol* CryoV2_log = new GeoLogVol(baseName+"VacTube2",CryoV2_tube,Vacuum);
     GeoPhysVol* CryoV2_phys = new GeoPhysVol(CryoV2_log);
     Cent_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(-Dz_end_tot/2.)));
     Cent_phys->add(CryoV2_phys);

// warm vessel
     rmin = rmax;
     rmax = rmin + DeltaRout_warm;
#ifdef DEBUG_GEO
     std::cout << "warm vessel from " << rmin << " to " << rmax << std::endl;
#endif
     GeoTubs* CryoW2_tube = new GeoTubs(
                                     rmin,
                                     rmax,
                                     (Cryo_Distz-Dz_end_tot)/2.,
                                     Phi_Min,
                                     Phi_Span);
      GeoLogVol* CryoW2_log = new GeoLogVol(baseName+"WarmTube2",CryoW2_tube,Aluminium);
      GeoPhysVol* CryoW2_phys = new GeoPhysVol(CryoW2_log);
      Cent_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(-Dz_end_tot/2.)));
      Cent_phys->add(CryoW2_phys);
#endif

// Cryostat end plates at high z
#ifdef BUILD_ENDCRYO


// warm vessel
#ifdef DEBUG_GEO
      std::cout << "End plate for warm vessel  Rmin,Rmax " << Rmin_mother << " "
             << Rmax_mother << std::endl;
      std::cout << " half size in z " << Dz_end_warm/2. << std::endl;
#endif
      GeoTubs* CryoEndW_tube = new GeoTubs(
                                     Rmin_mother,
                                     Rmax_mother, 
                                     Dz_end_warm/2.,
                                     Phi_Min,
                                     Phi_Span);
       GeoLogVol* CryoEndW_log = new GeoLogVol(baseName+"EndWarm",CryoEndW_tube,Aluminium);
       GeoPhysVol* CryoEndW_phys = new GeoPhysVol(CryoEndW_log);
       double zwarm = Cryo_Distz/2. - Dz_end_warm/2.;
#ifdef DEBUG_GEO
       std::cout << " position in mother at z " << zwarm << std::endl;
#endif
       Cent_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(zwarm)));
       Cent_phys->add(CryoEndW_phys);
   
// vaccum part
#ifdef DEBUG_GEO
       std::cout << "End plate for vacuum       Rmin,Rmax " << Rmin_mother << " "
             << Rmax_mother << std::endl;
       std::cout << " half size in z " << Dz_end_vac/2. << std::endl;
#endif
       GeoTubs* CryoEndV_tube = new GeoTubs(
                                     Rmin_mother,
                                     Rmax_mother,
                                     Dz_end_vac/2.,
                                     Phi_Min,
                                     Phi_Span);
       GeoLogVol* CryoEndV_log = new GeoLogVol(baseName+"EndVac",CryoEndV_tube,Vacuum);
       GeoPhysVol* CryoEndV_phys = new GeoPhysVol(CryoEndV_log);
       double zvac = Cryo_Distz/2. - Dz_end_warm - Dz_end_vac/2.;
#ifdef DEBUG_GEO
       std::cout << " position in mother at z " << zvac << std::endl;
#endif
       Cent_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(zvac)));
       Cent_phys->add(CryoEndV_phys);

// cold vessel
#ifdef DEBUG_GEO
       std::cout << "End plate for cold vessel  Rmin,Rmax " << Rmin_mother << " "
             << Rmax_mother << std::endl;
       std::cout << " half size in z " << Dz_end_cold/2. << std::endl;
#endif
       GeoTubs* CryoEndC_tube = new GeoTubs(
                                     Rmin_mother,
                                     Rmax_mother,
                                     Dz_end_cold/2.,
                                     Phi_Min,
                                     Phi_Span);
       GeoLogVol* CryoEndC_log = new GeoLogVol(baseName+"EndCold",CryoEndC_tube,Aluminium);
       GeoPhysVol* CryoEndC_phys = new GeoPhysVol(CryoEndC_log);
       double zcold = Cryo_Distz/2. - Dz_end_warm - Dz_end_vac -Dz_end_cold/2.;
#ifdef DEBUG_GEO
       std::cout << " position in mother at z " << zcold << std::endl;
#endif
       Cent_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(zcold)));
       Cent_phys->add(CryoEndC_phys);

#endif

// --------------------------------------------------------------------
// Place the barrel test module inside the LAR volume (moth_phys)
// --------------------------------------------------------------------

#ifdef BUILD_LARMODULE
    BarrelConstruction barrelConstruction(true); // full geometry
    barrelConstruction.setBarrelSagging(_barrelSagging);
    barrelConstruction.setBarrelCellVisLimit(_barrelVisLimit);
    GeoFullPhysVol* barrelEnvelope = barrelConstruction.GetPositiveEnvelope();

//z=0 of ECAM is z=0 of Atlas
//z=0 of moth_phys is at + LAr_z_max/2.-Cryo_z0 in atlas frame

     double Zcd = -LAr_z_max/2.+Cryo_z0;

#ifdef DEBUG_GEO
     std::cout << " " << std::endl;
     std::cout << " Position ECAM volume in mother LAr at z " << Zcd << std::endl;
#endif

     if (barrelEnvelope !=0) { 
          moth_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(Zcd)));
          moth_phys->add(barrelEnvelope);
     }
#endif

// ------------------------------------------------------------------------
// Place the Presampler test module inside the LAr volume (moth_phys)
// ------------------------------------------------------------------------
#ifdef BUILD_PRESAMPLER
     BarrelPresamplerConstruction barrelPSConstruction(1);
     GeoFullPhysVol* barrelPSEnvelope = barrelPSConstruction.GetPositiveEnvelope();
// PS lenght = 2*1582.5
// start should be a z=0 in Atlas  => z = -LAr_z_max/2.+Cryo_z0 in moth_phys
// center of PS in moth phys should be at 1582.5-Cryo_Distz+Cryo_z0 in moth_phys

//   Zcd = 1582.5-LAr_z_max/2.+Cryo_z0;
// new value of PS mother lenght
//       Zcd = 1550.0*mm-LAr_z_max/2.+Cryo_z0;
// also the PS is shifted by 3mm to start at z=3mm in Atlas equivalent frame
       double PresamplerMother_length=1549.*Gaudi::Units::mm;
       double presamplerShift = 3.*Gaudi::Units::mm;
       Zcd = presamplerShift+PresamplerMother_length-LAr_z_max/2.+Cryo_z0;

#ifdef DEBUG_GEO
     std::cout << " " << std::endl;
     std::cout << " Position PS volume in mother LAr at z " << Zcd << std::endl;
#endif

     if (barrelPSEnvelope !=0) {
       moth_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(Zcd)));
       moth_phys->add(barrelPSEnvelope);
       StoredPhysVol *sPhysVol = new StoredPhysVol(barrelPSEnvelope);
       StatusCode status=detStore->record(sPhysVol,"PRESAMPLER_B_POS");
       if(!status.isSuccess()) throw std::runtime_error ("Cannot store PRESAMPLER_B_POS");
     }

#endif
          
    return cryoMotherPhysical;

}

    
