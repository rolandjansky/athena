/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ModulesConstructionH62004
#include "ModulesConstructionH62004.h"
#include "ExcluderConstructionH62004.h"
#include "HECConstructionH62004.h"
#include "FCALConstructionH62004.h"

// need if we want to use EMEC from LArGeoTBEC:
#include "LArGeoTBEC/EMECModuleConstruction.h"

#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoShapeShift.h"
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

#include "LArG4RunControl/LArGeoTB2004Options.h"


LArGeo::ModulesConstructionH62004::LeakGeom::LeakGeom()
{
  for(int i=0; i<NUM_LEAK; ++i) {
    m_dX[i] = m_dY[i] = m_dZ[i] = 0.;
    m_shiftX[i] = m_shiftY[i] = m_shiftZ[i] = 0.;
    m_angleX[i] = m_angleY[i] = m_angleZ[i] = 0.;
  }

  m_dX[0] = m_dX[1] = 1.*Gaudi::Units::cm; m_dY[0]= m_dY[1] = 31.6*Gaudi::Units::cm; m_dZ[0] = m_dZ[1] = 50.*Gaudi::Units::cm; 
  m_shiftX[0] = -26.*Gaudi::Units::cm; m_shiftX[1] = -m_shiftX[0];
  m_shiftY[0] = m_shiftY[1] = 85.1*Gaudi::Units::cm; m_shiftZ[0] = m_shiftZ[1] = -3.*Gaudi::Units::cm;
  m_angleX[0] = m_angleX[1] = 4.668*Gaudi::Units::deg; m_angleZ[0] = m_angleZ[1] = 0.*Gaudi::Units::deg;
  m_angleY[0] = -30.*Gaudi::Units::deg; m_angleY[1] = -m_angleY[0]; 
  m_dX[2] = 98.1*Gaudi::Units::cm; m_dY[2] = 98.2*Gaudi::Units::cm; m_dZ[2] = 30.6*Gaudi::Units::cm;
  m_shiftX[2] = 0.*Gaudi::Units::cm; m_shiftY[2] = 89.7*Gaudi::Units::cm; m_shiftZ[2] = -42.*Gaudi::Units::cm;
  m_angleX[2] = 94.668*Gaudi::Units::deg; m_angleY[2] = 0.; m_angleZ[2] = 90.*Gaudi::Units::degree;
    
    
  m_dX[3] = 1.*Gaudi::Units::cm; m_dY[3] = 43.*Gaudi::Units::cm; m_dZ[3] = 40.*Gaudi::Units::cm;
  m_dX[4] = m_dX[3]; m_dY[4] = m_dY[3]; m_dZ[4] = m_dZ[3];
  m_shiftX[3] = -58.5*Gaudi::Units::cm; m_shiftY[3] = 12.2*Gaudi::Units::cm; m_shiftZ[3] = 5.*Gaudi::Units::cm;
  m_shiftX[4] = -m_shiftX[3]; m_shiftY[4] = m_shiftY[3]; m_shiftZ[4] = m_shiftZ[3];
  m_angleX[3] = m_angleX[4] = 4.668*Gaudi::Units::deg; m_angleY[3] = -45.*Gaudi::Units::deg; 
  m_angleY[4] = -m_angleY[3]; 
  m_angleZ[3] = 0.*Gaudi::Units::deg; 
  m_angleZ[4] = -m_angleZ[3];
  m_dX[5] = 130.*Gaudi::Units::cm; m_dY[5] = 131.*Gaudi::Units::cm; m_dZ[5] = 43.*Gaudi::Units::cm; 
  m_shiftX[5] = 0.*Gaudi::Units::cm; m_shiftY[5] = 18.1*Gaudi::Units::cm; m_shiftZ[5] = -62.*Gaudi::Units::cm;
  m_angleX[5] = 94.668*Gaudi::Units::deg; m_angleY[5] = 0.*Gaudi::Units::deg;
  m_angleZ[5] = 90.*Gaudi::Units::deg;

  m_dX[6] = m_dX[7] = 1.*Gaudi::Units::cm; m_dY[6] = m_dY[7] = 27.*Gaudi::Units::cm; m_dZ[6] = m_dZ[7] = 40.*Gaudi::Units::cm; 
  m_shiftX[6] = -58.*Gaudi::Units::cm; m_shiftY[6] = m_shiftY[7] = -57.85*Gaudi::Units::cm; m_shiftZ[6] = m_shiftZ[7] = -1.*Gaudi::Units::cm;
  m_shiftX[7] = - m_shiftX[6]; 
  m_angleX[6] = m_angleX[7] = 4.668*Gaudi::Units::deg; m_angleY[6] = -45.*Gaudi::Units::deg; m_angleZ[6] = m_angleZ[7] = 0.*Gaudi::Units::deg;
  m_angleY[7] = -m_angleY[6];
  m_dX[8] = 130.*Gaudi::Units::cm; m_dY[8] = 131.*Gaudi::Units::cm; m_dZ[8] = 27.*Gaudi::Units::cm;
  m_shiftX[8] = 0.*Gaudi::Units::cm; m_shiftY[8] = -51.9*Gaudi::Units::cm; m_shiftZ[8] = -67.*Gaudi::Units::cm;
  m_angleX[8] = 94.668*Gaudi::Units::degree; m_angleY[8] = 0.*Gaudi::Units::degree; m_angleZ[8] = 90.*Gaudi::Units::degree;
  m_dX[9] = 1.*Gaudi::Units::cm; m_dY[9] = 82.*Gaudi::Units::cm; m_dZ[9] = 44.5*Gaudi::Units::cm;
  m_shiftX[9] = 0.*Gaudi::Units::cm; m_shiftY[9] = -89.0*Gaudi::Units::cm; m_shiftZ[9] = 32.5*Gaudi::Units::cm;
  m_angleX[9] = 4.668*Gaudi::Units::degree; m_angleY[9] = 0.*Gaudi::Units::degree; m_angleZ[9] = 90.*Gaudi::Units::degree;    
    
  m_dX[10] = m_dX[11] =  1.*Gaudi::Units::cm; m_dY[10] = m_dY[11] = 41.5*Gaudi::Units::cm; m_dZ[10] = m_dZ[11] = 20.3*Gaudi::Units::cm;
  m_shiftX[10] = -15.4*Gaudi::Units::cm; m_shiftY[10] = m_shiftY[11] = 14.50*Gaudi::Units::cm; m_shiftZ[10] = m_shiftZ[11] = -39.*Gaudi::Units::cm;
  m_shiftX[11] = - m_shiftX[10];
  m_angleX[10] = m_angleX[11] = 4.668*Gaudi::Units::degree; m_angleY[10] = -45.*Gaudi::Units::degree; m_angleZ[10] = 0.*Gaudi::Units::degree;    
  m_angleY[11] = -m_angleY[10]; m_angleZ[11] = -m_angleZ[10];    
  
  m_dX[12] = m_dX[13] = 1.*Gaudi::Units::cm; m_dY[12] = m_dY[13] = 27.*Gaudi::Units::cm; m_dZ[12] = m_dZ[13] = 20.3*Gaudi::Units::cm;
  m_shiftX[12] = -15.4*Gaudi::Units::cm; m_shiftY[12] = m_shiftY[13] = -54.4*Gaudi::Units::cm; m_shiftZ[12] = m_shiftZ[13] = -43.8*Gaudi::Units::cm;
  m_shiftX[13] = -m_shiftX[12];
  m_angleX[12]  = m_angleX[13] = 4.668*Gaudi::Units::degree; m_angleY[12] = -45.*Gaudi::Units::degree; m_angleZ[12] = 0.*Gaudi::Units::degree;
  m_angleY[13] = -m_angleY[12]; m_angleZ[13] = -m_angleZ[12];
      
  m_dX[14] = m_dX[15] = 1.*Gaudi::Units::cm; m_dY[14] = m_dY[15] = 12.*Gaudi::Units::cm; m_dZ[14] = m_dZ[15] = 25.3*Gaudi::Units::cm;
  m_shiftX[14] = -19.5*Gaudi::Units::cm; m_shiftY[14] = m_shiftY[15] = -93.5*Gaudi::Units::cm; m_shiftZ[14] = m_shiftZ[15] = -46.5*Gaudi::Units::cm;
  m_shiftX[15] = -m_shiftX[14];
  m_angleX[14] = m_angleX[15] = 4.668*Gaudi::Units::degree; m_angleY[14] = -45.*Gaudi::Units::degree; m_angleZ[14] = m_angleZ[15] = 0.*Gaudi::Units::degree;
  m_angleY[15] = -m_angleY[14]; 
    
  m_dX[16] = 59.5*Gaudi::Units::cm; m_dY[16] = 60.0*Gaudi::Units::cm; m_dZ[16] = 12.0*Gaudi::Units::cm;
  m_shiftX[16] = 0.*Gaudi::Units::cm; m_shiftY[16] = -91.5*Gaudi::Units::cm; m_shiftZ[16] = -73.5*Gaudi::Units::cm;
  m_angleX[16] = 94.668*Gaudi::Units::degree; m_angleY[16] = 0.*Gaudi::Units::degree; m_angleZ[16] = 90.*Gaudi::Units::degree;
  m_dX[17] = 0.3*Gaudi::Units::cm; m_dY[17] = 35.*Gaudi::Units::cm; m_dZ[17] = 25.*Gaudi::Units::cm;
  m_shiftX[17] = 0.*Gaudi::Units::cm; m_shiftY[17] = -107.0*Gaudi::Units::cm; m_shiftZ[17] = -40.*Gaudi::Units::cm;
  m_angleX[17] = 4.668*Gaudi::Units::degree; m_angleY[17] = 0.*Gaudi::Units::degree; m_angleZ[17] = 90.*Gaudi::Units::degree;
}


