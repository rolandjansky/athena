/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MovableTableConstructionH62004.h"
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
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
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

LArGeo::MovableTableConstructionH62004::MovableTableConstructionH62004()
  :m_H62004MovableTablePhysical(0),
   m_detectorManager(0)
{
}


LArGeo::MovableTableConstructionH62004::~MovableTableConstructionH62004()
{
}



GeoVPhysVol* LArGeo::MovableTableConstructionH62004::GetEnvelope()
{

  if (m_H62004MovableTablePhysical) return m_H62004MovableTablePhysical;

  // Get access to the material manager:
  
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  StatusCode sc;
  IMessageSvc * msgSvc;
  sc = svcLocator->service("MessageSvc", msgSvc, true );
  if (sc==StatusCode::FAILURE) {
    throw std::runtime_error("Error in MovableTableConstructionH62004, cannot access MessageSvc");
  }

  MsgStream log(msgSvc, "LArGeo::MovableTableConstructionH62004"); 
  log << MSG::INFO << "+    HELLO from LArGeo::MovableTableConstructionH62004       +" << endmsg;


  StoreGateSvc *detStore;
  sc=svcLocator->service("DetectorStore", detStore, false );
  if (sc==StatusCode::FAILURE) {
    throw std::runtime_error("Error in MovableTableConstructionH62004, cannot access DetectorStore");
  }



  // Get the materials from the material manager:-----------------------------------------------------//
  //                                                                                                  //

  const StoredMaterialManager* materialManager = nullptr;
  sc=detStore->retrieve(materialManager, std::string("MATERIALS"));
  if (StatusCode::SUCCESS != sc) return NULL;

  const GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in MovableTableConstructionH62004, std::Air is not found.");
   
  // Is this ok for the Scintillator?
  // I don't really know for sure what kind of a scintillator we have.
  // Lots of Scintillators are PMMA (Plexiglas), which has a composition of C5 H8 O2 and density 1.18 g/cm3
  // The Tile uses a composition of C H (density 1.032)    
  // The old FrontBeam testbeam code uses a composition of C9 H10 (density 1.032)
  // ... because it's easiest at the moment and not all that different from the fractional
  // composition of the old tb code, take the Tile material (polysterene)...     
  const GeoMaterial *Scint  = materialManager->getMaterial("std::Polystyrene");
  if (!Scint) throw std::runtime_error("Error in MovableTableConstructionH62004, std::Polystyrene is not found.");
  
  //-------------------------------------------------------------------------------------------------//
  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////

 
  //  Here we creat the envelope for the Moveable FrontBeam Instrumentation.  This code is repeated
  //  createEnvelope() method below.  There should be a way to avoid this repitition.


  std::string baseName = "LAr::TBH62004";
  std::string H62004MovableName = baseName + "::MovableTable";

  //
  // Define dimension of Movable part & position of Front part
  //
  double bttb_x = 15.0*Gaudi::Units::cm;
  double bttb_y = 15.0*Gaudi::Units::cm;
  double bttb_z = 120.0*Gaudi::Units::cm;
  //double bttb_pos = 833.5*Gaudi::Units::cm;
  //
  // Define S scintilator dimension and positions
  //
  double btas_x = 7.5*Gaudi::Units::cm;
  double btas_y = 7.5*Gaudi::Units::cm;
  double btas_z = 1.0*Gaudi::Units::cm;
  double bb2_x = 3.0*Gaudi::Units::cm;
  double btas_pos[3] = {100.*Gaudi::Units::cm, 219.5*Gaudi::Units::cm, 232.0*Gaudi::Units::cm};
  double bh_x = 30.0*Gaudi::Units::cm;
  double bh_d = 6.0*Gaudi::Units::cm;
  double bh_shift = 12.0*Gaudi::Units::cm;
  double bb_shift = 2.5*Gaudi::Units::cm;

  double mwpc_pos[4] =  {44.5*Gaudi::Units::cm, 12.5*Gaudi::Units::cm, 87.0*Gaudi::Units::cm, 185.3*Gaudi::Units::cm};
  double bpc_pos[2] =  {140.5*Gaudi::Units::cm, 130.5*Gaudi::Units::cm};

  GeoBox* H62004MovableShape = new GeoBox( bttb_x, bttb_y, bttb_z );   
  const GeoLogVol* H62004FrontBeamLogical = new GeoLogVol( H62004MovableName, H62004MovableShape, Air );

  m_H62004MovableTablePhysical = new GeoPhysVol(H62004FrontBeamLogical);
  //m_H62004MovableTablePhysical->add( new GeoNameTag("LArTBFrontBeamPos") );

  //------ W1,W2,B1 Scintillators
  //   In the old stand-alone code, all three were round with a radius of 5cm 
  //   and 7.5mm thickness.
  //   Logbooks in the control-room say that their xyz sizes are:
  //   B1   : 30 x 30 x 10 Gaudi::Units::mm
  //   W1,2 : 150 x 150 x 10 Gaudi::Units::mm
  // They are certainly not round, so stick with the logbook values 
  // The beam sees the instrumentation in the following order:
  // W1, W2, B1, MWPC5

  log << MSG::INFO << "Create Movable Scintillators ..." << endmsg;

  // Create scintillator S1(num=4),S2,S3(num= 6,7)
  
  GeoBox* ScintShapeS1 = new GeoBox((btas_x-1.*Gaudi::Units::cm)/2., (btas_y-1.*Gaudi::Units::cm)/2., btas_z/2.);  
  GeoBox* ScintShapeS23 = new GeoBox(btas_x/2., btas_y/2., btas_z/2.);  
  std::string ScintName = H62004MovableName + "::Scintillator";
  GeoLogVol* S1ScintLogical = new GeoLogVol( ScintName, ScintShapeS1, Scint );
  GeoLogVol* S23ScintLogical = new GeoLogVol( ScintName, ScintShapeS23, Scint );
  GeoPhysVol* S1ScintPhysical = new GeoPhysVol( S1ScintLogical );    
  GeoPhysVol* S2ScintPhysical = new GeoPhysVol( S23ScintLogical );    

  m_H62004MovableTablePhysical->add( new GeoIdentifierTag(4) );
  m_H62004MovableTablePhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*Gaudi::Units::cm, 0.*Gaudi::Units::cm, btas_pos[0]-bttb_z ) ) ) ;     
  m_H62004MovableTablePhysical->add(S1ScintPhysical);
  for ( unsigned int i = 1; i <3; ++i ) {
    m_H62004MovableTablePhysical->add( new GeoIdentifierTag(i+5) );
    m_H62004MovableTablePhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*Gaudi::Units::cm, 0.*Gaudi::Units::cm, btas_pos[i]-bttb_z ) ) ) ; 
    m_H62004MovableTablePhysical->add( S2ScintPhysical ); 
  }
  // Create scintilators H (copy num 5) and B2 (copy num 8)
  GeoBox* boxH = new GeoBox(bh_x/2., bh_x/2., btas_z/2.);
  GeoTubs* tubH = new GeoTubs(0., bh_d/2., btas_z/2., 0., 2*M_PI);
  const GeoShapeSubtraction &shapeHSc = (*boxH).subtract(*tubH);
  GeoLogVol* logHSc = new GeoLogVol( ScintName, &shapeHSc, Scint);
  GeoPhysVol* physHSc = new GeoPhysVol(logHSc);
  m_H62004MovableTablePhysical->add( new GeoIdentifierTag(5) );
  m_H62004MovableTablePhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*Gaudi::Units::cm, 0.*Gaudi::Units::cm, (btas_pos[0]-bttb_z) + bh_shift) )  ) ;     
  m_H62004MovableTablePhysical->add(physHSc);

  GeoBox* boxB = new GeoBox(bb2_x/2., bb2_x/2., (btas_z+2.5*Gaudi::Units::cm)/2.);
  GeoLogVol* logBSc = new GeoLogVol( ScintName, boxB, Scint);
  GeoPhysVol* physBSc = new GeoPhysVol(logBSc);
  m_H62004MovableTablePhysical->add( new GeoIdentifierTag(8) );
  m_H62004MovableTablePhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*Gaudi::Units::cm, 0.*Gaudi::Units::cm, (btas_pos[2]-bttb_z) + bb_shift )  ) ) ;     
  m_H62004MovableTablePhysical->add(physBSc);
  
  //----- Done with Scintillators

  //------ Now create MWPC N2 & N3 & N4 
  log << MSG::INFO << " Create MWPC's " << endmsg;
  
  MWPCConstruction MWPC(1.*Gaudi::Units::mm);
  GeoVPhysVol* MwpcPhysical = MWPC.GetEnvelope();

  for(int i = 1; i < 4; ++i){
     m_H62004MovableTablePhysical->add( new GeoIdentifierTag(i+1) );
     m_H62004MovableTablePhysical->add( new GeoTransform(GeoTrf::Translate3D( 0.*Gaudi::Units::cm, 0.*Gaudi::Units::cm, mwpc_pos[i]-bttb_z ) ) );
     m_H62004MovableTablePhysical->add( MwpcPhysical );
  }
  //----- Done with the MWPC

  //----- Now create BPC
  log << MSG::INFO << " Create BPC 5&6 " << endmsg;

  BPCConstruction BPC(false);
  GeoVPhysVol* BPCPhysical = BPC.GetEnvelope();
  for(int i=1; i<3; ++i) {
     m_H62004MovableTablePhysical->add( new GeoIdentifierTag(7-i) );
     m_H62004MovableTablePhysical->add( new GeoTransform(GeoTrf::Translate3D( 0.*Gaudi::Units::cm, 0.*Gaudi::Units::cm, bpc_pos[i-1]-bttb_z) ) );
     m_H62004MovableTablePhysical->add(BPCPhysical);
  }

  // End Moveable FrontBeam detectors


  return m_H62004MovableTablePhysical;
}



