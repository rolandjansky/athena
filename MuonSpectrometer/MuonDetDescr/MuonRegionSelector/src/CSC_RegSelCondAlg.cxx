/**
 **   @file   CSC_RegSelCondAlg.cxx         
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
#include "CSCcabling/CSCcablingSvc.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h" 
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonStation.h"


#include "CSC_RegSelCondAlg.h"


CSC_RegSelCondAlg::CSC_RegSelCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  MuonRegSelCondAlg( name, pSvcLocator )
{ 
  ATH_MSG_DEBUG( "CSC_RegSelCondAlg::CSC_RegSelCondAlg() " << name );
}





StatusCode CSC_RegSelCondAlg::initialize() {
  ATH_CHECK(MuonRegSelCondAlg::initialize());
  ATH_CHECK(m_mdtCablingKey.initialize());
  return StatusCode::SUCCESS;
}



std::unique_ptr<RegSelSiLUT> CSC_RegSelCondAlg::createTable( const EventContext& ctx, EventIDRange& id_range ) const { 

  SG::ReadCondHandle<MuonMDT_CablingMap> mdtCabling( m_mdtCablingKey, ctx );

  if( !mdtCabling.range( id_range ) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << mdtCabling.key());
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }   
  

  /// now get the CSC cabling service ...

  const CSCcablingSvc*   cabling = nullptr;

  if ( service( "CSCcablingSvc", cabling ).isFailure() ) {  
    ATH_MSG_ERROR( "Could not retrieve CSC cabling for " << name() );
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }

  /// get the CSC detector manager

  const MuonGM::MuonDetectorManager* manager = nullptr;
  
  if ( (detStore()->retrieve( manager ).isFailure() ) ) { 
    ATH_MSG_ERROR( "Could not retrieve CSC Manager for " << name() );
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }

  const CscIdHelper*  helper = manager->cscIdHelper();
  
  std::vector<Identifier>::const_iterator  idfirst = helper->module_begin();
  std::vector<Identifier>::const_iterator  idlast =  helper->module_end();
 
  const IdContext ModuleContext = helper->module_context();

  ATH_MSG_DEBUG("createTable()");
  
  std::unique_ptr<RegSelSiLUT> lut = std::make_unique<RegSelSiLUT>();



  for ( std::vector<Identifier>::const_iterator itr=idfirst ; itr!=idlast ; itr++ )  {
    Identifier Id = *itr;
    IdentifierHash Idhash;

    helper->get_hash(Id, Idhash, &ModuleContext);

    ExpandedIdentifier exp_id;
    if (helper->get_expanded_id( Id, exp_id, &ModuleContext)) {
      ATH_MSG_DEBUG("Failed retrieving ExpandedIdentifier for PRD Identifier = " << Id.getString() << ". Skipping to the next PRD.");
      continue;
    }

    int detid   = ( exp_id[2]<0 ? -1 : 1 );
    int layerid = exp_id[1]+1;


    // retrieve CscReadoutElement
        
    Identifier Id2ndLayer;
    int chamberLayer = 2;
    const MuonGM::CscReadoutElement *csc = manager->getCscReadoutElement(Id);
    if (csc == NULL) {

      //       std::cout << "Csc Readout Element not found for this Id ---- try 2nd layer"<<std::endl;
      Id2ndLayer = helper->parentID(Id);
      Id2ndLayer = helper->channelID(Id2ndLayer, chamberLayer, 1, 0, 1);
      csc = manager->getCscReadoutElement(Id2ndLayer);
      if (csc == NULL)
	{
	  //    std::cout << "Csc Readout Element not found for 2nd layer too ---- skip"<<std::endl;
	  continue;
	}
    }
         
    double eta_min =  99999999.;
    double eta_max = -99999999.;
    double phi_min =  999999.;
    double phi_max = -999999.;

    double zmin =  999999999;
    double zmax = -999999999;

    double rmin = 0;
    double rmax = 0;

    double phi_test = 0.;
    Identifier Id_phi_max;
    Identifier Id_phi_min;
    Identifier Id_eta_max;
    Identifier Id_eta_min;
     
    // only use the extreme chamber layers and wirelayers 

    int chlayer_inc = helper->chamberLayerMax(Id)-1;
    if ( chlayer_inc<1 ) chlayer_inc = 1;
    for ( int chlayer=1 ; chlayer<=helper->chamberLayerMax(Id) ; chlayer+=chlayer_inc ) {  


      int wlayer_inc = helper->wireLayerMax(Id)-1;
      if ( wlayer_inc<1 ) wlayer_inc = 1;
      for ( int wlayer=1 ; wlayer<=helper->wireLayerMax(Id) ; wlayer+=wlayer_inc ){

	for ( int phis=1 ; phis<=csc->NphiStrips(wlayer) ; phis++ ) {
	   
	      
	  Identifier phis_id = helper->channelID(Id, chlayer, wlayer, 1, phis);
	  Amg::Vector3D phis_x = csc->localStripPos(phis_id);
	  double phis_lenght = csc->stripLength(phis_id);
	         
	  // positions of the strip endpoints

	  Amg::Vector3D phis_x1 = phis_x;
	  Amg::Vector3D phis_x2 = phis_x;
	         
	  //phis_x1.setZ(phis_x.z()-phis_lenght/2.0);
	  //phis_x2.setZ(phis_x.z()+phis_lenght/2.0);
	  phis_x1[2] = phis_x.z()-phis_lenght/2.0;
	  phis_x2[2] = phis_x.z()+phis_lenght/2.0;
	         
	  Amg::Vector3D gphis_x1 = csc->globalPos(phis_x1);
	  Amg::Vector3D gphis_x2 = csc->globalPos(phis_x2);
	         
	  phi_test=gphis_x1.phi();
	  // for detector in (-0.25,0.25) phi interval use +-3,14 phi interval
	  //    if(!(aux1==51 && aux3==1))if (phi_test < 0) phi_test += 2.*M_PI;
	  if(!(exp_id[1]==51 && exp_id[3]==1))if (phi_test < 0) phi_test += 2.*M_PI;
	      

	  if ( zmin>gphis_x1.z() ) zmin = gphis_x1.z();
	  if ( zmax<gphis_x1.z() ) zmax = gphis_x1.z();

	  if ( rmin==0 || rmin>gphis_x1.perp() ) rmin = gphis_x1.perp();
	  if ( rmax==0 || rmax<gphis_x1.perp() ) rmax = gphis_x1.perp();

	  if ( zmin>gphis_x2.z() ) zmin = gphis_x2.z();
	  if ( zmax<gphis_x2.z() ) zmax = gphis_x2.z();

	  if ( rmin==0 || rmin>gphis_x2.perp() ) rmin = gphis_x2.perp();
	  if ( rmax==0 || rmax<gphis_x2.perp() ) rmax = gphis_x2.perp();

	  // position of the strip centre
 

	  Amg::Vector3D gphis_x = csc->globalPos(phis_x);
	        
	  if ( rmin==0 || rmin>gphis_x.perp() )  rmin = gphis_x.perp();
	  if ( rmax==0 || rmax<gphis_x.perp() )  rmax = gphis_x.perp();

	  // phi
	  if(phi_test > phi_max){
	    Id_phi_max=phis_id;
	    phi_max=phi_test;
	  }
	  if(phi_test < phi_min){
	    Id_phi_min=phis_id;
	    phi_min=phi_test;
	  }
	  // eta
	  if(gphis_x1.eta() > eta_max) {
	    Id_eta_max=phis_id;
	    eta_max=gphis_x1.eta();
	  }
	  if(gphis_x1.eta() < eta_min) {
	    Id_eta_min=phis_id;
	    eta_min=gphis_x1.eta();
	  }
	         
	  phi_test=gphis_x2.phi();
	  // for detector in (-0.25,0.25) phi interval use +-3,14 phi interval
	  //  if(!(aux1==51 && aux3==1)) if (phi_test < 0) phi_test += 2.*M_PI;
	  if(!(exp_id[1]==51 && exp_id[3]==1)) if (phi_test < 0) phi_test += 2.*M_PI;
	         
	  // phi
	  if(phi_test > phi_max)  {
	    Id_phi_max=phis_id;
	    phi_max=phi_test;
	  }
	  if(phi_test < phi_min)  {
	    Id_phi_min=phis_id;
	    phi_min=phi_test;
	  }
	  // eta
	  if(gphis_x2.eta() > eta_max)   {
	    Id_eta_max=phis_id;
	    eta_max=gphis_x2.eta();
	  }
	  if(gphis_x2.eta() < eta_min)  {
	    Id_eta_min=phis_id;
	    eta_min=gphis_x2.eta();
	  }
	         
	}

	// only use the extreme and middle strips  

	int eta_inc = csc->NetaStrips(wlayer)/2;
	if ( eta_inc<1 ) eta_inc = 1;
	for ( int etas=1 ; etas<=csc->NetaStrips(wlayer) ; etas+=eta_inc ) { 

    
	  Identifier etas_id = helper->channelID(Id, chlayer, wlayer, 0, etas);
	  Amg::Vector3D etas_x = csc->localStripPos(etas_id);
	  double etas_lenght = csc->stripLength(etas_id);
	   
	  // strip endpoints
      
	  Amg::Vector3D etas_x1 = etas_x;
	  Amg::Vector3D etas_x2 = etas_x;
	         
	  //etas_x1.setY(etas_x.y()-etas_lenght/2.0);
	  //etas_x2.setY(etas_x.y()+etas_lenght/2.0);
	  etas_x1[1] = etas_x.y()-etas_lenght/2.0;
	  etas_x2[1] = etas_x.y()+etas_lenght/2.0;
	         
	  Amg::Vector3D getas_x1 = csc->globalPos(etas_x1);
	  Amg::Vector3D getas_x2 = csc->globalPos(etas_x2);
	         
	  if ( zmin>getas_x1.z() ) zmin = getas_x1.z();
	  if ( zmax<getas_x1.z() ) zmax = getas_x1.z();

	  if ( zmin>getas_x2.z() ) zmin = getas_x2.z();
	  if ( zmax<getas_x2.z() ) zmax = getas_x2.z();

	  if ( rmin==0 || rmin>getas_x1.perp() ) rmin = getas_x1.perp();
	  if ( rmax==0 || rmax<getas_x1.perp() ) rmax = getas_x1.perp();

	  if ( rmin==0 || rmin>getas_x2.perp() ) rmin = getas_x2.perp();
	  if ( rmax==0 || rmax<getas_x2.perp() ) rmax = getas_x2.perp();

	  // position of the strip centre
 
	  Amg::Vector3D getas_x = csc->globalPos(etas_x);
	        
	  if ( rmin==0 || rmin>getas_x.perp() )  rmin = getas_x.perp();
	  if ( rmax==0 || rmax<getas_x.perp() )  rmax = getas_x.perp();
	       
	  phi_test = getas_x1.phi();
	  // for detector in (-0.25,0.25) phi interval use +-3,14 phi interval
	  // if(!(aux1==51 && aux3==1)) if (phi_test < 0) phi_test += 2.*M_PI;
	  if(!(exp_id[1]==51 && exp_id[3]==1)) if (phi_test < 0) phi_test += 2.*M_PI;
	  // phi
	  if(phi_test > phi_max){
	    Id_phi_max=etas_id;
	    phi_max=phi_test;
	  }
	  if(phi_test < phi_min){
	    Id_phi_min=etas_id;
	    phi_min=phi_test;
	  }
	  // eta
	  if(getas_x1.eta() > eta_max)   {
	    Id_eta_max=etas_id;
	    eta_max=getas_x1.eta();
	  }
	  if(getas_x1.eta() < eta_min)  {
	    Id_eta_min=etas_id;
	    eta_min=getas_x1.eta();
	  }
	         
	  phi_test = getas_x2.phi();
	  // for detector in (-0.25,0.25) phi interval use +-3,14 phi interval
	  // if (!(aux1==51 && aux3==1))  if (phi_test < 0) phi_test += 2.*M_PI;
	  if (!(exp_id[1]==51 && exp_id[3]==1))  if (phi_test < 0) phi_test += 2.*M_PI;
	  // phi
	  if(phi_test > phi_max){
	    Id_phi_max=etas_id;
	    phi_max=phi_test;
	  }
	  if(phi_test < phi_min){
	    Id_phi_min=etas_id;
	    phi_min=phi_test;
	  }
	  // eta
	  if(getas_x2.eta() > eta_max)  {
	    Id_eta_max=etas_id;
	    eta_max=getas_x2.eta();
	  }
	  if(getas_x2.eta() < eta_min) {
	    Id_eta_min=etas_id;
	    eta_min=getas_x2.eta();
	  }
	         
	}
      } //gas gaps
    } // chamber layers 
    
    // if(aux1==51 && aux3==1)if (phi_min < 0) phi_min += 2.*M_PI;
    // if(aux1==51 && aux3==1)if (phi_max < 0) phi_max += 2.*M_PI;
    if(exp_id[1]==51 && exp_id[3]==1) { 
      if (phi_max < 0) phi_max += 2.*M_PI;
      if (phi_min < 0) phi_min += 2.*M_PI;
    }

    uint16_t subDetectorId = (detid == -1) ? 0x6a : 0x69;
    uint32_t cscrob = 0x0;
    cabling->hash2Rob(Idhash.value(),cscrob);
    cscrob = ((subDetectorId << 16) | cscrob);
    RegSelModule m( zmin, zmax, rmin, rmax, phi_min, phi_max, layerid, detid, cscrob, Idhash );
    lut->addModule( m );

 
  } // modules


  lut->initialise();

  return lut;

}