const LArGeo::ModulesConstructionH62004::LeakGeom
LArGeo::ModulesConstructionH62004::s_leakGeom;


LArGeo::ModulesConstructionH62004::ModulesConstructionH62004():m_ModulesPhys(0),m_Options(0),m_fcalVisLimit(-1)
{
  StatusCode status;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("DetectorStore", m_detectorStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in ModulesConstruction, cannot access DetectorStore");
  }

  status = m_detectorStore->retrieve(m_Options, "LArGeoTB2004Options");

  if(!status.isSuccess() ) {
     std::cout << "ModulesConstructionH62004:\tCan't access LArGeoTB2004Options, using default values\n";
  }
}

LArGeo::ModulesConstructionH62004::~ModulesConstructionH62004() {}

GeoVFullPhysVol* LArGeo::ModulesConstructionH62004::GetEnvelope()
{
  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
    throw std::runtime_error("Error in ModulesConstruction, cannot access Material manager");
  }

  if (m_ModulesPhys) return (m_ModulesPhys);
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in ModulesConstructionH62004, cannot access MessageSvc");
  }
  MsgStream log(msgSvc, "ModulesConstructionH62004");
  log << MSG::INFO;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "+         Start of H62004 GeoModel definition      +" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in ModulesConstruction, std::LiquidArgon is not found.");

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

  const GeoMaterial  *Alu = materialManager->getMaterial("std::Aluminium");
  if (!Alu) throw std::runtime_error("Error in ModulesConstruction, std::Aluminium is not found.");

  // DB numbers:
  double bryr_y = 200.0*Gaudi::Units::cm;
  double bcry_rlar = 125.5*Gaudi::Units::cm;
  double bcry_phistart = 0.*Gaudi::Units::degree;
  double bcry_phiend = 360.*Gaudi::Units::degree;
  //double EMECdzende         =  63.2*Gaudi::Units::cm;   // Can not get from parameters
  //double Zall = 62.6*cm; // Excluder dimension

  double   bepo_tx = 180.0*Gaudi::Units::degree;
  double   bepo_tz = 90.0*Gaudi::Units::degree;
  double   bepo_tz_e = ( M_PI / 4 )*Gaudi::Units::rad;
  double   bepo_ty = 90.0*Gaudi::Units::degree;
  double   bepo_Beta = 4.668*Gaudi::Units::degree;
  double   bepo_z_e = -42.86*Gaudi::Units::cm; // 43 Gaudi::Units::cm * cos(4.668)
  double   bepo_emec_shift = 2.5*Gaudi::Units::cm;
  double   bepo_excluder_shift = 34.4*Gaudi::Units::cm;
  //double   bepo_hec_shift = 63.6*Gaudi::Units::cm; // relative position of HEC versus EMEC
  //double   bepo_y_a = bcry_rlar-bepo_emec_shift-bepo_hec_shift;
  double   bepo_y_ex = bcry_rlar-bepo_excluder_shift;
  double   bepo_y_hecshift = 6.*Gaudi::Units::mm;
  double   bepo_y_emecshift = 11.*Gaudi::Units::mm;
  double   bepo_x = -2.75*Gaudi::Units::mm;
  double   bepo_x_e = -17.*Gaudi::Units::mm;
