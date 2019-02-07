/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// BarrelCryostatConstruction

// 28-Nov-2001 WGS: Revise to create cryostat mother volume, and to
// place all components of the barrel within this volume.

#include "LArGeoBarrel/BarrelCryostatConstruction.h"
#include "LArGeoBarrel/BarrelConstruction.h"
#include "LArGeoBarrel/BarrelPresamplerConstruction.h"
#include "LArGeoBarrel/BarrelDMConstruction.h"

#include "GeoModelKernel/GeoElement.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoCons.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/StoredAlignX.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/GeoDBUtils.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

// For transforms:
#include "CLHEP/Geometry/Transform3D.h" 
// For units:
#include "GaudiKernel/PhysicalConstants.h"
// For Transformation Fields:
#include "GeoGenericFunctions/Abs.h" 
#include "GeoGenericFunctions/Mod.h"
#include "GeoGenericFunctions/Rectangular.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/FixedConstant.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"

#include <string>
#include <cmath>
#include <map>
#include <climits>
#include <stdexcept>
using namespace GeoXF;
using namespace GeoGenfun;

// The objects for mapping plane indexes in Pcon to the record index
// in RDBRecordset
typedef std::map<int, unsigned int, std::less<int> > planeIndMap;

LArGeo::BarrelCryostatConstruction::BarrelCryostatConstruction(bool fullGeo):
  m_barrelSagging(0),
  m_barrelVisLimit(-1),
  m_cryoMotherPhysical(NULL),
  m_fullGeo(fullGeo)
{}

LArGeo::BarrelCryostatConstruction::~BarrelCryostatConstruction() {}


