/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRegionSelector/TRT_RegionSelectorTable.h"

#include "GaudiKernel/MsgStream.h"


#include "CLHEP/Units/SystemOfUnits.h"

#include "InDetReadoutGeometry/TRT_BarrelElement.h" 
#include "InDetReadoutGeometry/TRT_EndcapElement.h" 
#include "InDetIdentifier/TRT_ID.h" 
#include "Identifier/Identifier.h" 
#include "Identifier/IdentifierHash.h" 
#include "InDetReadoutGeometry/TRT_DetectorManager.h" 
#include "InDetReadoutGeometry/TRT_Numerology.h" 

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "RegSelLUT/StoreGateIDRS_ClassDEF.h" 

#include "TRT_Cabling/ITRT_CablingSvc.h"

#include "RegSelLUT/RegSelModule.h" 
#include "RegSelLUT/RegSelSiLUT.h" 



using namespace InDetDD;


#define USE_STOREGATE


/////////////////////////////////////////////////////////////////////////////

TRT_RegionSelectorTable::TRT_RegionSelectorTable(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  :  AthAlgTool(type,name,parent),
     m_TRT_IdMapping("TRT_CablingSvc", name),
     m_regionLUT(nullptr),
     m_managerName("TRT"),
     m_roiFileName("TRTRoITable.txt"),
     m_printHashId(true),
     m_printTable(false)
{
  declareInterface<IRegionIDLUT_Creator>(this);
  declareProperty("ManagerName", m_managerName);

  // The remaining properties are for debugging purposes.
  declareProperty("OutputFile",  m_roiFileName);
  declareProperty("PrintHashId", m_printHashId);
  declareProperty("PrintTable",  m_printTable);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TRT_RegionSelectorTable::initialize(){

  //  MsgStream msglog(msgSvc(), name());
  msg(MSG::INFO) << "initialize() " << name() << " " << PACKAGE_VERSION << endmsg;

  msg(MSG::INFO)  << "Tool Properties" << endmsg;
  msg(MSG::INFO)  << " Detector Manager: " << m_managerName << endmsg;
  if ( msgLvl(MSG::DEBUG) ) {
    msg(MSG::DEBUG) << " Output File:      " << m_roiFileName <<endmsg;
    msg(MSG::DEBUG) << " Print hashId:     " << ((m_printHashId) ? "true" : "false") <<endmsg;
    msg(MSG::DEBUG) << " Print Table:      " << ((m_printTable) ? "true" : "false") <<endmsg;
  }

  if (m_managerName.empty()) {
    msg(MSG::WARNING) << "Tool disabled." << endmsg;
    return StatusCode::FAILURE;
  } 
 
  StatusCode sc;

  sc = createTable();
  return sc;
}

TRT_RegionSelectorTable::~TRT_RegionSelectorTable()
{
  // table is stored in storegate so nothing to delete.
#ifndef USE_STOREGATE
  if ( m_regionLUT ) delete m_regionLUT;
#endif
}

// Get the lookup table.
RegSelSiLUT* TRT_RegionSelectorTable::getLUT() const
{
  return m_regionLUT;
}

StatusCode 
TRT_RegionSelectorTable::createTable()
{
  if ( msgLvl(MSG::DEBUG) )  msg(MSG::DEBUG) << "Creating region selector table"  << endmsg;
  StatusCode sc;
  // Retrieve manager
  const TRT_DetectorManager* manager;
  sc=detStore()->retrieve(manager, m_managerName);
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not find the Manager: "<< m_managerName << " !" << endmsg;
    return StatusCode::FAILURE;
  } else {
    if ( msgLvl(MSG::DEBUG) )  msg(MSG::DEBUG) << "Manager found" << endmsg;
  }
  
  // Get Tool Service
  IToolSvc* toolSvc;
  if (StatusCode::SUCCESS != service("ToolSvc", toolSvc))    {
    msg(MSG::ERROR) << " Can't get ToolSvc " << endmsg;
    return StatusCode::FAILURE;
  }
  
  // Get TRT IdentifierConversionTool
  sc = m_TRT_IdMapping.retrieve();

  if (sc != StatusCode::SUCCESS){
    msg(MSG::ERROR) << " Can't get TRTCablingSvc " << endmsg;
    return StatusCode::FAILURE;
  }
  // Get the id helper 
  const TRT_ID* idHelper = 0;
  if ( detStore()->retrieve( idHelper, "TRT_ID" ).isFailure() ) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  
  //-----------------------------------------------------
  unsigned int maxHash = idHelper->straw_layer_hash_max();
  // 
  // Try and iterate over all elements.  
  // There is no iterator but we can get the elements via the idHash. 
  // 
  // MS create the new TRT look up table
  RegSelSiLUT *rtrt = new RegSelSiLUT(RegSelSiLUT::TRT);
  const double twoPi=2.*M_PI;
  for (unsigned int index = 0; index < maxHash; index++) {
    IdentifierHash idHash = index;
    Identifier id = idHelper->layer_id(idHash);
    int idBarrelEndcap = idHelper->barrel_ec(id);
    int idSide = (idBarrelEndcap > 0);
    int idLayerWheel = idHelper->layer_or_wheel(id);
    int idPhiModule = idHelper->phi_module(id);
    int idStrawLayer = idHelper->straw_layer(id);
    const TRT_BarrelElement* Belement = nullptr;
    const TRT_EndcapElement* Eelement = nullptr;
    Identifier idelement;
    double InnerRadiusOfStraw = 2.; //hardcoded. No method? (it will NEVER change anyway)
    double phiMin,phiMax,rz;    
    if (idHelper->is_barrel(id)) {
      Belement = manager->getBarrelElement(idSide, idLayerWheel, idPhiModule, idStrawLayer);
      idelement = Belement->identify();
      int Nstraws = Belement->nStraws();
      const int theLastStraw=Nstraws-1;
      double Length =  Belement->strawLength();
      double x0 = Belement->strawXPos(0);
      double x1 = Belement->strawXPos(theLastStraw);
      double y0 = Belement->strawYPos(0);
      double y1 = Belement->strawYPos(theLastStraw);
      double r0 = sqrt(x0*x0 + y0*y0);
      double r1 = sqrt(x1*x1 + y1*y1);
      phiMin = atan2(y0,x0) - atan2(InnerRadiusOfStraw,r0); // over 0? 
      while (phiMin >  M_PI)  phiMin -= twoPi; //isnt there a 'mod' operation alternative??
      while (phiMin < -M_PI)  phiMin += twoPi;
      phiMax = atan2(y1,x1) + atan2(InnerRadiusOfStraw,r1); // over 0?
      while (phiMax >   M_PI) phiMax -= twoPi;
      while (phiMax <  -M_PI) phiMax += twoPi;
      double zmin = Belement->strawZPos(theLastStraw) - Length*0.5;
      double zmax = Belement->strawZPos(theLastStraw) + Length*0.5;
      double rmin = ( r0<r1 ? r0 : r1 ) - InnerRadiusOfStraw;
      double rmax = ( r1>r0 ? r1 : r0 ) + InnerRadiusOfStraw;
      std::vector<uint32_t> vrob;
      vrob = m_TRT_IdMapping->getRobID(idelement);  
      for (unsigned int ii=0 ; ii < vrob.size(); ++ii) { 
        RegSelModule smod( zmin, zmax, rmin, rmax, phiMin, phiMax, idLayerWheel, idSide, vrob[ii], idHash);
        rtrt->addModule(smod);
      }
    } else {
      Eelement = manager->getEndcapElement(idSide, idLayerWheel, idStrawLayer, idPhiModule);
      idelement = Eelement->identify();
      int Nstraws = Eelement->getDescriptor()->nStraws();
      double pitch = Eelement->getDescriptor()->strawPitch();
      double phi0  = Eelement->getDescriptor()->startPhi();
      double length = Eelement->getDescriptor()->strawLength();
      double radius = Eelement->getDescriptor()->innerRadius ();
      //check the following line, was HepGeom::Point3D<double>
      Amg::Vector3D center = Eelement->transform() * Amg::Vector3D(0,0,0);
      double dphi = atan2(InnerRadiusOfStraw, radius );
      phiMin = phi0 - dphi;
      while (phiMin >  M_PI)  phiMin -= twoPi;
      while (phiMin < -M_PI) phiMin += twoPi;
      phiMax = phi0 + pitch * (Nstraws-1) + dphi;
      while (phiMax >   M_PI) phiMax -= twoPi;
      while (phiMax <  -M_PI) phiMax += twoPi;	    
      rz = center.z();
      //why was this variable set twice to different things?.. and then never used?
      //double R = rz >= 0 ? radius : radius + length;
      //double R = rz >= 0 ? radius + length : radius ;
      // MS: infor for new maps
      double zmin = rz-InnerRadiusOfStraw;
      double zmax = rz+InnerRadiusOfStraw;
      double rmin = radius;
      double rmax = radius+length;
      std::vector<uint32_t> vrob;
      // ??????
      vrob = m_TRT_IdMapping->getRobID(idelement);  
      for (unsigned int ii=0 ; ii < vrob.size(); ++ii) { 
        RegSelModule smod( zmin, zmax, rmin, rmax, phiMin, phiMax, idLayerWheel, idSide+2, vrob[ii], idHash);
        rtrt->addModule(smod);
      }
    }
  }


  // MS: new region selector lookup table
  // initialise and save the new look up table
  msg(MSG::INFO) << " initialising new trt map " << endmsg;
  rtrt->initialise();

  // save pointer for access from the ToolSvc
#ifndef USE_STOREGATE
  if ( m_regionLUT ) delete m_regionLUT;
#endif
  m_regionLUT = rtrt;

#ifdef USE_STOREGATE
  std::string newtrtKey = "TRTRegSelSiLUT";
  if ( detStore()->contains< RegSelSiLUT >(newtrtKey) ) {
    msg(MSG::FATAL) << " TRTRegSelSiLUT " << newtrtKey << " already exists " << endmsg;
  } else {
    // create and store LUT
    sc = detStore()->record(rtrt, newtrtKey, true);
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) << " could not register trt RegSelSiLUT" << endmsg;
      return( StatusCode::FAILURE );
    } else {
      msg(MSG::INFO) << "trt RegSelSiLUT successfully saved in detector Store" << endmsg;
    }
  }
#endif

  // write out new new LUT to a file if need be
  if ( m_printTable ) rtrt->write("NewTRT"+m_roiFileName);
  
  // end of MS change

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TRT_RegionSelectorTable::execute() {

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TRT_RegionSelectorTable::finalize() {

  //  MsgStream msglog(msgSvc(), name());
  msg(MSG::INFO) << "finalize()" << endmsg;
  
  return StatusCode::SUCCESS;
}