//  double   bepo_x = 13.25*Gaudi::Units::mm;
//  double   bepo_x_e = -3.*Gaudi::Units::mm;
//  double   bepo_x = 17.25*Gaudi::Units::mm;
//  double   bepo_x_e = -3.*Gaudi::Units::mm;
  //double   bepo_z = -48.24*Gaudi::Units::cm; // 48.4 Gaudi::Units::cm * cos(4.668)
  double   bepo_y_e = bcry_rlar-bepo_emec_shift;
  double   bepo_pz = 45.0*Gaudi::Units::degree;

  std::string baseName = "LArGeoTB::LeakageDet::";

  double ylen;
  if(m_Options && m_Options->isRun1()) {
     ylen = bryr_y;
  } else {
     ylen = bryr_y - 200.*Gaudi::Units::mm;
  }
  GeoTubs *shapeMother = new GeoTubs( 0.0*Gaudi::Units::cm, bcry_rlar, ylen, bcry_phistart,bcry_phiend);
  GeoLogVol *logMother = new GeoLogVol(baseName + "LAr", shapeMother, LAr);

  m_ModulesPhys = new GeoFullPhysVol(logMother);

  std::string tag = std::string("MODULES_POS");
  StatusCode status;
  StoredPhysVol *sPhysVol = new StoredPhysVol(m_ModulesPhys);
  status=m_detectorStore->record(sPhysVol,tag);
  if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());

