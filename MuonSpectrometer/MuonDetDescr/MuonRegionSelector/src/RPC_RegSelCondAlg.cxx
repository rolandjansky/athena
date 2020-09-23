/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 **   @file   RPC_RegSelCondAlg.cxx         
 **            
 **           conditions algorithm to create the Si detector 
 **           lookup tables    
 **            
 **   @author sutt
 **   @date   Sun 22 Sep 2019 10:21:50 BST
 **
 **
 **/

#include "RPC_RegSelCondAlg.h"

#include "GaudiKernel/EventIDRange.h"
#include "StoreGate/WriteCondHandle.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "Identifier/IdentifierHash.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h" 
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonStation.h"

RPC_RegSelCondAlg::RPC_RegSelCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  MuonRegSelCondAlg( name, pSvcLocator )
{ 
  ATH_MSG_DEBUG( "RPC_RegSelCondAlg::RPC_RegSelCondAlg() " << name );
}

StatusCode RPC_RegSelCondAlg::initialize() {
  ATH_CHECK(MuonRegSelCondAlg::initialize());
  ATH_CHECK(m_rpcReadKey.initialize());
  return StatusCode::SUCCESS;
}

std::unique_ptr<RegSelSiLUT> RPC_RegSelCondAlg::createTable( const EventContext& ctx, EventIDRange& id_range ) const { 

  SG::ReadCondHandle<RpcCablingCondData> cablingCondData( m_rpcReadKey, ctx );

  if( !cablingCondData.range( id_range ) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << cablingCondData.key());
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }   


  const RpcCablingCondData* cabling{*cablingCondData};

  const MuonGM::MuonDetectorManager* manager = nullptr;

  if ( detStore()->retrieve( manager ).isFailure() ) { 
    ATH_MSG_ERROR( "Could not retrieve RPC Manager for " << name() );
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }

  const RpcIdHelper*  helper = manager->rpcIdHelper();
  
  std::vector<Identifier>::const_iterator  idfirst = helper->module_begin();
  std::vector<Identifier>::const_iterator  idlast =  helper->module_end();
 
  IdContext ModuleContext = helper->module_context();

  ATH_MSG_DEBUG("createTable()");
  
  std::unique_ptr<RegSelSiLUT> lut = std::make_unique<RegSelSiLUT>();
 
 
  //loop over modules (PrepRawData collections)
  for ( std::vector<Identifier>::const_iterator itr=idfirst  ;  itr!=idlast  ;  itr++  )  {

    Identifier prdId = *itr;
    IdentifierHash prdHashId;
    int gethash_code = helper->get_hash(prdId, prdHashId, &ModuleContext);
    if (gethash_code != 0 ) {
      ATH_MSG_DEBUG("Failed retrieving IdentifierHash for PRD Identifier = " << prdId.getString() << ". Skipping to the next PRD.");
      continue;
    }
    
    std::vector<uint32_t> robIds;
    if ( (cabling->giveROB_fromPRD(prdHashId, robIds)).isFailure() ) { 
      ATH_MSG_ERROR( "RegSelCondAlg_RPC could not get ROBid" );
      return std::unique_ptr<RegSelSiLUT>(nullptr);
    }

    
    if (robIds.size() < 1) {
      ATH_MSG_DEBUG("There is no ROB associated with the PRD HashId = " << (unsigned int)prdHashId << ". Skipping to the next PRD.");
      continue;
    }
    if (robIds.size()!=1 && robIds.size()!=2 && robIds.size()!=4) {
      ATH_MSG_WARNING("Unhandled number of ROBs per one PRD. Expected 1, 2 or 4, got " << robIds.size() << ". Omitting the PRD HashId = " << (unsigned int)prdHashId);
      continue;
    }
    
    ExpandedIdentifier exp_id;
    if ( helper->get_expanded_id( prdId, exp_id, &ModuleContext) ) {
      ATH_MSG_DEBUG("Failed retrieving ExpandedIdentifier for PRD Identifier = " << prdId.getString() << ". Skipping to the next PRD.");
      continue;
    }
    
    int detid   = ( exp_id[2]<0 ? -1 : 1 );
    int layerid = exp_id[1];
    if ( layerid==0 ) layerid = 11;  // this line should never be executed 
    
    MuonGM::RpcReadoutSet Set( manager, prdId );
    int ndbz = Set.NdoubletZ();

    double zmin =  99999999;
    double zmax = -99999999;
    double rmin =  99999999;
    double rmax = -99999999;
    double phimin =  999999;
    double phimax = -999999;
    Amg::Vector3D Pzmin;
    Amg::Vector3D Pzmax;
    unsigned nmodules = 0;

    for (int dbz=1; dbz<=ndbz; dbz++) {
        
      const MuonGM::RpcReadoutElement* rpcold = NULL;
      int ndbp = Set.NPhimodules(dbz);
        
      for (int dbp=1; dbp<=ndbp; dbp++) {

        const MuonGM::RpcReadoutElement* rpc = Set.readoutElement(dbz, dbp);
	
        if ( rpc != rpcold ) {
          
          // here a new module
          nmodules ++;
          rpcold = rpc;

	  Amg::Vector3D rpcPos = rpc->center();
          double zminMod = rpcPos.z()-0.5*rpc->getZsize();
          double zmaxMod = rpcPos.z()+0.5*rpc->getZsize();

          double rcen = std::sqrt(rpcPos.mag()*rpcPos.mag()-rpcPos.z()*rpcPos.z());
  
          double rminMod = rcen-0.5*rpc->getRsize();
          double rmaxMod = rcen+0.5*rpc->getRsize();

          double dphi = std::atan2(rpc->getSsize()/2,rpcPos.perp());
          double pminMod = rpcPos.phi() - dphi;
          double pmaxMod = rpcPos.phi() + dphi;
          
          if (zminMod < zmin) {
            zmin = zminMod;
            Pzmin = rpcPos;
            Pzmin[2] = zmin;
          }
          if (zmaxMod > zmax) {
            zmax = zmaxMod;
            Pzmax = rpcPos;
            Pzmax[2] = zmax;
          }
          if (rminMod < rmin) {
            rmin = rminMod;
          }
          if (rmaxMod > rmax) {
            rmax = rmaxMod;
          }
          if (pminMod < phimin) phimin = pminMod;
          if (pmaxMod > phimax) phimax = pmaxMod;
          
        }
      }
    }

    double phiMinFirst  = phimin;
    double phiMaxFirst  = 0.5*(phimin+phimax);
    double phiMinSecond = 0.5*(phimin+phimax);
    double phiMaxSecond = phimax;

    if (phiMinFirst < 0)  phiMinFirst  += 2*M_PI;
    if (phiMaxFirst < 0)  phiMaxFirst  += 2*M_PI;
    if (phiMinSecond < 0) phiMinSecond += 2*M_PI;
    if (phiMaxSecond < 0) phiMaxSecond += 2*M_PI;


    if (robIds.size()==1) {
      IdentifierHash lowerHashId(  (1<<16) | (unsigned int)prdHashId );
      
      RegSelModule m1( zmin, zmax, rmin, rmax, phiMinFirst, phiMaxSecond, layerid, detid, robIds[0], lowerHashId );
      lut->addModule( m1 );
    }
    //the code below exploits the fact that ROB Ids are sorted!!! They go in order: SIDE A, ROBs 0..15, then SIDE C ROBs 0..15
    //it is also assumed that if there are 4 ROBs, then zmin<0 and zmax>0
    else if (robIds.size()==2) {
      uint32_t lowerRobId=0;
      uint32_t higherRobId=0;
      if ( ((robIds[0] & 0xff) == 0) && ((robIds[1] & 0xff) == 15) ) { //the two ROBs around phi=0
        lowerRobId  = robIds[1];
        higherRobId = robIds[0];
      }
      else {
        lowerRobId  = robIds[0];
        higherRobId = robIds[1];
      }
      IdentifierHash lowerHashId(  (1<<16) | (unsigned int)prdHashId );
      IdentifierHash higherHashId( (2<<16) | (unsigned int)prdHashId );

      RegSelModule m1( zmin, zmax, rmin, rmax, phiMinFirst,  phiMaxFirst,  layerid, detid, lowerRobId,  lowerHashId  );
      RegSelModule m2( zmin, zmax, rmin, rmax, phiMinSecond, phiMaxSecond, layerid, detid, higherRobId, higherHashId );
      lut->addModule( m1 );
      lut->addModule( m2 );
    }
    else if (robIds.size()==4) { //this is the case only for two BOG chambers at z=0 read by ROBs 10-11 and 12-13, each on both sides A and C
      uint32_t sideA_lowerRobId  = robIds[0];
      uint32_t sideA_higherRobId = robIds[1];
      uint32_t sideC_lowerRobId  = robIds[2];
      uint32_t sideC_higherRobId = robIds[3];
      IdentifierHash sideA_lowerHashId(  (1<<16) | (unsigned int)prdHashId );
      IdentifierHash sideA_higherHashId( (2<<16) | (unsigned int)prdHashId );
      IdentifierHash sideC_lowerHashId(  (3<<16) | (unsigned int)prdHashId );
      IdentifierHash sideC_higherHashId( (4<<16) | (unsigned int)prdHashId );
      
      RegSelModule m1( 0, zmax, rmin, rmax, phiMinFirst,  phiMaxFirst,  layerid, detid, sideA_lowerRobId,  sideA_lowerHashId  );
      RegSelModule m2( 0, zmax, rmin, rmax, phiMinSecond, phiMaxSecond, layerid, detid, sideA_higherRobId, sideA_higherHashId );
      RegSelModule m3( zmin, 0, rmin, rmax, phiMinFirst,  phiMaxFirst,  layerid, detid, sideC_lowerRobId,  sideC_lowerHashId  );
      RegSelModule m4( zmin, 0, rmin, rmax, phiMinSecond, phiMaxSecond, layerid, detid, sideC_higherRobId, sideC_higherHashId );
      lut->addModule( m1 );
      lut->addModule( m2 );
      lut->addModule( m3 );
      lut->addModule( m4 );
    }
  } //end of loop over modules (PrepRawData collections)
  
  lut->initialise();

  return lut;

}