GeoFullPhysVol* LArGeo::BarrelCryostatConstruction::GetEnvelope()
{
  if (m_cryoMotherPhysical) return m_cryoMotherPhysical;

 // Get access to the material manager:
  
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in EMBConstruction, cannot access MessageSvc");
  }

  MsgStream log(msgSvc, "LAr::DetectorFactory"); 

  log  << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                   +" << std::endl;
  log << "+   HELLO from LArGeo::BarrelCryostatConstruction   +" << std::endl;
  log << "+                                                   +" << std::endl;
  log << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;


  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in LArDetectorFactory, cannot access DetectorStore");
  }

  // Get the materials from the material manager:-----------------------------------------------------//
  //                                                                                                  //
  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;

  const GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) {
    throw std::runtime_error("Error in BarrelCryostatConstruction, std::Air is not found.");
  }

  const GeoMaterial *Aluminium  = materialManager->getMaterial("std::Aluminium");
  if (!Aluminium) {
    throw std::runtime_error("Error in BarrelCryostatConstruction, std::Aluminium is not found.");
  }

  const GeoMaterial *Titanium  = materialManager->getMaterial("std::Titanium");
  if (!Titanium) {
    Titanium = Aluminium;
    //    No need to throw an error.  Some configurations do not build anything out of titanium.
    //    throw std::runtime_error("Error in BarrelCryostatConstruction, std::Titanium is not found.");
  }

  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) {
    throw std::runtime_error("Error in BarrelCryostatConstruction, std::LiquidArgon is not found.");
  }


  // ----- ----- ------ Get primary numbers from the GeomDB ----- ----- ----- 
  IGeoModelSvc *geoModel;
  IRDBAccessSvc* rdbAccess;
  planeIndMap innerWallPlanes, innerEndWallPlanes, outerWallPlanes, cryoMotherPlanes, totalLarPlanes, halfLarPlanes, sctEcCoolingPlanes;
  planeIndMap::const_iterator iter;
  const IRDBRecord* currentRecord;

  
  if(svcLocator->service ("GeoModelSvc",geoModel) == StatusCode::FAILURE)
    throw std::runtime_error("Error in BarrelCryostatConstruction, cannot access GeoModelSvc");
  if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("Error in BarrelCryostatConstruction, cannot access RDBAccessSvc");

  DecodeVersionKey larVersionKey(geoModel, "LAr");
  log << MSG::DEBUG << "Getting primary numbers for " << larVersionKey.node() << ", " << larVersionKey.tag() << endmsg;

  // ---- Alignable transforms for the barrel:
  // 1. HalfLar + Presampler (pos/neg)
  // 2. Coil
  std::string names[]={"EMB_POS","EMB_NEG","SOLENOID"};
  GeoAlignableTransform *xf[]={NULL,NULL,NULL};
  for (int n=0;n<3;n++) {
    
    IRDBRecordset_ptr larPosition =   rdbAccess->getRecordsetPtr("LArPosition",larVersionKey.tag(), larVersionKey.node());

    if (larPosition->size()==0 ) {
      larPosition = rdbAccess->getRecordsetPtr("LArPosition", "LArPosition-00");
      if (larPosition->size()==0 ) {
	throw std::runtime_error("Error, no lar position table in database!");
      }
    }

    const IRDBRecord *posRec = GeoDBUtils::getTransformRecord(larPosition, names[n]);
    if (!posRec) throw std::runtime_error("Error, no lar position record in the database") ;
    GeoTrf::Transform3D xfPos = GeoDBUtils::getTransform(posRec);
    xf[n] = new GeoAlignableTransform(xfPos);

    StoredAlignX *sAlignX = new StoredAlignX(xf[n]);
    StatusCode status=detStore->record(sAlignX,names[n]);
    if(!status.isSuccess()) throw std::runtime_error (("Cannot store " + names[n]).c_str() );
  }  

  GeoAlignableTransform *xfHalfLArPos=xf[0],  *xfHalfLArNeg=xf[1], *xfSolenoid=xf[2];



  
  IRDBRecordset_ptr cryoEars        = rdbAccess->getRecordsetPtr("CryoEars", larVersionKey.tag(),larVersionKey.node());
  IRDBRecordset_ptr idSupportRails  = rdbAccess->getRecordsetPtr("IdSupportRail", larVersionKey.tag(), larVersionKey.node());
  IRDBRecordset_ptr cryoCylinders   = rdbAccess->getRecordsetPtr("CryoCylinders", 
								 larVersionKey.tag(), 
								 larVersionKey.node());
  if(cryoCylinders->size()==0)
    cryoCylinders = rdbAccess->getRecordsetPtr("CryoCylinders","CryoCylinders-00");


  IRDBRecordset_ptr cryoPcons = rdbAccess->getRecordsetPtr("CryoPcons", 
							   larVersionKey.tag(), 
							   larVersionKey.node());
  if(cryoPcons->size()==0)
    cryoPcons = rdbAccess->getRecordsetPtr("CryoPcons","CryoPcons-00");


  for (unsigned int ind=0; ind<cryoPcons->size(); ind++)
  {
    int key = (*cryoPcons)[ind]->getInt("PLANE_ID");
    std::string pconName = (*cryoPcons)[ind]->getString("PCON");
    if(pconName=="Barrel::InnerWall")
      innerWallPlanes[key] = ind;
    if (pconName=="Barrel::InnerEndWall") 
      innerEndWallPlanes[key] = ind;
    else if(pconName=="Barrel::OuterWall")
      outerWallPlanes[key] = ind;
    else if(pconName=="Barrel::CryoMother")
      cryoMotherPlanes[key] = ind;
    else if(pconName=="Barrel::TotalLAr")
      totalLarPlanes[key] = ind;
    else if(pconName=="Barrel::HalfLAr")
      halfLarPlanes[key] = ind;
    else if(pconName=="Barrel::SctEcCooling")
      sctEcCoolingPlanes[key] = ind;
  }
  // ----- ----- ------ Get primary numbers from the GeomDB ----- ----- ----- 

  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////

  // 1) There is mother volume.
  // 2)    There are ~ 60 cylindrical layers in the mother
  //       Also ~ 12 conical layers in the mother.
  //       Also the liquid argon volume.
  // 3)       There are 3 half barrels in the liquid argon volume.

  // 1) The mother volume. ---------------------------------------------------------------------------//
  //                                                                                                  //
  std::string cryoMotherName = "LAr::Barrel::Cryostat::MotherVolume";                                 //

  // Define the mother volume for the entire barrel cryostat.
  // Everything else in the barrel (cryostat walls, detector,
  // presampler) should be placed inside here.

  // The size of this volume may change if the thickness of the
  // cabling in front of the endcaps changes.  Therefore, we must get
  // the z-shift from the LAr information database
  // (LArVDetectorParameters) and adjust the volume geometry
  // accordingly.

  //  double cryoMotherRin[]   = {1149.8*Gaudi::Units::mm, 1149.8*Gaudi::Units::mm,1149.8*Gaudi::Units::mm,1149.8*Gaudi::Units::mm,1149.8*Gaudi::Units::mm,1149.8*Gaudi::Units::mm};
  //  double cryoMotherRout[]  = {2890. *Gaudi::Units::mm, 2890. *Gaudi::Units::mm,2250. *Gaudi::Units::mm,2250. *Gaudi::Units::mm,2890. *Gaudi::Units::mm,2890. *Gaudi::Units::mm};  
  //  double cryoMotherZplan[] = {-3490.*Gaudi::Units::mm,-2850.*Gaudi::Units::mm,-2849.*Gaudi::Units::mm, 2849.*Gaudi::Units::mm, 2850.*Gaudi::Units::mm, 3490.*Gaudi::Units::mm};

  // Access source of detector parameters.
  //  VDetectorParameters* parameters = VDetectorParameters::GetInstance();

  // Get the z-Shift from the detector parameters routine.
  //  double zShift = parameters->GetValue("LArEMECZshift");

  // Adjust mother volume size.
  //  int lastPlaneCryo = ( sizeof(cryoMotherZplan) / sizeof(double) );
  //  cryoMotherZplan[lastPlaneCryo-1] += zShift;
  //  cryoMotherZplan[0] -= zShift;

  double dphi_all = 2.*M_PI;

  GeoPcon* cryoMotherShape =
    new GeoPcon(0.,                     // starting phi
		dphi_all );              // total phi

  for(unsigned int ind=0; ind<cryoMotherPlanes.size(); ind++)
  {
    iter = cryoMotherPlanes.find(ind);

    if(iter==cryoMotherPlanes.end())
      throw std::runtime_error("Error in BarrelCryostatConstruction, missing plane in CryoMother");
    else
    {
      currentRecord = (*cryoPcons)[(*iter).second];

      cryoMotherShape->addPlane(currentRecord->getDouble("ZPLANE"),
				currentRecord->getDouble("RMIN"),
				currentRecord->getDouble("RMAX"));
    }
  }

  //  for ( int i = 0; i != lastPlaneCryo; ++i )
  //    {
  //      cryoMotherShape->addPlane(
  //				cryoMotherZplan[i],        // position of z planes
  //				cryoMotherRin[i],          // tangent distance to inner surface
  //				cryoMotherRout[i]);        // tangent distance to outer surface
  // }

  const GeoLogVol* cryoMotherLogical =
    new GeoLogVol(cryoMotherName, cryoMotherShape, Air);

  m_cryoMotherPhysical = new GeoFullPhysVol(cryoMotherLogical);                         //
  //                                                                                                  //
  //--------------------------------------------------------------------------------------------------//

  //--------------------------------------------------------------------------------------------------//
  // Cylindrical layers for the cryo mother:                                                          //
  //
  for (unsigned int ind=0; ind < cryoCylinders->size(); ind++) 
  {
    currentRecord = (*cryoCylinders)[ind];

    if(currentRecord->getString("CYL_LOCATION")=="Barrel::CryoMother")
    {

      const GeoMaterial *material  = materialManager->getMaterial(currentRecord->getString("MATERIAL"));

      if (!material) 
      {
	std::ostringstream errorMessage;
	errorMessage << "Error in BarrelCrysostat Construction" << std::endl;
	errorMessage << "Material " << currentRecord->getString("MATERIAL") << " is not found" << std::endl;
	throw std::runtime_error(errorMessage.str().c_str());
      }

      
      std::ostringstream cylStream;
      int cylID = currentRecord->getInt("CYL_ID");
      cylStream << "LAr::Barrel::Cryostat::Cylinder::#" << cylID;
      std::string cylName= cylStream.str();

      int cylNumber = currentRecord->getInt("CYL_NUMBER");
      double zMin = currentRecord->getDouble("ZMIN")*Gaudi::Units::cm;
      double dZ   = currentRecord->getDouble("DZ")*Gaudi::Units::cm;
      double zInCryostat = zMin + dZ / 2.;
      
      if(m_fullGeo){
	if ((*cryoEars).size()>0 && cylID==1) {
	  const IRDBRecord * record = (*cryoEars)[0];
	  double rmin   =  record->getDouble("EARRMIN");
	  double rmax   =  record->getDouble("EARRMAX");
	  double zthick =  record->getDouble("EARZTHICKNESS");
	  double yvert  =  record->getDouble("EARVERTICALEXTENT");
	  GeoTubs *tubs =  new GeoTubs(rmin,
				       rmax,
				       zthick/ 2.,
				       (double) 0.,
				       dphi_all);
	  
	  GeoBox *box = new GeoBox( rmax, yvert/2, rmax);
	  
	  const GeoShape & shape = tubs->intersect(*box);
	  const GeoLogVol *logVol = new GeoLogVol ("LAr::Barrel::Cryostat::Sector::Ear",&shape, material);
	  GeoPhysVol *earPhysVol = new GeoPhysVol(logVol);

	  m_cryoMotherPhysical->add(new GeoNameTag(std::string("CryostatEarForward")));
	  m_cryoMotherPhysical->add(new GeoIdentifierTag(cylNumber));
	  m_cryoMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zInCryostat)));
	  m_cryoMotherPhysical->add(earPhysVol);

	  m_cryoMotherPhysical->add(new GeoNameTag(cylName+std::string("CryostatEarBackward")));
	  m_cryoMotherPhysical->add(new GeoIdentifierTag(cylNumber));
	  m_cryoMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(-zInCryostat)));
	  m_cryoMotherPhysical->add(earPhysVol);                
	}
      
	if ((*cryoEars).size() > 0  && cylID==6) {
	  const IRDBRecord * record = (*cryoEars)[0];
	  double rmin   =  record->getDouble("LEGRMIN");
	  double rmax   =  record->getDouble("LEGRMAX");
	  double zthick =  record->getDouble("LEGZTHICKNESS");
	  double yvert  =  record->getDouble("LEGYMAX");
	  double angle  =  record->getDouble("LEGANGLE");

	  GeoTubs *tubs =  new GeoTubs(rmin,
				       rmax,
				       zthick/ 2.,
				       -angle*(M_PI/180.0),
				       M_PI  + 2*angle*(M_PI/180));

	  GeoTrf::TranslateY3D  offset(rmax-yvert);
	  GeoBox * box =  new GeoBox(rmax,rmax, rmax);
	  const GeoShape & shape = tubs->subtract((*box)<<offset);
	  
	  const GeoLogVol *logVol = new GeoLogVol ("LAr::Barrel::Cryostat::Sector::Leg",&shape, material);
	  GeoPhysVol *legPhysVol = new GeoPhysVol(logVol);
	
	  m_cryoMotherPhysical->add(new GeoNameTag(std::string("CryostatLegForward")));
	  m_cryoMotherPhysical->add(new GeoIdentifierTag(cylNumber));
	  m_cryoMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zInCryostat+zthick)));
	  m_cryoMotherPhysical->add(legPhysVol);
	  
	  m_cryoMotherPhysical->add(new GeoNameTag(cylName+std::string("CryostatLegBackward")));
	  m_cryoMotherPhysical->add(new GeoIdentifierTag(cylNumber));
	  m_cryoMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(-zInCryostat-zthick)));
	  m_cryoMotherPhysical->add(legPhysVol);                
	}
      }

      GeoTubs* solidBarrelCylinder = 0;
      const GeoLogVol* logicBarrelCylinder = 0;
    
      // For Reco Geometry construct only solenoid cylinders
      if(m_fullGeo || (10<=cylID && cylID<=14)) {
	solidBarrelCylinder
	  = new GeoTubs(currentRecord->getDouble("RMIN")*Gaudi::Units::cm,
			currentRecord->getDouble("RMIN")*Gaudi::Units::cm + currentRecord->getDouble("DR")*Gaudi::Units::cm,
			currentRecord->getDouble("DZ")*Gaudi::Units::cm / 2.,
			(double) 0.,
			dphi_all);

	logicBarrelCylinder
	  = new GeoLogVol(cylName,solidBarrelCylinder,material);      
      }

      if(logicBarrelCylinder) {
	// If ZMIN < 0 place the cylinder onse
	// If ZMIN >=0 place each cylinder twice, at +z and -z.
	if(zMin<0) {
	  GeoFullPhysVol* physBarrelCylinder = new GeoFullPhysVol(logicBarrelCylinder);

	  m_cryoMotherPhysical->add(new GeoNameTag(cylName+std::string("Phys")));
	  m_cryoMotherPhysical->add(new GeoIdentifierTag(cylNumber));

	  // put alignamble transform for cryostat cylinders 10<=cylID<=14
	  if(10<=cylID && cylID<=14)
	    m_cryoMotherPhysical->add(xfSolenoid);

	  m_cryoMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zInCryostat)));
	  m_cryoMotherPhysical->add(physBarrelCylinder);

	  
	  // NOTE: there are 5 volumes for solenoid with NO common direct parent
	  // I'm storing the first one #10. (FIXME) 
	  // NOTE:  Pretty soon there will
	  // be no need at all to store physical volumes in the manager or in storegate!
	  if(cylID==10) {
	    StoredPhysVol *sPhysVol = new StoredPhysVol(physBarrelCylinder);
	    StatusCode status=detStore->record(sPhysVol,"SOLENOID");
	    if(!status.isSuccess()) throw std::runtime_error ("Cannot store SOLENOID");
	  }
	}else{
	  GeoPhysVol* physBarrelCylinder = new GeoPhysVol(logicBarrelCylinder);
	  
	  m_cryoMotherPhysical->add(new GeoNameTag(cylName+std::string("PhysForward")));
	  m_cryoMotherPhysical->add(new GeoIdentifierTag(cylNumber));
	  m_cryoMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zInCryostat)));
	  m_cryoMotherPhysical->add(physBarrelCylinder);
	  
	  m_cryoMotherPhysical->add(new GeoNameTag(cylName+std::string("PhysBackward")));
	  m_cryoMotherPhysical->add(new GeoIdentifierTag(cylNumber));
	  m_cryoMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(-zInCryostat)));
	  m_cryoMotherPhysical->add(physBarrelCylinder);                
	}           
      }                           
    }
  }                                                                                                   //
  //--------------------------------------------------------------------------------------------------//

  if(m_fullGeo) {
    // Make a Polycon for the outer wall:
    GeoPcon *outerWallPcon = new GeoPcon(0,dphi_all);
    
    for(unsigned int ind=0; ind<outerWallPlanes.size(); ind++) {
      iter = outerWallPlanes.find(ind);
      
      if(iter==outerWallPlanes.end())
	throw std::runtime_error("Error in BarrelCryostatConstruction, missing plane in OuterWall");
      else {
	currentRecord = (*cryoPcons)[(*iter).second];
	outerWallPcon->addPlane(currentRecord->getDouble("ZPLANE"),
				currentRecord->getDouble("RMIN"),
				currentRecord->getDouble("RMAX"));
      }
    }

    const GeoLogVol *outerWallLog = new GeoLogVol("LAr::Barrel::Cryostat::OuterWall", outerWallPcon, Aluminium);
    m_cryoMotherPhysical->add(new GeoNameTag(std::string("Barrel Cryo OuterWall Phys")));
    GeoPhysVol *outerWallPhys = new GeoPhysVol(outerWallLog);
    m_cryoMotherPhysical->add(outerWallPhys);
  
    // Make a Polycon for the inner wall:
    GeoPcon *innerWallPcon = new GeoPcon(0,dphi_all);

    for(unsigned int ind=0; ind<innerWallPlanes.size(); ind++) {
      iter = innerWallPlanes.find(ind);
      
      if(iter==innerWallPlanes.end())
	throw std::runtime_error("Error in BarrelCryostatConstruction, missing plane in InnerWall");
      else {
	currentRecord = (*cryoPcons)[(*iter).second];
	innerWallPcon->addPlane(currentRecord->getDouble("ZPLANE"),
				currentRecord->getDouble("RMIN"),
				currentRecord->getDouble("RMAX"));
      }
    }

    const GeoLogVol *innerWallLog = new GeoLogVol("LAr::Barrel::Cryostat::InnerWall", innerWallPcon, Aluminium);
    m_cryoMotherPhysical->add(new GeoNameTag(std::string("Barrel Cryo InnerWall Phys")));
    GeoPhysVol *innerWallPhys = new GeoPhysVol(innerWallLog);
    m_cryoMotherPhysical->add(innerWallPhys);

    // Vis a la fin du cryostat

    IRDBRecordset_ptr cryoBolts = rdbAccess->getRecordsetPtr("LArBarrelCryoBolts",
                                                              larVersionKey.tag(),
                                                              larVersionKey.node());
    if (cryoBolts->size() >0) {
        log << MSG::INFO << " new description with barrel croystat bolts" << endmsg;
        const IRDBRecord * cryoBoltsRecord = (*cryoBolts) [0];
        double rmax_vis = cryoBoltsRecord->getDouble("RBOLT");
        int    Nvis     = cryoBoltsRecord->getInt("NBOLT");
        double PhiPos0  = cryoBoltsRecord->getDouble("PHI0");
        double RhoPosB  = cryoBoltsRecord->getDouble("RADIUS");
        int index1      = cryoBoltsRecord->getInt("INDEXWALL1");
        int index2      = cryoBoltsRecord->getInt("INDEXWALL2");
        const GeoMaterial *bolt_material  = materialManager->getMaterial(cryoBoltsRecord->getString("MATERIAL"));
        if (!bolt_material) {
          throw std::runtime_error("Error in BarrelCryostatConstruction, material for bolt not found");
        }


        double z1=-1.;
        double z2=-1.;
        if ((iter = innerWallPlanes.find(index1))!= innerWallPlanes.end()) {
          const IRDBRecord * pconsRecord = (*cryoPcons)  [(*iter).second];
          z1           =  pconsRecord->getDouble("ZPLANE");
        }
        if ((iter = innerWallPlanes.find(index2))!= innerWallPlanes.end()) {
          const IRDBRecord * pconsRecord = (*cryoPcons)  [(*iter).second];
          z2           =  pconsRecord->getDouble("ZPLANE");
        }
        if (z1>0. && z2>0.) {
          double zthick_vis=(z2-z1)-0.1;

          GeoTubs *tub_vis =  new GeoTubs(0.,rmax_vis,zthick_vis/2., (double) 0., dphi_all);
          const GeoLogVol * log_vis = new GeoLogVol("LAr::Barrel::Cryostat::InnerWall::Vis",tub_vis,bolt_material);
          GeoPhysVol* phys_vis = new GeoPhysVol(log_vis);

          double xxVis=((double)(Nvis));
          double ZposB=0.5*(z1+z2);
          double twopi128 = 2.*M_PI/xxVis;
          GeoGenfun::Variable      i;                                                        
          GeoGenfun::Mod Mod1(1.0),Mod128(xxVis),Mod2(2.0);                                  
          GeoGenfun::GENFUNCTION  PhiPos = PhiPos0 + twopi128*Mod128(i);                
          GeoGenfun::GENFUNCTION Int = i - Mod1;
          GeoGenfun::Cos  Cos;
          GeoGenfun::Sin  Sin;
          GeoXF::TRANSFUNCTION TX =                                                       
            GeoXF::Pow(GeoTrf::TranslateX3D(1.0),RhoPosB*Cos(PhiPos))*                         
            GeoXF::Pow(GeoTrf::TranslateY3D(1.0),RhoPosB*Sin(PhiPos))*                         
            GeoXF::Pow(GeoTrf::TranslateZ3D(2*ZposB),Int(i/128))*
            GeoTrf::TranslateZ3D(-ZposB);                                         
          GeoSerialTransformer *st = new GeoSerialTransformer(phys_vis, &TX, 2*Nvis);
          innerWallPhys->add(st);
        }
     }  //  bolts found in the geometry database
     else {
        log << MSG::INFO << " old description withut bold in the geometry database " << endmsg;
     }

    
    // extra Cone for systematics in upstream matter
    IRDBRecordset_ptr extraCones = rdbAccess->getRecordsetPtr("LArCones", 
							      larVersionKey.tag(), 
							      larVersionKey.node());
    if (extraCones->size() > 0 ) {
      int nextra=0;
      for (unsigned int i=0; i<extraCones->size(); i++) {
	std::string conName = (*extraCones)[i]->getString("CONE");
	//      std::cout << " cone name " << conName << std::endl;
	if (conName.find("ExtraInCryo") != std::string::npos) {
	  nextra++;
	  double extra_dz = 0.5*( (*extraCones)[i]->getDouble("DZ") );
	  double extra_rmin1 = (*extraCones)[i]->getDouble("RMIN1");
	  double extra_rmin2 = (*extraCones)[i]->getDouble("RMIN2");
	  double extra_rmax1 = (*extraCones)[i]->getDouble("RMAX1");
	  double extra_rmax2 = (*extraCones)[i]->getDouble("RMAX2");
	  double extra_phi0  = (*extraCones)[i]->getDouble("PHI0");
	  double extra_dphi  = (*extraCones)[i]->getDouble("DPHI");
	  double extra_zpos  = (*extraCones)[i]->getDouble("ZPOS");
	  // GU 20-feb-06
	  // if extra_dphi is close to 2pi (6.283185) it seems safer for G4 navigation
	  // to impose exactly same dphi as mother volume instead of a sligthly smaller dphi
	  if (extra_dphi>6.2831) extra_dphi = dphi_all;
	  //       std::cout << " rmin1,rmin2,rmax1,rmax2,dz,phi0,dphi,zpos " << extra_rmin1 << " " << extra_rmin2 << " "
	  //                 << extra_rmax1 << " " << extra_rmax2 << " " << extra_dz << " " << extra_phi0 << " "
	  //                 << extra_dphi << " " << extra_zpos << std::endl;
	  GeoCons* extraCons
	    = new GeoCons(extra_rmin1,extra_rmin2,extra_rmax1,extra_rmax2,extra_dz,extra_phi0,extra_dphi);
	  
	  std::ostringstream extraStream;
	  extraStream << "LAr::Barrel::Cryostat::ExtraMat" << nextra;
	  std::string extraName= extraStream.str();
	  //      std::cout << " extraName " << extraName << std::endl;
	  
	  GeoLogVol* extraLog = new GeoLogVol(extraName,extraCons,Aluminium);
	  GeoPhysVol* extraPhys = new GeoPhysVol(extraLog);
	  m_cryoMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(extra_zpos)));
	  m_cryoMotherPhysical->add(extraPhys); 
	}
      }
    }

    // Throw in coil support block
    {
      const GeoMaterial* myMaterial;
      double length,height,width,pairSeparation,distFromRidge;
      int nPairTot,indexWall;
      IRDBRecordset_ptr newBlocks        = rdbAccess->getRecordsetPtr("LArBarBumperBlocks", larVersionKey.tag(),larVersionKey.node());
      if (newBlocks->size() >0 ) {
            log << MSG::INFO << " new coil bumper description " << endmsg;
            const IRDBRecord * newBlocksRecord = (*newBlocks) [0];
            length         =   newBlocksRecord->getDouble("LENGTH");     // deltaX
            height         =   newBlocksRecord->getDouble("HEIGHT");     // delta Y
            width          =   newBlocksRecord->getDouble("WIDTH");      // lenght in Z
            pairSeparation =   newBlocksRecord->getDouble("PAIRSEP");
            distFromRidge  =   newBlocksRecord->getDouble("DISTANCEFROMRIDGE");
            nPairTot       =   newBlocksRecord->getInt("NPAIRTOT");
            indexWall      =   newBlocksRecord->getInt("INDEXWALL");
            myMaterial  = materialManager->getMaterial(newBlocksRecord->getString("MATERIAL"));
            if (!myMaterial) {
              throw std::runtime_error("Error in BarrelCryostatConstruction, material for coil bumpber not found");
            }
      }
      else {
         log << MSG::INFO << " old coil bumper description " << endmsg;
        IRDBRecordset_ptr tiBlocks        = rdbAccess->getRecordsetPtr("TiBlocks", larVersionKey.tag(),larVersionKey.node());
        const IRDBRecord * tiBlocksRecord = (*tiBlocks)  [0];
        length         =   tiBlocksRecord->getDouble("LENGTH");    // delta X
        height         =   tiBlocksRecord->getDouble("HEIGHT");     // delta Y
        width          =   tiBlocksRecord->getDouble("WIDTH");     // lenght in Z
        pairSeparation =   tiBlocksRecord->getDouble("PAIRSEP");
        distFromRidge  =   tiBlocksRecord->getDouble("DISTANCEFROMRIDGE");
        nPairTot       =   tiBlocksRecord->getInt("NPAIRTOT");
        indexWall=8;
        myMaterial=Titanium;
      }

      double r=-1.;
      double z=-1.;
      if ((iter = innerWallPlanes.find(indexWall))!= innerWallPlanes.end()) {
        const IRDBRecord * pconsRecord = (*cryoPcons)  [(*iter).second];
        r              = pconsRecord->getDouble("RMAX");
        z              = pconsRecord->getDouble("ZPLANE");
      }

      if (r>0.) {

      
      // Make one block:
        GeoBox *box = new GeoBox(length/2.0, height/2.0, width/2.0);
        GeoLogVol *logVol = new GeoLogVol("LAr::Barrel::Cryostat::Sector::TitaniumBlock", box,myMaterial);
        GeoPhysVol *physVol = new GeoPhysVol(logVol);
      
        double angle=pairSeparation/r;
        double pos  = -z+width/2 + distFromRidge;
// position of bumpers in new description
        if  (newBlocks->size() >0) {
          GeoGenfun::Variable      i;                                                        //
          GeoGenfun::Mod Mod1(1.0),Mod2(2.0);                                                //
          GeoGenfun::GENFUNCTION Truncate = i - Mod1(i);                                     //
          GeoGenfun::GENFUNCTION AngleZ = -angle/2.+angle*Truncate(Mod2(i/2))+ 2.*M_PI/(1.0*nPairTot)*Truncate(i/4) + 2*M_PI/(2.*nPairTot);
          GeoGenfun::GENFUNCTION TransZ = -pos + 2.*pos*Mod2(i);
      
          TRANSFUNCTION tx = 
            GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),TransZ)*
            GeoXF::Pow(GeoTrf::RotateZ3D(1.0),AngleZ)*
            GeoTrf::Translate3D(0.,r+height/2,0.);
          GeoSerialTransformer *t  = new GeoSerialTransformer(physVol, &tx, nPairTot*4);
          m_cryoMotherPhysical->add(t);
        }