// position leakage detectors
/*    
  for(int cal = 0; cal <= 5; ++cal){
    for(int dir = 0; dir <= 2; ++dir){
      for(int sid = 0; sid <= 1; ++sid){
        GeoLogVol *leak_log = construct(materialManager, sid, dir, cal);
        if(leak_log == 0) continue;
        leak_log->SetSensitiveDetector(LeakSDdetector);
	leak_log->SetVisAttributes(G4VisAttributes::Invisible);
//	leak_log->SetVisAttributes(new G4VisAttributes(G4Color(0.7,0.1,0.7)));
        GeoTrf::Transform3D leak_pos = position(sid, dir, cal);
        G4VPhysicalVolume *leak_phys =
        new G4PVPlacement(leak_pos,
                          leak_log,
                          leak_log->GetName(),
                          logMother,
                          false,
                          10000+GetID(sid,dir,cal)); 
      }
    }
  }
*/  
 
// position EMEC excluder
  
  ExcluderConstructionH62004 excluder(0);
  /*if(excluder != 0)*/{
//     std::cout<<"ModulesConstructionH62004::GetEnvelope positioning Excluder"<<std::endl;
    GeoVFullPhysVol* excluderEnvelope = excluder.GetEnvelope();
    /*if(excluderEnvelope != 0)*/{
      GeoTrf::Transform3D rot2 = GeoTrf::RotateX3D(bepo_Beta) * GeoTrf::RotateX3D(bepo_ty) * GeoTrf::RotateZ3D(bepo_tz);
      m_ModulesPhys->add(new GeoSerialIdentifier(0));
      m_ModulesPhys->add(new GeoTransform(GeoTrf::Translation3D(0.,bepo_y_ex,bepo_z_e+42.*Gaudi::Units::mm) * rot2));
      m_ModulesPhys->add(excluderEnvelope);
    }
  }
  

// position FCAL excluder
  
 
  ExcluderConstructionH62004 fcexcluder(1);
  /*if(fcexcluder != 0)*/{
//     std::cout<<"ModulesConstructionH62004::GetEnvelope positioning Excluder"<<std::endl;
    GeoVFullPhysVol* fcexcluderEnvelope = fcexcluder.GetEnvelope();
    if(fcexcluderEnvelope != 0){
      GeoTrf::Transform3D rot2 = GeoTrf::RotateX3D(0.8*bepo_Beta) * GeoTrf::RotateX3D(-bepo_ty) * GeoTrf::RotateZ3D(bepo_tx);
      m_ModulesPhys->add(new GeoSerialIdentifier(0));
//      m_ModulesPhys->add(new GeoTransform(GeoTrf::Transform3D(rot2,GeoTrf::Vector3D(0.,bepo_y_ex-138.*Gaudi::Units::mm,-477.3*Gaudi::Units::mm))));
      m_ModulesPhys->add(new GeoTransform(GeoTrf::Translation3D(0.,bepo_y_ex-146.*Gaudi::Units::mm,-412.0*Gaudi::Units::mm) * rot2));
      m_ModulesPhys->add(fcexcluderEnvelope);
    }
  }


// position Front excluder
  
  ExcluderConstructionH62004 frontexcluder(2);

  /*if(frontexcluder != 0)*/{
    GeoVFullPhysVol* frontexcluderEnvelope = frontexcluder.GetEnvelope();
    if(frontexcluderEnvelope != 0){
      GeoTrf::RotateZ3D rot2((90.)*Gaudi::Units::degree);
      m_ModulesPhys->add(new GeoSerialIdentifier(0));
      m_ModulesPhys->add(new GeoTransform(GeoTrf::Translation3D(0.,0.,20.*Gaudi::Units::mm) * rot2));
      m_ModulesPhys->add(frontexcluderEnvelope);
      /*
      G4VPhysicalVolume* frontexcluderPhysical =
//	    new G4PVPlacement(GeoTrf::Transform3D(rot2,GeoTrf::Vector3D(0.,0.,220.*Gaudi::Units::mm)), // Translation 
	    new G4PVPlacement(GeoTrf::Transform3D(rot2,GeoTrf::Vector3D(0.,0.,20.*Gaudi::Units::mm)), // Translation 
			      frontexcluderEnvelope,                     // Logical volume
			      frontexcluderEnvelope->GetName(),          // Name 
			      logMother,                        // Mother volume 
			      false,                            // Boolean volume?  
			      0);                               // Copy number
			      */
    }
  }


