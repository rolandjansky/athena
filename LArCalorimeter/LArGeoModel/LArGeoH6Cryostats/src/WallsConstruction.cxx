/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH6Cryostats/WallsConstruction.h"

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
#include "CxxUtils/make_unique.h" 
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

LArGeo::WallsConstruction::WallsConstruction()
  : m_WallsPhysical(nullptr)
{
}


LArGeo::WallsConstruction::~WallsConstruction()
{
}



GeoVPhysVol* LArGeo::WallsConstruction::GetEnvelope()
{

  if (m_WallsPhysical) return m_WallsPhysical;

  // Get access to the material manager:
  
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);

  if(!status.isFailure()){
    m_msg = CxxUtils::make_unique<MsgStream>(msgSvc, "WallsConstruction");
  } else {
    throw std::runtime_error("WallsConstruction: cannot initialze message service");
  }

  (*m_msg) << MSG::INFO << "WallsConstruction - creating the walls in front of the cryostat " << endmsg;


  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in WallsConstruction, cannot access DetectorStore");
  }


  ServiceHandle<IGeoModelSvc> geoModelSvc ("GeoModelSvc", "WallsConstruction");
  if (geoModelSvc.retrieve().isFailure()) {
    throw std::runtime_error ("Cannot locate GeoModelSvc!!");
  }



  // Get the materials from the material manager:-----------------------------------------------------//
  //                                                                                                  //

  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;

  std::string name;
 
  


  const GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in WallsConstruction, std::Air is not found.");

  const GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in WallsConstruction, std::Iron is not found.");

  const GeoMaterial *Lead  = materialManager->getMaterial("std::Lead");
  if (!Lead) throw std::runtime_error("Error in WallsConstruction, std::Lead is not found.");
   
  const GeoMaterial *Aluminium  = materialManager->getMaterial("std::Aluminium");
  if (!Aluminium) throw std::runtime_error("Error in WallsConstruction, std::Aluminium is not found.");

  // Is this ok for the Scintillator?
  // I don't really know for sure what kind of a scintillator we have.
  // Lots of Scintillators are PMMA (Plexiglas), which has a composition of C5 H8 O2 and density 1.18 g/cm3
  // The Tile uses a composition of C H (density 1.032)    
  // The old Walls testbeam code uses a composition of C9 H10 (density 1.032)
  // ... because it's easiest at the moment and not all that different from the fractional
  // composition of the old tb code, take the Tile material (polysterene)...     
  const GeoMaterial *Scint  = materialManager->getMaterial("std::Polystyrene");
  if (!Scint) throw std::runtime_error("Error in WallsConstruction, std::Polystyrene is not found.");
  
  const GeoMaterial *Mylar  = materialManager->getMaterial("std::Mylar");
  if (!Mylar) throw std::runtime_error("Error in WallsConstruction, std::Mylar is not found.");
  
  
  //                                                                                                 //
  //-------------------------------------------------------------------------------------------------//


  
  std::string AtlasVersion = geoModelSvc->atlasVersion();
  std::string LArVersion = geoModelSvc->LAr_VersionOverride();

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";


  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////

 
  //  Here we creat the envelope for the Moveable Walls Instrumentation.  This code is repeated
  //  createEnvelope() method below.  There should be a way to avoid this repitition.

  //------ The Walls 

  // All these numbers should really go into the database!... 
  // This hard-coding is only temporary!

  std::string baseName = "LAr::TBH6";
  std::string WallsName = baseName + "::Walls";

  const double WallsX   = 1500.*Gaudi::Units::mm;
  const double WallsY   = 2000.*Gaudi::Units::mm;
  const double WallsZ   = 560.5*Gaudi::Units::mm;


  GeoBox* WallsShape = new GeoBox( WallsX, WallsY, WallsZ );   
  const GeoLogVol* WallsLogical = new GeoLogVol( WallsName, WallsShape, Air );

  // This is the Mother Volume for all Walls in front of the cryostat:
  m_WallsPhysical = new GeoPhysVol(WallsLogical);


  // Into the mother, place all indiviual walls (Iron, Lead, Scntillator and additional iron plates)
  // All these walls are never moved with respect to one another, so do them all here.

  //----- First the IronWall

  (*m_msg) << "Create Iron Wall " << endmsg;
  
  const double IronX =  1499.*Gaudi::Units::mm;
  const double IronY =  1999.*Gaudi::Units::mm;
  const double IronZ =  200.0*Gaudi::Units::mm;
  const double IronHoleX =   51.5*Gaudi::Units::mm;
  const double IronHoleY =  1999.*Gaudi::Units::mm;
  const double IronHoleZ =   200.*Gaudi::Units::mm;
  const double IronPosZ  =   270.*Gaudi::Units::mm;

  // The wall itself:
  GeoBox* IronWallShape = new GeoBox(IronX, IronY, IronZ);  
  std::string IronWallName = baseName + "::IronWall";
  GeoLogVol* IronWallLogical = new GeoLogVol( IronWallName, IronWallShape, Iron );
  GeoPhysVol* IronWallPhysical = new GeoPhysVol( IronWallLogical );    

  //Have to put a hole into the wall:
  GeoBox* IronHoleShape = new GeoBox(IronHoleX, IronHoleY, IronHoleZ);  
  std::string IronHoleName = baseName + "::IronWall::Hole";
  GeoLogVol* IronHoleLogical = new GeoLogVol( IronHoleName, IronHoleShape, Air );
  GeoPhysVol* IronHolePhysical = new GeoPhysVol( IronHoleLogical );    
  IronWallPhysical->add(IronHolePhysical);

  // Add the iron wall to the Wall mother:
  m_WallsPhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*Gaudi::Units::mm, 0.*Gaudi::Units::mm, (WallsZ-IronPosZ) ) ) ) ; 
  m_WallsPhysical->add( new GeoNameTag(IronWallName) );
  m_WallsPhysical->add( IronWallPhysical );
  
  //----- Done with IronWall



  //----- Now the LeadWall

  (*m_msg) << "Create Lead Wall " << endmsg;
  
  const double LeadX =  1499.*Gaudi::Units::mm;
  const double LeadY =  1999.*Gaudi::Units::mm;
  const double LeadZ =     6.*Gaudi::Units::mm;
  const double LeadHoleX =  23.5*Gaudi::Units::mm;
  const double LeadHoleY = 1999.*Gaudi::Units::mm; 
  const double LeadHoleZ =    6.*Gaudi::Units::mm;
  const double LeadPosZ  = 1045.*Gaudi::Units::mm;

  // The wall itself:
  GeoBox* LeadWallShape = new GeoBox(LeadX, LeadY, LeadZ);  
  std::string LeadWallName = baseName + "::LeadWall";
  GeoLogVol* LeadWallLogical = new GeoLogVol( LeadWallName, LeadWallShape, Lead );
  GeoPhysVol* LeadWallPhysical = new GeoPhysVol( LeadWallLogical );    

  //Have to put a hole into the wall:
  GeoBox* LeadHoleShape = new GeoBox(LeadHoleX, LeadHoleY, LeadHoleZ);  
  std::string LeadHoleName = baseName + "::LeadWall::Hole";
  GeoLogVol* LeadHoleLogical = new GeoLogVol( LeadHoleName, LeadHoleShape, Air );
  GeoPhysVol* LeadHolePhysical = new GeoPhysVol( LeadHoleLogical );    
  LeadWallPhysical->add(LeadHolePhysical);

  // Add the lead wall to the Wall mother:
  m_WallsPhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*Gaudi::Units::mm, 0.*Gaudi::Units::mm, (WallsZ-LeadPosZ) ) ) ) ; 
  m_WallsPhysical->add( new GeoNameTag(LeadWallName) );
  m_WallsPhysical->add( LeadWallPhysical );
  
  //----- Done with LeadWall


  //----- Now the ScintWall

  (*m_msg) << "Create Scint Wall " << endmsg;
  
  const double ScintX =  1499.*Gaudi::Units::mm;
  const double ScintY =  1999.*Gaudi::Units::mm;
  const double ScintZ =    6.5*Gaudi::Units::mm;
  const double ScintHoleX =  92.5*Gaudi::Units::mm;
  const double ScintHoleY = 1999.*Gaudi::Units::mm; 
  const double ScintHoleZ =   6.5*Gaudi::Units::mm;
  const double ScintPosZ  =  625.*Gaudi::Units::mm;

  // The wall itself:
  GeoBox* ScintWallShape = new GeoBox(ScintX, ScintY, ScintZ);  
  std::string ScintWallName = baseName + "::ScintWall";
  GeoLogVol* ScintWallLogical = new GeoLogVol( ScintWallName, ScintWallShape, Scint );
  GeoPhysVol* ScintWallPhysical = new GeoPhysVol( ScintWallLogical );    

  //Have to put a hole into the wall:
  GeoBox* ScintHoleShape = new GeoBox(ScintHoleX, ScintHoleY, ScintHoleZ);  
  std::string ScintHoleName = baseName + "::ScintWall::Hole";
  GeoLogVol* ScintHoleLogical = new GeoLogVol( ScintHoleName, ScintHoleShape, Air );
  GeoPhysVol* ScintHolePhysical = new GeoPhysVol( ScintHoleLogical );    
  ScintWallPhysical->add(ScintHolePhysical);

  // Add the scintillator wall to the Wall mother:
  m_WallsPhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*Gaudi::Units::mm, 0.*Gaudi::Units::mm, (WallsZ-ScintPosZ) ) ) ) ; 
  m_WallsPhysical->add( new GeoNameTag(ScintWallName) );
  m_WallsPhysical->add( ScintWallPhysical );
  
  //----- Done with ScintWall



  //----- Finally the iron plates

  //(*m_msg) << "Create Iron Plate " << endmsg;
  
  const double IronPlateX =   50.*Gaudi::Units::mm;
  const double IronPlateY =  150.*Gaudi::Units::mm;
  const double IronPlateZ =    4.*Gaudi::Units::mm;
  const double IronPlatePosZ  =  493.*Gaudi::Units::mm;
  const int nPlate = 0 ;
  const int PlatePlace = 1 ; // There were two locations used for these plates - unclear which one when and exactly 
                             //            where they were....! For the moment, sort of copy the standalone code
                             // right now: +1   between Iron wall and Cryostat,
                             //            -1   between Lead wall and table
                             // This should really be passed on via jobOptions!
  
  if (nPlate>0) {
    // Print out a warning here, because extra matrial is in place only for a few special runs
    (*m_msg) << MSG::WARNING << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "  << endmsg;
    (*m_msg) << MSG::WARNING << " EXTRA MATERIAL IN THE BEAM: " << nPlate << " plates "  << endmsg;
    if (PlatePlace== 1) (*m_msg) << MSG::WARNING << " between cryostat and Iron Wall " << endmsg;
    //if (PlatePlace==-1) (*m_msg) << MSG::WARNING << " between Lead Wall and Table " << endmsg;
    (*m_msg) << MSG::WARNING << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "  << endmsg;

    std::vector<double> v_PlateZ;
    for (int iz=0; iz<(nPlate); iz++)  v_PlateZ.push_back(IronPlatePosZ + double(iz)*(2*IronPlateZ+0.5)); 
    
    // The plate itself:
    GeoBox* IronPlateShape = new GeoBox(IronPlateX, IronPlateY, IronPlateZ);  
    std::string IronPlateName = baseName + "::IronPlate";
    GeoLogVol* IronPlateLogical = new GeoLogVol( IronPlateName, IronPlateShape, Iron );
    GeoPhysVol* IronPlatePhysical = new GeoPhysVol( IronPlateLogical );    
    
    
    // Add the iron plate to the Plate mother:
    for (int iz=0; iz<(nPlate); iz++) {
      m_WallsPhysical->add( new GeoIdentifierTag(iz) );
      m_WallsPhysical->add( new GeoTransform( GeoTrf::Translate3D( 0.*Gaudi::Units::mm, 0.*Gaudi::Units::mm, double(PlatePlace)*(v_PlateZ[iz]) ) ) ) ; 
      m_WallsPhysical->add( new GeoNameTag(IronPlateName) );
      m_WallsPhysical->add( IronPlatePhysical );
    }
  }
  //----- Done with Iron Plates

  // Done with all walls in front of the cryostat.


  return m_WallsPhysical;
}



