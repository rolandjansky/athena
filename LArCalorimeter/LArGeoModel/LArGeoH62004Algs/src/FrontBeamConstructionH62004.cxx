/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FrontBeamConstructionH62004.h"
#include "LArGeoH6Cryostats/MWPCConstruction.h"
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
#include "GeoModelKernel/GeoDefinitions.h"
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

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <string>
#include <cmath>
#include <iostream>

LArGeo::FrontBeamConstructionH62004::FrontBeamConstructionH62004()
  :m_H62004FrontBeamPhysical(0),
   m_detectorManager(0)
{
}


LArGeo::FrontBeamConstructionH62004::~FrontBeamConstructionH62004()
{
}



GeoVPhysVol* LArGeo::FrontBeamConstructionH62004::GetEnvelope()
{

  if (m_H62004FrontBeamPhysical) return m_H62004FrontBeamPhysical;

  // Get access to the material manager:
  
  StatusCode sc;
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  sc=svcLocator->service("MessageSvc", msgSvc, true );
  if (sc==StatusCode::FAILURE) {
    throw std::runtime_error("Error in FrontBeamConstructionH62004, cannot access MessageSvc");
  }

  MsgStream log(msgSvc, "LArGeo::FrontBeamConstructionH62004"); 
  log << MSG::INFO << "+    HELLO from LArGeo::FrontBeamConstructionH62004       +" << endmsg;


  StoreGateSvc *detStore;
  sc=svcLocator->service("DetectorStore", detStore, false );
  if (sc==StatusCode::FAILURE) {
    throw std::runtime_error("Error in FrontBeamConstructionH62004, cannot access DetectorStore");
  }



  // Get the materials from the material manager:-----------------------------------------------------//
  //                                                                                                  //

  const StoredMaterialManager* materialManager = nullptr;
  sc=detStore->retrieve(materialManager, std::string("MATERIALS"));
  if (StatusCode::SUCCESS != sc) return NULL;

  const GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in FrontBeamConstructionH62004, std::Air is not found.");
   
  // Is this ok for the Scintillator?
  // I don't really know for sure what kind of a scintillator we have.
  // Lots of Scintillators are PMMA (Plexiglas), which has a composition of C5 H8 O2 and density 1.18 g/cm3
  // The Tile uses a composition of C H (density 1.032)    
  // The old FrontBeam testbeam code uses a composition of C9 H10 (density 1.032)
  // ... because it's easiest at the moment and not all that different from the fractional
  // composition of the old tb code, take the Tile material (polysterene)...     
  const GeoMaterial *Scint  = materialManager->getMaterial("std::Polystyrene");
  if (!Scint) throw std::runtime_error("Error in FrontBeamConstructionH62004, std::Polystyrene is not found.");
  
  //-------------------------------------------------------------------------------------------------//
  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////

 
  //  Here we creat the envelope for the Moveable FrontBeam Instrumentation.  This code is repeated
  //  createEnvelope() method below.  There should be a way to avoid this repitition.

  //------ The FrontBeam 

  std::string baseName = "LAr::TBH62004";
  std::string H62004FrontBeamName = baseName + "::FrontBeam";

  //
  // Define dimension of Front part & position of Front part
  //
  // DB ?
  const double bard_x = 20.0*Gaudi::Units::cm;
  const double bard_y = 20.0*Gaudi::Units::cm;
  //const double bard_z = 35.0*Gaudi::Units::cm;
  const double bard_z = 100.0*Gaudi::Units::cm;
  const double fbpc_z[2] = {60.4*Gaudi::Units::cm,112.7*Gaudi::Units::cm};
  // Position in exp_hall
  //const double z_bard=-2160.0*Gaudi::Units::cm+80.1*Gaudi::Units::cm+16.*Gaudi::Units::cm+bard_z;
  //const double z_bardm=-2160.0*Gaudi::Units::cm+1362.3*Gaudi::Units::cm;



  GeoBox* H62004FrontBeamShape = new GeoBox( bard_x, bard_y, bard_z );   
  const GeoLogVol* H62004FrontBeamLogical = new GeoLogVol( H62004FrontBeamName, H62004FrontBeamShape, Air );

  m_H62004FrontBeamPhysical = new GeoPhysVol(H62004FrontBeamLogical);
  //m_H62004FrontBeamPhysical->add( new GeoNameTag("LArTBFrontBeamPos") );

  //------ W1,W2,B1 Scintillators
  //   In the old stand-alone code, all three were round with a radius of 5cm 
  //   and 7.5mm thickness.
  //   Logbooks in the control-room say that their xyz sizes are:
  //   B1   : 30 x 30 x 10 Gaudi::Units::mm
  //   W1,2 : 150 x 150 x 10 Gaudi::Units::mm
  // They are certainly not round, so stick with the logbook values 
  // The beam sees the instrumentation in the following order:
  // W1, W2, B1, MWPC5

  log << MSG::INFO << "Create Front Scintillators ..." << endmsg;
  
  const double Wxy=  75.0*Gaudi::Units::mm;
  const double Wz =   5.0*Gaudi::Units::mm;
  const double Bxy=  15.0*Gaudi::Units::mm;
  const double Bz =   5.0*Gaudi::Units::mm;

  std::vector<double> v_ScintXY;
  std::vector<double> v_ScintZ;
  v_ScintXY.push_back(Wxy);
  v_ScintXY.push_back(Wxy); 
  v_ScintXY.push_back(Bxy); 
  v_ScintZ.push_back(10.*Gaudi::Units::mm); 
  v_ScintZ.push_back(40.*Gaudi::Units::mm); 
  v_ScintZ.push_back(180.*Gaudi::Units::mm);

   // Create one Scintillator and place it twice along z:
 
  GeoBox* ScintShapeW = new GeoBox(Wxy, Wxy, Wz);  
  GeoBox* ScintShapeB = new GeoBox(Bxy, Bxy, Bz);  
  std::string ScintName = H62004FrontBeamName + "::Scintillator";
  GeoLogVol* WScintLogical = new GeoLogVol( ScintName, ScintShapeW, Scint );
  GeoLogVol* BScintLogical = new GeoLogVol( ScintName, ScintShapeB, Scint );
  GeoPhysVol* WScintPhysical = new GeoPhysVol( WScintLogical );    
  GeoPhysVol* BScintPhysical = new GeoPhysVol( BScintLogical );    
  //WScintPhysical->add( new GeoNameTag(ScintName) );
  //BScintPhysical->add( new GeoNameTag(ScintName) );
  for ( unsigned int i = 0; i < v_ScintZ.size(); i++ ) {
    m_H62004FrontBeamPhysical->add( new GeoIdentifierTag(i+1) );
    m_H62004FrontBeamPhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*Gaudi::Units::cm, 0.*Gaudi::Units::cm, (v_ScintZ[ i ]-bard_z) ) ) ) ;     m_H62004FrontBeamPhysical->add( new GeoNameTag(ScintName) );

    switch(i) {
    case 0: case 1: { m_H62004FrontBeamPhysical->add( WScintPhysical ); break; }
    case 2:         { m_H62004FrontBeamPhysical->add( BScintPhysical ); break; }
    default: { throw std::runtime_error("H62004FrontBeam wants too many Scintillators!!");   break; }
    }
  }

  //----- Done with Scintillators



  //------ Now create MWPC5 
  log << MSG::INFO << " Create MWPC5 " << endmsg;
  
  MWPCConstruction MWPC5 (2.*Gaudi::Units::mm);
  GeoVPhysVol* MwpcPhysical = MWPC5.GetEnvelope();

  const double MwpcPos = 445.*Gaudi::Units::mm;

  m_H62004FrontBeamPhysical->add( new GeoIdentifierTag(5) );
  m_H62004FrontBeamPhysical->add( new GeoTransform(GeoTrf::Translate3D( 0.*Gaudi::Units::cm, 0.*Gaudi::Units::cm, (MwpcPos-bard_z) ) ) );
  m_H62004FrontBeamPhysical->add( MwpcPhysical );

  //----- Done with the MWPC

  //----- Now create BPC
  log << MSG::INFO << " Create BPC 1&2 " << endmsg;

  BPCConstruction BPC (false);
  GeoVPhysVol* BPCPhysical = BPC.GetEnvelope();
  for(int i=1; i<3; ++i) {
     m_H62004FrontBeamPhysical->add( new GeoIdentifierTag(i) );
     m_H62004FrontBeamPhysical->add( new GeoTransform(GeoTrf::Translate3D( 0.*Gaudi::Units::cm, 0.*Gaudi::Units::cm, (MwpcPos-bard_z) + fbpc_z[i-1]) ) );
     m_H62004FrontBeamPhysical->add(BPCPhysical);
  }

  // End Moveable FrontBeam detectors


  return m_H62004FrontBeamPhysical;
}



