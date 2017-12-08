/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArDetectorConstructionTBEC

#include "LArDetectorConstructionTBEC.h"
#include "CryostatConstructionTBEC.h"
#include "LArGeoEndcap/EMECConstruction.h"
#include "LArGeoEndcap/EndcapPresamplerConstruction.h"

#include "GeoModelKernel/GeoElement.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"

// For transforms:
#include "CLHEP/Geometry/Transform3D.h" 
// For units:
#include "CLHEP/Units/PhysicalConstants.h"

// For the database:
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

// For run options :
#include "LArG4RunControl/LArGeoTBGeometricOptions.h"

#include <string>
#include <cmath>

// Handle Axis, for debug
#define AXIS_ON 0

// The "database record" types.
typedef struct {
  int cylNumber;
  GeoMaterial* mat;
  double Rmin;
  double Dr;
  double Zmin;
  double Dz;
} cryoEndcapCylDBRecord_t;


LArGeo::LArDetectorConstructionTBEC::LArDetectorConstructionTBEC():
m_eta_pos(0),
m_eta_cell(0),
m_phi_pos(0),
m_phi_cell(0),
m_hasLeadCompensator(false),
m_hasPresampler(false),
m_ModuleRotation(0),
m_YShift(0),
m_tbecEnvelopePhysical(NULL),
m_pAccessSvc(NULL)
{;}

void LArGeo::LArDetectorConstructionTBEC::getSimulationParameters()
{
  m_eta_cell = 12;
  m_phi_cell = 16;

  StoreGateSvc* detStore;
  const LArGeoTBGeometricOptions  *largeotbgeometricoptions;
  
  StatusCode status;
  ISvcLocator* svcLocator = Gaudi::svcLocator(); 
	IMessageSvc * msgSvc;
	if(svcLocator->service("MessageSvc", msgSvc, true) == StatusCode::FAILURE){
		throw std::runtime_error("Error in LArDetectorConstructionTBEC, cannot access MessageSvc");
	}
	MsgStream log(msgSvc, "LArGeo::LArDetectorConstructionTBEC"); 

	status = svcLocator->service("DetectorStore", detStore);
  
	if(status.isSuccess()){
		status = detStore->retrieve(largeotbgeometricoptions, "LArGeoTBGeometricOptions");
		if(!status.isFailure()){
			m_eta_cell = largeotbgeometricoptions->CryoEtaPosition();
			m_phi_cell = largeotbgeometricoptions->CryoPhiPosition();
		} else {
			log << MSG::WARNING
			    << "Can't access LArGeoTBGeometricOptions, using default values"
				<< endmsg;
		}
	} else {
		throw std::runtime_error("LArDetectorConstructionTBEC: cannot initialize \
StoreGate interface");
	}

  if ( m_eta_cell < 0.5 ) m_eta_pos = 0.05*( m_eta_cell + 0.5 ) + 1.375; // Cell 0 has double eta width
  else if ( m_eta_cell > 43.5 ) m_eta_pos = 0.1*( m_eta_cell - 43.5 ) + 2.5; // Inner Wheel
  else m_eta_pos = 0.025*( m_eta_cell - 0.5 ) + 1.425; // Outer Wheel

  if ( m_eta_cell <= 43.5 ) m_phi_pos = -( ( m_phi_cell - 16. )*1.40625 + 0.46875 )*CLHEP::deg; // Outer Wheel
  else m_phi_pos = -( 4*( m_phi_cell - 4. )*1.40625 + 0.46875 )*CLHEP::deg; // Inner Wheel
    

/*
  printf( "LArDetectorConstructionTBEC\teta_cell = %6.2lf ( eta = %6.2lf     )\n", m_eta_cell, 
m_eta_pos );
  printf( "LArDetectorConstructionTBEC\tphi_cell = %6.2lf ( phi = %6.2lf CLHEP::deg )\n", m_phi_cell, 
m_phi_pos/CLHEP::deg );
*/

/*
  printf( "LArDetectorConstructionTBEC\tModuleRotation = %6.2lf CLHEP::deg\n", m_ModuleRotation/CLHEP::deg );
  printf( "LArDetectorConstructionTBEC\tYShift = %6.1lf CLHEP::mm\n", m_YShift/CLHEP::mm );
*/
}

LArGeo::LArDetectorConstructionTBEC::~LArDetectorConstructionTBEC() {}

