/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This will construct a generic MWPC for the H6 beamline that leads to the H1 cryostat.
// There are two kinds of MWPCs - They differ in the size of the wire step (1mm or 2mm).
// The wire-step can be passed as an argument (A wire step of 1mm is default)
// 20. March 2007  Margret Fincke-Keeler

#include "LArGeoH6Cryostats/MWPCConstruction.h"

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
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"

// For transforms:
#include "CLHEP/Geometry/Transform3D.h" 
#include "CLHEP/GenericFunctions/Variable.hh"
// For units:
#include "CLHEP/Units/PhysicalConstants.h"

// For the database:
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include <string>
#include <cmath>
#include <iostream>

LArGeo::MWPCConstruction::MWPCConstruction(double Step)
  : m_wireStep(Step),
    m_MWPCPhysical(nullptr)
{
 
}


LArGeo::MWPCConstruction::~MWPCConstruction()
{
}



GeoVPhysVol* LArGeo::MWPCConstruction::GetEnvelope()
{

  if (m_MWPCPhysical) return m_MWPCPhysical;

  // Message service:  
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);
  
  if(!status.isFailure()){
    m_msg = CxxUtils::make_unique<MsgStream>(msgSvc, "MWPCConstruction");
  } else {
    throw std::runtime_error("MWPCConstruction: cannot initialze message service");
  }

  (*m_msg) << MSG::INFO << "MWPCConstruction - creating an MWPC with wire step " << m_wireStep << "mm !  " << endreq;



  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in MWPCConstruction, cannot access DetectorStore");
  }


  ServiceHandle<IGeoModelSvc> geoModelSvc ("GeoModelSvc", "WallsConstruction");
  if (geoModelSvc.retrieve().isFailure()) {
    throw std::runtime_error ("Cannot locate GeoModelSvc!!");
  }


  // Get the materials from the material manager:-----------------------------------------------------//
  //                                                                                                  //

  DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;

  std::string name;
  double density;
  GeoElement* W=materialManager->getElement("Wolfram");
  GeoMaterial* Tungsten = new GeoMaterial(name="Tungsten", density=19.3*CLHEP::g/CLHEP::cm3);
  Tungsten->add(W,1.);
  Tungsten->lock();
  
  
  GeoElement* Ar=materialManager->getElement("Argon");
  GeoElement*  C=materialManager->getElement("Carbon");
  GeoElement*  H=materialManager->getElement("Hydrogen");
  GeoMaterial* Isobutane = new GeoMaterial(name="Isobutane", density=2.67*CLHEP::g/CLHEP::cm3);
  Isobutane->add(C,0.8266);
  Isobutane->add(H,0.1734);
  Isobutane->lock();
  GeoMaterial* ArIso = new GeoMaterial(name="ArIso", density=0.0025*CLHEP::g/CLHEP::cm3);
  ArIso->add(Ar,0.61);
  ArIso->add(Isobutane,0.39);
  ArIso->lock();



