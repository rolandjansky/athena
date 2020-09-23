/**
 **   @file   MDT_RegSelCondAlg.cxx         
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

#include "MDT_RegSelCondAlg.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "Identifier/IdentifierHash.h"

#include <iostream>
#include <fstream>
#include <string>

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h" 
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonStation.h"



MDT_RegSelCondAlg::MDT_RegSelCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  MuonRegSelCondAlg( name, pSvcLocator )
{ 
  ATH_MSG_DEBUG( "MDT_RegSelCondAlg::MDT_RegSelCondAlg() " << name );
}




StatusCode MDT_RegSelCondAlg::initialize() {
  ATH_CHECK(MuonRegSelCondAlg::initialize());
  ATH_CHECK(m_cablingKey.initialize());
  return StatusCode::SUCCESS;
}




std::unique_ptr<RegSelSiLUT> MDT_RegSelCondAlg::createTable( const EventContext& ctx, EventIDRange& id_range ) const { 

  SG::ReadCondHandle<MuonMDT_CablingMap> cabling( m_cablingKey, ctx );

  if( !cabling.range( id_range ) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << cabling.key());
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }   

  /// create the new lookup table

  const MuonGM::MuonDetectorManager* manager = nullptr; // again 0 would do as well here 
  
  StatusCode sc = detStore()->retrieve( manager );

  /// can't use ATH_CHECK here as we can;'t return the StatusCode 
  if ( sc.isFailure() ) { 
    ATH_MSG_ERROR( "Could not retrieve MDT Manager for " << name() );
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }
  
  const MdtIdHelper* helper = manager->mdtIdHelper();
  
  /// ne wlookup table 
  std::unique_ptr<RegSelSiLUT> lut = std::make_unique<RegSelSiLUT>();
  

  IdContext ModuleContext = helper->module_context();
    
  std::vector<Identifier>::const_iterator  itr    = helper->module_begin();
  std::vector<Identifier>::const_iterator  idlast = helper->module_end();
  
  for ( ; itr!=idlast; itr++ )  {

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
         
    IdContext mdtChannelContext = helper->channel_context();

    // get the element corresponding to multilayer = 1
    const MuonGM::MdtReadoutElement* mdt1 = manager->getMdtReadoutElement(Id);
    if (mdt1 == NULL) {
      continue;
    }

    Identifier Id2 = helper->channelID(Id, 2, 1, 1);

    // get the element corresponding to multilayer = 2
    const MuonGM::MdtReadoutElement* mdt2 = manager->getMdtReadoutElement(Id2);

    double tubePitch = mdt1->tubePitch();

    int ntlay    = mdt1->getNLayers();
    int ntubesl1 = mdt1->getNtubesperlayer();
    int ntubesl2 = 0;

    if (mdt2 != NULL) ntubesl2 = mdt2->getNtubesperlayer();

    Identifier Idv[4];
    Idv[0] = helper->channelID(Id, 1, 1, 1);
    Idv[1] = helper->channelID(Id, 1, 1, ntubesl1);
    Idv[2] = helper->channelID(Id, 2, ntlay, 1);
    Idv[3] = helper->channelID(Id, 2, ntlay, ntubesl2);

    //      std::cout<<" Number of tube layers "<<ntlay;
    //      std::cout<<" Number of tubes / layer (1 ,2) "<<ntubesl1<<", "<<ntubesl2;
        
    double rmin =  99999999.;
    double rmax = -99999999.;
    double zmin =  99999999.;
    double zmax = -99999999.;
    double emin =  99999999.;
    double emax = -99999999.;
    double phimin =  999999.;
    double phimax = -999999.;

    double zpos21 = 0.;
    Identifier Idsl = helper->channelID(Id, 1, 2, 1);
    if (mdt1->barrel()) {
      zpos21 = (mdt1->tubePos(Idsl)).z()-(mdt1->tubePos(Idv[0])).z();
    }
    else  {
      zpos21 = (mdt1->tubePos(Idsl)).perp()-(mdt1->tubePos(Idv[0])).perp();
    }
        
    /// what is this loop over? This is awful code, this i parameter hides
    /// the "i" from loop over the iterators, or it did until I changed it 
    //  to a more sensibly named "itr" 
    for (int i=0; i<4; i++) {
     
      const MuonGM::MdtReadoutElement* mdt = NULL;
      
      if ( i<2 )  mdt = mdt1;
      else mdt = mdt2;
      if (mdt == NULL) {
	//  std::cout<<" element not found for index i = "<<i<<" --------- "<<std::endl;
	if (i==2) {
	  Idv[2] = helper->channelID(Id, 1, ntlay, 1);
	  mdt = manager->getMdtReadoutElement(Idv[2]);
	}
	else if (i==3) {
	  Idv[3] = helper->channelID(Id, 1, ntlay, ntubesl1);
	  mdt = manager->getMdtReadoutElement(Idv[3]);
	}
	else {
	    //      std::cout<<" Skipping element; i = "<<i<<" ----- "<<std::endl;
	  continue;
	}
                
      }  
          
      Amg::Vector3D mdtPos = mdt->tubePos(Idv[i]);

      Amg::Vector3D mdtPos1 = mdtPos;
      Amg::Vector3D mdtPos2 = mdtPos;

      double scaleMin  = (mdtPos.perp()-tubePitch/2.)/mdtPos.perp();
      double scalePlus = (mdtPos.perp()+tubePitch/2.)/mdtPos.perp();

      if (mdt->barrel()) {
          
	// these are z ranges of the first or last tube layer 
	// mdtPos1.setZ(mdtPos.z()-tubePitch/2.);
	// mdtPos2.setZ(mdtPos.z()+tubePitch/2.);
	mdtPos1[2] = mdtPos.z()-tubePitch/2.;
	mdtPos2[2] = mdtPos.z()+tubePitch/2.;

	// correct the z ranges of the first or last tube layer to account for tube staggering
	if (zpos21 > 1.) {
	    // mdtPos2.setZ(mdtPos2.z()+tubePitch/2.);
	    mdtPos2[2] = mdtPos2.z()+tubePitch/2.;
	}
	else if (zpos21 < -1.) {
	    //mdtPos1.setZ(mdtPos1.z()-tubePitch/2.);
	  mdtPos1[2] = mdtPos1.z()-tubePitch/2.;
	}
            
	if (i<2) {
	  mdtPos1[0] *= scaleMin;
	  mdtPos1[1] *= scaleMin;
	  mdtPos2[0] *= scaleMin;
	  mdtPos2[1] *= scaleMin;
	  // mdtPos1.setPerp(mdtPos.perp()-tubePitch/2.);
	  // mdtPos2.setPerp(mdtPos.perp()-tubePitch/2.);
	}
	else {
	  mdtPos1[0] *= scalePlus;
	  mdtPos1[1] *= scalePlus;
	  mdtPos2[0] *= scalePlus;
	  mdtPos2[1] *= scalePlus;
	  // mdtPos1.setPerp(mdtPos.perp()+tubePitch/2.);
	  // mdtPos2.setPerp(mdtPos.perp()+tubePitch/2.);
	}
      }
      else  { 
	
	// these are z ranges of the first or last tube layer 
	mdtPos1[0] *= scaleMin;
	mdtPos1[1] *= scaleMin;
	mdtPos2[0] *= scalePlus;
	mdtPos2[1] *= scalePlus;
	// mdtPos1.setPerp(mdtPos.perp()-tubePitch/2.);
	// mdtPos2.setPerp(mdtPos.perp()+tubePitch/2.);
	// correct the z ranges of the first or last tube layer to account for tube staggering
	if (zpos21 > 1.) { 
	  scalePlus = (mdtPos2.perp()+tubePitch/2.)/mdtPos2.perp();
	  mdtPos2[0] *= scalePlus;
	  mdtPos2[1] *= scalePlus;
	  // mdtPos2.setPerp(mdtPos2.perp()+tubePitch/2.);
	}
	else if (zpos21 < -1.) {
	  scaleMin  = (mdtPos1.perp()-tubePitch/2.)/mdtPos1.perp();
	  mdtPos1[0] *= scaleMin;
	  mdtPos1[1] *= scaleMin;
	  // mdtPos1.setPerp(mdtPos1.perp()-tubePitch/2.);
	}
	if (i<2) {
	  if (mdt->sideA()){ 
	    // mdtPos1.setZ(mdtPos.z()-tubePitch/2.);
	    // mdtPos2.setZ(mdtPos.z()-tubePitch/2.);
	    mdtPos1[2] = mdtPos.z()-tubePitch/2.;
	    mdtPos2[2] = mdtPos.z()-tubePitch/2.;
	  }
	  else {
	    // mdtPos1.setZ(mdtPos.z()+tubePitch/2.);
	    // mdtPos2.setZ(mdtPos.z()+tubePitch/2.);
	    mdtPos1[2] = mdtPos.z()+tubePitch/2.;
	    mdtPos2[2] = mdtPos.z()+tubePitch/2.;
	  }
	}
	else {
	  if (mdt->sideA()) {
	    // mdtPos1.setZ(mdtPos.z()+tubePitch/2.);
	    // mdtPos2.setZ(mdtPos.z()+tubePitch/2.);
	    mdtPos1[2] = mdtPos.z()+tubePitch/2.;
	    mdtPos2[2] = mdtPos.z()+tubePitch/2.;
	  }
	  else {
	    // mdtPos1.setZ(mdtPos.z()-tubePitch/2.);
	    // mdtPos2.setZ(mdtPos.z()-tubePitch/2.);
	    mdtPos1[2] = mdtPos.z()-tubePitch/2.;
	    mdtPos2[2] = mdtPos.z()-tubePitch/2.;
	  }
	}
      }
      
      double eminMod = 0.;
      double emaxMod = 0.;
      double zminMod = 0.;
      double zmaxMod = 0.;
      double rminMod = 0.;
      double rmaxMod = 0.;
      double dphi    = 0.;

      if (mdt->barrel()) {
	eminMod = mdtPos1.eta(); 
	emaxMod = mdtPos2.eta(); 
        
	zminMod = mdtPos1.z();   
	zmaxMod = mdtPos2.z();

	rminMod = mdtPos1.perp();   
	rmaxMod = mdtPos2.perp();
                
	dphi = atan2(mdt->getSsize()/2., (mdtPos.perp()-tubePitch/2.));
      }            
      else {
	  if (mdt->sideA()) {
	      eminMod = mdtPos2.eta(); 
	      emaxMod = mdtPos1.eta(); 
                                                
	      zminMod = mdtPos2.z();   
	      zmaxMod = mdtPos1.z();   

	      rminMod = mdtPos1.perp();   
	      rmaxMod = mdtPos2.perp();   
	  }
	  else {
	      eminMod = mdtPos1.eta(); 
	      emaxMod = mdtPos2.eta(); 
                                                
	      zminMod = mdtPos1.z();   
	      zmaxMod = mdtPos2.z();                       

	      rminMod = mdtPos1.perp();   
	      rmaxMod = mdtPos2.perp();                       
	  }
	  
	  dphi = atan2(mdt->tubeLength(Idv[i])/2., (mdtPos.perp()-tubePitch/2.));
      }

      double pminMod = mdtPos.phi() - dphi;
      double pmaxMod = mdtPos.phi() + dphi;

      if (zminMod < zmin) {
	zmin = zminMod;
      }

      if (zmaxMod > zmax) {
	zmax = zmaxMod;
      }

      if (pminMod < phimin) phimin = pminMod;
      if (pmaxMod > phimax) phimax = pmaxMod;
      if (eminMod < emin) emin = eminMod;
      if (emaxMod > emax) emax = emaxMod;
      if (rminMod < rmin) rmin = rminMod;
      if (rmaxMod > rmax) rmax = rmaxMod;
      //  std::cout<<" Module emin - emax "<<emin<<" "<<emax<<" phimin - phimax "<<phimin<<" "<<phimax<<std::endl;
            
    }
    
    // here define the eta and phi(0-2*pi) ranges
    if (phimin<0) phimin = phimin + 2*M_PI;
    if (phimax<0) phimax = phimax + 2*M_PI;
    
    // calculate 4 sub detectors for the mdt, fore and aft barrel, 
    // and fore and aft endcaps

    if ( mdt1->barrel() ) { 
      if      ( mdt1->sideA() ) detid =  1;
      else if ( mdt1->sideC() ) detid = -1;
    }
    else { 
      if      ( mdt1->sideA() ) detid =  2;
      else if ( mdt1->sideC() ) detid = -2;
    }
    
    
    // std::cout << "detid " << detid;
    
    // if ( mdt1->barrel() ) detid =  0;
    // if ( mdt1->sideA() )  detid =  1;
    // if ( mdt1->sideC() )  detid = -1;
    
    // std::cout << " -> " << detid << std::endl;

    uint32_t RobId = cabling->getROBId(Idhash);
    
    RegSelModule m( zmin, zmax, rmin, rmax, phimin, phimax, layerid, detid, RobId, Idhash );
    
    lut->addModule( m );

  }
    
  
  lut->initialise();

  return lut;

}







