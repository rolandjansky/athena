/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MiddleBeamConstructionH62004.h"
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
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

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

LArGeo::MiddleBeamConstructionH62004::MiddleBeamConstructionH62004()
  :m_H62004MiddleBeamPhysical(0),
   m_detectorManager(0)
{
}


LArGeo::MiddleBeamConstructionH62004::~MiddleBeamConstructionH62004()
{
}



GeoVPhysVol* LArGeo::MiddleBeamConstructionH62004::GetEnvelope()
{

  if (m_H62004MiddleBeamPhysical) return m_H62004MiddleBeamPhysical;

  // Get access to the material manager:
  
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in MiddleBeamConstructionH62004, cannot access MessageSvc");
  }

  MsgStream log(msgSvc, "LArGeo::MiddleBeamConstructionH62004"); 
  log << MSG::INFO << "+    HELLO from LArGeo::MiddleBeamConstructionH62004       +" << endmsg;


  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in MiddleBeamConstructionH62004, cannot access DetectorStore");
  }

  // Get the materials from the material manager:-----------------------------------------------------//
  //                                                                                                  //

  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;

  const GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in MiddleBeamConstructionH62004, std::Air is not found.");
   
   //-------------------------------------------------------------------------------------------------//


  DecodeVersionKey larVersion("LAr");
  std::string detectorKey  = larVersion.tag();
  std::string detectorNode = larVersion.node();


  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////

 
  //  Here we creat the envelope for the Moveable FrontBeam Instrumentation.  This code is repeated
  //  createEnvelope() method below.  There should be a way to avoid this repitition.

  //------ The FrontBeam 

  std::string baseName = "LAr::TBH62004";
  std::string H62004MiddleBeamName = baseName + "::MiddleBeam";

  //
  // Define dimension of Middle part & position of Front part
  //
  double bmtb_x = 12.0*Gaudi::Units::cm;
  double bmtb_y = 12.0*Gaudi::Units::cm;
  double bmtb_z = 25.0*Gaudi::Units::cm;
  //double bmtb_pos = 10.0*Gaudi::Units::cm;
  double bpco_pos[4] =  {1.*Gaudi::Units::cm, 1.*Gaudi::Units::cm, 15.3*Gaudi::Units::cm, 15.3*Gaudi::Units::cm};
  double bpco_shift[4] =  {0.*Gaudi::Units::cm, 7.8*Gaudi::Units::cm, 0.*Gaudi::Units::cm, 7.5*Gaudi::Units::cm};
  double bpc_old_z = (5.100/2)*Gaudi::Units::cm;

  GeoBox* H62004MiddleBeamShape = new GeoBox( bmtb_x, bmtb_y, bmtb_z );   
  const GeoLogVol* H62004MiddleBeamLogical = new GeoLogVol( H62004MiddleBeamName, H62004MiddleBeamShape, Air );

  m_H62004MiddleBeamPhysical = new GeoPhysVol(H62004MiddleBeamLogical);


  //----- Now create BPC
  log << MSG::INFO << " Create BPC 1&2 " << endmsg;

  BPCConstruction *BPC = new BPCConstruction(true);
  GeoVPhysVol* BPCPhysical = BPC->GetEnvelope();
  for(int i=0; i<4; ++i) {
     m_H62004MiddleBeamPhysical->add( new GeoIdentifierTag((3+i/2)*10+i) );
     switch(i) {
	  case 0: case 2: {
			     m_H62004MiddleBeamPhysical->add( new GeoTransform(GeoTrf::Translate3D( 0.*Gaudi::Units::cm, 0.*Gaudi::Units::cm, bpco_pos[i]+bpco_shift[i]+bpc_old_z-bmtb_z) ) );
                            m_H62004MiddleBeamPhysical->add(BPCPhysical);
			    break;}
	  case 1: case 3: { 
                            m_H62004MiddleBeamPhysical->add( new GeoTransform(GeoTrf::RotateZ3D(90.*Gaudi::Units::deg) *  GeoTrf::Translate3D( 0.*Gaudi::Units::cm, 0.*Gaudi::Units::cm, bpco_pos[i]+bpco_shift[i]+bpc_old_z-bmtb_z) ) );
                            m_H62004MiddleBeamPhysical->add(BPCPhysical);
			    break;}
    }
  }

  // End Middle FrontBeam detectors


  return m_H62004MiddleBeamPhysical;
}



