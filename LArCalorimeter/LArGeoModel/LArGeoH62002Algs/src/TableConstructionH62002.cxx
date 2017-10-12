/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TableConstructionH62002.h"
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

#include "GeoModelInterfaces/IGeoDbTagSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include <string>
#include <cmath>
#include <iostream>

LArGeo::TableConstructionH62002::TableConstructionH62002()
  : m_H62002TablePhysical(nullptr)
{
}


LArGeo::TableConstructionH62002::~TableConstructionH62002()
{
}



GeoVPhysVol* LArGeo::TableConstructionH62002::GetEnvelope()
{

  if (m_H62002TablePhysical) return m_H62002TablePhysical;

  
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in TableConstructionH62002, cannot access MessageSvc");
  }

  MsgStream log(msgSvc, "LArGeo::TableConstructionH62002"); 
  log << MSG::INFO;
  log << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                     +" << std::endl;
  log << "+    HELLO from LArGeo::TableConstructionH62002       +" << std::endl;
  log << "+                                                     +" << std::endl;
  log << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;


  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in TableConstructionH62002, cannot access DetectorStore");
  }

  ServiceHandle<IGeoDbTagSvc> geoDbTagSvc ("GeoDbTagSvc", "WallsConstruction");
  if (geoDbTagSvc.retrieve().isFailure()) {
    throw std::runtime_error ("Cannot locate GeoDbTagSvc!!");
  }

  // Get the materials from the material manager:-----------------------------------------------------//
  //                                                                                                  //  
  DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;
  
  GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in TableConstructionH62002, std::Air is not found.");
   
  GeoMaterial *Aluminium  = materialManager->getMaterial("std::Aluminium");
  if (!Aluminium) throw std::runtime_error("Error in TableConstructionH62002, std::Aluminium is not found.");

  // Is this ok for the Scintillator?
  // I don't really know for sure what kind of a scintillator we have.
  // Lots of Scintillators are PMMA (Plexiglas), which has a composition of C5 H8 O2 and density 1.18 g/cm3
  // The Tile uses a composition of C H (density 1.032)    
  // The old testbeam code uses a composition of C9 H10 (density 1.032)
  // ... because it's easiest at the moment and not all that different from the fractional
  // composition of the old tb code, take the Tile material (polysterene)...     
  GeoMaterial *Scint  = materialManager->getMaterial("std::Polystyrene");
  if (!Scint) throw std::runtime_error("Error in TableConstructionH62002, std::Polystyrene is not found.");
  
  GeoMaterial *Mylar  = materialManager->getMaterial("std::Mylar");
  if (!Mylar) throw std::runtime_error("Error in TableConstructionH62002, std::Mylar is not found.");
  
  
  //                                                                                                 //
  //-------------------------------------------------------------------------------------------------//

  std::string AtlasVersion = geoDbTagSvc->atlasVersion();
  std::string LArVersion = geoDbTagSvc->LAr_VersionOverride();
   
  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";


  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////

 
  //  Here we creat the envelope for the Moveable Table Instrumentation.  This code is repeated
  //  createEnvelope() method below.  There should be a way to avoid this repitition.

  //------ The Table 

  std::string baseName = "LAr::TBH62002";
  std::string H62002TableName = baseName + "::Table";

  const double H62002TableXY  =  150.*CLHEP::mm;
  const double H62002TableZ   = 1200.*CLHEP::mm;


  GeoBox* H62002TableShape = new GeoBox( H62002TableXY, H62002TableXY, H62002TableZ );   
  const GeoLogVol* H62002TableLogical = new GeoLogVol( H62002TableName, H62002TableShape, Air );

  m_H62002TablePhysical = new GeoPhysVol(H62002TableLogical);
  //m_H62002TablePhysical->add( new GeoNameTag("LArTBTablePos") );



 

  //------ F1/F2Scintillators
  log << "Create F1/F2 Scintillators ..." << std::endl;
  
  // Universal size:
  const double Fx = 10.0*CLHEP::mm;
  const double Fy = 10.0*CLHEP::mm;
  const double Fz = 10.0*CLHEP::mm;

  std::vector<double> v_ScintZ;
  v_ScintZ.push_back(2195.*CLHEP::mm); // <--  = btas_pos
  v_ScintZ.push_back(2320.*CLHEP::mm);
  const double ScintDx = Fx;
  const double ScintDy = Fy;
  const double ScintDz = Fz;

   // Create one Scintillator and place it twice along z:
 
  GeoBox* ScintShape = new GeoBox(ScintDx, ScintDy, ScintDz);  // A generic Box Scintillator
  std::string ScintName = baseName + "::Scintillator";
  GeoLogVol* ScintLogical = new GeoLogVol( ScintName, ScintShape, Scint );
  GeoPhysVol* ScintPhysical = new GeoPhysVol( ScintLogical );    
  for ( unsigned int i = 0; i < v_ScintZ.size(); i++ ) {
    m_H62002TablePhysical->add( new GeoIdentifierTag(i) );
    m_H62002TablePhysical->add( new GeoTransform( HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, (v_ScintZ[ i ]-H62002TableZ) ) ) );
    log << MSG::INFO << " Position the F Scintillator at: " << v_ScintZ[ i ] << endmsg ;
    m_H62002TablePhysical->add( ScintPhysical );
  } 

  //----- Done with F1/F2 Scintillators






  //------ Get the MWPCs from LArGeoH6Cryostats
  const int MwpcNumber = 3;
  std::vector<double> v_MwpcPos;
  v_MwpcPos.push_back(105.*CLHEP::mm);
  v_MwpcPos.push_back(825.*CLHEP::mm); 
  v_MwpcPos.push_back(1815.*CLHEP::mm);
  double WireStep = 1.*CLHEP::mm;
  MWPCConstruction mwpcXConstruction (WireStep);
  GeoVPhysVol* mwpcEnvelope = mwpcXConstruction.GetEnvelope();
  for ( int imwpc = 0; imwpc<MwpcNumber ; imwpc++)
    { 
      m_H62002TablePhysical->add(new GeoIdentifierTag(imwpc+2));
      m_H62002TablePhysical->add( new GeoTransform(HepGeom::Translate3D( 0.*CLHEP::cm, 0.*CLHEP::cm, (v_MwpcPos[imwpc]-H62002TableZ) ) ) );
      m_H62002TablePhysical->add(mwpcEnvelope);    
    }
  //------ Done with creating an MWPC from LArGeoH6Cryostats



  // End Moveable Table detectors


  return m_H62002TablePhysical;
}



