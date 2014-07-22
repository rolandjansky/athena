//
//   @file    RPC_RegionSelectorTable.cxx         
//   
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: RPC_RegionSelectorTable.cxx, v0.0   Tue  4 Aug 2009 16:38:38 BST sutt $



#include "MuonRegionSelector/RPC_RegionSelectorTable.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "Identifier/IdentifierHash.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;


#include "RegSelLUT/StoreGateIDRS_ClassDEF.h" 

#include "RegSelLUT/RegSelModule.h" 
#include "RegSelLUT/RegSelSiLUT.h" 



#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h" 

#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"

#include "RegSelLUT/RegSelSiLUT.h"

#include <iostream>
#include <vector>
#include <fstream>





RPC_RegionSelectorTable::RPC_RegionSelectorTable(const std::string& type, 
						 const std::string& name,
						 const IInterface*  parent)
  :  AthAlgTool(type,name,parent),  m_regionLUT(NULL)
{
  declareInterface<IRegionIDLUT_Creator>(this);
  msg(MSG::INFO) << "RPC_RegionSelectorTable::RPC_RegionSelectorTable()" << endreq;
}


RPC_RegionSelectorTable::~RPC_RegionSelectorTable() { 
  if ( m_regionLUT ) delete m_regionLUT;
}


StatusCode RPC_RegionSelectorTable::initialize() { 
  msg(MSG::INFO) << "initialize() RPC_RegionSelectorTable" << endreq;
  return createTable();
}


StatusCode RPC_RegionSelectorTable::finalize() { 
  msg(MSG::INFO) << "finalize()" << endreq;
  return StatusCode::SUCCESS;
}


RegSelSiLUT* RPC_RegionSelectorTable::getLUT() const {
  return m_regionLUT;
} 



  
StatusCode RPC_RegionSelectorTable::createTable() { 

  const MuonGM::MuonDetectorManager*	p_MuonMgr = NULL;
  
  StatusCode status = detStore()->retrieve( p_MuonMgr );
	
  const RpcIdHelper*  p_IdHelper = p_MuonMgr->rpcIdHelper();
  
  std::vector<Identifier>::const_iterator  idfirst = p_IdHelper->module_begin();
  std::vector<Identifier>::const_iterator  idlast =  p_IdHelper->module_end();
 
  IdContext ModuleContext = p_IdHelper->module_context();

  std::cout << "MuonCreator::buildRPCTable()" << std::endl;
  
  RegSelSiLUT* rpclut = new RegSelSiLUT();
  
  for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)  {

        Identifier Id = *i;
	IdentifierHash Idhash;
	//	int gethash_code = 
	p_IdHelper->get_hash(Id, Idhash, &ModuleContext);
	
	ExpandedIdentifier exp_id;
	//      int expid = 
	p_IdHelper->get_expanded_id( Id, exp_id, &ModuleContext);
       	
	int detid   = ( exp_id[2]<0 ? -1 : 1 );
	int layerid = exp_id[1];
	if ( layerid==0 ) layerid = 11;  // this line should never be executed 
	
	MuonGM::RpcReadoutSet Set(p_MuonMgr, Id);
        //    int nmod = Set.NreadoutElements();
	//    std::cout<<" Number of modules  in this RpcSet "<<nmod<<std::endl;
	//    std::cout<<" Number of doubletZ in this RpcSet "<<Set.NdoubletZ()<<std::endl;
	//    std::cout<<" Number of doubletR in this RpcSet "<<Set.NdoubletR()<<std::endl;
        int ndbz = Set.NdoubletZ();
	
	//    std::cout<<" Number of modules in Phi/DoubletZ: ";
	//    for (int i=1; i<=ndbz; i++) std::cout<<Set.NPhimodules(i)<<" ";
	//    std::cout<<std::endl;

	//    std::cout<<" Number of modules  in this RpcSet "<<nmod<<" Number of doubletZ in this RpcSet "<<Set.NdoubletZ()<<std::endl;
	//    std::cout<<" Number of modules in Phi/DoubletZ: ";
	//    for (int i=1; i<=ndbz; i++) std::cout<<Set.NPhimodules(i)<<" ";
	//    std::cout<<std::endl;

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
        
	    const MuonGM::RpcReadoutElement* _rpcold = NULL;
	    int ndbp = Set.NPhimodules(dbz);
	  
	    for (int dbp=1; dbp<=ndbp; dbp++) {
	    
	        const MuonGM::RpcReadoutElement* _rpc = Set.readoutElement(dbz, dbp);
		
		if ( _rpc != _rpcold ) {
		
                    // here a new module

		    nmodules ++;
                    _rpcold = _rpc;

                    Amg::Vector3D rpcPos = _rpc->center();
                    double zminMod = rpcPos.z()-0.5*_rpc->getZsize();
                    double zmaxMod = rpcPos.z()+0.5*_rpc->getZsize();

		    //double rcen = std::sqrt(rpcPos.r()*rpcPos.r()-rpcPos.z()*rpcPos.z());
		    double rcen = std::sqrt(rpcPos.mag()*rpcPos.mag()-rpcPos.z()*rpcPos.z());

		    double rminMod = rcen-0.5*_rpc->getRsize();
                    double rmaxMod = rcen+0.5*_rpc->getRsize();
		    
                    double dphi = atan2(_rpc->getSsize()/2.,rpcPos.perp());
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

        if (phimin<0) phimin = phimin + 2*M_PI;
        if (phimax<0) phimax = phimax + 2*M_PI;
	// don't need eta, use the more flexible z, r and phi now
        // double theta_min = Pzmin.theta();
	//  double eta_min = Pzmin.eta();
	//  double eta_max = Pzmax.eta();

	RegSelModule m( zmin, zmax, rmin, rmax, phimin, phimax, layerid, detid, 0, Idhash );

	rpclut->addModule( m );
    
  }
  
  rpclut->initialise();
  // rpclut->write("rpc.map");

  m_regionLUT = rpclut;

  return StatusCode::SUCCESS;
}

  










