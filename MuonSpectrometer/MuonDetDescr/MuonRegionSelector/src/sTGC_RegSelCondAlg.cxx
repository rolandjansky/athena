/**
 **   @file   sTGC_RegSelCondAlg.cxx         
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


#include <iostream>
#include <fstream>
#include <string>

#include "CLHEP/Units/SystemOfUnits.h"
#include "Identifier/IdentifierHash.h"

#include "RegSelLUT/RegSelModule.h" 
#include "RegSelLUT/RegSelSiLUT.h" 

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h" 

/// sTGC naming convention headers
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonAGDDDescription/sTGCDetectorHelper.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"

#include "MuonReadoutGeometry/MuonStation.h"

#include "RegSelLUT/RegSelSiLUT.h"


/// not implemented yet
/// #include "Muon_sTGC_Cabling/MuonMM_CablingSvc.h"

#include "sTGC_RegSelCondAlg.h"


sTGC_RegSelCondAlg::sTGC_RegSelCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  MuonRegSelCondAlg( name, pSvcLocator )
{ 
  ATH_MSG_DEBUG( "sTGC_RegSelCondAlg::sTGC_RegSelCondAlg() " << name );
}




StatusCode sTGC_RegSelCondAlg::initialize() {
  ATH_CHECK(MuonRegSelCondAlg::initialize());
  ATH_CHECK(m_mdtCablingKey.initialize());
  return StatusCode::SUCCESS;
}



std::unique_ptr<RegSelSiLUT> sTGC_RegSelCondAlg::createTable( const EventContext& ctx, EventIDRange& id_range ) const { 

  /// get the MDT cabling map as a proxy for the id_range
  
  SG::ReadCondHandle<MuonMDT_CablingMap> mdtCabling( m_mdtCablingKey, ctx );
 
  if( !mdtCabling.range( id_range ) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << mdtCabling.key());
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }   
  

  /// no NSW cabling available at the moment ...

  /// get the MM detector manager

  const MuonGM::MuonDetectorManager* manager = nullptr;
  
  if ( (detStore()->retrieve( manager ).isFailure() ) ) { 
    ATH_MSG_ERROR( "Could not retrieve Muon Manager for " << name() );
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }

  const sTgcIdHelper*  helper = manager->stgcIdHelper();

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

      const MuonGM::sTgcReadoutElement* mm = manager->getsTgcReadoutElement(Id);
      if (!mm) continue;

      //      std::cout << "stgc station name: " << mm->getStationName() << "\teta: " << mm->getStationEta() << "\tphi: " << mm->getStationPhi() << std::endl; 
  
      int multilayer = helper->multilayer(Id);

      char side     = mm->getStationEta() < 0 ? 'C' : 'A';

      char sector_l = mm->getStationName().substr(2,1)=="L" ? 'L' : 'S';

      sTGCDetectorHelper aHelper;
      sTGCDetectorDescription* md = aHelper.Get_sTGCDetector( sector_l, std::abs(mm->getStationEta()), mm->getStationPhi(), multilayer, side );
  
      Amg::Vector3D mmPos = mm->center();      
  
      double swidth = md->sWidth();
      double lwidth = md->lWidth();

      double ycutout = md->yCutout();

      double length = md->Length();
      double depth  = md->Tck();

      double moduleR = std::sqrt( mmPos.mag()*mmPos.mag() -  mmPos.z()*mmPos.z());

      double zmin = mmPos.z()-0.5*depth;
      double zmax = mmPos.z()+0.5*depth;

      double rmin = moduleR-0.5*length;
      double rmax = std::sqrt( (moduleR+0.5*length)*(moduleR+0.5*length) + lwidth*lwidth/4 );

      double dphi1 = std::atan( (0.5*lwidth)/(moduleR+0.5*length) );
      double dphi2 = std::atan( (0.5*swidth)/(moduleR-0.5*length) );

      double dphi = ( dphi1 > dphi2 ? dphi1 : dphi2 );

      if ( ycutout > 0 ) { 
	double rcutout = moduleR+0.5*length - ycutout;
	double dphicutout = std::atan( (0.5*lwidth)/rcutout );
        if ( dphi < dphicutout ) dphi = dphicutout;
      }

      double phimin = mmPos.phi()-dphi;
      double phimax = mmPos.phi()+dphi;

      if ( phimin >  M_PI ) phimin -= 2*M_PI;
      if ( phimin < -M_PI ) phimin += 2*M_PI;

      int layerid = multilayer;
      int detid   = ( side == 'C' ? -1 : 1 );

      int robId  = 0;

      //      std::cout << "sTGC::module r: " << moduleR << "\tlength: " << length << "\tswidth: " << swidth << "\tlwidth: " << lwidth << "\tycut: " << ycutout << std::endl; 
      
      RegSelModule m( zmin, zmax, rmin, rmax, phimin, phimax, int(layerid), int(detid), uint32_t(robId), hashId );

      lut->addModule( m );

  }

  lut->initialise(); 

  return lut;
}
