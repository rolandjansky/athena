/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    RPC_RegionSelectorTable.cxx         
//   
//
//                   
// 
//
//   $Id: RPC_RegionSelectorTable.cxx, v0.0   Tue  4 Aug 2009 16:38:38 BST sutt $


#include "MuonRegionSelector/RPC_RegionSelectorTable.h"
#include "MuonRegionSelector/RPC_RegSelLUT.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "Identifier/IdentifierHash.h"

#include "RegSelLUT/StoreGateIDRS_ClassDEF.h" 
#include "RegSelLUT/RegSelModule.h" 
#include "RegSelLUT/RegSelSiLUT.h" 

#include "GeoPrimitives/GeoPrimitives.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h" 
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"

#include <vector>



RPC_RegionSelectorTable::RPC_RegionSelectorTable(const std::string& type, 
						 const std::string& name,
						 const IInterface*  parent)
  :  AthAlgTool(type,name,parent), m_regionLUT(NULL), m_rpcCabling(NULL)
{
  declareInterface<IRegionIDLUT_Creator>(this);
  ATH_MSG_INFO("RPC_RegionSelectorTable::RPC_RegionSelectorTable()");
}


RPC_RegionSelectorTable::~RPC_RegionSelectorTable() { 
  if ( m_regionLUT ) delete m_regionLUT;
}


StatusCode RPC_RegionSelectorTable::initialize() { 
  ATH_MSG_INFO("initialize() RPC_RegionSelectorTable");
  
  //get RPC CablingSvc Server from 
  const IRPCcablingServerSvc* RpcCabGet = 0;
  CHECK( service("RPCcablingServerSvc", RpcCabGet) );
  ATH_MSG_VERBOSE("RPCcablingServerSvc retrieved");
  
  //get RPC CablingSvc from the server
  CHECK( RpcCabGet->giveCabling(m_rpcCabling) );
  if (msgLvl(MSG::VERBOSE)) {
    const Service* castedRpcCabling = dynamic_cast<const Service*>(m_rpcCabling);
    if (castedRpcCabling) 
      ATH_MSG_VERBOSE("RPC CablingSvc obtained: " << castedRpcCabling->name());
    else {
      ATH_MSG_ERROR("Failed dynamic casting of m_rpcCabling to (const Service*)");
      return StatusCode::FAILURE;
    }
  }

  return createTable();
}


StatusCode RPC_RegionSelectorTable::finalize() { 
  ATH_MSG_INFO("finalize()");
  return StatusCode::SUCCESS;
}


RegSelSiLUT* RPC_RegionSelectorTable::getLUT() {
  return m_regionLUT;
} 


StatusCode RPC_RegionSelectorTable::createTable() { 

  const MuonGM::MuonDetectorManager*	p_MuonMgr = nullptr;
  
  CHECK( detStore()->retrieve( p_MuonMgr ) );
	
  const RpcIdHelper*  p_IdHelper = p_MuonMgr->rpcIdHelper();
  
  std::vector<Identifier>::const_iterator  idfirst = p_IdHelper->module_begin();
  std::vector<Identifier>::const_iterator  idlast =  p_IdHelper->module_end();
 
  IdContext ModuleContext = p_IdHelper->module_context();

  ATH_MSG_INFO("createTable()");
  
  RegSelSiLUT* rpclut = new RPC_RegSelLUT();
  
  //loop over modules (PrepRawData collections)
  for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)  {

    Identifier prdId = *i;
    IdentifierHash prdHashId;
    int gethash_code = p_IdHelper->get_hash(prdId, prdHashId, &ModuleContext);
    if (gethash_code != 0 ) {
      ATH_MSG_DEBUG("Failed retrieving IdentifierHash for PRD Identifier = " << prdId.getString() << ". Skipping to the next PRD.");
      continue;
    }
    
    std::vector<uint32_t> robIds;
    StatusCode sc = m_rpcCabling->giveROB_fromPRD(prdHashId, robIds);
    if ( !sc.isSuccess() ) {
      REPORT_ERROR(sc);
      delete rpclut;
      return sc;
    };
    
    if (robIds.size() < 1) {
      ATH_MSG_DEBUG("There is no ROB associated with the PRD HashId = " << (unsigned int)prdHashId << ". Skipping to the next PRD.");
      continue;
    }
    if (robIds.size()!=1 && robIds.size()!=2 && robIds.size()!=4) {
      ATH_MSG_WARNING("Unhandled number of ROBs per one PRD. Expected 1, 2 or 4, got " << robIds.size() << ". Omitting the PRD HashId = " << (unsigned int)prdHashId);
      continue;
    }
    
    ExpandedIdentifier exp_id;
    if (p_IdHelper->get_expanded_id( prdId, exp_id, &ModuleContext)) {
      ATH_MSG_DEBUG("Failed retrieving ExpandedIdentifier for PRD Identifier = " << prdId.getString() << ". Skipping to the next PRD.");
      continue;
    }
       	
    int detid   = ( exp_id[2]<0 ? -1 : 1 );
    int layerid = exp_id[1];
    if ( layerid==0 ) layerid = 11;  // this line should never be executed 
	
    MuonGM::RpcReadoutSet Set(p_MuonMgr, prdId);
    //int nmod = Set.NreadoutElements();
    int ndbz = Set.NdoubletZ();

    //std::cout<<" Number of modules  in this RpcSet "<<nmod<<" Number of doubletZ in this RpcSet "<<Set.NdoubletZ()<<std::endl;
    //std::cout<<" Number of modules in Phi/DoubletZ: ";
    //for (int i=1; i<=ndbz; i++) std::cout<<Set.NPhimodules(i)<<" ";
    //std::cout<<std::endl;

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

          //double rcen = std::sqrt(rpcPos.r()*rpcPos.r()-rpcPos.z()*rpcPos.z());
          double rcen = std::sqrt(rpcPos.mag()*rpcPos.mag()-rpcPos.z()*rpcPos.z());
  
          double rminMod = rcen-0.5*rpc->getRsize();
          double rmaxMod = rcen+0.5*rpc->getRsize();

          double dphi = atan2(rpc->getSsize()/2.,rpcPos.perp());
          double pminMod = rpcPos.phi() - dphi;
          double pmaxMod = rpcPos.phi() + dphi;
          
          if (zminMod < zmin) {
            zmin = zminMod;
            Pzmin = rpcPos;
            //Pzmin.setZ(zmin);
            Pzmin[2] = zmin;
          }
          if (zmaxMod > zmax) {
            zmax = zmaxMod;
            Pzmax = rpcPos;
            //Pzmax.setZ(zmax);
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
      rpclut->addModule( m1 );
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
      rpclut->addModule( m1 );
      rpclut->addModule( m2 );
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
      rpclut->addModule( m1 );
      rpclut->addModule( m2 );
      rpclut->addModule( m3 );
      rpclut->addModule( m4 );
    }
  } //end of loop over modules (PrepRawData collections)
  
  rpclut->initialise();
  //rpclut->write("rpc.map");

  m_regionLUT = rpclut;

  return StatusCode::SUCCESS;
}