// position Back excluder
  
  ExcluderConstructionH62004 backexcluder(3);

  /*if(backexcluder != 0)*/{
    GeoVFullPhysVol* backexcluderEnvelope = backexcluder.GetEnvelope();
    if(backexcluderEnvelope != 0){
      GeoTrf::RotateZ3D rot2((-90.-29.)*Gaudi::Units::degree);
      m_ModulesPhys->add(new GeoSerialIdentifier(0));
      m_ModulesPhys->add(new GeoTransform(GeoTrf::Translation3D(0.,0.,0.*Gaudi::Units::mm) * rot2));
      m_ModulesPhys->add(backexcluderEnvelope);
      /*
      G4VPhysicalVolume* backexcluderPhysical =
	    new G4PVPlacement(GeoTrf::Transform3D(rot2,GeoTrf::Vector3D(0.,0.,0.*Gaudi::Units::mm)), // Translation 
			      backexcluderEnvelope,                     // Logical volume
			      backexcluderEnvelope->GetName(),          // Name 
			      logMother,                        // Mother volume 
			      false,                            // Boolean volume?  
			      0);                               // Copy number
			      */
    }
  }
  
  

  // Let's try assembly volume

 // G4AssemblyVolume* assemblyMother = new G4AssemblyVolume();

  // Transformation for assembly - should be putted to each piece now.
  GeoTrf::Transform3D rota = GeoTrf::RotateX3D(bepo_Beta) * GeoTrf::RotateX3D(bepo_ty) * GeoTrf::RotateZ3D(-bepo_pz);
  GeoTrf::Transform3D trans = GeoTrf::Translation3D(0.,bepo_y_e,bepo_z_e+65.*Gaudi::Units::mm) * rota;
  
