/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "WarmTCConstructionH62004.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoCons.h"
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
#include "GeoGenericFunctions/AbsFunction.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"
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


LArGeo::WarmTCConstructionH62004::WarmTCConstructionH62004():m_WarmTCPhys(0) {

  //StoreGateSvc* detStore;

   ISvcLocator* svcLocator = Gaudi::svcLocator();
   if (svcLocator->service("DetectorStore", m_detectorStore, false )==StatusCode::FAILURE) {
      throw std::runtime_error("Error in ModulesConstruction, cannot access DetectorStore");
   }
}

LArGeo::WarmTCConstructionH62004::~WarmTCConstructionH62004() {;}


GeoVFullPhysVol* LArGeo::WarmTCConstructionH62004::GetEnvelope()
{
   const StoredMaterialManager* materialManager = nullptr;
   if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
      throw std::runtime_error("Error in ModulesConstruction, cannot access Material manager");
   }

   if (m_WarmTCPhys) return (m_WarmTCPhys);
   ISvcLocator *svcLocator = Gaudi::svcLocator();
   IMessageSvc * msgSvc;
   if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
      throw std::runtime_error("Error in WarmTCConstructionH62004, cannot access MessageSvc");
   }
   MsgStream log(msgSvc, "WarmTCConstructionH62004");
	
// SHOULD GO INTO DB !!!!

// Muon scintilators are the first one:
double Muon_x = 20.0*Gaudi::Units::cm;
double Muon_y = 120.0*Gaudi::Units::cm;
double Muon_z = 1.0*Gaudi::Units::cm;
// The extra space to accumulate muon sci. into mother:
double Muon_dist = 120.0*Gaudi::Units::mm;
//
// WTC dimensions
double WTC_len = 591.5*Gaudi::Units::mm;
double WTC_high = 1250.0*Gaudi::Units::mm;
double WTC_sci_z = 12.7*Gaudi::Units::mm;
double WTC_sci_x = 190.0*Gaudi::Units::mm;
double WTC_sci_y = 1160.0*Gaudi::Units::mm;

//  Define dimension WTC mother
//
double x_m = WTC_high / 2;
double y_m = WTC_high / 2;
double z_m = (86.0*Gaudi::Units::mm + WTC_len + WTC_sci_z + Muon_dist + Muon_z) / 2;
//
// Define dimension of Fe absorber
//
double Fe_x = WTC_high / 2;
double Fe_y = WTC_high / 2;
double Fe_z = (99.0 / 2)*Gaudi::Units::mm;
//
// Define dimension of X scintilator
//
double x_x = 6 * WTC_sci_x / 2;
double x_y = WTC_sci_y / 2;
//
// Define dimension of Y scintilator
//
double y_x = WTC_sci_y / 2;
double y_y = 6 * WTC_sci_x / 2;
//
// Define widht of scintilator, same for X & Y
//
double z_s = WTC_sci_z / 2;
//
// Calculate position of Fe & scintilators inside mother WTC
// x & y position is always 0, all is need is z position!!
//
double z_x[3], z_y[3], z_Fe[4];
 z_x[0]  = -z_m + Muon_dist + Muon_z + z_s;                   // X scin. Layer 1
 z_y[0]  = z_x[0] + 54*Gaudi::Units::mm;               // Y scin. Layer 2
 z_Fe[0] = z_x[0] + 86.0*Gaudi::Units::mm + Fe_z;      // 1st Fe abs.
 z_y[1]  = z_Fe[0] + 125.5*Gaudi::Units::mm - Fe_z;    // Y scin. Layer 3
 z_Fe[1] = z_Fe[0] + 2 * Fe_z + 53.0*Gaudi::Units::mm; // 2nd Fe abs.
 z_x[1]  = z_Fe[0] - Fe_z + 278.5*Gaudi::Units::mm;    // X scin. Layer 4
 z_Fe[2] = z_Fe[1] + 2 *Fe_z + 52.5*Gaudi::Units::mm;  // 3rd Fe abs.
 z_y[2]  = z_Fe[0] - Fe_z + 433.0*Gaudi::Units::mm;    // Y scin. Layer 5
 z_Fe[3] = z_Fe[2] + 2 *Fe_z + 61.5*Gaudi::Units::mm;  // 4rd Fe abs.
 z_x[2]  = z_Fe[0] - Fe_z + WTC_len;     // X scin. Layer 6
//
// Tilding of the TC
//double WTC_tild = -1.1*Gaudi::Units::deg;   // 24 Gaudi::Units::mm tild on 1250 Gaudi::Units::mm length
//double WTC_tild = 0.*Gaudi::Units::deg;   // 24 Gaudi::Units::mm tild on 1250 Gaudi::Units::mm length
// Define position in test beam line....
//
//double WTC_x = 0.0*Gaudi::Units::mm;
//double WTC_y = 0.0*Gaudi::Units::mm;
//double WTC_z = 460.0*Gaudi::Units::mm - 120.*Gaudi::Units::mm - 10.*Gaudi::Units::mm;

