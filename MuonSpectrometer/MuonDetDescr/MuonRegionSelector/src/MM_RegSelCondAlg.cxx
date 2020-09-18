/**
 **   @file   MM_RegSelCondAlg.cxx         
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

#include "CLHEP/Units/SystemOfUnits.h"
#include "Identifier/IdentifierHash.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "MuonCablingData/MuonMDT_CablingMap.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h" 

#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"
#include "MuonAGDDDescription/MMDetectorDescription.h"

#include "MuonReadoutGeometry/MuonStation.h"

#include "RegSelLUT/RegSelSiLUT.h"

/// not implemented yet
/// #include "MuonMM_Cabling/MuonMM_CablingSvc.h"


#include "MM_RegSelCondAlg.h"


MM_RegSelCondAlg::MM_RegSelCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  MuonRegSelCondAlg( name, pSvcLocator )
{ 
  ATH_MSG_DEBUG( "MM_RegSelCondAlg::MM_RegSelCondAlg() " << name );
}




StatusCode MM_RegSelCondAlg::initialize() {
  ATH_CHECK(MuonRegSelCondAlg::initialize());
  ATH_CHECK(m_mdtCablingKey.initialize());
  return StatusCode::SUCCESS;
}



std::unique_ptr<RegSelSiLUT> MM_RegSelCondAlg::createTable( const EventContext& ctx, EventIDRange& id_range ) const { 

  SG::ReadCondHandle<MuonMDT_CablingMap> mdtCabling( m_mdtCablingKey, ctx );
 
  if( !mdtCabling.range( id_range ) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << mdtCabling.key());
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }   
  

  /// no NSW cabling available at the moment ...

  /// get the MM detector manager

  const MuonGM::MuonDetectorManager* manager = nullptr;
  
  if ( (detStore()->retrieve( manager ).isFailure() ) ) { 
    ATH_MSG_ERROR( "Could not retrieve CSC Manager for " << name() );
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }

  const MmIdHelper*  helper = manager->mmIdHelper();

  
  std::vector<Identifier>::const_iterator  idfirst = helper->module_begin();
  std::vector<Identifier>::const_iterator  idlast =  helper->module_end();
 
  /// do we want the module context ...
  ///  const IdContext ModuleContext = helper->module_context();
  
  /// or the detector element context ? Keep them both untill we are sure 
  const IdContext ModuleContext = helper->detectorElement_context();

  ATH_MSG_DEBUG("createTable()");
  
  std::unique_ptr<RegSelSiLUT> lut = std::make_unique<RegSelSiLUT>();


  for ( std::vector<Identifier>::const_iterator i=idfirst ; i!=idlast ; i++ ) {

      Identifier     Id = *i;
      IdentifierHash hashId;

      helper->get_hash( Id, hashId, &ModuleContext );

      const MuonGM::MMReadoutElement* mm = manager->getMMReadoutElement(Id);
      if (!mm) continue;

      int multilayer = helper->multilayer(Id);

      char side     = mm->getStationEta() < 0 ? 'C' : 'A';

      char sector_l = mm->getStationName().substr(2,1)=="L" ? 'L' : 'S';

      MMDetectorHelper aHelper;
      MMDetectorDescription* md = aHelper.Get_MMDetector( sector_l, std::abs(mm->getStationEta()), mm->getStationPhi(), multilayer, side );

      /// now calculate the required limits

      Amg::Vector3D mmPos = mm->center();      
  
      
      double swidth = md->sWidth();
      double lwidth = md->lWidth();

      double length = md->Length();
      double depth  = md->Tck();

      double moduleR = std::sqrt( mmPos.mag()*mmPos.mag() -  mmPos.z()*mmPos.z());

      double zmin = mmPos.z()-0.5*depth;
      double zmax = mmPos.z()+0.5*depth;

      //      std::cout << "MM:module: rcen: " << moduleR << "\tlength: " << length << "\tswidth: " << swidth << "\tlwidth: " << lwidth << std::endl; 

      double rmin = moduleR-0.5*length;
      double rmax = std::sqrt( (moduleR+0.5*length)*(moduleR+0.5*length) + 0.25*lwidth*lwidth );

      double dphi1 = std::atan( (0.5*lwidth)/(moduleR+0.5*length) );
      double dphi2 = std::atan( (0.5*swidth)/(moduleR-0.5*length) );

      double dphi = ( dphi1 > dphi2 ? dphi1 : dphi2 );

      double phimin = mmPos.phi()-dphi;
      double phimax = mmPos.phi()+dphi;

      if ( phimin >  M_PI ) phimin -= 2*M_PI;
      if ( phimin < -M_PI ) phimin += 2*M_PI;

      if ( phimax >  M_PI ) phimax -= 2*M_PI;
      if ( phimax < -M_PI ) phimax += 2*M_PI;

      int layerid = multilayer;
      int detid   = ( side == 'C' ? -1 : 1 );

      /// no cabling map at the moment - don't store robID 
      int robId  = 0;

      RegSelModule m( zmin, zmax, rmin, rmax, phimin, phimax, int(layerid), int(detid), uint32_t(robId), hashId );

      lut->addModule( m );

  }

  lut->initialise();

  return lut;

}