//positions emec

  // Z-positions DB values !!!!

  double HECzStart         =  427.7*Gaudi::Units::cm;
  double EMECzStart         = 364.1*Gaudi::Units::cm;
  double FCALzStart         =  466.85*Gaudi::Units::cm;
  double FCALzEnd           =  588.28*Gaudi::Units::cm;

  if((!m_Options) || m_Options->isEmec()){
    GeoTrf::RotateZ3D MrotEmec(bepo_tz_e);
     // original value:
//     GeoTrf::Vector3D pos3Emec(    0*Gaudi::Units::mm,   9.0*Gaudi::Units::mm ,   55.9*Gaudi::Units::mm);
//     GeoTrf::Vector3D pos3Emec(    3.636*Gaudi::Units::mm,   9.0*Gaudi::Units::mm ,   55.9*Gaudi::Units::mm);
//     GeoTrf::Vector3D pos3Emec(    bepo_x_e,   9.*Gaudi::Units::mm ,   61.*Gaudi::Units::mm);
    GeoTrf::Translation3D pos3Emec((bepo_x_e - bepo_y_emecshift )/2./sin(bepo_tz_e)
				   , (bepo_x_e + bepo_y_emecshift )/2./sin(bepo_tz_e)
				   , 61.*Gaudi::Units::mm);
//     GeoTrf::Vector3D pos3Emec(    0.*Gaudi::Units::mm, bepo_x_e,   61.*Gaudi::Units::mm);

//     std::cout<<"ModulesConstructionH62004 calling EMECModuleConstruction....."<<std::endl;
  //use this line for physical construction of the EMEC inner wheel only:
//     EMECModuleConstruction*  emecConstruction = new EMECModuleConstruction(true, true, false);
     EMECModuleConstruction emecConstruction(true, true, false);

     GeoVFullPhysVol* emecEnvelope = emecConstruction.GetEnvelope();
     m_ModulesPhys->add(new GeoNameTag("LAr"));
     m_ModulesPhys->add( new GeoTransform(trans) );
     m_ModulesPhys->add( new GeoTransform( pos3Emec * MrotEmec ));
     m_ModulesPhys->add(emecEnvelope);
//     std::cout<<"ModulesConstructionH62004  EMECModuleConstruction done...."<<std::endl;
  }


  if((!m_Options) || m_Options->isHec()){
    //positions hec

    HECConstructionH62004 hec;
    /*if(hec != 0)*/{
      GeoVPhysVol* hecEnvelope = hec.GetEnvelope();
      if(hecEnvelope != 0){
//        rot.rotateZ(-bepo_pz);
//        rot.rotateX(bepo_ty);
//        rot.rotateX(bepo_Beta);
  
//        GeoTrf::Transform3D hpos = GeoTrf::Transform3D(rot,GeoTrf::Vector3D(bepo_x,bepo_y_a,bepo_z));
//        GeoTrf::Vector3D hecshift(0.,6.*Gaudi::Units::mm,HECzStart-EMECzStart);
//          GeoTrf::Vector3D hecshift((bepo_x - bepo_y_hecshift)/2./sin(bepo_tz_e), (bepo_x + bepo_y_hecshift)/2./sin(bepo_tz_e), HECzStart-EMECzStart);
//        GeoTrf::Vector3D hecshift(-21.*Gaudi::Units::mm, bepo_x, HECzStart-EMECzStart);
//        GeoTrf::Vector3D hecshift(-5.*Gaudi::Units::mm, bepo_x, HECzStart-EMECzStart);
//        GeoTrf::Vector3D hecshift(0.*Gaudi::Units::mm, bepo_x, HECzStart-EMECzStart);
//       Gaudi::Units::HepRotation norot;
        m_ModulesPhys->add(new GeoTransform(trans));
        m_ModulesPhys->add( new GeoTransform(GeoTrf::Translate3D((bepo_x - bepo_y_hecshift)/2./sin(bepo_tz_e)
								 , (bepo_x + bepo_y_hecshift)/2./sin(bepo_tz_e)
								 , HECzStart-EMECzStart)) );
        m_ModulesPhys->add(hecEnvelope);

//       assemblyMother->AddPlacedVolume( hecEnvelope, hectrans );
      }
    }
  }
   
  if((!m_Options) || m_Options->isFcal()){ 
    FCALConstructionH62004 fcal;
    /*if(fcal != 0)*/{
      fcal.setFCALVisLimit(m_fcalVisLimit); 
      GeoVPhysVol* fcalEnvelope = fcal.GetEnvelope();
      if(fcalEnvelope != 0){
//        Gaudi::Units::HepRotation rotFCal;
       // rotFCal.rotateY(0.*Gaudi::Units::deg);
       //  rotFCal.rotateZ(-bepo_pz);
       //  rotFCal.rotateX(bepo_ty);
       //  rotFCal.rotateX(bepo_Beta);
//        GeoTrf::Vector3D fcalshift(0.,-7.*Gaudi::Units::mm,FCALzStart-EMECzStart+(FCALzEnd-FCALzStart)/2.);
        m_ModulesPhys->add(new GeoTransform(trans));
        m_ModulesPhys->add( new GeoTransform(GeoTrf::Translate3D(9.*Gaudi::Units::mm
								 ,0.*Gaudi::Units::mm
								 ,FCALzStart-EMECzStart+(FCALzEnd-FCALzStart)/2.)) );
        m_ModulesPhys->add(fcalEnvelope);
      }
    }
  }
  
  if((!m_Options) || m_Options->isColdnose()){
  // Position a cold nose
  //
  
  double box_x = (650./2.)*Gaudi::Units::mm;
  double box_y = (356./2.)*Gaudi::Units::mm;
  double box_z = (50.8/2.)*Gaudi::Units::mm;
  double btot = 494.*Gaudi::Units::mm;
  double tub_r = 452.*Gaudi::Units::mm;
  double tub1_dr = 8.*Gaudi::Units::mm;
  double tub1_z = (1320./2.)*Gaudi::Units::mm;
  double cyl_dr = 10.*Gaudi::Units::mm;
  double cyl_r1 = 262.*Gaudi::Units::mm;
  double cyl_r2 = 336.5*Gaudi::Units::mm;
  double cyl_z = (912./2.)*Gaudi::Units::mm;
  double cyl_shift = (10. + 25.)*Gaudi::Units::mm; 
//  double NoseZshift = -360.*Gaudi::Units::mm;
  double NoseZshift = -63.1*Gaudi::Units::mm;
//  double NoseYshift = 94.4*Gaudi::Units::mm;
//  double NoseYshift = 96.4*Gaudi::Units::mm;
  double NoseYshift = 98.4*Gaudi::Units::mm;
//  double NoseXshift = -195.*Gaudi::Units::mm;
  double NoseXshift = -94.4*Gaudi::Units::mm;

  GeoBox* Box1 = new GeoBox(box_x, box_y, box_z);
  double alpha = acos(box_x/tub_r);
  double ax = M_PI - 2*alpha;
  GeoTubs* Tub= new GeoTubs(0., tub_r, box_z, alpha, ax);

//  tRot.rotateX(90*Gaudi::Units::degree);
  GeoTrf::Translate3D TubTrans(0.,btot-box_y-tub_r,0.);
  const GeoShapeUnion &uSolid = (*Box1).add((*Tub)<<TubTrans);

  GeoTubs* Tub1 = new GeoTubs(tub_r, tub_r+tub1_dr, tub1_z, alpha, ax);  
//  GeoTrf::Vector3D TubShift2(0.,btot-box_y-tub_r,-tub1_z+box_z);
//  GeoTrf::Vector3D TubShift2(0.,0.,0.);
//  tRot.rotateX(90*Gaudi::Units::degree);
  GeoTrf::Translate3D UnTrans(0.,-135.5*Gaudi::Units::mm,-tub1_z+box_z);
  const GeoShapeUnion &uSolid2 = uSolid.add((*Tub1)<<UnTrans);

  GeoCons* Cone = new GeoCons(cyl_r2, cyl_r1, cyl_r2+cyl_dr, cyl_r1+cyl_dr, cyl_z, M_PI/4.,M_PI/2.);
//  GeoTrf::Vector3D CylShift(0.,-box_y+cyl_shift,cyl_z+box_z-3.*Gaudi::Units::mm);
  GeoTrf::Translation3D CylShift(0.,-box_y+cyl_shift,cyl_z+box_z-5.*Gaudi::Units::mm);
//  GeoTrf::Vector3D CylShift(0.,0.,cyl_z+box_z);
  GeoTrf::RotateX3D tRot(1.*Gaudi::Units::degree);
//  tRot.rotateZ(-90*Gaudi::Units::degree);
  GeoTrf::Transform3D CylTrans = CylShift * tRot;
  const GeoShapeUnion &uSolid3 =  uSolid2.add((*Cone)<<CylTrans);

  GeoLogVol* Cnose = new GeoLogVol("LArGeoTB::LeakageDet::ColdNose", &uSolid3 , Alu);
  GeoPhysVol *PhysCnose = new GeoPhysVol(Cnose);
//  cnose->SetVisAttributes(G4VisAttributes::Invisible);
  GeoTrf::Transform3D CRotation = GeoTrf::RotateZ3D(-3*M_PI/4.) * GeoTrf::RotateX3D(M_PI);
//  GeoTrf::Vector3D tShift(NoseXshift,0.,FCALzStart-EMECzStart+NoseZshift);
  GeoTrf::Translation3D tShift(NoseXshift,NoseYshift,FCALzStart-EMECzStart+NoseZshift);
//  GeoTrf::Vector3D tShift(0.,NoseYshift,NoseZshift);
  GeoTrf::Transform3D CTrans = tShift * CRotation;
  m_ModulesPhys->add( new GeoTransform(trans) ) ;
  m_ModulesPhys->add(new GeoTransform(CTrans));
  m_ModulesPhys->add(PhysCnose);
//  assemblyMother->AddPlacedVolume( cnose, CTrans );
  /*
  G4VPhysicalVolume* nosePhysical = new
        G4PVPlacement(GeoTrf::Transform3D(CRotation,tShift),
                      cnose, cnose->GetName(),logMother, false, 0);                
  */ 

  // Do an imprint of assembly:
  
  /*
  Gaudi::Units::HepRotation rota;
  rota.rotateZ(-bepo_pz);
  rota.rotateX(bepo_ty);
  rota.rotateX(bepo_Beta);
  
  GeoTrf::Transform3D trans(rota, GeoTrf::Vector3D(0.,bepo_y_e,bepo_z_e));
  
  assemblyMother->MakeImprint( logMother, trans );  
  */
  } 
  return m_ModulesPhys;
}

