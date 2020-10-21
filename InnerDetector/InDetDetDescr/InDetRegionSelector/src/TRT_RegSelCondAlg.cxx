/**
 **   @file   TRT_RegSelCondAlg.cxx         
 **            
 **           conditions algorithm to create the Si detector 
 **           lookup tables    
 **            
 **   @author sutt
 **   @date   Sun 22 Sep 2019 10:21:50 BST
 **
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/


#include "GaudiKernel/EventIDRange.h"
#include "StoreGate/WriteCondHandle.h"


#include "InDetReadoutGeometry/SiDetectorManager.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "GaudiKernel/MsgStream.h"

#include "TRT_ReadoutGeometry/TRT_BarrelElement.h" 
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h" 

#include "InDetIdentifier/TRT_ID.h" 

#include "Identifier/Identifier.h" 
#include "Identifier/IdentifierHash.h" 

#include "TRT_ReadoutGeometry/TRT_DetectorManager.h" 
#include "TRT_ReadoutGeometry/TRT_Numerology.h" 

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "TRT_Cabling/ITRT_CablingSvc.h"

#include "RegSelLUT/RegSelSiLUT.h" 

#include "TRT_RegSelCondAlg.h"




TRT_RegSelCondAlg::TRT_RegSelCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm( name, pSvcLocator ),
  m_managerName("TRT"),
  m_printTable(false)
{ 
  ATH_MSG_DEBUG( "TRT_RegSelCondAlg::TRT_RegSelCondAlg() " << this->name() );
  declareProperty( "PrintTable",  m_printTable=false );  
  declareProperty( "ManagerName", m_managerName );  
}




StatusCode TRT_RegSelCondAlg::initialize()
{
  ATH_MSG_DEBUG("TRT_RegSelCondAlg::initialize() ");
  ATH_CHECK(m_condCablingKey.initialize());
  ATH_CHECK(m_tableKey.initialize());
  ATH_MSG_INFO("TRT_RegSelCondAlg::initialize() " << m_tableKey );
  return StatusCode::SUCCESS;
}




StatusCode TRT_RegSelCondAlg::execute(const EventContext& ctx)  const
{

  ATH_MSG_DEBUG( "Creating region selector table" );

  SG::WriteCondHandle<IRegSelLUTCondData> lutCondData( m_tableKey, ctx );
  if (lutCondData.isValid()) {
    /// inpractice, this should never be called, although in serial athena,                                                                          
    /// because the implementation of the conditions behaviour is flawed in                                                                         
    /// the framework, this routine will be called every event (!) regardless                                                                       
    /// of whether it should be called or not so we need this check to                                                                               
    /// prevent unecessary code execution on out our side                                                                                            
    ATH_MSG_DEBUG("CondHandle " << lutCondData.fullKey() << " is already valid." );
    return StatusCode::SUCCESS;
  }


  StatusCode sc;
  // Retrieve manager
  const InDetDD::TRT_DetectorManager* manager;

  if ( (sc=detStore()->retrieve(manager, m_managerName)).isFailure() ) {
    ATH_MSG_FATAL(  "Could not find the Manager: "<< m_managerName << " !" );
    return StatusCode::FAILURE;
  } 

  ATH_MSG_DEBUG( "Manager found " << m_managerName );

  
  // Get Tool Service
  IToolSvc* toolSvc;
  if (StatusCode::SUCCESS != service("ToolSvc", toolSvc))    {
    msg(MSG::ERROR) << " Can't get ToolSvc " << endmsg;
    return StatusCode::FAILURE;
  }


  
  // Get TRT cabling mapping 
  
  ServiceHandle<ITRT_CablingSvc>  mapping( "TRT_CablingSvc", name() );

  if ( mapping.retrieve().isFailure() ) { 
    msg(MSG::ERROR) << " Can't get TRT CablingSvc " << endmsg;
    return StatusCode::FAILURE;
  }

  
  // Get the id helper 
  const TRT_ID* idHelper = 0;
  if ( detStore()->retrieve( idHelper, "TRT_ID" ).isFailure() ) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  /// FIXME: since we store the LUT as conditions data, we need an EventIDRange for  
  ///        the WriteCondHandle, but the SCT cabling is not yet conditions data so
  ///        have to get the pixelCabling for the SCT case also so that the
  ///        EventIDRange can be extracted
  ///        Once the TRT cabling is iself conditions data based, we will be able to 
  ///        getthe corresponding EventIDRange from there

  SG::ReadCondHandle<PixelCablingCondData> pixCabling( m_condCablingKey, ctx );

  EventIDRange id_range;

  if( !pixCabling.range( id_range ) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << pixCabling.key());
    return StatusCode::FAILURE;
  }   

  
  // Try and iterate over all elements.  
  // There is no iterator but we can get the elements via the idHash. 

  unsigned int maxHash = idHelper->straw_layer_hash_max();

  // create the new TRT look up table

  std::unique_ptr<RegSelSiLUT> rd = std::make_unique<RegSelSiLUT>(RegSelSiLUT::TRT);

  constexpr double twoPi=2.*M_PI;
  constexpr double InnerRadiusOfStraw = 2.; //hardcoded. No method? (it will NEVER change anyway)

  for (unsigned int index = 0; index < maxHash; index++) {
    IdentifierHash idHash = index;
    Identifier id = idHelper->layer_id(idHash);
    int idBarrelEndcap = idHelper->barrel_ec(id);
    int idSide = (idBarrelEndcap > 0);
    int idLayerWheel = idHelper->layer_or_wheel(id);
    int idPhiModule = idHelper->phi_module(id);
    int idStrawLayer = idHelper->straw_layer(id);
    const InDetDD::TRT_BarrelElement* Belement = nullptr;
    const InDetDD::TRT_EndcapElement* Eelement = nullptr;
    Identifier idelement;
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
      phiMin = std::atan2(y0,x0) - std::atan2(InnerRadiusOfStraw,r0); // over 0? 
      while (phiMin >  M_PI)  phiMin -= twoPi; //isnt there a 'mod' operation alternative??
      while (phiMin < -M_PI)  phiMin += twoPi;
      phiMax = std::atan2(y1,x1) + std::atan2(InnerRadiusOfStraw,r1); // over 0?
      while (phiMax >   M_PI) phiMax -= twoPi;
      while (phiMax <  -M_PI) phiMax += twoPi;
      double zmin = Belement->strawZPos(theLastStraw) - Length*0.5;
      double zmax = Belement->strawZPos(theLastStraw) + Length*0.5;
      double rmin = ( r0<r1 ? r0 : r1 ) - InnerRadiusOfStraw;
      double rmax = ( r1>r0 ? r1 : r0 ) + InnerRadiusOfStraw;
      std::vector<uint32_t> vrob = mapping->getRobID(idelement);  
      for (unsigned int ii=0 ; ii < vrob.size(); ++ii) { 
        RegSelModule smod( zmin, zmax, rmin, rmax, phiMin, phiMax, idLayerWheel, idSide, vrob[ii], idHash);
        rd->addModule(smod);
      }
    } else {
      Eelement  = manager->getEndcapElement(idSide, idLayerWheel, idStrawLayer, idPhiModule);
      idelement = Eelement->identify();
      int Nstraws   = Eelement->getDescriptor()->nStraws();
      double pitch  = Eelement->getDescriptor()->strawPitch();
      double phi0   = Eelement->getDescriptor()->startPhi();
      double length = Eelement->getDescriptor()->strawLength();
      double radius = Eelement->getDescriptor()->innerRadius ();
      //check the following line, was HepGeom::Point3D<double>
      Amg::Vector3D center = Eelement->transform() * Amg::Vector3D(0,0,0);
      double dphi = std::atan2(InnerRadiusOfStraw, radius );
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
      std::vector<uint32_t> vrob = mapping->getRobID(idelement);  
      for (unsigned int ii=0 ; ii < vrob.size(); ++ii) { 
        RegSelModule smod( zmin, zmax, rmin, rmax, phiMin, phiMax, idLayerWheel, idSide+2, vrob[ii], idHash);
        rd->addModule(smod);
      }
    }
  }



  // initialise and save the new look up table
  ATH_MSG_DEBUG( " initialising new trt map " );

  rd->initialise();

  // write out new new LUT to a file if need be
  if ( m_printTable ) rd->write( name()+".map" );

  IRegSelLUTCondData* rcd = new IRegSelLUTCondData( std::move(rd) );
  
  try { 
    /// leave this commented here since this is where it should really be,
    /// but we had to move it up in the code to handle the flawed conditions 
    /// handling in the serial athena use case
    //    SG::WriteCondHandle<IRegSelLUTCondData> lutCondData( m_tableKey, ctx );
    if( lutCondData.record( id_range, rcd ).isFailure() ) {
      ATH_MSG_ERROR( "Could not record " << m_tableKey 
		     << " " << lutCondData.key()
		     << " with range " << id_range );  
      return StatusCode::FAILURE;   
    } 
    ATH_MSG_INFO( "RegSelCondAlg LUT recorded: " << m_tableKey);
  }
  catch (...) { 
    ATH_MSG_ERROR("TRT_RegSelCondAlg::execute() failed to record table: " << m_tableKey);
    return StatusCode::FAILURE;   
  }   
  
  
  ATH_MSG_DEBUG("TRT_RegSelCondAlg::execute() -- exit -- ");

  return StatusCode::SUCCESS;
}