// position of bumper in old description
        else {
          GeoGenfun::Variable      i;                                                        //
          GeoGenfun::Mod Mod1(1.0),Mod2(2.0);                                                //
          GeoGenfun::GENFUNCTION Truncate = i - Mod1(i);                                     //
      
          TRANSFUNCTION tx = 
           Pow(GeoTrf::RotateZ3D(2*M_PI/nPairTot),Truncate(i/4))*
           Pow(GeoTrf::RotateZ3D(angle),Mod2(i/2))*
           GeoTrf::RotateZ3D(-angle/2)*
           Pow(GeoTrf::TranslateZ3D(2*pos),Mod2(i))*
           GeoTrf::Translate3D(0,r+height/2, -pos);
          GeoSerialTransformer *t  = new GeoSerialTransformer(physVol, &tx, nPairTot*4);
          m_cryoMotherPhysical->add(t);
        }

     }   // r>0.
     else {
       log << MSG::WARNING << " could not find wall index plane => no coil bumper description " << endmsg;
     }

    }  // end of coil supports

    
    // Make a Polycon for the inner endwall:
    if(innerEndWallPlanes.size() > 0) {
      GeoPcon *innerEndWallPcon = new GeoPcon(0,dphi_all);

      for(unsigned int ind=0; ind<innerEndWallPlanes.size(); ind++) {
	iter = innerEndWallPlanes.find(ind);
      
	if(iter==innerEndWallPlanes.end())
	  throw std::runtime_error("Error in BarrelCryostatConstruction, missing plane in InnerEndWall");
	else {
	  currentRecord = (*cryoPcons)[(*iter).second];
	  innerEndWallPcon->addPlane(currentRecord->getDouble("ZPLANE"),
				     currentRecord->getDouble("RMIN"),
				     currentRecord->getDouble("RMAX"));
	}
      }

      const GeoLogVol *innerEndWallLog = new GeoLogVol("LAr::Barrel::Cryostat::InnerEndWall", innerEndWallPcon, Aluminium);
      m_cryoMotherPhysical->add(new GeoNameTag(std::string("Barrel Cryo InnerEndWall Phys")));
      GeoPhysVol *innerEndWallPhys = new GeoPhysVol(innerEndWallLog);
      m_cryoMotherPhysical->add(innerEndWallPhys);
      m_cryoMotherPhysical->add(new GeoTransform(GeoTrf::RotateY3D(M_PI)));
      m_cryoMotherPhysical->add(innerEndWallPhys);
    }

    //------------------------------------------------------------------------------------------------//
    /*if ((*idSupportRails).size()>0 && railrec->size() >0) {
     }  this part of code is moved into SupportRailFactory.cxx (InDetServMatGeoModel package) --- Adam Agocs             */
  } // if(m_fullGeo)
  //--------------------------------------------------------------------------------------------------//

  //--------------------------------------------------------------------------------------------------//
  // The total liquid argon volume inside the cryostat.  This will be
  // sub-divided into sensitive-detector regions in the detector
  // routine.

  //  double totalLArRin[]   = { 1565.5*Gaudi::Units::mm, 1385.*Gaudi::Units::mm, 1385.*Gaudi::Units::mm, 1565.5*Gaudi::Units::mm };
  //  double totalLArRout[]  = { 2140. *Gaudi::Units::mm, 2140.*Gaudi::Units::mm, 2140.*Gaudi::Units::mm, 2140. *Gaudi::Units::mm };  
  //  double totalLArZplan[] = {-3267. *Gaudi::Units::mm,-3101.*Gaudi::Units::mm, 3101.*Gaudi::Units::mm, 3267. *Gaudi::Units::mm };
    
  GeoPcon* totalLArShape =
    new GeoPcon(0.,                     // starting phi
		dphi_all);               // total phi

  for(unsigned int ind=0; ind<totalLarPlanes.size(); ind++)
  {
    iter = totalLarPlanes.find(ind);

    if(iter==totalLarPlanes.end())
      throw std::runtime_error("Error in BarrelCryostatConstruction, missing plane in CryoMother");
    else
    {
      currentRecord = (*cryoPcons)[(*iter).second];
      totalLArShape->addPlane(currentRecord->getDouble("ZPLANE"),
			      currentRecord->getDouble("RMIN"),
			      currentRecord->getDouble("RMAX"));
    }
  }

  std::string totalLArName = "LAr::Barrel::Cryostat::TotalLAr";
  const GeoLogVol* totalLArLogical = new GeoLogVol(totalLArName, totalLArShape, LAr);
    
    // When you place a polycone inside another polycone, the z=0
    // co-ordinates will be the same.  (Let's hope this is still true
    // in GeoModel.)
  m_cryoMotherPhysical->add(new GeoNameTag("Total LAR Volume"));
  GeoPhysVol* totalLArPhysical = new GeoPhysVol(totalLArLogical);
  m_cryoMotherPhysical->add(totalLArPhysical);
  //                                                                                                //    

  // 19-Feb-2003 ML: mother volumes for halfBarrels
    
  // To allow for mis-alignments, provide a 3mm gap at low z.  (This
  // is just a first pass; we may want to introduce other adjustments
  // to this shape to allow for mis-alignments in other dimensions.)

  // increase internal radius to allow misalignments
  // -----------------------------------------------  double rInShift = 0.*Gaudi::Units::mm;

  //  double halfLArZplan[] = { 3.0 *Gaudi::Units::mm, 3101.*Gaudi::Units::mm, 3267. *Gaudi::Units::mm };
  //  double halfLArRin[]   = {1385.*Gaudi::Units::mm + rInShift, 1385.*Gaudi::Units::mm + rInShift, 1565.5*Gaudi::Units::mm  + rInShift};
  //  double halfLArRout[]  = {2140.*Gaudi::Units::mm, 2140.*Gaudi::Units::mm, 2140. *Gaudi::Units::mm };  
    
  std::string halfLArName = "LAr::Barrel::Cryostat::HalfLAr";
  GeoPcon* halfLArShape =
    new GeoPcon(
		0.,                    // starting phi
		dphi_all               // total phi
		);

  for(unsigned int ind=0; ind<halfLarPlanes.size(); ind++)
  {
    iter = halfLarPlanes.find(ind);

    if(iter==halfLarPlanes.end())
      throw std::runtime_error("Error in BarrelCryostatConstruction, missing plane in CryoMother");
    else
    {
      currentRecord = (*cryoPcons)[(*iter).second];
      halfLArShape->addPlane(currentRecord->getDouble("ZPLANE"),
			     currentRecord->getDouble("RMIN"),
			     currentRecord->getDouble("RMAX"));
    }
  }
    
  // Define logical volumes for both halves of the barrel.
  const GeoLogVol* halfLArLogicalPos =
    new GeoLogVol(halfLArName + "::Pos", halfLArShape, LAr);
  GeoPhysVol* halfLArPhysicalPos = new GeoPhysVol(halfLArLogicalPos);
  
  const GeoLogVol* halfLArLogicalNeg =
    new GeoLogVol(halfLArName + "::Neg", halfLArShape, LAr);
  GeoPhysVol* halfLArPhysicalNeg = new GeoPhysVol(halfLArLogicalNeg);
  
  totalLArPhysical->add(new GeoNameTag(halfLArName + "::PosPhysical"));

  // add alignable transform
  totalLArPhysical->add(xfHalfLArPos);
  totalLArPhysical->add(halfLArPhysicalPos);
  
  totalLArPhysical->add(new GeoNameTag(halfLArName + "::NegPhysical"));

  // add alignable transform
  totalLArPhysical->add(xfHalfLArNeg);
  totalLArPhysical->add( new GeoTransform(GeoTrf::RotateY3D(180.*Gaudi::Units::deg)) );
  totalLArPhysical->add(halfLArPhysicalNeg);
  
  {

    // 27-Nov-2001 WGS: Place the barrel LAr and detector inside the cryostat.
    // There are two placements: one for the z>0 section, one for the z<0 section.
    
    
    BarrelConstruction barrelConstruction(m_fullGeo);
    barrelConstruction.setBarrelSagging(m_barrelSagging);
    barrelConstruction.setBarrelCellVisLimit(m_barrelVisLimit);
    

    // The "envelope" determined by the EMB should be a GeoFullPhysVol.
    GeoFullPhysVol* barrelPosEnvelope = barrelConstruction.GetPositiveEnvelope();
    if ( barrelPosEnvelope != 0 )
      halfLArPhysicalPos->add(barrelPosEnvelope);

    // The "envelope" determined by the EMB should be a GeoFullPhysVol.
    GeoFullPhysVol* barrelNegEnvelope = barrelConstruction.GetNegativeEnvelope();
    if ( barrelNegEnvelope != 0 )
      halfLArPhysicalNeg->add(barrelNegEnvelope);

    if(m_fullGeo) {
      //--------------------------------------------------------------------------------------------------//
      // Cylindrical layers for half LAr
      //
      for (unsigned int ind=0; ind < cryoCylinders->size(); ind++) {
	currentRecord = (*cryoCylinders)[ind];
	
	if(currentRecord->getString("CYL_LOCATION")=="Barrel::HalfLAr"||
	   currentRecord->getString("CYL_LOCATION")=="Barrel::TotalLAr") {
	  bool isHalfLar = currentRecord->getString("CYL_LOCATION")=="Barrel::HalfLAr";

	  const GeoMaterial *material  = materialManager->getMaterial(currentRecord->getString("MATERIAL"));
	
	  if (!material) {
	    std::ostringstream errorMessage;
	    errorMessage << "Error in BarrelCrysostat Construction" << std::endl;
	    errorMessage << "Material " << currentRecord->getString("MATERIAL") << " is not found" << std::endl;
	    throw std::runtime_error(errorMessage.str().c_str());
	  }

	  std::ostringstream cylStream;
	  cylStream << "LAr::Barrel::Cryostat::";
	
	  if (!currentRecord->isFieldNull("QUALIFIER")) {
	    std::string qualifier = currentRecord->getString("QUALIFIER");
	    if (qualifier.size()) cylStream << qualifier << "::";
	  }
	  cylStream <<  "Cylinder::#" << currentRecord->getInt("CYL_ID");
	  std::string cylName= cylStream.str(); 

	  GeoTubs* solidBarrelCylinder 
	  = new GeoTubs(currentRecord->getDouble("RMIN")*Gaudi::Units::cm,
			currentRecord->getDouble("RMIN")*Gaudi::Units::cm + currentRecord->getDouble("DR")*Gaudi::Units::cm,
			currentRecord->getDouble("DZ")*Gaudi::Units::cm / 2.,
			(double) 0.,
			dphi_all);
    
	  const GeoLogVol* logicBarrelCylinder
	    = new GeoLogVol(cylName,solidBarrelCylinder,material);
	  
	  GeoPhysVol* physBarrelCylinder = new GeoPhysVol(logicBarrelCylinder);
	  
	  double zInCryostat = currentRecord->getDouble("ZMIN")*Gaudi::Units::cm + currentRecord->getDouble("DZ")*Gaudi::Units::cm / 2.;
	  
	  int cylNumber = currentRecord->getInt("CYL_NUMBER");
	  
	  
	  if(isHalfLar) {
	    // Place each cylinder twice, once in each half-barrel:
	    halfLArPhysicalPos->add(new GeoNameTag(cylName+std::string("PhysForward")));
	    halfLArPhysicalPos->add(new GeoIdentifierTag(cylNumber));
	    //	  halfLArPhysicalPos->add(xfPos);
	    halfLArPhysicalPos->add(new GeoTransform(GeoTrf::TranslateZ3D(zInCryostat)));
	    halfLArPhysicalPos->add(physBarrelCylinder);
	    
	    halfLArPhysicalNeg->add(new GeoNameTag(cylName+std::string("PhysBackward")));
	    halfLArPhysicalNeg->add(new GeoIdentifierTag(cylNumber));
	    //	  halfLArPhysicalNeg->add(xfNeg);
	    halfLArPhysicalNeg->add(new GeoTransform(GeoTrf::TranslateZ3D(zInCryostat)));
	    halfLArPhysicalNeg->add(physBarrelCylinder);
	  } else {
	    totalLArPhysical->add(new GeoNameTag(cylName+std::string("PhysForward")));
	    totalLArPhysical->add(new GeoIdentifierTag(cylNumber));
	    totalLArPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zInCryostat)));
	    totalLArPhysical->add(physBarrelCylinder);

	    totalLArPhysical->add(new GeoNameTag(cylName+std::string("PhysBackward")));
	    totalLArPhysical->add(new GeoIdentifierTag(cylNumber));
	    totalLArPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(-zInCryostat)));
	    totalLArPhysical->add(physBarrelCylinder);
	  }
	}
      }                                                                                                 //
      //--------------------------------------------------------------------------------------------------//
    }
    
  }
  {
    // ----- Presampler ------    
    double PresamplerMother_length = 1549.0*Gaudi::Units::mm;  // Copied from PresParameterDef.icc
    double presamplerShift = 3.*Gaudi::Units::mm;
    BarrelPresamplerConstruction barrelPSConstruction(m_fullGeo);
    
    // The "envelope" determined by the EMB should be a GeoFullPhysVol.
    GeoFullPhysVol* barrelPSPosEnvelope = barrelPSConstruction.GetPositiveEnvelope();
    GeoTransform *xfPos = new GeoTransform(GeoTrf::Transform3D(GeoTrf::TranslateZ3D(PresamplerMother_length+presamplerShift)));
    {
      halfLArPhysicalPos->add(xfPos);
      //halfLArPhysicalPos->add(new GeoNameTag("PositivePSBarrel"));
      halfLArPhysicalPos->add(barrelPSPosEnvelope);

      StoredPhysVol *sPhysVol = new StoredPhysVol(barrelPSPosEnvelope);
      StatusCode status=detStore->record(sPhysVol,"PRESAMPLER_B_POS");
      if(!status.isSuccess()) throw std::runtime_error ("Cannot store PRESAMPLER_B_POS");
    }
    // The "envelope" determined by the EMB should be a GeoFullPhysVol.
    GeoFullPhysVol* barrelPSNegEnvelope = barrelPSConstruction.GetNegativeEnvelope();
    GeoTransform *xfNeg = new GeoTransform(GeoTrf::Transform3D(GeoTrf::TranslateZ3D(PresamplerMother_length+presamplerShift)));
    {
      halfLArPhysicalNeg->add(xfNeg);
      //halfLArPhysicalPos->add(new GeoNameTag("NegativePSBarrel"));
      halfLArPhysicalNeg->add(barrelPSNegEnvelope);

      StoredPhysVol *sPhysVol = new StoredPhysVol(barrelPSNegEnvelope);
      StatusCode status=detStore->record(sPhysVol,"PRESAMPLER_B_NEG");
      if(!status.isSuccess()) throw std::runtime_error ("Cannot store PRESAMPLER_B_NEG");
    }
    
  }						     

  // SCT-EC Cooling
  IRDBRecordset_ptr cryoPconPhiSect = rdbAccess->getRecordsetPtr("CryoPconPhiSect", larVersionKey.tag(),larVersionKey.node());  
  for(unsigned i=0; i<cryoPconPhiSect->size(); ++i) {
    double startPhi = (*cryoPconPhiSect)[i]->getDouble("STARTPHI");
    double dPhi     = (*cryoPconPhiSect)[i]->getDouble("DPHI");
    double centerPhi = startPhi + 0.5*dPhi;

    const GeoMaterial* material  = materialManager->getMaterial((*cryoPconPhiSect)[i]->getString("MATERIAL"));
    if (!material) {
      std::string message = std::string("Error in BarrelCryostatConstruction! ") + (*cryoPconPhiSect)[i]->getString("MATERIAL") + std::string(" is not found.");
      throw std::runtime_error(message.c_str());
    }

    GeoPcon* pcon = new GeoPcon(startPhi*Gaudi::Units::deg,dPhi*Gaudi::Units::deg);

    for(unsigned int ii=0; ii<sctEcCoolingPlanes.size(); ii++) {
      iter = sctEcCoolingPlanes.find(ii);
      
      if(iter==sctEcCoolingPlanes.end()) {
	std::ostringstream stream;
	stream << "Error in BarrelCryostatConstruction, missing plane " << ii <<" in SCT-EC cooling"; 
	throw std::runtime_error(stream.str().c_str());
      }
      else {
	currentRecord = (*cryoPcons)[(*iter).second];
	pcon->addPlane(currentRecord->getDouble("ZPLANE"),
		       currentRecord->getDouble("RMIN"),
		       currentRecord->getDouble("RMAX"));
      }
    } // iterate over planes

    const GeoLogVol* sctCiCoolingLog = new GeoLogVol("LAr::Barrel::Cryostat::SctCiCooling",pcon,material);
    GeoPhysVol* sctCiCoolingPhys = new GeoPhysVol(sctCiCoolingLog);

    GeoTransform* xfPos1 = new GeoTransform(GeoTrf::Transform3D::Identity());
    GeoTransform* xfPos2 = new GeoTransform(GeoTrf::RotateZ3D(180*Gaudi::Units::deg));
    GeoTransform* xfNeg1 = new GeoTransform(GeoTrf::RotateZ3D((180+2*centerPhi)*Gaudi::Units::deg)*GeoTrf::RotateY3D(180*Gaudi::Units::deg));
    GeoTransform* xfNeg2 = new GeoTransform(GeoTrf::RotateZ3D(2*centerPhi*Gaudi::Units::deg)*GeoTrf::RotateY3D(180*Gaudi::Units::deg));
    
    m_cryoMotherPhysical->add(xfPos1);
    m_cryoMotherPhysical->add(sctCiCoolingPhys);
    m_cryoMotherPhysical->add(xfPos2);
    m_cryoMotherPhysical->add(sctCiCoolingPhys);
    m_cryoMotherPhysical->add(xfNeg1);
    m_cryoMotherPhysical->add(sctCiCoolingPhys);
    m_cryoMotherPhysical->add(xfNeg2);
    m_cryoMotherPhysical->add(sctCiCoolingPhys);
  } // iterate over Phi Sections



  if(rdbAccess->getChildTag("LArBarrelDM",larVersionKey.tag(),larVersionKey.node())!="" && m_fullGeo) {
    // Dead material in barrel
    BarrelDMConstruction barrelDMConstruction;
    barrelDMConstruction.create (m_cryoMotherPhysical);
  }  

  return m_cryoMotherPhysical;
}