//   // Test to see whether we can get the ArIso from the database  
//   GeoMaterial *ArIso2  = materialManager->getMaterial("LArTB::Argon70Isobutane30");
//   if (!ArIso2) (*m_msg) << MSG::INFO  << "Found no Argon70Isobutane30 in database" << endreq;
//   if (ArIso2)  (*m_msg) << MSG::INFO  << "Did find LArTB::Argon70Isobutane30 in database!" << endreq;

  GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in MWPCConstruction, std::Air is not found.");
   
  GeoMaterial *Aluminium  = materialManager->getMaterial("std::Aluminium");
  if (!Aluminium) throw std::runtime_error("Error in MWPCConstruction, std::Aluminium is not found.");
  
  GeoMaterial *Mylar  = materialManager->getMaterial("std::Mylar");
  if (!Mylar) throw std::runtime_error("Error in MWPCConstruction, std::Mylar is not found.");
  
  
  //                                                                                                 //
  //-------------------------------------------------------------------------------------------------//


  
  std::string AtlasVersion = geoModelSvc->atlasVersion();
  std::string LArVersion = geoModelSvc->LAr_VersionOverride();

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";


  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////

 
  //  Here we creat the envelope for the Moveable FrontBeam Instrumentation.  This code is repeated
  //  createEnvelope() method below.  There should be a way to avoid this repitition.


  //------ Now create a MWPC
 
  (*m_msg)  << " Create MWPC5 " << endreq;

  std::string baseName = "LAr::TB";
  std::string MWPCName = baseName + "::MWPC";
  
  // This creates a square wire-chamber: 
  const double MWPCDxy = 64.0*CLHEP::mm;
  const double MWPCDz = 16.586*CLHEP::mm;  


  GeoBox* MWPCShape = new GeoBox(MWPCDxy, MWPCDxy, MWPCDz);  // A generic WWPC
  const GeoLogVol* MWPCLogical = new GeoLogVol( MWPCName, MWPCShape, ArIso ); 

  m_MWPCPhysical = new GeoPhysVol(MWPCLogical);
  

  //..... Add Mylar to MWPC:
  const double MylarDz = 0.015*CLHEP::mm; 

  GeoBox* MylarShape = new GeoBox(MWPCDxy, MWPCDxy, MylarDz);  // Mylar fits across the MWPC in x,y

  for ( int side = 0; side<2 ; side++)
    {
      double MylarPos;
      if (side==0) MylarPos = MylarDz-MWPCDz;
      else         MylarPos = MWPCDz-MylarDz;
      std::string MylarName = MWPCName + "::Mylar";
      GeoLogVol* MylarLogical = new GeoLogVol( MylarName, MylarShape, Mylar );   
      GeoPhysVol* MylarPhysical = new GeoPhysVol( MylarLogical );
      m_MWPCPhysical->add( new GeoIdentifierTag( side ) );
      m_MWPCPhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, (MylarPos) ) ) );
      m_MWPCPhysical->add( MylarPhysical );
    }
  // Done with the Mylar Foils 



  //..... Add Al walls to MWPC5:
  const double Aluz = 0.014*CLHEP::mm; 
  const double AluDz = Aluz;
  const double Alu_f = 7.*CLHEP::mm;   
  const double Alu_s = 15.*CLHEP::mm;

  GeoBox* AluShape = new GeoBox(MWPCDxy, MWPCDxy, AluDz);  // Al foil fits across the MWPC in x,y
  for ( int pos = 0; pos<4 ; pos++)
    {
      double AluPos;
      switch(pos) {
      case 0: { AluPos = -Alu_s-AluDz; break; }
      case 1: { AluPos = -Alu_f+AluDz; break; }
      case 2: { AluPos =  Alu_f-AluDz; break; }
      case 3: { AluPos =  Alu_s+AluDz; break; }
      default: { throw std::runtime_error("MWPC5 - too many Al foils!");   break; }
      }

      std::string AluName = MWPCName + "::AlFoil";
      GeoLogVol* AluLogical = new GeoLogVol( AluName, AluShape, Aluminium );  
      GeoPhysVol* AluPhysical = new GeoPhysVol( AluLogical );
      m_MWPCPhysical->add( new GeoIdentifierTag( pos ) );
      m_MWPCPhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, (AluPos) ) ) );
      m_MWPCPhysical->add( AluPhysical );
    }
  


  //..... Add a sensitive X and Y plane to MWPC5:
  const double Senz = 4.0*CLHEP::mm; 
  const double SenDz = Senz;  // z-Thickness of sensitive volume
  const double SenPos = 11.*CLHEP::mm;  // z-Position of sensitive volume  
  //const double Step = 2.*CLHEP::mm;  // wire-step size for MWPC5

  GeoBox* SenPlaneShape = new GeoBox(MWPCDxy, MWPCDxy, SenDz);  // Sensitive Volume fits across the MWPC in x,y

  std::string XPlaneName = MWPCName + "::XPlane";
  std::string YPlaneName = MWPCName + "::YPlane";
  GeoLogVol* XPlaneLogical = new GeoLogVol( XPlaneName, SenPlaneShape, ArIso );  
  GeoLogVol* YPlaneLogical = new GeoLogVol( YPlaneName, SenPlaneShape, ArIso );  
  GeoPhysVol* XPlanePhysical = new GeoPhysVol( XPlaneLogical );
  GeoPhysVol* YPlanePhysical = new GeoPhysVol( YPlaneLogical );
  m_MWPCPhysical->add( new GeoIdentifierTag( 0 ) );
  m_MWPCPhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, (-SenPos) ) ) );
  m_MWPCPhysical->add( XPlanePhysical );  
  m_MWPCPhysical->add( new GeoIdentifierTag( 0 ) );
  m_MWPCPhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, (SenPos) ) ) );
  m_MWPCPhysical->add( YPlanePhysical );
  
 
