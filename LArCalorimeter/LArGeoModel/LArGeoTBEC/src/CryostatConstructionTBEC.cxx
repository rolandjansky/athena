/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/Units.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"

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
  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;
  
  const GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in CryostatConstructionTBEC, std::Air is not found.");
  
  const GeoMaterial *Al  = materialManager->getMaterial("std::Aluminium");
  if (!Al) throw std::runtime_error("Error in CryostatConstructionTBEC, std::Aluminium is not found.");

  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in CryostatConstructionTBEC, std::LiquidArgon is not found.");
  
  const GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in CryostatConstructionTBEC, std::Iron is not found.");
  
  const GeoMaterial *Gten  = materialManager->getMaterial("LAr::G10");
  if (!Gten) throw std::runtime_error("Error in CryostatConstructionTBEC, LAr::G10 is not found.");
  
  const GeoMaterial *Vacuum  = materialManager->getMaterial("LAr::Vacuum");
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
  GeoBox* cryoMotherShape = new GeoBox( 152.*GeoModelKernelUnits::cm, 195.*GeoModelKernelUnits::cm, 60.09*GeoModelKernelUnits::cm );
  const GeoLogVol* cryoMotherLogical = new GeoLogVol( cryoMotherName, cryoMotherShape, Air );
  //GeoFullPhysVol* m_cryoEnvelopePhysical = new GeoFullPhysVol( cryoMotherLogical );
  m_cryoEnvelopePhysical = new GeoFullPhysVol( cryoMotherLogical );
  
  // Cryostat walls
  
  std::string ExtWallName = baseName + "::ExternalWarmWall";
  GeoBox* ExtWallShape = new GeoBox( 152.*GeoModelKernelUnits::cm, 195.*GeoModelKernelUnits::cm, 60.09*GeoModelKernelUnits::cm );
  const GeoLogVol* ExtWallLogical = new GeoLogVol( ExtWallName, ExtWallShape, Al );
  GeoPhysVol* ExtWallPhysical = new GeoPhysVol( ExtWallLogical );
  
  std::string WallName = baseName + "::WarmWallInterval";
  GeoBox* WallShape = new GeoBox( ( 152. - 0.8 )*GeoModelKernelUnits::cm, ( 195. - 0.8 )*GeoModelKernelUnits::cm, ( 60.09 - 0.8 )*GeoModelKernelUnits::cm );
  const GeoLogVol* WallLogical = new GeoLogVol( WallName, WallShape, Vacuum );
  GeoPhysVol* WallPhysical = new GeoPhysVol( WallLogical );
  
  std::string IntWallName = baseName + "::InternalWarmWall";
  GeoBox* IntWallShape = new GeoBox( 148.4*GeoModelKernelUnits::cm, 191.6*GeoModelKernelUnits::cm, 46.8*GeoModelKernelUnits::cm );
  const GeoLogVol* IntWallLogical = new GeoLogVol( IntWallName, IntWallShape, Al );
  GeoPhysVol* IntWallPhysical = new GeoPhysVol( IntWallLogical );
  
  std::string VacuumName = baseName + "::Vacuum";
  GeoBox* VacuumShape = new GeoBox( ( 148.4 - 0.8 )*GeoModelKernelUnits::cm, ( 191.6 - 0.8 )*GeoModelKernelUnits::cm, ( 46.8 - 0.8 )*GeoModelKernelUnits::cm );
  const GeoLogVol* VacuumLogical = new GeoLogVol( VacuumName, VacuumShape, Vacuum );
  GeoPhysVol* VacuumPhysical = new GeoPhysVol( VacuumLogical );
  
  std::string ColdWallName = baseName + "::ColdWall";
  GeoBox* ColdWallShape = new GeoBox( 142.5*GeoModelKernelUnits::cm, 184.85*GeoModelKernelUnits::cm, 38.*GeoModelKernelUnits::cm );
  const GeoLogVol* ColdWallLogical = new GeoLogVol( ColdWallName, ColdWallShape, Iron );
  GeoPhysVol* ColdWallPhysical = new GeoPhysVol( ColdWallLogical );
  
  std::string LArName = baseName + "::LiquidArgon";
  GeoBox* LArShape = new GeoBox( ( 142.5 - .5 )*GeoModelKernelUnits::cm, ( 184.85 - .5 )*GeoModelKernelUnits::cm, ( 38. - .5 )*GeoModelKernelUnits::cm );
  const GeoLogVol* LArLogical = new GeoLogVol( LArName, LArShape, LAr );
  // GeoPhysVol* m_LArPhysical = new GeoPhysVol( LArLogical );
  m_LArPhysical = new GeoPhysVol( LArLogical );
  
  ColdWallPhysical->add( new GeoIdentifierTag( 1 ) );
  ColdWallPhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm ) ) );
  ColdWallPhysical->add( m_LArPhysical );
  
  VacuumPhysical->add( new GeoIdentifierTag( 1 ) );
  VacuumPhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm ) ) );
  VacuumPhysical->add( ColdWallPhysical );
  
  IntWallPhysical->add( new GeoIdentifierTag( 1 ) );
  IntWallPhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm ) ) );
  IntWallPhysical->add( VacuumPhysical );
  
  WallPhysical->add( new GeoIdentifierTag( 1 ) );
  WallPhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm ) ) );
  WallPhysical->add( IntWallPhysical );
  
  ExtWallPhysical->add( new GeoIdentifierTag( 1 ) );
  ExtWallPhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm ) ) );
  ExtWallPhysical->add( WallPhysical );
  
  m_cryoEnvelopePhysical->add( new GeoIdentifierTag( 1 ) );
  m_cryoEnvelopePhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm ) ) );					   
  m_cryoEnvelopePhysical->add( ExtWallPhysical );
  
  // Pressure cone
  
  std::string PConeName = baseName + "::PressureCone::Mother";
  GeoTubs* PConeShape = new GeoTubs( 0.*GeoModelKernelUnits::cm, 6.5*GeoModelKernelUnits::cm, 4.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::deg, 360.*GeoModelKernelUnits::deg );
  const GeoLogVol* PConeLogical = new GeoLogVol( PConeName, PConeShape, Vacuum );
  GeoPhysVol* PConePhysical = new GeoPhysVol( PConeLogical );
  
  std::string IntFlangeName = baseName + "::PressureCone::InternalFlange";
  GeoTubs* IntFlangeShape = new GeoTubs( 0.*GeoModelKernelUnits::cm, 4.9*GeoModelKernelUnits::cm, 0.4*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::deg, 360.*GeoModelKernelUnits::deg );
  const GeoLogVol* IntFlangeLogical = new GeoLogVol( IntFlangeName, IntFlangeShape, Gten );
  GeoPhysVol* IntFlangePhysical = new GeoPhysVol( IntFlangeLogical );
  
  std::string ExtFlangeName = baseName + "::PressureCone::ExternalFlange";
  GeoTubs* ExtFlangeShape = new GeoTubs( 5.*GeoModelKernelUnits::cm, 6.5*GeoModelKernelUnits::cm, 0.4*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::deg, 360.*GeoModelKernelUnits::deg );
  const GeoLogVol* ExtFlangeLogical = new GeoLogVol( ExtFlangeName, ExtFlangeShape, Gten );
  GeoPhysVol* ExtFlangePhysical = new GeoPhysVol( ExtFlangeLogical );
  
  std::string ConeName = baseName + "::PressureCone::Cone";
  GeoCons* ConeShape = new GeoCons( 5.4*GeoModelKernelUnits::cm, 4.5*GeoModelKernelUnits::cm, 5.5*GeoModelKernelUnits::cm, 4.6*GeoModelKernelUnits::cm, 3.2*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::deg, 360.*GeoModelKernelUnits::deg );
  const GeoLogVol* ConeLogical = new GeoLogVol( ConeName, ConeShape, Gten );
  GeoPhysVol* ConePhysical = new GeoPhysVol( ConeLogical );
  
  PConePhysical->add( new GeoIdentifierTag( 1 ) );
  PConePhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, 3.6*GeoModelKernelUnits::cm ) ) );
  PConePhysical->add( IntFlangePhysical );
  
  PConePhysical->add( new GeoIdentifierTag( 1 ) );
  PConePhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, -3.6*GeoModelKernelUnits::cm ) ) );
  PConePhysical->add( ExtFlangePhysical );
  
  PConePhysical->add( new GeoIdentifierTag( 1 ) );
  PConePhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm ) ) );
  PConePhysical->add( ConePhysical );
  
  for ( int i  = 0; i < 3; i++ ) for ( int j = 0; j < 13; j++ ) {
  	double x = 135.1*GeoModelKernelUnits::cm - 19.3*( j + 1 )*GeoModelKernelUnits::cm;
	double y = 19.3*( i - 1 )*GeoModelKernelUnits::cm;
	VacuumPhysical->add( new GeoIdentifierTag( 1 + i*13 + j ) );
  	VacuumPhysical->add( new GeoTransform( GeoTrf::Translate3D( x, y, -42.*GeoModelKernelUnits::cm ) ) );
  	VacuumPhysical->add( PConePhysical );
  }
  
  // Zig-zag structure
  
  std::string ZigZagMotherName = baseName + "::ZigZag::Mother";
  GeoBox* ZigZagMotherShape = new GeoBox( 130.*GeoModelKernelUnits::cm, 15.*GeoModelKernelUnits::cm, 6.45*GeoModelKernelUnits::cm );
  const GeoLogVol* ZigZagMotherLogical = new GeoLogVol( ZigZagMotherName, ZigZagMotherShape, Vacuum );
  GeoPhysVol* ZigZagMotherPhysical = new GeoPhysVol( ZigZagMotherLogical );
  
  std::string ZigZagStrAName = baseName + "::ZigZag::StrA";
  GeoBox* ZigZagStrAShape = new GeoBox( 2.45*GeoModelKernelUnits::cm, 5.*GeoModelKernelUnits::cm, .4*GeoModelKernelUnits::cm );
  const GeoLogVol* ZigZagStrALogical = new GeoLogVol( ZigZagStrAName, ZigZagStrAShape, Al );
  GeoPhysVol* ZigZagStrAPhysical = new GeoPhysVol( ZigZagStrALogical );
  
  std::string ZigZagStrBName = baseName + "::ZigZag::StrB";
  GeoBox* ZigZagStrBShape = new GeoBox( 8.53*GeoModelKernelUnits::cm, 5.*GeoModelKernelUnits::cm, .4*GeoModelKernelUnits::cm );
  const GeoLogVol* ZigZagStrBLogical = new GeoLogVol( ZigZagStrBName, ZigZagStrBShape, Al );
  GeoPhysVol* ZigZagStrBPhysical = new GeoPhysVol( ZigZagStrBLogical );
  
  std::string ZigZagStrCName = baseName + "::ZigZag::StrC";
  GeoTrd* ZigZagStrCShape = new GeoTrd( 1.03*GeoModelKernelUnits::cm, .453*GeoModelKernelUnits::cm, 5.*GeoModelKernelUnits::cm, 5.*GeoModelKernelUnits::cm, .283*GeoModelKernelUnits::cm );
  const GeoLogVol* ZigZagStrCLogical = new GeoLogVol( ZigZagStrCName, ZigZagStrCShape, Al );
  GeoPhysVol* ZigZagStrCPhysical = new GeoPhysVol( ZigZagStrCLogical );
  
  std::string ZigZagStrDName = baseName + "::ZigZag::StrD";
  GeoTrd* ZigZagStrDShape = new GeoTrd( .005*GeoModelKernelUnits::cm, .31*GeoModelKernelUnits::cm, 5.*GeoModelKernelUnits::cm, 5.*GeoModelKernelUnits::cm, .365*GeoModelKernelUnits::cm );
  const GeoLogVol* ZigZagStrDLogical = new GeoLogVol( ZigZagStrDName, ZigZagStrDShape, Al );
  GeoPhysVol* ZigZagStrDPhysical = new GeoPhysVol( ZigZagStrDLogical );
  
  int StrAIdTag = 1;
  
  for ( int i = 0; i < 9; i++ ) {
  	ZigZagMotherPhysical->add( new GeoIdentifierTag( StrAIdTag ) );
  	ZigZagMotherPhysical->add( new GeoTransform( GeoTrf::Translate3D( ( 124.4 - 31.1*i )*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, 6.05*GeoModelKernelUnits::cm ) ) );
  	ZigZagMotherPhysical->add( ZigZagStrAPhysical );
	StrAIdTag++;
  }
  for ( int j = 0; j < 2; j++ ) for ( int i = 0; i < 8; i++ ) {
  	ZigZagMotherPhysical->add( new GeoIdentifierTag( StrAIdTag ) );
  	ZigZagMotherPhysical->add( new GeoTransform( GeoTrf::Translate3D( ( 108.85 - 31.1*i )*GeoModelKernelUnits::cm, ( 2*j - 1 )*10.*GeoModelKernelUnits::cm, 6.05*GeoModelKernelUnits::cm ) ) );
  	ZigZagMotherPhysical->add( ZigZagStrAPhysical );
	StrAIdTag++;
  }
  
  int StrBIdTag = 1;
  const double xB1[ 2 ] = { ( -6.77 + 108.85 )*GeoModelKernelUnits::cm, ( 6.77 + 108.85 )*GeoModelKernelUnits::cm };
  const double xB2[ 2 ] = { ( -6.77 + 124.4 )*GeoModelKernelUnits::cm, ( 6.77 + 93.3 )*GeoModelKernelUnits::cm  };
  const double alpha[ 2 ] = { 45.*GeoModelKernelUnits::deg, -45.*GeoModelKernelUnits::deg };
  
  for ( int k = 0; k < 2; k++ )  for ( int i = 0; i < 8; i++ ) {
  	ZigZagMotherPhysical->add( new GeoIdentifierTag( StrBIdTag ) );
  	ZigZagMotherPhysical->add( new GeoTransform( GeoTrf::Transform3D(GeoTrf::Translate3D( xB1[ k ] - 31.1*i*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, -.1*GeoModelKernelUnits::cm )*GeoTrf::RotateY3D( alpha[ k ] ) ) ) );
  	ZigZagMotherPhysical->add( ZigZagStrBPhysical );
	StrBIdTag++;
	
	for ( int j = 0; j < 2; j++ ) {
		ZigZagMotherPhysical->add( new GeoIdentifierTag( StrBIdTag ) );
  		ZigZagMotherPhysical->add( new GeoTransform( GeoTrf::Transform3D( GeoTrf::Translate3D( xB2[ k ] - 31.1*i*GeoModelKernelUnits::cm, ( -10. + 20.*j )*GeoModelKernelUnits::cm, -.1*GeoModelKernelUnits::cm ) *GeoTrf::RotateY3D( alpha[ k ] )) ) );
  		ZigZagMotherPhysical->add( ZigZagStrBPhysical );
		StrBIdTag++;	
	}
  }
  
  int StrCIdTag = 1;
  
  for ( int i = 0; i < 9; i++ ) {
  	if ( i < 8 ) {
  		ZigZagMotherPhysical->add( new GeoIdentifierTag( StrCIdTag ) );
  		ZigZagMotherPhysical->add( new GeoTransform( GeoTrf::Translate3D( ( 108.85 - 31.1*i )*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, -6.15*GeoModelKernelUnits::cm ) ) );
  		ZigZagMotherPhysical->add( ZigZagStrCPhysical );
		StrCIdTag++;
	}
	for ( int j = 0; j < 2; j++ ) {
		ZigZagMotherPhysical->add( new GeoIdentifierTag( StrCIdTag ) );
  		ZigZagMotherPhysical->add( new GeoTransform( GeoTrf::Translate3D( ( 124.4 - 31.1*i )*GeoModelKernelUnits::cm, ( -10. + 20.*j )*GeoModelKernelUnits::cm, -6.15*GeoModelKernelUnits::cm ) ) );
  		ZigZagMotherPhysical->add( ZigZagStrCPhysical );
		StrCIdTag++;
	}
  }
  
  int StrDIdTag = 1;
  const double xD1[ 2 ] = { ( -2.598 + 124.4 )*GeoModelKernelUnits::cm, ( 2.598 + 124.4 )*GeoModelKernelUnits::cm };
  const double xD2[ 2 ] = { ( -2.598 + 108.85 )*GeoModelKernelUnits::cm, ( 2.598 + 108.85 )*GeoModelKernelUnits::cm };
  const double beta[ 2 ] = { -22.5*GeoModelKernelUnits::deg, 22.5*GeoModelKernelUnits::deg };
  
  for ( int k = 0; k < 2; k++ )  for ( int i = 0; i < 9; i++ ) {
  	ZigZagMotherPhysical->add( new GeoIdentifierTag( StrDIdTag ) );
  	ZigZagMotherPhysical->add( new GeoTransform( GeoTrf::Transform3D( GeoTrf::Translate3D( xD1[ k ] - 31.1*i*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, 5.995*GeoModelKernelUnits::cm ) *GeoTrf::RotateY3D( beta[ k ] )) ) );
  	ZigZagMotherPhysical->add( ZigZagStrDPhysical );
	StrDIdTag++;
	
	if ( i < 8 ) for ( int j = 0; j < 2; j++ ) {
		ZigZagMotherPhysical->add( new GeoIdentifierTag( StrDIdTag ) );
  		ZigZagMotherPhysical->add( new GeoTransform( GeoTrf::Transform3D( GeoTrf::Translate3D( xD2[ k ] - 31.1*i*GeoModelKernelUnits::cm, ( -10. +20.*j )*GeoModelKernelUnits::cm, 5.995*GeoModelKernelUnits::cm ) *GeoTrf::RotateY3D( beta[ k ] )) ) );
  		ZigZagMotherPhysical->add( ZigZagStrDPhysical );
		StrDIdTag++;	
	}
  }
   
  WallPhysical->add( new GeoIdentifierTag( 1 ) );
  WallPhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*GeoModelKernelUnits::cm, 0.*GeoModelKernelUnits::cm, -53.2*GeoModelKernelUnits::cm ) ) );
  WallPhysical->add( ZigZagMotherPhysical );
    
return m_cryoEnvelopePhysical;
}

GeoPhysVol* LArGeo::CryostatConstructionTBEC::GetLArPhysical()
{
  return m_LArPhysical;
}
