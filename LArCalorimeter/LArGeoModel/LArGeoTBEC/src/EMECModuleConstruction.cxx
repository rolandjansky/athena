/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// EMECModuleConstruction

// Revision history:

//  LArEMECModuleConstruction:

// 08.08.02 copied from LArEMECConstruction
// 09-May-2001 WGS: Copied from Andrei Soukharev's EMECDetectorConstruction.
// 09-May-2001 WGS: Put in the LAr parent volumes for the endcaps.
// 09-Jan-2002 WGS: The LAr parent volume now comes from the calling routine.
//                  Any shifts in z are handled in the parent volume.


//  EMECModuleConstruction:
// 08-Dec-2005 V.N. Copied from LArEMECModuleConstruction and EMECConstruction
// 18-Feb-2006 V.N  Put options to select inne and outer wheels separatly.


#include "LArGeoTBEC/EMECModuleConstruction.h"
#include "LArGeoEndcap/EMECSupportConstruction.h"

#include "GeoSpecialShapes/LArCustomShape.h"

#include "GeoModelKernel/GeoElement.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoCons.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoTube.h" 
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/Units.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <string>
#include <cmath>
#include <iostream>

static const bool DEBUG_EMEC = true;

LArGeo::EMECModuleConstruction::EMECModuleConstruction( bool tbb, bool iwb, bool owb ) { 

  m_isTB         = tbb;
  m_isInnerWheel = iwb;
  m_isOuterWheel = owb; 

/*
        for testbeams setups module could be positioned in arbitrary place,
        so envelope is created is such a way, so that the front face
        of active region is at z = 0         
*/

}

LArGeo::EMECModuleConstruction::~EMECModuleConstruction() {}