//.... The X and Y planes have "divisions"
//     These divisions will eventually be the sensitive volumes

  Genfun::Variable Index;
  int NDiv= int ( 2*MWPCDxy / m_wireStep ) ;
  GeoXF::TRANSFUNCTION TX = GeoXF::Pow(HepGeom::TranslateX3D(1.0), -MWPCDxy + m_wireStep/2.  + m_wireStep*Index);
  GeoXF::TRANSFUNCTION TY = GeoXF::Pow(HepGeom::TranslateY3D(1.0), -MWPCDxy + m_wireStep/2. + m_wireStep*Index);
  GeoBox* XPlaneDiv = new GeoBox(m_wireStep/2., MWPCDxy , SenDz); 
  GeoBox* YPlaneDiv = new GeoBox(MWPCDxy , m_wireStep/2., SenDz);   
  std::string XDivName = MWPCName + "::XDiv";
  std::string YDivName = MWPCName + "::YDiv";
  GeoLogVol* XDivLogical = new GeoLogVol( XDivName, XPlaneDiv, ArIso );  
  GeoLogVol* YDivLogical = new GeoLogVol( YDivName, YPlaneDiv, ArIso );  
  GeoPhysVol* XDivPhysical = new GeoPhysVol( XDivLogical );
  GeoPhysVol* YDivPhysical = new GeoPhysVol( YDivLogical );

  GeoSerialIdentifier *sIX = new GeoSerialIdentifier(0);
  GeoSerialTransformer *sTSX = new GeoSerialTransformer(XDivPhysical,  &TX, NDiv );
  GeoSerialIdentifier *sIY = new GeoSerialIdentifier(0);
  GeoSerialTransformer *sTSY = new GeoSerialTransformer(YDivPhysical,  &TY, NDiv );
  XPlanePhysical->add(sIX);
  XPlanePhysical->add(sTSX);
  YPlanePhysical->add(sIY);
  YPlanePhysical->add(sTSY);

//.... Put wires into the X/Y "divisions"
  const double WireDiam = 0.006*CLHEP::mm;
  const double WireLen = MWPCDxy;
  GeoTubs* WireShape = new GeoTubs(0.*CLHEP::cm, WireDiam/2., WireLen , 0.*CLHEP::deg,360.*CLHEP::deg); 
  std::string WireName = MWPCName + "::Wire";
  GeoLogVol* WireLogical = new GeoLogVol(WireName, WireShape, Tungsten);  
  GeoPhysVol* WirePhysical = new GeoPhysVol( WireLogical );
  XDivPhysical->add(new GeoTransform(HepGeom::RotateX3D( 90.*CLHEP::deg )));
  XDivPhysical->add(WirePhysical);
  YDivPhysical->add(new GeoTransform(HepGeom::RotateY3D( 90.*CLHEP::deg )));
  YDivPhysical->add(WirePhysical);


//----- Done with the MWPC


  // End Moveable MWPC detectors


  return m_MWPCPhysical;
}



