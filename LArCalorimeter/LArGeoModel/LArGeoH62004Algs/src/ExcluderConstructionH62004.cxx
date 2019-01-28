/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ExcluderConstructionH62004.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/Units.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/StoredPhysVol.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include <string>
#include <cmath>
#include <iostream>


LArGeo::ExcluderConstructionH62004::ExcluderConstructionH62004(int which)
  :m_which(which)
{
}

LArGeo::ExcluderConstructionH62004::~ExcluderConstructionH62004() {;}


GeoVFullPhysVol*  LArGeo::ExcluderConstructionH62004::GetEnvelope() {
//----------------------------------------------------------------
// Elements , Mixtures and Materials 
//----------------------------------------------------------------
  ISvcLocator *svcLocator = Gaudi::svcLocator();

  StoreGateSvc *detectorStore;
  if (svcLocator->service("DetectorStore", detectorStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in ModulesConstruction, cannot access DetectorStore");
  }
  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
    return NULL;
  }


  const GeoElement *elH = materialManager->getElement("Hydrogen");
  if (!elH) throw std::runtime_error("Error in ExcluderConstruction, Hydrogen is not found.");

  const GeoElement *elC = materialManager->getElement("Carbon");
  if (!elC) throw std::runtime_error("Error in ExcluderConstruction, Carbon is not found.");

  const GeoElement* elN = materialManager->getElement("Nitrogen");
  if (!elN) throw std::runtime_error("Error in ExcluderConstruction, Nitrogen is not found.");

  const GeoElement *elO = materialManager->getElement("Oxygen");
  if (!elO) throw std::runtime_error("Error in ExcluderConstruction, Oxygen is not found.");

  const GeoMaterial* LAr = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in ExcluderConstruction, std::LiquidArgon is not found.");

  const GeoMaterial* Fe = materialManager->getMaterial("std::Iron");
  if (!Fe) throw std::runtime_error("Error in ExcluderConstruction, std::Iron is not found.");

  const GeoMaterial* Cu = materialManager->getMaterial("std::Copper");
  if (!Cu) throw std::runtime_error("Error in ExcluderConstruction, std::Copper is not found.");

  // How to get Rohacell ????
  // Taking Margret's one
   // Define Rohacell Foam. 
  // Rohacell foam has density: 0.11g/cm3
  std::string name;
  double density;
  const GeoElement*  C=materialManager->getElement("Carbon");
  const GeoElement*  H=materialManager->getElement("Hydrogen");
  const GeoElement*  O=materialManager->getElement("Oxygen");
  const GeoElement*  N=materialManager->getElement("Nitrogen");
  GeoMaterial* Rohacell = new GeoMaterial(name="Rohacell", density=0.112*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  Rohacell->add(C,0.6465);
  Rohacell->add(H,0.07836);
  Rohacell->add(O,0.19137);
  Rohacell->add(N,0.08377);
  Rohacell->lock();

  /*
  a = 12.957*GeoModelKernelUnits::g/Gaudi::Units::mole;                                                       
  density = 0.112*g/cm3;                                              
  z = 6.18;
  G4Material* Rohacell = new G4Material(name="Rohacell",z, a, density);
  */

  //----------------------------------------------------------------
  // End of MATERIALS implementation.
  //----------------------------------------------------------------
  
//----------------------------------------------------------------
// Excluder GEOMETRY
//----------------------------------------------------------------


  GeoLogVol*   logicEx = 0;  //pointer to the logical excluder
  GeoVFullPhysVol* physiEx = 0; // return physical volume
  GeoPcon*        fEx;
//  GeoLogVol*   lfEx; 
//  G4VPhysicalVolume*  pfEx;
  GeoPcon*        mEx;
//  GeoLogVol*   lmEx; 
//  G4VPhysicalVolume*  pmEx;
  GeoPcon*        bEx;
//  GeoLogVol*   lbEx; 
//  G4VPhysicalVolume*  pbEx;


  switch(m_which) {
     case 0: { // EMEC excluder
     // DB values
     double Rmin = 725.*Gaudi::Units::mm;
     double Rmin_2 = 780.*Gaudi::Units::mm;
     double Rmax = 980.*Gaudi::Units::mm;

     double Zall = 626*Gaudi::Units::mm;
     double Zback = 91*Gaudi::Units::mm;
     double Zfront = 60*Gaudi::Units::mm;

     double alpha = 22.5*Gaudi::Units::degree;
     double beta  = 6.375*Gaudi::Units::degree; 


//     double gamma = 8.589*degree;
     double delta = 2.720*Gaudi::Units::degree;

//  solidEx = new GeoTubs("MotherEx",Rmin,Rmax,Zall/2.,-(alpha+gamma), 2*(alpha+gamma));
//     GeoTubs* solidEx = new GeoTubs(Rmin,Rmax,Zall/2.,-(alpha+beta), 2*(alpha+beta));
//     logicEx = new GeoLogVol(solidEx,LAr, "LArG4TB::EMEC::Excluder");
//     logicEx->SetVisAttributes(new G4VisAttributes(G4Color(1.,0.,0.)));
//   logicEx->SetVisAttributes(G4VisAttributes::Invisible);

     const int zplanes = 2;
     const double rmin[zplanes] = {Rmin,Rmin};
     const double rmin_2[zplanes] = {Rmin_2,Rmin_2};
     const double rmax[zplanes] = {Rmax,Rmax};
     const double zcoord[2*zplanes] = {-Zall/2., -Zall/2.+Zfront, Zall/2.-Zback, Zall/2.};
  
     GeoTrf::Transform3D rot(GeoTrf::Transform3D::Identity());

     fEx = new GeoPcon(-(alpha-delta),2*(alpha-delta));
     for(int i=0; i<zplanes; ++i) {
	fEx->addPlane(zcoord[i],rmin[i],rmax[i]);
     }
//     std::string fExName = "LArG4TB::EMEC::Exclude::Front";
//     lfEx = new GeoLogVol(fEx,Rohacell,fExName);
//     lfEx->SetVisAttributes(G4VisAttributes::Invisible);
//     lfEx->SetVisAttributes(new G4VisAttributes(G4Color(1.,0.,0.)));
//     pfEx = new G4PVPlacement(G4Transform3D(rot,G4ThreeVector()),lfEx,fExName,logicEx,false,1);

     mEx = new GeoPcon(-(alpha+beta),2*(alpha+beta));
     for(int i=0; i<zplanes; ++i) {
	mEx->addPlane(zcoord[i+1],rmin[i],rmax[i]);
     }
//     std::string mExName = "LArG4TB::EMEC::Exclude::Middle";
//     lmEx = new GeoLogVol(mEx,Rohacell,mExName);
//     lmEx->SetVisAttributes(G4VisAttributes::Invisible);
//     lmEx->SetVisAttributes(new G4VisAttributes(G4Color(0.,0.,1.)));
//     pmEx = new G4PVPlacement(G4Transform3D(rot,G4ThreeVector()),lmEx,mExName,logicEx,false,2);

     bEx = new GeoPcon(-(alpha+beta),2*(alpha+beta));
     for(int i=0; i<zplanes; ++i) {
	bEx->addPlane(zcoord[i+2],rmin_2[i],rmax[i]);
     }
//     std::string bExName = "LArG4TB::EMEC::Exclude::Back";
//     lbEx = new GeoLogVol(bEx,Rohacell,bExName);
//     lbEx->SetVisAttributes(G4VisAttributes::Invisible);
//     lbEx->SetVisAttributes(new G4VisAttributes(G4Color(0.7,0.,0.5)));
//     pbEx = new G4PVPlacement(G4Transform3D(rot,G4ThreeVector()),lbEx,bExName,logicEx,false,3);
     /*
     G4UnionSolid  *usf = new G4UnionSolid("unionSolidf", fEx, mEx, rot, G4ThreeVector());
     G4UnionSolid  *usf1 = new G4UnionSolid("unionSolidf1", usf, bEx, rot, G4ThreeVector());
     */
//     const GeoShapeUnion & usf = fEx->add(*((GeoShape*)&((*mEx)<<rot)));
     const GeoShapeUnion & usf = (*fEx).add(*mEx<<rot);
     const GeoShapeUnion & usf1 = usf.add(*bEx<<rot);
     logicEx = new GeoLogVol("LArGeoTB::EMEC::Excluder",&usf1,Rohacell);

     break; 
	     }
     case 1 : { // FCAL excluder

                double Rmax = 335.*Gaudi::Units::mm;
//                double Rmax_1 = 253.*Gaudi::Units::mm;
                double bepo_Beta = 4.668*Gaudi::Units::degree; // DB !!!
		double bepo_ty = 90.0*Gaudi::Units::degree; // DB !!

//                double Zall = (1021.4/2.)*Gaudi::Units::mm;
                double Zall = (912./2.)*Gaudi::Units::mm;
//		double Zpara = (168.47/2.)*Gaudi::Units::mm;
//		double Zpara = (247.87/2.)*Gaudi::Units::mm;
		double Xall = (171./2.)*Gaudi::Units::mm;
		double Yall = (300./2.)*Gaudi::Units::mm;
		double Rmax_1 = Rmax - 2.*Zall*tan(bepo_Beta);

		GeoPara *pEx = new GeoPara(Zall,Yall,Xall,0*Gaudi::Units::degree,bepo_Beta,0.*Gaudi::Units::degree);
		GeoCons *tEx = new GeoCons(0.,0.,Rmax,Rmax_1,Zall,0.,M_PI);
		GeoBox  *box = new GeoBox(Yall,Xall,Zall);
                GeoTrf::RotateX3D Rot(bepo_Beta);
		GeoTrf::Translation3D  trans1(0., sqrt(Rmax_1*Rmax_1 - Yall*Yall) + Xall + Zall*tan(bepo_Beta),  0*Gaudi::Units::mm);
		GeoTrf::Transform3D offset = trans1 * Rot;
		const GeoShapeIntersection  &is = (*tEx).intersect(*box<<offset);

		GeoTrf::Transform3D Rot1 = GeoTrf::RotateX3D(bepo_Beta) * GeoTrf::RotateZ3D(bepo_ty) * GeoTrf::RotateY3D(bepo_ty);
//		G4ThreeVector  translation(0., (203.74-168.47/2.)*Gaudi::Units::mm, 0.*Gaudi::Units::mm);
		GeoTrf::Translation3D  translation(0., sqrt(Rmax_1*Rmax_1 - Yall*Yall)-Xall+Zall*tan(bepo_Beta),0.*Gaudi::Units::mm);
		GeoTrf::Transform3D offset1 = translation * Rot1;
		const GeoShapeUnion  &us = is.add(*pEx<<offset1);  
		std::string bExName = "LArGeoTB::FCAL::Excluder";
	        logicEx = new GeoLogVol(bExName, &us,Rohacell);
//	        logicEx = new GeoLogVol(bExName, &is,Rohacell);

	         break;
	      }
     case 2 : { // Front excluder


//                double Rmax = bcry_rlar;
                double Rmax =  125.5*Gaudi::Units::cm; // DB !!!
		double bepo_Beta = 4.668*Gaudi::Units::degree; // DB !!!

                double Zall = (1200./2.)*Gaudi::Units::mm;
		double angle = 32.*Gaudi::Units::degree;
//		double Xall = 119.35*Gaudi::Units::cm;
		double Xall = Rmax*cos(angle/2);
		double Yall = Rmax*sin(angle/2);

		GeoTubs *tEx = new GeoTubs(0.,Rmax,Zall,-angle/2.,angle);
		GeoPara  *box = new GeoPara(Xall,Yall,1.1*Zall,0.,-bepo_Beta,0.);

		GeoTrf::Translate3D offset(0., 0.*Gaudi::Units::mm, 0*Gaudi::Units::mm);
		const GeoShapeSubtraction &is = (*tEx).subtract((*box)<<(offset));
//		G4UnionSolid *is = new G4UnionSolid("isEx",tEx,box,Rot,trans1);
		std::string FrontExName = "LArGeoTB::Front::Excluder";
	        logicEx = new GeoLogVol(FrontExName,&is,Rohacell);

		 break;
	      }
     case 3 : { // Back excluder

//                double Rmax = bcry_rlar;
		double Rmax =  125.5*Gaudi::Units::cm; // DB !!!


                double Zall = (1600./2.)*Gaudi::Units::mm;
		double angle = 58.*Gaudi::Units::degree;
		double Xall = Rmax*cos(angle/2.);
		double Yall = Rmax*sin(angle/2.);

		GeoTubs *tEx = new GeoTubs(0.,Rmax,Zall,0.,angle);
		GeoBox  *box = new GeoBox(Xall,Yall,1.1*Zall);
		GeoTrf::RotateZ3D Rot(angle/2.);
		GeoTrf::Translation3D  trans1(0., 0.*Gaudi::Units::mm,  0*Gaudi::Units::mm);
		GeoTrf::Transform3D offset = trans1 * Rot;
		const GeoShapeSubtraction &is = (*tEx).subtract((*box)<<(offset));
		std::string BackExName = "LArGeoTB::Back::Excluder";
	        logicEx = new GeoLogVol(BackExName,&is,Rohacell);

		 break;
	      }
  }

       
       physiEx = new GeoFullPhysVol(logicEx);

       return physiEx;

}