GeoVFullPhysVol* LArGeo::EMECModuleConstruction::GetEnvelope()
{
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in EndcapCryostatConstruction, cannot access DetectorStore");
  }
  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return 0;


  //////////////////////////////////////////////////////////////////
  // Define materials
  // Note added 03-Jul-2003 WGS:
  // All this has to be replaced by the GeoModel materials manager,
  // but leave it as-is for now.
  //////////////////////////////////////////////////////////////////

  std::string name,symbol;
 
  double density;
  double m1,m2,m3;
  double Totalthick,/*Totalthicko,Totalthicki,*/Totalthicke;
  double Tgpb,Tgfe,Tggl,Thpb,Thfe,Thgl,Thcu,Thka,ThGten;
  double FracH,FracO,FracC,FracN,FracAr;
  double FracGten,Fracpb,Fracfe,Fracgl,Fraccu,Fracka;
  double aH,aO,aC,aN,aAr;

  const GeoElement* H=materialManager->getElement("Hydrogen");
  const GeoElement* C=materialManager->getElement("Carbon");
  const GeoElement* N=materialManager->getElement("Nitrogen");
  const GeoElement* O=materialManager->getElement("Oxygen");
  const GeoElement* Al=materialManager->getElement("Aluminium");
  const GeoElement* Ar=materialManager->getElement("Argon");
  const GeoElement* Fe=materialManager->getElement("Iron");
  const GeoElement* Cu=materialManager->getElement("Copper"); 
  const GeoElement* Pb=materialManager->getElement("Lead");

  //LAr

  GeoMaterial* LAr = new GeoMaterial(name="LiquidArgon", density=1.396*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  LAr->add(Ar,1.);
  LAr->lock();
 
  //Alu

  GeoMaterial* Alu = new GeoMaterial(name="Alu", density=2.7*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  Alu->add(Al,1.);
  Alu->lock();

  //Iron

  GeoMaterial* Iron = new GeoMaterial(name="Iron", density=7.87*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  Iron->add(Fe,1.);
  Iron->lock();

  //Copper

  GeoMaterial* Copper = new GeoMaterial(name="Copper", density=8.96*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  Copper->add(Cu,1.);
  Copper->lock();

  //Lead

  GeoMaterial* Lead = new GeoMaterial(name="Lead", density=11.35*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  Lead->add(Pb,1.);
  Lead->lock();

  // Air	,  at 20 deg C, 1 atm density=1.2931*mg/cm3

  GeoMaterial* Air=new GeoMaterial(name="Air", density=1.290*Gaudi::Units::mg/Gaudi::Units::cm3);
  Air->add(N, .8);
  Air->add(O, .2);
  Air->lock();

  //Prepreg glue for absorbers, composition to be checked!
  //ref:STR.CAL.01.CRB.6,(23-Jan-2003,J.T.)

  GeoMaterial* Glue=new GeoMaterial(name="Glue", density=1.8*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  aH=8.*H->getA();
  aO=2.*O->getA();
  aC=5.*C->getA();
  double Atot=aH+aO+aC;
  {
    const double inv_Atot = 1. / Atot;
    FracH=aH*inv_Atot;
    FracO=aO*inv_Atot;
    FracC=aC*inv_Atot;
  }
  Glue->add(H,FracH);
  Glue->add(O,FracO);
  Glue->add(C,FracC);
  Glue->lock();

  //PermaliE730 for Front middle ring, composition to be checked!
  //                     ref.: STR.CAL.01.CRB.6,(23-Jan-2003,J.T.)

  GeoMaterial* PermaliE730=new GeoMaterial(name="PermaliE730",density=1.83*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  aH=8.*H->getA();
  aO=2.*O->getA();
  aC=5.*C->getA();
  Atot=aH+aO+aC;
  {
    const double inv_Atot = 1. / Atot;
    FracH=aH*inv_Atot;
    FracO=aO*inv_Atot;
    FracC=aC*inv_Atot;
  }
  PermaliE730->add(H,FracH);
  PermaliE730->add(O,FracO);
  PermaliE730->add(C,FracC);
  PermaliE730->lock();

  // Gten  ( C8 H14 O4 ), alias glass epoxy for long.&transv.bars

  GeoMaterial* Gten = new GeoMaterial(name="Gten", density=1.8*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  aH=14.*H->getA();
  aO= 4.*O->getA();
  aC= 8.*C->getA();
  Atot=aH+aO+aC;
  {
    const double inv_Atot = 1. / Atot;
    FracH=aH*inv_Atot;
    FracO=aO*inv_Atot;
    FracC=aC*inv_Atot;
  }
  Gten->add(H,FracH);
  Gten->add(O,FracO);
  Gten->add(C,FracC);
  Gten->lock();
 
  // Kapton 11-Jan-2002 ML from accbgeo.age: the Kapton_E density is 1.46g/cm3
  //        one assumes it is the same as for the Kapton_H -> C22 H10 O5 N2
 
  GeoMaterial* Kapton= new GeoMaterial(name="Kapton",density=1.46*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  aH=10.*H->getA();
  aO= 5.*O->getA();
  aC=22.*C->getA();
  aN= 2.*N->getA();
  Atot=aH+aO+aC+aN;
  {
    const double inv_Atot = 1. / Atot;
    FracH=aH*inv_Atot;
    FracO=aO*inv_Atot;
    FracC=aC*inv_Atot;
    FracN=aN*inv_Atot;
  }
  Kapton->add(H,FracH);
  Kapton->add(O,FracO);
  Kapton->add(C,FracC);
  Kapton->add(N,FracN);
  Kapton->lock();

  // THIN absorber: outer wheel
  //               11-Jan-2002 ML source: endegeo.age and Fig.7-3 of TDR
  Tggl = 0.3*Gaudi::Units::mm;
  Tgfe = 0.4*Gaudi::Units::mm;
  Tgpb = 1.7*Gaudi::Units::mm;
  Totalthick=Tggl+Tgfe+Tgpb;
  m1=Tggl*Glue->getDensity();
  m2=Tgfe*Iron->getDensity();
  m3=Tgpb*Lead->getDensity();
  double Totalmass=m1+m2+m3;
  density=Totalmass*(1./Totalthick);
  {
    const double inv_Totalmass = 1. / Totalmass;
    Fracgl=m1*inv_Totalmass;
    Fracfe=m2*inv_Totalmass;
    Fracpb=m3*inv_Totalmass;
  }
  GeoMaterial* Thin_abs = new GeoMaterial(name="Thinabs",density);
  Thin_abs->add(Glue,Fracgl);
  Thin_abs->add(Iron,Fracfe);
  Thin_abs->add(Lead,Fracpb); 
  Thin_abs->lock();

  // THICK absorber: inner wheel
  //                 11-Jan-2002 ML source: endegeo.age and Fig.7-3 of TDR
  Thgl = 0.3*Gaudi::Units::mm;
  Thfe = 0.4*Gaudi::Units::mm;
  Thpb = 2.2*Gaudi::Units::mm;
  Totalthick=Thgl+Thfe+Thpb;
  m1=Thgl*Glue->getDensity();
  m2=Thfe*Iron->getDensity();
  m3=Thpb*Lead->getDensity();
  Totalmass=m1+m2+m3;
  density=Totalmass*(1./Totalthick);
  {
    const double inv_Totalmass = 1. / Totalmass;
    Fracgl=m1*inv_Totalmass;
    Fracfe=m2*inv_Totalmass;
    Fracpb=m3*inv_Totalmass;
  }
  GeoMaterial* Thick_abs = new GeoMaterial(name="Thickabs",density);
  Thick_abs->add(Glue,Fracgl);
  Thick_abs->add(Iron,Fracfe);
  Thick_abs->add(Lead,Fracpb); 
  Thick_abs->lock();

  // Electrode,   as a mixture Kapton+Cu, 11-Jan-2002 ML

  Thcu = 0.105*Gaudi::Units::mm;
  Thka = 0.170*Gaudi::Units::mm;  //together with glue J.T.
  Totalthicke = Thcu+Thka;
  m1=Thcu*Copper->getDensity();
  m2=Thka*Kapton->getDensity();
  Totalmass=m1+m2;
  density=Totalmass*(1./Totalthicke);
  {
    const double inv_Totalmass = 1. / Totalmass;
    Fraccu=m1*inv_Totalmass;
    Fracka=m2*inv_Totalmass;
  }
  GeoMaterial* Kapton_Cu= new GeoMaterial(name="KaptonC",density);
  Kapton_Cu->add(Copper,Fraccu);
  Kapton_Cu->add(Kapton,Fracka);
  Kapton_Cu->lock();

  //  ELECTRONICS as a mix_molecule CH2Ar made of
  //                                CH2 (from polyethylene -CH2- chain) 
  //                           and  Ar  (from Liquid Argon)
  aH =2.*H->getA();
  aC =1.*C->getA();
  aAr=1.*Ar->getA();
  Atot=aH+aC+aAr;
  {
    const double inv_Atot = 1. / Atot;
    FracH =aH*inv_Atot;
    FracC =aC*inv_Atot;
    FracAr=aAr*inv_Atot;
  }
  GeoMaterial* Elect_LAr= new GeoMaterial(name="Elnics",density=1.28*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  Elect_LAr->add(H ,FracH);
  Elect_LAr->add(C ,FracC);
  Elect_LAr->add(Ar,FracAr);
  Elect_LAr->lock();

  GeoMaterial* innerAbsorberMaterial = Thick_abs;
  GeoMaterial* outerAbsorberMaterial = Thin_abs;
  GeoMaterial* innerElectrodMaterial = Kapton_Cu;
  GeoMaterial* outerElectrodMaterial = Kapton_Cu;

  // G10FeInner for barrette in the inner wheel(J.T.08.01.2003)

  Thfe  =0.4*Gaudi::Units::mm;
  Thgl  =0.3*Gaudi::Units::mm;
  Thpb  =2.2*Gaudi::Units::mm;
  ThGten=Thpb;
  Totalthick =Thfe+Thgl+ThGten;
  //Totalthicki=Totalthick;
  m1=Thfe  *Iron->getDensity();
  m2=Thgl  *Glue->getDensity();
  m3=ThGten*Gten->getDensity();
  Totalmass=m1+m2+m3;
  {
    const double inv_Totalmass = 1. / Totalmass;
    Fracfe  =m1*inv_Totalmass;
    Fracgl  =m2*inv_Totalmass;
    FracGten=m3*inv_Totalmass;
  }
  density =   Totalmass*(1./Totalthick);

  GeoMaterial* G10FeInner=
      new GeoMaterial(name="LAr::EMEC::G10FeInner",density);
  G10FeInner->add(Iron,Fracfe);
  G10FeInner->add(Glue,Fracgl);
  G10FeInner->add(Gten,FracGten);
  G10FeInner->lock();

 // G10FeOuter for barrette in the outer wheel(J.T.08.01.2003)

  Thfe  =0.4*Gaudi::Units::mm;
  Thgl  =0.3*Gaudi::Units::mm;
  Thpb  =1.7*Gaudi::Units::mm;
  ThGten=Thpb;
  Totalthick =Thfe+Thgl+ThGten;
  //Totalthicko=Totalthick;
  m1=Thfe  *Iron->getDensity();
  m2=Thgl  *Glue->getDensity();
  m3=ThGten*Gten->getDensity();
  Totalmass=m1+m2+m3;
  {
    const double inv_Totalmass = 1. / Totalmass;
    Fracfe  =m1*inv_Totalmass;
    Fracgl  =m2*inv_Totalmass;
    FracGten=m3*inv_Totalmass;
  }
  density =   Totalmass*(1./Totalthick);
  GeoMaterial* G10FeOuter=
         new GeoMaterial(name="LAr::EMEC::G10FeOuter",density);
  G10FeOuter->add(Iron,Fracfe);
  G10FeOuter->add(Glue,Fracgl);
  G10FeOuter->add(Gten,FracGten);


  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////

  std::string baseName = "LAr::EMEC";

  // Define the mother volume for the emec.  Everything
  // else in the emec (wheels,structure, etc.) should be
  // placed inside here.
  
// J.T************
// V.N: From LarWheelSolid, to get bounding polycone. Previoulsy G4 routine. No GeoModel equivalent so far ...
//
  
  double zWheelFrontFace 		= 3689.5*Gaudi::Units::mm;
  
  double dWRPtoFrontFace 		= 11.*Gaudi::Units::mm;

  double dMechFocaltoWRP                 = 3691. *Gaudi::Units::mm;  //=endg_z1*Gaudi::Units::cm
                                                         //"LArEMECNomLarOrig" 
  double rOuterCutoff                    = 2034. *Gaudi::Units::mm;  //=endg_rlimit*Gaudi::Units::cm
                                              //"LArEMECMaxRadiusActivePart

//*****************      
  
  zWheelFrontFace+= dWRPtoFrontFace;

  // V.N : Select inner/outer radii depending on wheels.

  double Rin1, Rin2, Rout1, Rout2;

  if ( m_isInnerWheel ) {
    Rin1 = 279.*Gaudi::Units::mm;
    Rin2 = 324.*Gaudi::Units::mm;
  }
  else {
    Rin1 = 590.*Gaudi::Units::mm;
    Rin2 = 678.*Gaudi::Units::mm; 
  }
  if ( m_isOuterWheel ) {
    Rout1 = 2070.*Gaudi::Units::mm;
    Rout2 = 2070.*Gaudi::Units::mm;
  }
  else {
    Rout1 = 647.*Gaudi::Units::mm;
    Rout2 = 732.*Gaudi::Units::mm;
  }

  // --> EndOfRadiiSelection <--
   
  double emecMotherZplan[] = { 3639.5*Gaudi::Units::mm, 3639.5*Gaudi::Units::mm + 630.*Gaudi::Units::mm };    //cold (J.T)                               
  double emecMotherRin[]   = { Rin1, Rin2 };	
  double emecMotherRout[]  = { Rout1, Rout2 }; 
  int lastPlaneEmec = ( sizeof( emecMotherZplan )/sizeof( double ) );

  if ( m_isTB ) { 
     for ( int i = 0; i < lastPlaneEmec; i++ ) emecMotherZplan[ i ] -= zWheelFrontFace;
     zWheelFrontFace = 0.*Gaudi::Units::mm;
  }

  double phiPosition = M_PI/2;
  double phiSize = M_PI/8 + 0.065; // half-angle of inner part of module

// V.N : computing Bounding polycone in following lines ********************************
// This should be moved to a sub-routine as it is common to EMEC

  // Within the LAr endcap cryostat we have two LAr EM endcap wheels:
  // an inner wheel and an outer wheel. 

  // The radial boundaries of the inner and outer wheels are defined
  // by values of eta, the distance from z=0 to the front face of the
  // wheel, and the thickness of the wheel.

  double eta_hi  = 3.2;
  double eta_mid = 2.5;
  double eta_low = 1.375;

  // Compute the angles defined by eta, and the wheel radii versus z.
  double tanThetaInner = 2. * exp(-eta_hi ) / (1. - exp(2.*-eta_hi ));
  double tanThetaMid   = 2. * exp(-eta_mid) / (1. - exp(2.*-eta_mid));
  double tanThetaOuter = 2. * exp(-eta_low) / (1. - exp(2.*-eta_low));

  double zWheelThickness = 514.*Gaudi::Units::mm;     // endg_etot-2.*(endg_sabl*Gaudi::Units::cm-2.*Gaudi::Units::mm)
  double gapBetweenWheels= 1.5*Gaudi::Units::mm*2.;  // "LArEMECHalfCrack"*2.

//J.T************
// zWheelFrontFace for mechanical design
  double zWheelFrontFaceMech = dMechFocaltoWRP + dWRPtoFrontFace; // V.N : renamed for compatibility
//--************

  double zWheelBackFaceMech = zWheelFrontFaceMech + zWheelThickness;
   
  double zWheelInner[2];
  zWheelInner[0] = 0.;
  zWheelInner[1] = zWheelThickness;

  double rMinInner[2];
  rMinInner[0] = zWheelFrontFaceMech * tanThetaInner;
  rMinInner[1] = zWheelBackFaceMech  * tanThetaInner;

  // Note that there is a 3mm gap between the outer surface of the
  // inner wheel (rMaxInner) and the inner surface of the outer wheel
  // (rMinOuter).

  double rMaxInner[2];
  rMaxInner[0] = zWheelFrontFaceMech * tanThetaMid - gapBetweenWheels/2.;
  rMaxInner[1] = zWheelBackFaceMech  * tanThetaMid - gapBetweenWheels/2.;

  // The outer wheel's outer surface is more complicated.  Ignore the
  // [1] index in the following array definitions for a moment:

  double zWheelOuter[3];
  zWheelOuter[0] = zWheelInner[0];
  zWheelOuter[2] = zWheelInner[1];

  double rMinOuter[3];
  rMinOuter[0] = rMaxInner[0] + gapBetweenWheels;
  rMinOuter[2] = rMaxInner[1] + gapBetweenWheels;

  double rMaxOuter[3];
  rMaxOuter[0] = zWheelFrontFaceMech * tanThetaOuter;
  rMaxOuter[2] = zWheelBackFaceMech  * tanThetaOuter;

  // If we simply left it at that, the outer wheel would protrude
  // beyond the edge of the cryostat.  We have to "slice off" the lip
  // of the outer wheel to create a shape that's roughtly like a
  // bathtub plug. 

  // Use the endpoints of the outer arrays to define lines in the
  // (z,r) plane.
  double slopeMinOuter = ( rMinOuter[2]   - rMinOuter[0] )
                         / ( zWheelOuter[2] - zWheelOuter[0] );
  double slopeMaxOuter = ( rMaxOuter[2]   - rMaxOuter[0] )
                         / ( zWheelOuter[2] - zWheelOuter[0] );
  double interceptMinOuter = rMinOuter[0] - slopeMinOuter*zWheelOuter[0];
  double interceptMaxOuter = rMaxOuter[0] - slopeMaxOuter*zWheelOuter[0];

  // At what value of z does the outer line hit the cutoff?
  zWheelOuter[1] = ( rOuterCutoff - interceptMaxOuter ) / slopeMaxOuter;

  // Calculate the radii at this z:
  rMinOuter[1] = slopeMinOuter * zWheelOuter[1] + interceptMinOuter;
  rMaxOuter[1] = rOuterCutoff;

  // Now override the radius to keep the wheel inside the cryostat:
  rMaxOuter[2] = rOuterCutoff;

  // We've defined the geometries of the LAr EM endcap volumes.  Now
  // place them into the detector.

  // A note on the translation: It's useful (especially for the
  // routines in LArWheelSolidInit) if the EMEC is positioned such that
  // its front face is at z=0 in its internal co-ordinates.

  if ( DEBUG_EMEC )
    {
      std::cout<<"InnerWheel,z(0)="<<zWheelInner[0]<<
        " rmin="<<rMinInner[0]<<" rmax="<<rMaxInner[0]<<std::endl;
      std::cout<<"           z(1)="<<zWheelInner[1]<<
        " rmin="<<rMinInner[1]<<" rmax="<<rMaxInner[1]<<std::endl;
      std::cout<<"OuterWheel,z(0)="<<zWheelOuter[0]<<
        " rmin="<<rMinOuter[0]<<" rmax="<<rMaxOuter[0]<<std::endl;
      std::cout<<"           z(1)="<<zWheelOuter[1]<<
	" rmin="<<rMinOuter[1]<<" rmax="<<rMaxOuter[1]<<std::endl;
      std::cout<<"           z(2)="<<zWheelOuter[2]<<
	" rmin="<<rMinOuter[2]<<" rmax="<<rMaxOuter[2]<<std::endl;
    }

  //////////////////////////////////////////////////////////////////
  // Fill geometry
  //////////////////////////////////////////////////////////////////

  std::string emecMotherName = baseName + "::Mother";

  GeoPcon* emecMotherShape = new GeoPcon( phiPosition - phiSize, 2.*phiSize );  //start phi,total phi
  for( int i = 0; i < lastPlaneEmec; i++ ) emecMotherShape->addPlane( emecMotherZplan[i], 
  								      emecMotherRin[i], 
								      emecMotherRout[i] );
  const GeoLogVol* emecMotherLogical = new GeoLogVol( emecMotherName, emecMotherShape, LAr );
  GeoFullPhysVol* emecMotherPhysical = new GeoFullPhysVol( emecMotherLogical );
     
  if ( m_isInnerWheel ) {
    std::string innerName = baseName + "::InnerWheel";
    GeoPcon* innerShape= new GeoPcon( phiPosition - phiSize, 2.*phiSize );
    innerShape->addPlane( zWheelInner[0], rMinInner[0], rMaxInner[0] );
    innerShape->addPlane( zWheelInner[1], rMinInner[1], rMaxInner[1] );
    GeoLogVol*  innerLogical  = new GeoLogVol ( innerName, innerShape, LAr );
    GeoFullPhysVol* innerPhysical = new GeoFullPhysVol( innerLogical );
    emecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
    emecMotherPhysical->add( new GeoTransform( GeoTrf::TranslateZ3D( zWheelFrontFace ) ) );
    emecMotherPhysical->add( innerPhysical );

    // Place the custom accordion volumes in the liquid argon.

    std::string IAWname = innerName + "::Absorber";
    std::string IEWname = innerName + "::Electrode";

    LArCustomShape* innerAbsorberShape  = new LArCustomShape(IAWname);
    LArCustomShape* innerElectrodeShape = new LArCustomShape(IEWname);

    GeoLogVol* innerAbsorberLogical  = 
	    new GeoLogVol( IAWname, innerAbsorberShape , innerAbsorberMaterial );
    GeoLogVol* innerElectrodeLogical = 
	    new GeoLogVol( IEWname, innerElectrodeShape, innerElectrodMaterial );
		    
    GeoFullPhysVol* innerAbsorberPhysical  = new GeoFullPhysVol( innerAbsorberLogical );
    GeoPhysVol* innerElectrodePhysical = new GeoPhysVol( innerElectrodeLogical );
    innerPhysical->add( new GeoIdentifierTag( 1 ) );
    innerPhysical->add( innerAbsorberPhysical );
    innerPhysical->add( new GeoIdentifierTag( 1 ) );
    innerPhysical->add( innerElectrodePhysical );
    {
      StoredPhysVol *sPhysVol = new StoredPhysVol(innerPhysical);
      StatusCode status=detStore->record(sPhysVol,"EMEC_INNER_WHEEL_POS");
      if(!status.isSuccess()) throw std::runtime_error ("Cannot store EMEC_INNER_WHEEL");
    }   
  }    
   
  if ( m_isOuterWheel ) {
    std::string outerName = baseName + "::OuterWheel";
    GeoPcon* outerShape= new GeoPcon( phiPosition - phiSize, 2.*phiSize );
    outerShape->addPlane( zWheelOuter[0], rMinOuter[0], rMaxOuter[0] );
    outerShape->addPlane( zWheelOuter[1], rMinOuter[1], rMaxOuter[1] );
    outerShape->addPlane( zWheelOuter[2], rMinOuter[2], rMaxOuter[2] );
    GeoLogVol*  outerLogical  = new GeoLogVol ( outerName, outerShape, LAr );
    GeoFullPhysVol* outerPhysical = new GeoFullPhysVol(outerLogical);
    emecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
    emecMotherPhysical->add( new GeoTransform( GeoTrf::TranslateZ3D( zWheelFrontFace ) ) );
    emecMotherPhysical->add( outerPhysical );

    // Place the custom accordion volumes in the liquid argon.

    std::string OAWname = outerName + "::Absorber";
    std::string OEWname = outerName + "::Electrode";

    LArCustomShape* outerAbsorberShape  = new LArCustomShape( OAWname );
    LArCustomShape* outerElectrodeShape = new LArCustomShape( OEWname );

    GeoLogVol* outerAbsorberLogical  = 
	    new GeoLogVol( OAWname,outerAbsorberShape ,outerAbsorberMaterial );
    GeoLogVol* outerElectrodeLogical = 
	    new GeoLogVol( OEWname,outerElectrodeShape, outerElectrodMaterial );
		    
    GeoPhysVol* outerAbsorberPhysical  = new GeoPhysVol( outerAbsorberLogical );
    GeoPhysVol* outerElectrodePhysical = new GeoPhysVol( outerElectrodeLogical );
    outerPhysical->add( new GeoIdentifierTag( 1 ) );
    outerPhysical->add( outerAbsorberPhysical);
    outerPhysical->add( new GeoIdentifierTag( 1 ) );
    outerPhysical->add( outerElectrodePhysical ); 
    {
      StoredPhysVol *sPhysVol = new StoredPhysVol(outerPhysical);
      StatusCode status=detStore->record(sPhysVol,"EMEC_OUTER_WHEEL_POS");
      if(!status.isSuccess()) throw std::runtime_error ("Cannot store EMEC_OUTER_WHEEL");
    }   


  }         

// ***********************************
//Description of nonsensitive pieces:*
// ***********************************

  double z0;

// Volumes for FRONT and BACK support structures

  //
  // V.N : Select supports for front and back
  //

  int FrontIndx=0, BackIndx=0;

  if ( m_isInnerWheel && m_isOuterWheel ) {
    FrontIndx = 0;
    BackIndx = 1;
  }
  else if ( m_isInnerWheel && ! m_isOuterWheel ) {
    FrontIndx = 10;
    BackIndx = 11;
  }
  else if ( m_isOuterWheel && ! m_isInnerWheel ) {
    FrontIndx = 12;
    BackIndx = 13;
  }

  // V.N. --> Support selected

  z0 = zWheelFrontFace - 61.*Gaudi::Units::mm;
  EMECSupportConstruction *fsc = new EMECSupportConstruction( FrontIndx, true, "LAr::EMEC::", M_PI/2 );
  GeoPhysVol* physicalFSM = fsc->GetEnvelope();
  emecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
  emecMotherPhysical->add( new GeoTransform( GeoTrf::TranslateZ3D( z0 ) ) );
  emecMotherPhysical->add( physicalFSM );

  z0 = zWheelFrontFace + 514.*Gaudi::Units::mm + 55.*Gaudi::Units::mm;
  EMECSupportConstruction *bsc = new EMECSupportConstruction( BackIndx, true, "LAr::EMEC::", M_PI/2 );
  GeoPhysVol *physicalBSM = bsc->GetEnvelope();
  emecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
  GeoTrf::Transform3D rotBSM(GeoTrf::RotateZ3D(2*M_PI/2)*GeoTrf::RotateX3D(-M_PI));
  emecMotherPhysical->add( new GeoTransform( GeoTrf::Transform3D(GeoTrf::Translate3D( 0., 0., z0 )*rotBSM ) ) );
  emecMotherPhysical->add( physicalBSM );

  if ( m_isOuterWheel ) {
    z0 = zWheelFrontFace + 514.*Gaudi::Units::mm/2;
    EMECSupportConstruction *osc = new EMECSupportConstruction( 2, true, "LAr::EMEC::", M_PI/2 );
    GeoPhysVol *physicalOSM = osc->GetEnvelope();
    emecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
    emecMotherPhysical->add( new GeoTransform( GeoTrf::TranslateZ3D( z0 ) ) );  
    emecMotherPhysical->add( physicalOSM );
  }
 
  if ( m_isInnerWheel ) {
    z0 = zWheelFrontFace + 514.*Gaudi::Units::mm/2;
    EMECSupportConstruction *isc = new EMECSupportConstruction( 3, true, "LAr::EMEC::", M_PI/2 );
    GeoPhysVol *physicalISM = isc->GetEnvelope();
    emecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
    emecMotherPhysical->add(new GeoTransform( GeoTrf::TranslateZ3D( z0 ) ) )  ;  
    emecMotherPhysical->add( physicalISM );
  }
 
  z0 = zWheelFrontFace + 514.*Gaudi::Units::mm/2;
  EMECSupportConstruction *msc = new EMECSupportConstruction( 4, true, "LAr::EMEC::", M_PI/2 );
  GeoPhysVol *physicalMSM = msc->GetEnvelope();
  emecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
  emecMotherPhysical->add(new GeoTransform( GeoTrf::TranslateZ3D( z0 ) ) )  ;  
  emecMotherPhysical->add( physicalMSM );  

  return emecMotherPhysical;
}