int LArGeo::ModulesConstructionH62004::GetID(int side, int dir, int calo)
{
     int i;
     if(calo<0 || calo >5) return 0;
     if(side<0 || side >1) return 0;

     switch(dir) {
        case 0: {
                   if(calo<=2) i=1; else i=5-calo;
                   return 3*calo+i+side;
                }
        case 2: {
                   if(side==1) return 0;
                   if(calo==2) return 10;
                   else if(calo==5) return 18;
                   else return 0;
                }
        case 1: {
                   if(side==1) return 0;
                   if(calo==3 || calo==4) return 0;
                   if(calo==5) i=2; else i=3;
                   return 3*calo+i;
                }

        default : {return 0;}
     }

}


GeoLogVol*
LArGeo::ModulesConstructionH62004::construct(const StoredMaterialManager* materialManager,
                                             int side, int dir, int calo)
{
//--------- Material definition for leakage detectors ---------
  //double density, pressure, temperature;
  std::string name;
  
  //density     = universe_mean_density;    //from PhysicalConstants.h
  //pressure    = 3.e-18*pascal;
  //temperature = 2.73*kelvin;
  const GeoMaterial* Vacuum = materialManager->getMaterial("std::Vacuum");
  if (!Vacuum) throw std::runtime_error("Error in ModulesConstruction, std::Vacuum is not found.");

//----------------- construct ID and name
  int myID = GetID(side,dir,calo);
  if(myID == 0 || myID > NUM_LEAK) return 0;
  if(s_leakGeom.m_dX[myID-1] == 0 && s_leakGeom.m_dY[myID-1] == 0 && s_leakGeom.m_dZ[myID-1] == 0) return 0;
  name = "LArGeoTB::LeakageDet::";  
  switch(calo){
  case 0:
    name += "emec";
    break;
  case 1:
    name += "hec1";
    break;
  case 2:
    name += "hec2";
    break;
  case 3:
    name += "fcal1";
    break;
  case 4:
    name += "fcal2";
    break;
  case 5:
    name += "coldtc";
    break;
  default:
    std::cerr << "Wrong calo for leakage !!" << std::endl;
    return 0;
  }
  switch(dir){
  case 0:
    name += "_side";
    break;
  case 1:
    name += "_up";
    break;
  case 2:
    name += "_back";
    break;
  default:
    std::cerr << "Wrong direction for leakage !!" << std::endl;
    return 0;
  }
  if(dir != 1){
    switch(side){
    case 0:
      name += "_left";
      break;
    case 1:
      name += "_right";
      break;
    default:
      std::cerr << "Wrong side for leakage !!" << std::endl;
      return 0;
    }
  }

//  std::cout<<"ModulesConstructionH62004::construct: "<<myID<<": "<<name<<std::endl;

//------------------ now construct shape and logical volume ---------------
  GeoLogVol *volume_log;
  if(myID == 6 || myID == 9 || myID == 17) {
     GeoTubs *tub = new GeoTubs(s_leakGeom.m_dX[myID-1],s_leakGeom.m_dY[myID-1],s_leakGeom.m_dZ[myID-1],-43.*Gaudi::Units::degree,86.*Gaudi::Units::degree);
     volume_log = new GeoLogVol(name,tub,Vacuum); 
  } else if(myID == 3) {
     GeoTubs *tub = new GeoTubs(s_leakGeom.m_dX[myID-1],s_leakGeom.m_dY[myID-1],s_leakGeom.m_dZ[myID-1],-32.*Gaudi::Units::degree,64.*Gaudi::Units::degree);
     volume_log = new GeoLogVol(name,tub,Vacuum); 
#if 0 // impossible case...
  } else if(myID == 19) {
    GeoTrd *trd = new GeoTrd(s_leakGeom.m_dX[myID-1]-16.*Gaudi::Units::cm,s_leakGeom.m_dX[myID-1],s_leakGeom.m_dY[myID-1],s_leakGeom.m_dY[myID-1],s_leakGeom.m_dZ[myID-1]);
    volume_log = new GeoLogVol(name,trd,Vacuum); 
#endif
  } else {
    GeoBox *mbox = new GeoBox(s_leakGeom.m_dX[myID-1], s_leakGeom.m_dY[myID-1], s_leakGeom.m_dZ[myID-1]);
    volume_log = new GeoLogVol(name,mbox,Vacuum);
  }
  return volume_log;
}

GeoTrf::Transform3D LArGeo::ModulesConstructionH62004::position(int side, int dir, int calo)
{
  GeoTrf::Transform3D leak_position(GeoTrf::Transform3D::Identity());

  int myID = GetID(side,dir,calo);
  if(myID == 0) {
    std::cerr << "Error! First construct the leakage detector !!" << std::endl;
  }
  assert(myID>0 && myID <= NUM_LEAK);
  if(myID > NUM_LEAK) return leak_position;

  GeoTrf::Transform3D rotM = GeoTrf::RotateX3D(s_leakGeom.m_angleX[myID-1]) * GeoTrf::RotateY3D(s_leakGeom.m_angleY[myID-1]) * GeoTrf::RotateZ3D(s_leakGeom.m_angleZ[myID-1]);

  leak_position = GeoTrf::Transform3D(GeoTrf::Translation3D(s_leakGeom.m_shiftX[myID-1],s_leakGeom.m_shiftY[myID-1],s_leakGeom.m_shiftZ[myID-1]) * rotM);

  return leak_position;
}
     		      
