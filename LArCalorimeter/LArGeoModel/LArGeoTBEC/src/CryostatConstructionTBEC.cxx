/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CryostatConstructionTBEC

#include "CryostatConstructionTBEC.h"

#include "GeoModelKernel/GeoElement.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h" 
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoTrd.h"   
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"

// For transforms:
#include "CLHEP/Geometry/Transform3D.h" 
// For units:
#include "CLHEP/Units/PhysicalConstants.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include <string>
#include <cmath>

LArGeo::CryostatConstructionTBEC::CryostatConstructionTBEC():
m_cryoEnvelopePhysical(NULL),
m_LArPhysical(NULL)
{;}

LArGeo::CryostatConstructionTBEC::~CryostatConstructionTBEC() {;}

GeoVFullPhysVol* LArGeo::CryostatConstructionTBEC::GetEnvelope()
{

  if (m_cryoEnvelopePhysical) return m_cryoEnvelopePhysical;
 
  // Get access to the material manager:
  
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in CryostatConstructionTBEC, cannot access MessageSvc");
  }

  MsgStream log(msgSvc, "LArGeo::CryostatConstructionTBEC"); 

  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "+    HELLO from LArGeo::CryostatConstructionTBEC   +" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;


  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in CryostatConstructionTBEC, cannot access DetectorStore");
  }

  // Get the materials from the material manager:-----------------------------------------------------//
  //                                                                                                  //
  DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;
  
  GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in CryostatConstructionTBEC, std::Air is not found.");
  
  GeoMaterial *Al  = materialManager->getMaterial("std::Aluminium");
  if (!Al) throw std::runtime_error("Error in CryostatConstructionTBEC, std::Aluminium is not found.");

  GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in CryostatConstructionTBEC, std::LiquidArgon is not found.");
  
  GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in CryostatConstructionTBEC, std::Iron is not found.");
  
  GeoMaterial *Gten  = materialManager->getMaterial("LAr::G10");
  if (!Gten) throw std::runtime_error("Error in CryostatConstructionTBEC, LAr::G10 is not found.");
  
  GeoMaterial *Vacuum  = materialManager->getMaterial("LAr::Vacuum");
  if (!Vacuum) throw std::runtime_error("Error in CryostatConstructionTBEC, std::Vacuum is not found.");
  
  //                                                                                                 //
  //-------------------------------------------------------------------------------------------------//


  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////

  // Set up strings for volume names.
  std::string baseName = "LAr::TBEC::Cryostat";

  // Define the mother volume for the endcap cryostat.  Everything
  // else in the endcap (cryostat walls, detectors, etc.) should be
  // placed inside here.

  // The position of this volume may change if the thickness of the
  // cabling in front of the endcaps changes.  Therefore, we must get
  // the z-shift from the database and adjust the volume geometry
  // accordingly.

  std::string cryoMotherName = baseName + "::MotherVolume";
  GeoBox* cryoMotherShape = new GeoBox( 152.*CLHEP::cm, 195.*CLHEP::cm, 60.09*CLHEP::cm );
  const GeoLogVol* cryoMotherLogical = new GeoLogVol( cryoMotherName, cryoMotherShape, Air );
  //GeoFullPhysVol* m_cryoEnvelopePhysical = new GeoFullPhysVol( cryoMotherLogical );
  m_cryoEnvelopePhysical = new GeoFullPhysVol( cryoMotherLogical );
  
  // Cryostat walls
  
  std::string ExtWallName = baseName + "::ExternalWarmWall";
  GeoBox* ExtWallShape = new GeoBox( 152.*CLHEP::cm, 195.*CLHEP::cm, 60.09*CLHEP::cm );
  const GeoLogVol* ExtWallLogical = new GeoLogVol( ExtWallName, ExtWallShape, Al );
  GeoPhysVol* ExtWallPhysical = new GeoPhysVol( ExtWallLogical );
  
  std::string WallName = baseName + "::WarmWallInterval";
  GeoBox* WallShape = new GeoBox( ( 152. - 0.8 )*CLHEP::cm, ( 195. - 0.8 )*CLHEP::cm, ( 60.09 - 0.8 )*CLHEP::cm );
  const GeoLogVol* WallLogical = new GeoLogVol( WallName, WallShape, Vacuum );
  GeoPhysVol* WallPhysical = new GeoPhysVol( WallLogical );
  
  std::string IntWallName = baseName + "::InternalWarmWall";
  GeoBox* IntWallShape = new GeoBox( 148.4*CLHEP::cm, 191.6*CLHEP::cm, 46.8*CLHEP::cm );
  const GeoLogVol* IntWallLogical = new GeoLogVol( IntWallName, IntWallShape, Al );
  GeoPhysVol* IntWallPhysical = new GeoPhysVol( IntWallLogical );
  
  std::string VacuumName = baseName + "::Vacuum";
  GeoBox* VacuumShape = new GeoBox( ( 148.4 - 0.8 )*CLHEP::cm, ( 191.6 - 0.8 )*CLHEP::cm, ( 46.8 - 0.8 )*CLHEP::cm );
  const GeoLogVol* VacuumLogical = new GeoLogVol( VacuumName, VacuumShape, Vacuum );
  GeoPhysVol* VacuumPhysical = new GeoPhysVol( VacuumLogical );
  
  std::string ColdWallName = baseName + "::ColdWall";
  GeoBox* ColdWallShape = new GeoBox( 142.5*CLHEP::cm, 184.85*CLHEP::cm, 38.*CLHEP::cm );
  const GeoLogVol* ColdWallLogical = new GeoLogVol( ColdWallName, ColdWallShape, Iron );
  GeoPhysVol* ColdWallPhysical = new GeoPhysVol( ColdWallLogical );
  
  std::string LArName = baseName + "::LiquidArgon";
  GeoBox* LArShape = new GeoBox( ( 142.5 - .5 )*CLHEP::cm, ( 184.85 - .5 )*CLHEP::cm, ( 38. - .5 )*CLHEP::cm );
  const GeoLogVol* LArLogical = new GeoLogVol( LArName, LArShape, LAr );
  // GeoPhysVol* m_LArPhysical = new GeoPhysVol( LArLogical );
  m_LArPhysical = new GeoPhysVol( LArLogical );
  
  ColdWallPhysical->add( new GeoIdentifierTag( 1 ) );
  ColdWallPhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, 0.*CLHEP::cm ) ) );
  ColdWallPhysical->add( m_LArPhysical );
  
  VacuumPhysical->add( new GeoIdentifierTag( 1 ) );
  VacuumPhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, 0.*CLHEP::cm ) ) );
  VacuumPhysical->add( ColdWallPhysical );
  
  IntWallPhysical->add( new GeoIdentifierTag( 1 ) );
  IntWallPhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, 0.*CLHEP::cm ) ) );
  IntWallPhysical->add( VacuumPhysical );
  
  WallPhysical->add( new GeoIdentifierTag( 1 ) );
  WallPhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, 0.*CLHEP::cm ) ) );
  WallPhysical->add( IntWallPhysical );
  
  ExtWallPhysical->add( new GeoIdentifierTag( 1 ) );
  ExtWallPhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, 0.*CLHEP::cm ) ) );
  ExtWallPhysical->add( WallPhysical );
  
  m_cryoEnvelopePhysical->add( new GeoIdentifierTag( 1 ) );
  m_cryoEnvelopePhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, 0.*CLHEP::cm ) ) );					   
  m_cryoEnvelopePhysical->add( ExtWallPhysical );
  
  // Pressure cone
  
  std::string PConeName = baseName + "::PressureCone::Mother";
  GeoTubs* PConeShape = new GeoTubs( 0.*CLHEP::cm, 6.5*CLHEP::cm, 4.*CLHEP::cm, 0.*CLHEP::deg, 360.*CLHEP::deg );
  const GeoLogVol* PConeLogical = new GeoLogVol( PConeName, PConeShape, Vacuum );
  GeoPhysVol* PConePhysical = new GeoPhysVol( PConeLogical );
  
  std::string IntFlangeName = baseName + "::PressureCone::InternalFlange";
  GeoTubs* IntFlangeShape = new GeoTubs( 0.*CLHEP::cm, 4.9*CLHEP::cm, 0.4*CLHEP::cm, 0.*CLHEP::deg, 360.*CLHEP::deg );
  const GeoLogVol* IntFlangeLogical = new GeoLogVol( IntFlangeName, IntFlangeShape, Gten );
  GeoPhysVol* IntFlangePhysical = new GeoPhysVol( IntFlangeLogical );
  
  std::string ExtFlangeName = baseName + "::PressureCone::ExternalFlange";
  GeoTubs* ExtFlangeShape = new GeoTubs( 5.*CLHEP::cm, 6.5*CLHEP::cm, 0.4*CLHEP::cm, 0.*CLHEP::deg, 360.*CLHEP::deg );
  const GeoLogVol* ExtFlangeLogical = new GeoLogVol( ExtFlangeName, ExtFlangeShape, Gten );
  GeoPhysVol* ExtFlangePhysical = new GeoPhysVol( ExtFlangeLogical );
  
  std::string ConeName = baseName + "::PressureCone::Cone";
  GeoCons* ConeShape = new GeoCons( 5.4*CLHEP::cm, 4.5*CLHEP::cm, 5.5*CLHEP::cm, 4.6*CLHEP::cm, 3.2*CLHEP::cm, 0.*CLHEP::deg, 360.*CLHEP::deg );
  const GeoLogVol* ConeLogical = new GeoLogVol( ConeName, ConeShape, Gten );
  GeoPhysVol* ConePhysical = new GeoPhysVol( ConeLogical );
  
  PConePhysical->add( new GeoIdentifierTag( 1 ) );
  PConePhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, 3.6*CLHEP::cm ) ) );
  PConePhysical->add( IntFlangePhysical );
  
  PConePhysical->add( new GeoIdentifierTag( 1 ) );
  PConePhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, -3.6*CLHEP::cm ) ) );
  PConePhysical->add( ExtFlangePhysical );
  
  PConePhysical->add( new GeoIdentifierTag( 1 ) );
  PConePhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, 0.*CLHEP::cm ) ) );
  PConePhysical->add( ConePhysical );
  
  for ( int i  = 0; i < 3; i++ ) for ( int j = 0; j < 13; j++ ) {
  	double x = 135.1*CLHEP::cm - 19.3*( j + 1 )*CLHEP::cm;
	double y = 19.3*( i - 1 )*CLHEP::cm;
	VacuumPhysical->add( new GeoIdentifierTag( 1 + i*13 + j ) );
  	VacuumPhysical->add( new GeoTransform( HepGeom::Translate3D( x, y, -42.*CLHEP::cm ) ) );
  	VacuumPhysical->add( PConePhysical );
  }
  
  // Zig-zag structure
  
  std::string ZigZagMotherName = baseName + "::ZigZag::Mother";
  GeoBox* ZigZagMotherShape = new GeoBox( 130.*CLHEP::cm, 15.*CLHEP::cm, 6.45*CLHEP::cm );
  const GeoLogVol* ZigZagMotherLogical = new GeoLogVol( ZigZagMotherName, ZigZagMotherShape, Vacuum );
  GeoPhysVol* ZigZagMotherPhysical = new GeoPhysVol( ZigZagMotherLogical );
  
  std::string ZigZagStrAName = baseName + "::ZigZag::StrA";
  GeoBox* ZigZagStrAShape = new GeoBox( 2.45*CLHEP::cm, 5.*CLHEP::cm, .4*CLHEP::cm );
  const GeoLogVol* ZigZagStrALogical = new GeoLogVol( ZigZagStrAName, ZigZagStrAShape, Al );
  GeoPhysVol* ZigZagStrAPhysical = new GeoPhysVol( ZigZagStrALogical );
  
  std::string ZigZagStrBName = baseName + "::ZigZag::StrB";
  GeoBox* ZigZagStrBShape = new GeoBox( 8.53*CLHEP::cm, 5.*CLHEP::cm, .4*CLHEP::cm );
  const GeoLogVol* ZigZagStrBLogical = new GeoLogVol( ZigZagStrBName, ZigZagStrBShape, Al );
  GeoPhysVol* ZigZagStrBPhysical = new GeoPhysVol( ZigZagStrBLogical );
  
  std::string ZigZagStrCName = baseName + "::ZigZag::StrC";
  GeoTrd* ZigZagStrCShape = new GeoTrd( 1.03*CLHEP::cm, .453*CLHEP::cm, 5.*CLHEP::cm, 5.*CLHEP::cm, .283*CLHEP::cm );
  const GeoLogVol* ZigZagStrCLogical = new GeoLogVol( ZigZagStrCName, ZigZagStrCShape, Al );
  GeoPhysVol* ZigZagStrCPhysical = new GeoPhysVol( ZigZagStrCLogical );
  
  std::string ZigZagStrDName = baseName + "::ZigZag::StrD";
  GeoTrd* ZigZagStrDShape = new GeoTrd( .005*CLHEP::cm, .31*CLHEP::cm, 5.*CLHEP::cm, 5.*CLHEP::cm, .365*CLHEP::cm );
  const GeoLogVol* ZigZagStrDLogical = new GeoLogVol( ZigZagStrDName, ZigZagStrDShape, Al );
  GeoPhysVol* ZigZagStrDPhysical = new GeoPhysVol( ZigZagStrDLogical );
  
  int StrAIdTag = 1;
  
  for ( int i = 0; i < 9; i++ ) {
  	ZigZagMotherPhysical->add( new GeoIdentifierTag( StrAIdTag ) );
  	ZigZagMotherPhysical->add( new GeoTransform( HepGeom::Translate3D( ( 124.4 - 31.1*i )*CLHEP::cm, 0.*CLHEP::cm, 6.05*CLHEP::cm ) ) );
  	ZigZagMotherPhysical->add( ZigZagStrAPhysical );
	StrAIdTag++;
  }
  for ( int j = 0; j < 2; j++ ) for ( int i = 0; i < 8; i++ ) {
  	ZigZagMotherPhysical->add( new GeoIdentifierTag( StrAIdTag ) );
  	ZigZagMotherPhysical->add( new GeoTransform( HepGeom::Translate3D( ( 108.85 - 31.1*i )*CLHEP::cm, ( 2*j - 1 )*10.*CLHEP::cm, 6.05*CLHEP::cm ) ) );
  	ZigZagMotherPhysical->add( ZigZagStrAPhysical );
	StrAIdTag++;
  }
  
  int StrBIdTag = 1;
  const double xB1[ 2 ] = { ( -6.77 + 108.85 )*CLHEP::cm, ( 6.77 + 108.85 )*CLHEP::cm };
  const double xB2[ 2 ] = { ( -6.77 + 124.4 )*CLHEP::cm, ( 6.77 + 93.3 )*CLHEP::cm  };
  const double alpha[ 2 ] = { 45.*CLHEP::deg, -45.*CLHEP::deg };
  
  for ( int k = 0; k < 2; k++ )  for ( int i = 0; i < 8; i++ ) {
  	ZigZagMotherPhysical->add( new GeoIdentifierTag( StrBIdTag ) );
  	ZigZagMotherPhysical->add( new GeoTransform( HepGeom::Transform3D( CLHEP::HepRotationY( alpha[ k ] ), 
								     CLHEP::Hep3Vector( xB1[ k ] - 31.1*i*CLHEP::cm, 0.*CLHEP::cm, -.1*CLHEP::cm ) ) ) );
  	ZigZagMotherPhysical->add( ZigZagStrBPhysical );
	StrBIdTag++;
	
	for ( int j = 0; j < 2; j++ ) {
		ZigZagMotherPhysical->add( new GeoIdentifierTag( StrBIdTag ) );
  		ZigZagMotherPhysical->add( new GeoTransform( HepGeom::Transform3D( CLHEP::HepRotationY( alpha[ k ] ), 
								     CLHEP::Hep3Vector( xB2[ k ] - 31.1*i*CLHEP::cm, ( -10. + 20.*j )*CLHEP::cm, -.1*CLHEP::cm ) ) ) );
  		ZigZagMotherPhysical->add( ZigZagStrBPhysical );
		StrBIdTag++;	
	}
  }
  
  int StrCIdTag = 1;
  
  for ( int i = 0; i < 9; i++ ) {
  	if ( i < 8 ) {
  		ZigZagMotherPhysical->add( new GeoIdentifierTag( StrCIdTag ) );
  		ZigZagMotherPhysical->add( new GeoTransform( HepGeom::Translate3D( ( 108.85 - 31.1*i )*CLHEP::cm, 0.*CLHEP::cm, -6.15*CLHEP::cm ) ) );
  		ZigZagMotherPhysical->add( ZigZagStrCPhysical );
		StrCIdTag++;
	}
	for ( int j = 0; j < 2; j++ ) {
		ZigZagMotherPhysical->add( new GeoIdentifierTag( StrCIdTag ) );
  		ZigZagMotherPhysical->add( new GeoTransform( HepGeom::Translate3D( ( 124.4 - 31.1*i )*CLHEP::cm, ( -10. + 20.*j )*CLHEP::cm, -6.15*CLHEP::cm ) ) );
  		ZigZagMotherPhysical->add( ZigZagStrCPhysical );
		StrCIdTag++;
	}
  }
  
  int StrDIdTag = 1;
  const double xD1[ 2 ] = { ( -2.598 + 124.4 )*CLHEP::cm, ( 2.598 + 124.4 )*CLHEP::cm };
  const double xD2[ 2 ] = { ( -2.598 + 108.85 )*CLHEP::cm, ( 2.598 + 108.85 )*CLHEP::cm };
  const double beta[ 2 ] = { -22.5*CLHEP::deg, 22.5*CLHEP::deg };
  
  for ( int k = 0; k < 2; k++ )  for ( int i = 0; i < 9; i++ ) {
  	ZigZagMotherPhysical->add( new GeoIdentifierTag( StrDIdTag ) );
  	ZigZagMotherPhysical->add( new GeoTransform( HepGeom::Transform3D( CLHEP::HepRotationY( beta[ k ] ), 
								     CLHEP::Hep3Vector( xD1[ k ] - 31.1*i*CLHEP::cm, 0.*CLHEP::cm, 5.995*CLHEP::cm ) ) ) );
  	ZigZagMotherPhysical->add( ZigZagStrDPhysical );
	StrDIdTag++;
	
	if ( i < 8 ) for ( int j = 0; j < 2; j++ ) {
		ZigZagMotherPhysical->add( new GeoIdentifierTag( StrDIdTag ) );
  		ZigZagMotherPhysical->add( new GeoTransform( HepGeom::Transform3D( CLHEP::HepRotationY( beta[ k ] ), 
								     CLHEP::Hep3Vector( xD2[ k ] - 31.1*i*CLHEP::cm, ( -10. +20.*j )*CLHEP::cm, 5.995*CLHEP::cm ) ) ) );
  		ZigZagMotherPhysical->add( ZigZagStrDPhysical );
		StrDIdTag++;	
	}
  }
   
  WallPhysical->add( new GeoIdentifierTag( 1 ) );
  WallPhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, -53.2*CLHEP::cm ) ) );
  WallPhysical->add( ZigZagMotherPhysical );
    
return m_cryoEnvelopePhysical;
}

GeoPhysVol* LArGeo::CryostatConstructionTBEC::GetLArPhysical()
{
  return m_LArPhysical;
}