// Some elements
 
    const GeoElement* elH = materialManager->getElement("Hydrogen");
    const GeoElement* elC = materialManager->getElement("Carbon");      
    //const GeoElement* elO = materialManager->getElement("Oxygen");  
    //const GeoElement* elN = materialManager->getElement("Nitrogen");     
    const GeoMaterial* Iron = materialManager->getMaterial("std::Iron");    
    const GeoMaterial *Air = materialManager->getMaterial("std::Air");
 // Scintillator
    double density = 1.032*GeoModelKernelUnits::g/Gaudi::Units::cm3;
    GeoMaterial* Scintillator=new GeoMaterial("Scintillator",density);
    Scintillator->add(elC,0.9147);
    Scintillator->add(elH,0.0853);
    Scintillator->lock();
  

// "mother" volume for Fe abs. & scint.
//
 StatusCode status;
 std::string mname = std::string("WarmTC_Mother");
 GeoBox *wtc_box = new GeoBox(x_m,y_m,z_m);
 GeoLogVol *wtc_log = new GeoLogVol(mname,wtc_box,Air);

 m_WarmTCPhys = new GeoFullPhysVol(wtc_log);

 std::string tag = std::string("WARMTC_POS");
 StoredPhysVol *sPhysVol = new StoredPhysVol(m_WarmTCPhys);
 status=m_detectorStore->record(sPhysVol,tag);
 if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store ")+tag).c_str());

//
// Create the muon scintilator and position
 double a,n;
 std::string muname = "LAr::WarmTC::MuonWall";
 GeoBox *mu_box = new GeoBox(Muon_x/2, Muon_y/2, Muon_z/2);
 GeoLogVol *mu_log = new GeoLogVol(muname, mu_box, Scintillator);
 GeoPhysVol *mu_phys = new GeoPhysVol(mu_log);
 for(int i=1; i<=3; ++i) {
    a = -5.*i*Gaudi::Units::mm + (2*i-1)*Muon_x/2;
    n = pow(-1,i) * Muon_z/2 - z_m + Muon_z;
    GeoTrf::Vector3D posShift(a,0.0*Gaudi::Units::mm,n);
    m_WarmTCPhys->add(new GeoSerialIdentifier(6-i));
    m_WarmTCPhys->add(new GeoTransform(GeoTrf::Translate3D(a,0.0*Gaudi::Units::mm,n)));
    m_WarmTCPhys->add(mu_phys); 
    
    n = pow(-1,i+1) * Muon_z/2 - z_m + Muon_z;
    m_WarmTCPhys->add(new GeoSerialIdentifier(5+i));
    m_WarmTCPhys->add(new GeoTransform(GeoTrf::Translate3D(-a,0.0*Gaudi::Units::mm,n)));
    m_WarmTCPhys->add(mu_phys); 
    
 }

//
// Create Fe abs. plate and position them
//
 std::string aname = "LAr::WarmTC::Absorber";
 GeoBox *Fe_box = new GeoBox(Fe_x,Fe_y,Fe_z);
 GeoLogVol *Fe_log = new GeoLogVol(aname,Fe_box,Iron);  
 GeoPhysVol *Fe_phys = new GeoPhysVol(Fe_log);
 
 for(int i=0; i<4; i++) {
   m_WarmTCPhys->add(new GeoSerialIdentifier(i+1)); 
   m_WarmTCPhys->add(new GeoTransform(GeoTrf::Translate3D(0.0*Gaudi::Units::mm,0.0*Gaudi::Units::mm,z_Fe[i])));
   m_WarmTCPhys->add(Fe_phys);
 }
 
//
// Create X scintilators and position them
// 
 std::string sname = "LAr::WarmTC::Sci";
 GeoBox *X_box = new GeoBox(x_x,x_y,z_s);
 GeoLogVol *X_log = new GeoLogVol(sname+"::X", X_box,Scintillator);  
 GeoPhysVol *X_phys = new GeoPhysVol(X_log);
 
 for(int i=0; i<3; i++) {
    m_WarmTCPhys->add(new GeoSerialIdentifier(i+1));
    m_WarmTCPhys->add(new GeoTransform(GeoTrf::Translate3D(0.0*Gaudi::Units::mm,0.0*Gaudi::Units::mm,z_x[i])));
    m_WarmTCPhys->add(X_phys);
 }
 
//
// Create Y scintilators and position them
// 
 sname = "LAr::WarmTC::Sci";
 GeoBox *Y_box = new GeoBox(y_x,y_y,z_s);
 GeoLogVol *Y_log = new GeoLogVol(sname+"::Y", Y_box,Scintillator);  
 GeoPhysVol *Y_phys = new GeoPhysVol(Y_log);
 
 for(int i=0; i<3; i++) {
    m_WarmTCPhys->add(new GeoSerialIdentifier(i+1));
    m_WarmTCPhys->add(new GeoTransform(GeoTrf::Translate3D(0.0*Gaudi::Units::mm,0.0*Gaudi::Units::mm,z_y[i])));
    m_WarmTCPhys->add(Y_phys);
  }

 std::cout<<"About to return WarmTC PhysVol"<<std::endl;
 return m_WarmTCPhys;
}