GeoVPhysVol* LArGeo::LArDetectorConstructionTBEC::GetEnvelope()
{

  if (m_tbecEnvelopePhysical) return m_tbecEnvelopePhysical;

  ISvcLocator *svcLocator = Gaudi::svcLocator();
	IMessageSvc * msgSvc;
	if(svcLocator->service("MessageSvc", msgSvc, true) == StatusCode::FAILURE){
		throw std::runtime_error("Error in LArDetectorConstructionTBEC, cannot access MessageSvc");
	}

	MsgStream log(msgSvc, "LArGeo::LArDetectorConstructionTBEC"); 
  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in LArDetectorConstructionTBEC, cannot access DetectorStore");
  }
  DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return 0;

  GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) {
    throw std::runtime_error("Error in LArDetectorConstructionTBEC, std::Air is not found.");
  }

  StatusCode sc;
  sc=svcLocator->service("RDBAccessSvc",m_pAccessSvc);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
  }

  DecodeVersionKey larVersion("LAr");
  std::string detectorKey  = larVersion.tag();
  std::string detectorNode = larVersion.node();

  // Default values....
  m_hasLeadCompensator = false;
  m_hasPresampler = false;
  m_ModuleRotation = 0.*CLHEP::deg;
  m_YShift = 0.*CLHEP::mm;
 
  IRDBRecordset_ptr tbecGeometry   = m_pAccessSvc->getRecordsetPtr("TBECGeometry",detectorKey, detectorNode); 
  if ((*tbecGeometry).size()!=0) {
    m_hasLeadCompensator = (*tbecGeometry)[0]->getInt("LEADCOMPENSATOR");
    m_hasPresampler      = (*tbecGeometry)[0]->getInt("PRESAMPLER");
    m_ModuleRotation     = (*tbecGeometry)[0]->getDouble("MODULEROTATION");
    m_YShift             = (*tbecGeometry)[0]->getDouble("YSHIFT");
  }

	log << MSG::INFO << "LeadCompensator = ";
	if(m_hasLeadCompensator) log << "true";
	else log << "false";
	log << endmsg;

	log << MSG::INFO << "Presampler = ";
	if(m_hasPresampler) log << "true";
	else log << "false";
	log << endmsg;

  // Retrieve simulation parameters from Storegate
  getSimulationParameters();

  std::string baseName = "LAr::TBEC::MotherVolume";

  GeoBox* tbecMotherShape = new GeoBox( 5.*CLHEP::m, 5.*CLHEP::m, 15.*CLHEP::m );

  const GeoLogVol* tbecMotherLogical =
    new GeoLogVol(baseName, tbecMotherShape, Air);

  m_tbecEnvelopePhysical = new GeoPhysVol(tbecMotherLogical);

  m_tbecEnvelopePhysical->add( new GeoNameTag("LArEndcapPos") );
  m_tbecEnvelopePhysical->add( createEnvelope() );
  return m_tbecEnvelopePhysical;
}

