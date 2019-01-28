/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// This will construct a generic BPC for the H6 beamline that leads to the H1 cryostat.
// There are two kinds of BPCs -  old and new one
#include "LArGeoH6Cryostats/BPCConstruction.h"

#include "GeoModelKernel/GeoElement.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoVFullPhysVol.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoSerialDenominator.h"  
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/Units.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoGenericFunctions/Variable.h"

// For the database:
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <string>
#include <cmath>
#include <iostream>

LArGeo::BPCConstruction::BPCConstruction(bool old)
   :m_BPCPhysical(0),
    m_msg(0) 
{
  m_oldType = old;
}


LArGeo::BPCConstruction::~BPCConstruction()
{
}



GeoVPhysVol* LArGeo::BPCConstruction::GetEnvelope()
{

  if (m_BPCPhysical) return m_BPCPhysical;

  // Message service:  
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);
  
  if(!status.isFailure()){
    m_msg = new MsgStream(msgSvc, "BPCConstruction");
  } else {
    throw std::runtime_error("BPCConstruction: cannot initialze message service");
  }

  (*m_msg) << MSG::INFO << "BPCConstruction - creating an BPC oldType: " << m_oldType << " !  " << endmsg;



  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in BPCConstruction, cannot access DetectorStore");
  }


  ServiceHandle<IGeoModelSvc> geoModelSvc ("GeoModelSvc", "BPCConstruction");
  if (geoModelSvc.retrieve().isFailure()) {
    throw std::runtime_error ("Cannot locate m_geoModelSvc!!");
  }



  // Get the materials from the material manager:-----------------------------------------------------//
  //                                                                                                  //

  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;

  std::string name;
  double density;
  const GeoElement* W=materialManager->getElement("Wolfram");
  GeoMaterial* Tungsten = new GeoMaterial(name="Tungsten", density=19.3*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  Tungsten->add(W,1.);
  Tungsten->lock();
  
  
  const GeoElement* Ar=materialManager->getElement("Argon");
  const GeoElement*  C=materialManager->getElement("Carbon");
  const GeoElement*  O=materialManager->getElement("Oxygen");
  const GeoElement*  H=materialManager->getElement("Hydrogen");
  const GeoElement*  Al=materialManager->getElement("Aluminium");
  GeoMaterial* CO2 =  new GeoMaterial(name="CO2", density=1.84E-03*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  CO2->add(C,0.273);
  CO2->add(O,0.727);
  CO2->lock();
  GeoMaterial* ArCO2_1 = new GeoMaterial(name="ArCO2_1", density=(0.8*1.782e-03 + 0.2*1.84E-03)*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  ArCO2_1->add(Ar,0.8);
  ArCO2_1->add(CO2,0.2);
  ArCO2_1->lock();
  GeoMaterial* ArCO2_2 = new GeoMaterial(name="ArCO2_2", density=(0.9*1.782e-03 + 0.1*1.84E-03)*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  ArCO2_2->add(Ar,0.9);
  ArCO2_2->add(CO2,0.1);
  ArCO2_2->lock();
  // AlMylar   AlC5H4O2 ??????    
  density = 1.39*GeoModelKernelUnits::g/Gaudi::Units::cm3;
  GeoMaterial* AlMylar=new GeoMaterial(name="AlMylar",density=1.39*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  AlMylar->add(C,0.487980);
  AlMylar->add(O,0.260014);
  AlMylar->add(H,0.032761);
  AlMylar->add(Al,0.219245);
  AlMylar->lock();

  const GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in BPCConstruction, std::Air is not found.");
   
  const GeoMaterial *Aluminium  = materialManager->getMaterial("std::Aluminium");
  if (!Aluminium) throw std::runtime_error("Error in BPCConstruction, std::Aluminium is not found.");
  
  const GeoMaterial *Mylar  = materialManager->getMaterial("std::Mylar");
  if (!Mylar) throw std::runtime_error("Error in BPCConstruction, std::Mylar is not found.");
  
  
  //-------------------------------------------------------------------------------------------------//


  
  std::string AtlasVersion = geoModelSvc->atlasVersion();
  std::string LArVersion = geoModelSvc->LAr_VersionOverride();

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";


  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////
  double bpc_x = 15.0*Gaudi::Units::cm;
  double bpc_y = 15.0*Gaudi::Units::cm;
  double bpc_z = (8.684/2)*Gaudi::Units::cm;
  double bpc_send = (1.14/2)*Gaudi::Units::cm;
  double bpc_sen = (1.06)*Gaudi::Units::cm;
  double bpc_div = 5.06*Gaudi::Units::mm;
  //double bpc_space = 2.6*Gaudi::Units::mm;
  double bpc_ml = 0.0010*Gaudi::Units::cm;
  double bpc_alml = 0.0012*Gaudi::Units::cm;
  double bpc_frame = 12.1*Gaudi::Units::mm;
  double bpc_alframe = 8.*Gaudi::Units::mm;
  double bpc_step = 0.6*Gaudi::Units::cm;
  double bpc_cstep = 0.3*Gaudi::Units::cm;
  double bpc_wd = 0.0020*Gaudi::Units::cm;
  double bpc_cwd = 0.0100*Gaudi::Units::cm;
  
  double bpc_old_x = 12.0*Gaudi::Units::cm;
  double bpc_old_y = 12.0*Gaudi::Units::cm;
  double bpc_old_z = (5.100/2)*Gaudi::Units::cm;
  double bpc_old_div = 7.6*Gaudi::Units::mm;
  double bpc_old_alml = 0.0020*Gaudi::Units::cm;
  double bpc_old_ml = 0.0050*Gaudi::Units::cm;
  double bpc_old_frame = 10.*Gaudi::Units::mm;
  double bpc_old_alframe = 2.*Gaudi::Units::mm;
  double bpc_old_alframe1 = 12.*Gaudi::Units::mm;
  double bpc_old_send = (1.7/2)*Gaudi::Units::cm;
  double bpc_old_sen = 0.5*Gaudi::Units::cm;
  double bpc_old_space = 1.*Gaudi::Units::mm;
  double bpc_old_step = 0.6*Gaudi::Units::cm;
  double bpc_old_cstep = 0.3*Gaudi::Units::cm;
 

  //  Here we creat the envelope for the Moveable FrontBeam Instrumentation.  This code is repeated
  //  createEnvelope() method below.  There should be a way to avoid this repitition.


  //------ Now create a BPC
 
  (*m_msg)  << MSG::INFO <<" Create BPC " << endmsg;

  std::string BPCName;
  if(m_oldType) BPCName = "LAr::TB::BPCOLD"; else BPCName = "LAr::TB::BPC";
  
  // This creates a square chamber: 
  GeoBox* shape_bpc;
  if(m_oldType) shape_bpc = new GeoBox(bpc_old_x, bpc_old_y, bpc_old_z); 
  else          shape_bpc = new GeoBox(bpc_x, bpc_y, bpc_z);
  GeoLogVol* log_bpc;
  if(m_oldType) log_bpc = new GeoLogVol(BPCName, shape_bpc, ArCO2_2);
  else          log_bpc = new GeoLogVol(BPCName, shape_bpc, ArCO2_1);
  m_BPCPhysical = new GeoPhysVol(log_bpc);
  
  // AlMylar wall inside BPS
  GeoBox* shape_bpc_almylar;
  if(m_oldType) shape_bpc_almylar = new GeoBox(bpc_old_x, bpc_old_y, bpc_old_alml);
  else          shape_bpc_almylar = new GeoBox(bpc_x, bpc_y, bpc_alml);
  GeoLogVol* log_bpc_almylar = new GeoLogVol(BPCName + "::bpcalmylar",shape_bpc_almylar, AlMylar);
  GeoPhysVol* phys_bpc_almylar = new GeoPhysVol(log_bpc_almylar); 
  for(int i = 0; i < 2; i ++){
    double mylar_pos = 0;
    if(m_oldType) {
       if(i == 0) mylar_pos = bpc_old_z - bpc_old_alml;
       if(i == 1) mylar_pos = bpc_old_alml - bpc_old_z;
    } else {
       if(i == 0) mylar_pos = bpc_z - bpc_alml;
       if(i == 1) mylar_pos = bpc_alml - bpc_z;
    }
    m_BPCPhysical->add( new GeoIdentifierTag( i ) );
    m_BPCPhysical->add( new GeoTransform( GeoTrf::Translate3D(0., 0., mylar_pos) ) );
    m_BPCPhysical->add( phys_bpc_almylar );
  }

  // Mylar wall inside BPC
  GeoBox* shape_bpc_mylar;
  if(m_oldType) shape_bpc_mylar = new GeoBox(bpc_old_x, bpc_old_y, bpc_old_ml);
  else          shape_bpc_mylar = new GeoBox(bpc_x, bpc_y, bpc_ml);
  GeoLogVol* log_bpc_mylar = new GeoLogVol(BPCName + "::bpc_mylar", shape_bpc_mylar, Mylar);
  GeoPhysVol* phys_bpc_mylar = new GeoPhysVol(log_bpc_mylar);
  for(int i = 0; i < 2; ++i){
    double mylar_pos = 0;
    if(m_oldType) {
//       if(i == 0) mylar_pos = bpc_old_z - bpc_old_frame - bpc_old_alframe - bpc_old_ml/2.;
//       if(i == 1) mylar_pos = - bpc_old_z + bpc_old_ml/2. + bpc_old_frame + bpc_old_alframe1;
       if(i == 0) mylar_pos = bpc_old_z - bpc_old_frame - bpc_old_alframe - bpc_old_ml + bpc_old_space;
       if(i == 1) mylar_pos = - bpc_old_z + bpc_old_ml + bpc_old_frame + bpc_old_alframe1 - bpc_old_space;
    } else {
       if(i == 0) mylar_pos = bpc_z - bpc_frame - bpc_alframe - bpc_alml;
       if(i == 1) mylar_pos = bpc_alml - bpc_z + bpc_frame + bpc_alframe;
    }
    m_BPCPhysical->add( new GeoIdentifierTag( i ) );
    m_BPCPhysical->add( new GeoTransform( GeoTrf::Translate3D(0., 0., mylar_pos) ) );
    m_BPCPhysical->add( phys_bpc_mylar );
  }
 

  // X sensitive plane (old has only one sensitive plane, so we choose X)
  GeoBox* shape_bpc_xplane;
  if(m_oldType) shape_bpc_xplane = new GeoBox(bpc_old_x, bpc_old_y, bpc_old_send);
  else          shape_bpc_xplane = new GeoBox(bpc_x, bpc_y, bpc_send);
  GeoLogVol* log_bpc_xplane;
  if(m_oldType) log_bpc_xplane = new GeoLogVol(BPCName + "::bpco_plane", shape_bpc_xplane, ArCO2_2);
  else          log_bpc_xplane = new GeoLogVol(BPCName + "::bpc_xplane", shape_bpc_xplane, ArCO2_1);
  GeoPhysVol* phys_bpc_xplane = new GeoPhysVol(log_bpc_xplane);
  m_BPCPhysical->add( new GeoIdentifierTag( 0 ) );
  if(m_oldType) m_BPCPhysical->add( new GeoTransform( GeoTrf::Translate3D(0., 0., bpc_old_sen) ) );
  else          m_BPCPhysical->add( new GeoTransform( GeoTrf::Translate3D(0., 0., -bpc_sen-bpc_send) ) );
  m_BPCPhysical->add(phys_bpc_xplane); 
  
  // division of X plane 
  int Ndiv;
  if(m_oldType) Ndiv = int(2.0*bpc_old_x/bpc_old_step);
  else          Ndiv = int(2.0*bpc_x/bpc_step);
  GeoBox* shape_bpc_xdiv;
  if(m_oldType) shape_bpc_xdiv = new GeoBox(bpc_old_step/2., bpc_old_y, bpc_old_div);
  else          shape_bpc_xdiv = new GeoBox(bpc_step/2., bpc_y, bpc_div);
  GeoLogVol* log_bpc_xdiv;
  if(m_oldType) log_bpc_xdiv = new GeoLogVol(BPCName + "::bpco_div", shape_bpc_xdiv, ArCO2_2);
  else          log_bpc_xdiv = new GeoLogVol(BPCName + "::bpc_xdiv", shape_bpc_xdiv, ArCO2_1);
  GeoPhysVol* phys_bpc_xdiv = new GeoPhysVol(log_bpc_xdiv);
  GeoGenfun::Variable Index;
  GeoXF::TRANSFUNCTION TXO = GeoXF::Pow(GeoTrf::TranslateX3D(1.0), -bpc_old_x+(2*Index+1)*bpc_old_step/2.);
  GeoXF::TRANSFUNCTION TX = GeoXF::Pow(GeoTrf::TranslateX3D(1.0), -bpc_x+(2*Index+1)*bpc_step/2.);
  phys_bpc_xplane->add( new GeoSerialIdentifier(0) );
  if(m_oldType) phys_bpc_xplane->add( new GeoSerialTransformer(phys_bpc_xdiv,  &TXO, Ndiv ) );
  else          phys_bpc_xplane->add( new GeoSerialTransformer(phys_bpc_xdiv,  &TX, Ndiv ) );

  // Y sensitive plane
  GeoPhysVol* phys_bpc_yplane = 0;
  GeoPhysVol* phys_bpc_ydiv = 0;
  if(!m_oldType) {
     GeoBox* shape_bpc_yplane = new GeoBox(bpc_x, bpc_y, bpc_send);
     GeoLogVol* log_bpc_yplane = new GeoLogVol(BPCName + "::bpc_yplane",shape_bpc_yplane, ArCO2_1);
     phys_bpc_yplane = new GeoPhysVol(log_bpc_yplane);
     m_BPCPhysical->add( new GeoIdentifierTag( 0 ) );
     m_BPCPhysical->add( new GeoTransform( GeoTrf::Translate3D(0., 0., bpc_sen+bpc_send) ) );
     m_BPCPhysical->add( phys_bpc_yplane );

     // division of Y plane 
     GeoBox* shape_bpc_ydiv = new GeoBox(bpc_x, bpc_step/2.,bpc_div);
     GeoLogVol* log_bpc_ydiv = new GeoLogVol(BPCName + "::bpc_ydiv", shape_bpc_ydiv, ArCO2_1);
     phys_bpc_ydiv = new GeoPhysVol(log_bpc_ydiv);
     GeoXF::TRANSFUNCTION TY = GeoXF::Pow(GeoTrf::TranslateY3D(1.0), -bpc_y+(2*Index+1)*bpc_step/2);
     phys_bpc_yplane->add( new GeoSerialIdentifier(0) );
     phys_bpc_yplane->add( new GeoSerialTransformer(phys_bpc_ydiv, &TY, Ndiv ) );
  }
  
  // wires in each division
  GeoTubs* shape_bpc_wire;
  if(m_oldType) shape_bpc_wire = new GeoTubs(0., bpc_wd, bpc_old_x, 0.*Gaudi::Units::deg, 360.*Gaudi::Units::deg);
  else          shape_bpc_wire = new GeoTubs(0., bpc_wd, bpc_x, 0.*Gaudi::Units::deg, 360.*Gaudi::Units::deg);
  GeoLogVol* log_bpc_wire;
  if(m_oldType) log_bpc_wire = new GeoLogVol(BPCName + "::bpco_wire", shape_bpc_wire, Tungsten);
  else          log_bpc_wire = new GeoLogVol(BPCName + "::bpc_wire", shape_bpc_wire, Tungsten);
  GeoPhysVol* phys_bpc_wire = new GeoPhysVol(log_bpc_wire);
  phys_bpc_xdiv->add( new GeoIdentifierTag( 1 ) );
  phys_bpc_xdiv->add( new GeoTransform( GeoTrf::RotateX3D( 90.*Gaudi::Units::deg ) ) );
  phys_bpc_xdiv->add(phys_bpc_wire);
  if(!m_oldType) {
     phys_bpc_ydiv->add( new GeoIdentifierTag( 1 ) );
     phys_bpc_ydiv->add( new GeoTransform( GeoTrf::RotateY3D( 90.*Gaudi::Units::deg ) ) );
     phys_bpc_ydiv->add(phys_bpc_wire);
  }

// cathode wires around each division
  if(m_oldType) Ndiv = int(2.0*bpc_old_x/bpc_cstep);
  else          Ndiv = int(2.0*bpc_x/bpc_cstep);
  GeoTubs* shape_bpc_cwire;
  if(m_oldType) shape_bpc_cwire = new GeoTubs(0., bpc_cwd, bpc_old_x, 0.*Gaudi::Units::deg, 360.*Gaudi::Units::deg);
  else          shape_bpc_cwire = new GeoTubs( 0., bpc_cwd, bpc_x, 0.*Gaudi::Units::deg, 360.*Gaudi::Units::deg);
  GeoLogVol* log_bpc_cwire;
  if(m_oldType) log_bpc_cwire = new GeoLogVol(BPCName + "::bpco_cwire",shape_bpc_cwire, Tungsten);
  else          log_bpc_cwire = new GeoLogVol(BPCName + "::bpc_cwire",shape_bpc_cwire, Tungsten);
  GeoPhysVol* phys_bpc_cwire = new GeoPhysVol(log_bpc_cwire);
//  GeoXF::TRANSFUNCTION TXXMO = GeoTrf::RotateX3D( 90.*Gaudi::Units::deg ) * GeoXF::Pow(GeoTrf::TranslateX3D(1.0), -bpc_old_x+(2*Index+1)*bpc_old_cstep/2.) * GeoTrf::TranslateZ3D(-bpc_old_send-bpc_cwd+bpc_old_space);
//  GeoXF::TRANSFUNCTION TXXPO = GeoTrf::RotateX3D( 90.*Gaudi::Units::deg ) * GeoXF::Pow(GeoTrf::TranslateX3D(1.0), -bpc_old_x+(2*Index+1)*bpc_old_cstep/2.) * GeoTrf::TranslateZ3D(bpc_old_send-bpc_old_space+bpc_cwd);
  GeoXF::TRANSFUNCTION TXXMO = GeoXF::Pow(GeoTrf::TranslateX3D(1.0), -bpc_old_x+(2*Index+1)*bpc_old_cstep/2.) * GeoTrf::TranslateZ3D(-bpc_old_send-2.*bpc_cwd+bpc_old_space) * GeoTrf::RotateX3D( 90.*Gaudi::Units::deg );
  GeoXF::TRANSFUNCTION TXXPO = GeoXF::Pow(GeoTrf::TranslateX3D(1.0), -bpc_old_x+(2*Index+1)*bpc_old_cstep/2.) * GeoTrf::TranslateZ3D(bpc_old_send-bpc_old_space+2.*bpc_cwd) * GeoTrf::RotateX3D( 90.*Gaudi::Units::deg );
//  GeoXF::TRANSFUNCTION TXXM = GeoTrf::RotateX3D( 90.*Gaudi::Units::deg ) * GeoXF::Pow(GeoTrf::TranslateX3D(1.0), -bpc_x+(2*Index+1)*bpc_cstep/2.) * GeoTrf::TranslateZ3D(-bpc_div-bpc_cwd);
//  GeoXF::TRANSFUNCTION TXXP = GeoTrf::RotateX3D( 90.*Gaudi::Units::deg ) * GeoXF::Pow(GeoTrf::TranslateX3D(1.0), -bpc_x+(2*Index+1)*bpc_cstep/2.) * GeoTrf::TranslateZ3D(bpc_div+bpc_cwd);
  GeoXF::TRANSFUNCTION TXXM = GeoXF::Pow(GeoTrf::TranslateX3D(1.0), -bpc_x+(2*Index+1)*bpc_cstep/2.) * GeoTrf::TranslateZ3D(-bpc_div-bpc_cwd) * GeoTrf::RotateX3D( 90.*Gaudi::Units::deg );
  GeoXF::TRANSFUNCTION TXXP = GeoXF::Pow(GeoTrf::TranslateX3D(1.0), -bpc_x+(2*Index+1)*bpc_cstep/2.) * GeoTrf::TranslateZ3D(bpc_div+bpc_cwd) * GeoTrf::RotateX3D( 90.*Gaudi::Units::deg );
  phys_bpc_xplane->add( new GeoSerialIdentifier(0) );
  if(m_oldType) phys_bpc_xplane->add( new GeoSerialTransformer(phys_bpc_cwire, &TXXMO, Ndiv) );
  else          phys_bpc_xplane->add( new GeoSerialTransformer(phys_bpc_cwire, &TXXM, Ndiv) );
  phys_bpc_xplane->add( new GeoSerialIdentifier(Ndiv) );
  if(m_oldType) phys_bpc_xplane->add( new GeoSerialTransformer(phys_bpc_cwire, &TXXPO, Ndiv) );
  else          phys_bpc_xplane->add( new GeoSerialTransformer(phys_bpc_cwire, &TXXP, Ndiv) );
  if(!m_oldType) {
//     GeoXF::TRANSFUNCTION TYYM = GeoTrf::RotateY3D( 90.*Gaudi::Units::deg ) * GeoXF::Pow(GeoTrf::TranslateY3D(1.0), -bpc_y+(2*Index+1)*bpc_cstep/2.) * GeoTrf::TranslateZ3D(-bpc_div-bpc_cwd);
//     GeoXF::TRANSFUNCTION TYYP = GeoTrf::RotateY3D( 90.*Gaudi::Units::deg ) * GeoXF::Pow(GeoTrf::TranslateY3D(1.0), -bpc_y+(2*Index+1)*bpc_cstep/2.) * GeoTrf::TranslateZ3D(bpc_div+bpc_cwd);
     GeoXF::TRANSFUNCTION TYYM = GeoXF::Pow(GeoTrf::TranslateY3D(1.0), -bpc_y+(2*Index+1)*bpc_cstep/2.) * GeoTrf::TranslateZ3D(-bpc_div-bpc_cwd) * GeoTrf::RotateY3D( 90.*Gaudi::Units::deg );
     GeoXF::TRANSFUNCTION TYYP = GeoXF::Pow(GeoTrf::TranslateY3D(1.0), -bpc_y+(2*Index+1)*bpc_cstep/2.) * GeoTrf::TranslateZ3D(bpc_div+bpc_cwd) * GeoTrf::RotateY3D( 90.*Gaudi::Units::deg );
     phys_bpc_yplane->add( new GeoSerialIdentifier(0) );
     phys_bpc_yplane->add( new GeoSerialTransformer(phys_bpc_cwire, &TYYM, Ndiv) );
     phys_bpc_yplane->add( new GeoSerialIdentifier(Ndiv) );
     phys_bpc_yplane->add( new GeoSerialTransformer(phys_bpc_cwire, &TYYP, Ndiv) );
  }

  return m_BPCPhysical;
}