GeoFullPhysVol* LArGeo::LArDetectorConstructionTBEC::createEnvelope()
{
  // Get access to the material manager:
  
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in LArDetectorConstructionTBEC, cannot access MessageSvc");
  }

	MsgStream log(msgSvc, "LArGeo::LArDetectorConstructionTBEC"); 
	log << MSG::DEBUG << "createEnvelope() started" << endmsg;


  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in LArDetectorConstructionTBEC, cannot access DetectorStore");
  }

  // Get the materials from the material manager:-----------------------------------------------------//
  //                                                                                                  //
  DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;
  
  GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in LArDetectorConstructionTBEC, std::Air is not found.");
 
  GeoMaterial *Lead  = materialManager->getMaterial("std::Lead");
  if (!Lead) throw std::runtime_error("Error in LArDetectorConstructionTBEC, std::Lead is not found.");
  
  
  //                                                                                                 //
  //-------------------------------------------------------------------------------------------------//
  DecodeVersionKey larVersion("LAr");
  std::string detectorKey  = larVersion.tag();
  std::string detectorNode = larVersion.node();


  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////

  // Set up strings for volume names.
  std::string baseName = "LAr::TBEC";

  // Define the mother volume for the endcap cryostat.  Everything
  // else in the endcap (cryostat walls, detectors, etc.) should be
  // placed inside here.

  // The position of this volume may change if the thickness of the
  // cabling in front of the endcaps changes.  Therefore, we must get
  // the z-shift from the database and adjust the volume geometry
  // accordingly.

  std::string tbecMotherName = baseName + "::MotherVolume";
  GeoBox* tbecMotherShape = new GeoBox( 5.*CLHEP::m, 5.*CLHEP::m, 15.*CLHEP::m  );
  const GeoLogVol* tbecMotherLogical = new GeoLogVol( tbecMotherName, tbecMotherShape, Air );
  GeoFullPhysVol* tbecMotherPhysical = new GeoFullPhysVol( tbecMotherLogical );
  
  double xcent = -120.*CLHEP::cm, zcent = 395.7*CLHEP::cm;
  double zfface = zcent - 60.09*CLHEP::cm;
  log << MSG::DEBUG << "eta = " << m_eta_pos;
  if ( m_eta_pos > 5. ) m_eta_pos = 0.;
  else m_eta_pos = 2*atan( exp( -m_eta_pos ) );
  log << ", positioning cryostat with angle " << m_eta_pos*(1./CLHEP::deg) << " CLHEP::deg";
  log << endmsg;
  
  // Tubular axis, dummy
  
  if ( AXIS_ON ) {
  
     double axisZHalfLength = 5*CLHEP::m;
  
     GeoTube* axisShape = new GeoTube( 0.*CLHEP::cm, 1.*CLHEP::cm, axisZHalfLength );
  
     // x-axis
     std::string XAxisName = baseName + "::XAxis";
     const GeoLogVol* XAxisLogical = new GeoLogVol( XAxisName, axisShape, Air );
     GeoPhysVol* XAxisPhysVol = new GeoPhysVol( XAxisLogical );
  
     tbecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
     tbecMotherPhysical->add( new GeoTransform( HepGeom::Transform3D( CLHEP::HepRotationY( 90.*CLHEP::deg ), 
  							        CLHEP::Hep3Vector( axisZHalfLength, 0.*CLHEP::m, 0.*CLHEP::m ) ) ) );
     tbecMotherPhysical->add( XAxisPhysVol );
  
     // y-axis
     std::string YAxisName = baseName + "::YAxis";
     const GeoLogVol* YAxisLogical = new GeoLogVol( YAxisName, axisShape, Air );
     GeoPhysVol* YAxisPhysVol = new GeoPhysVol( YAxisLogical );
  
     tbecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
     tbecMotherPhysical->add( new GeoTransform( HepGeom::Transform3D( CLHEP::HepRotationX( -90.*CLHEP::deg ),
  					     		        CLHEP::Hep3Vector( 0.*CLHEP::m, axisZHalfLength, 0.*CLHEP::m ) ) ) );
     tbecMotherPhysical->add( YAxisPhysVol );
  
     //z-axis
     std::string ZAxisName = baseName + "::ZAxis";
     const GeoLogVol* ZAxisLogical = new GeoLogVol( ZAxisName, axisShape, Air );
     GeoPhysVol* ZAxisPhysVol = new GeoPhysVol( ZAxisLogical );
  
     tbecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
     tbecMotherPhysical->add( new GeoTransform( HepGeom::TranslateZ3D( axisZHalfLength ) ) );
     tbecMotherPhysical->add( ZAxisPhysVol );
  }
 
  // Lead compensator, Optionnal
  
  if ( m_hasLeadCompensator ) {
     std::string CompensatorName = baseName + "::LeadCompensator";
     GeoBox* CompensatorShape = new GeoBox( 152.*CLHEP::cm, 195.*CLHEP::cm, 0.56*CLHEP::cm );
     const GeoLogVol* CompensatorLogical = new GeoLogVol( CompensatorName, CompensatorShape, Lead );
     GeoPhysVol* CompensatorPhysical = new GeoPhysVol( CompensatorLogical );
	
     tbecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
     tbecMotherPhysical->add( new GeoTransform( HepGeom::Transform3D( CLHEP::HepRotationY( m_eta_pos ),
						   		CLHEP::Hep3Vector( xcent, 0.*CLHEP::cm, 300.*CLHEP::cm ).rotateY( m_eta_pos ) ) ) );
     tbecMotherPhysical->add( CompensatorPhysical );
  }
  
  // Cryostat
  
  CryostatConstructionTBEC cryoConstruction;
  GeoVFullPhysVol* cryoPhys = cryoConstruction.GetEnvelope();
  GeoPhysVol* LArPhysical = cryoConstruction.GetLArPhysical();
  
  tbecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
  tbecMotherPhysical->add( new GeoTransform( HepGeom::Transform3D( CLHEP::HepRotationY( m_eta_pos ),
						   	     CLHEP::Hep3Vector( xcent, 0.*CLHEP::cm, zcent ).rotateY( m_eta_pos ) ) ) );
  tbecMotherPhysical->add( cryoPhys );
  
  // Beam chambers
  
  log << MSG::VERBOSE << "Creating beam chambers ..." << std::endl;
  
  const double beamCZ[ 4 ] = { 17.9*CLHEP::m, 7.673*CLHEP::m, 1.352*CLHEP::m, .256*CLHEP::m };
  const double beamCSize = 11.*CLHEP::cm, beamCTh = 28*CLHEP::mm; // divided by 2, for half-length
  
  GeoBox* BeamCShape = new GeoBox( beamCSize, beamCSize, beamCTh );
  for ( int i = 0; i < 4; i++ ) {
  	std::string BeamCName = baseName + "::BeamChamber";
	BeamCName+= char( i ) + '0';
  	GeoLogVol* BeamCLogical = new GeoLogVol( BeamCName, BeamCShape, Air );
  	GeoPhysVol* BeamCPhysical = new GeoPhysVol( BeamCLogical );
	
	tbecMotherPhysical->add( new GeoIdentifierTag( 1 ) );
  	tbecMotherPhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, zfface - beamCZ[ i ] ) ) );
  	tbecMotherPhysical->add( BeamCPhysical );
  } 
  
  // End cap module
	log << MSG::DEBUG << std::endl
	    << "Module deviation: " << m_ModuleRotation * (1./CLHEP::deg) << " CLHEP::deg" << std::endl
            << "Phi position: " << m_phi_pos * (1./CLHEP::deg) << " CLHEP::deg" << std::endl
            << "Y shift: " << m_YShift * (1./CLHEP::mm) << " CLHEP::mm"
            << endmsg;
  
  // z = 0 in emecMother is at active region's front face
  
  EMECConstruction emecModuleConstruction( true, true, true );
  GeoFullPhysVol *emecEnvelope= (GeoFullPhysVol *) emecModuleConstruction.GetEnvelope();
  StoredPhysVol *sPhysVol = new StoredPhysVol(emecEnvelope);
  StatusCode status=detStore->record(sPhysVol,"EMEC_POS");
  if(!status.isSuccess()) throw std::runtime_error ("Cannot store EMEC_POS");  


  CLHEP::HepRotation Mrot;
  Mrot.rotateY( m_ModuleRotation );
  Mrot.rotateZ( m_phi_pos + 90*CLHEP::deg );
  CLHEP::Hep3Vector pos( -xcent, m_YShift, -51.4/2*CLHEP::cm );
  
  if ( LArPhysical != 0 ) {
  
     LArPhysical->add( new GeoIdentifierTag( 1 ) );     
     LArPhysical->add( new GeoTransform( HepGeom::Transform3D( Mrot, pos ) ) );
     LArPhysical->add( emecEnvelope );			  		
  }
  
  pos-= CLHEP::Hep3Vector( 0.*CLHEP::mm, 0.*CLHEP::mm, 61.*CLHEP::mm +2.*CLHEP::mm +13.5*CLHEP::mm );
  
  if ( m_hasPresampler ) {

    EndcapPresamplerConstruction PresamplerConstruction( true );
    GeoFullPhysVol* PresamplerEnvelope = PresamplerConstruction.Envelope();

    StoredPhysVol *sPhysVol = new StoredPhysVol(PresamplerEnvelope);
    StatusCode status=detStore->record(sPhysVol,"PRESAMPLER_EC_POS");
    if(!status.isSuccess()) throw std::runtime_error ("Cannot store PRESAMPLER_EC_POS");
	      
    if ( LArPhysical != 0 ) {
     
       LArPhysical->add( new GeoIdentifierTag( 1 ) );
       LArPhysical->add( new GeoTransform( HepGeom::Transform3D( Mrot, pos ) ) );
       LArPhysical->add( PresamplerEnvelope );
    }
  }
    
return tbecMotherPhysical;
}
