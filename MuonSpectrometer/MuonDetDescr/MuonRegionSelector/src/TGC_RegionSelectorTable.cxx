//
//   @file    TGC_RegionSelectorTable.cxx         
//   
//
//                   
// 
//   Copyright (x1C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TGC_RegionSelectorTable.cxx, v0.0   Tue  4 Aug 2009 16:38:38 BST sutt $



#include "MuonRegionSelector/TGC_RegionSelectorTable.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "Identifier/IdentifierHash.h"

#include <iostream>
#include <fstream>
#include <string>


#include "RegSelLUT/StoreGateIDRS_ClassDEF.h" 

#include "RegSelLUT/RegSelModule.h" 
#include "RegSelLUT/RegSelSiLUT.h" 



#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h" 
//#include "MuonReadoutGeometry/MdtReadoutElement.h"
//#include "MuonReadoutGeometry/CscReadoutElement.h"
//#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
//#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonStation.h"

#include "RegSelLUT/RegSelSiLUT.h"
#include "MuonTGC_Cabling/MuonTGC_CablingSvc.h"

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;




TGC_RegionSelectorTable::TGC_RegionSelectorTable(const std::string& type, 
						 const std::string& name,
						 const IInterface*  parent)
  :  AthAlgTool(type,name,parent),  
     m_tgcCabling(0),
     m_regionLUT(0) 
{
  declareInterface<IRegionIDLUT_Creator>(this);
}


TGC_RegionSelectorTable::~TGC_RegionSelectorTable() { 
  if ( m_regionLUT ) {
    delete m_regionLUT;
    m_regionLUT =0;
  }
}


StatusCode TGC_RegionSelectorTable::initialize() { 
  msg(MSG::INFO) << "initialize() TGC_RegionSelectorTable" << endmsg;

   // locate new cabling service    
  StatusCode sc = service("MuonTGC_CablingSvc",m_tgcCabling);
  if (sc.isFailure() ){
    msg() << MSG::ERROR << "Could not retrieve " << m_tgcCabling << endmsg;
    return sc;
  }
  msg() << MSG::DEBUG << "Retrieved service " << m_tgcCabling << endmsg;
  
  return createTable();
}


StatusCode TGC_RegionSelectorTable::finalize() { 
  msg(MSG::INFO) << "finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


RegSelSiLUT* TGC_RegionSelectorTable::getLUT() const {
  return m_regionLUT;
} 



  
StatusCode TGC_RegionSelectorTable::createTable() { 

  const MuonGM::MuonDetectorManager*	p_MuonMgr = NULL;
  
  StatusCode status = detStore()->retrieve( p_MuonMgr );
	
  RegSelSiLUT* tgclut = new RegSelSiLUT();

  const TgcIdHelper*  p_IdHelper = p_MuonMgr->tgcIdHelper();

  std::vector<Identifier>::const_iterator idfirst = p_IdHelper->module_begin();
  std::vector<Identifier>::const_iterator idlast  = p_IdHelper->module_end();

  const IdContext ModuleContext = p_IdHelper->module_context();

  int maxRodId = 0;
  int maxSswId = 0;
  int maxSbloc = 0;
  int minChannelId = 0;
  int maxChannelId = 0;
  m_tgcCabling->getReadoutIDRanges(maxRodId, maxSswId, maxSbloc, minChannelId, maxChannelId);

  for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++) {
   
      Identifier     Id = *i;
      IdentifierHash hashId;

      p_IdHelper->get_hash( Id,hashId,&ModuleContext);
   
      ExpandedIdentifier exp_id;
      if (p_IdHelper->get_expanded_id( Id, exp_id, &ModuleContext)) {
        ATH_MSG_DEBUG("Failed retrieving ExpandedIdentifier for PRD Identifier = " << Id.getString() << ". Skipping to the next PRD.");
        continue;
      }
      
      int detid   = ( exp_id[2]<0 ? -1 : 1 );
      int layerid = exp_id[1]+1;

      const MuonGM::TgcReadoutElement* tgc = p_MuonMgr->getTgcReadoutElement(Id);
      if (tgc == NULL)  {
	  continue;
      }
	        
      int gapMin = p_IdHelper->gasGapMin(Id);
      int gapMax = p_IdHelper->gasGapMax(Id);

      Identifier chId;
      chId = p_IdHelper -> channelID(Id,gapMin,0,1);
      const int chmax = p_IdHelper -> channelMax(chId);
      Amg::Vector3D posmax = tgc->channelPos(gapMin,0,chmax); // gapMax gives posmax!
      chId = p_IdHelper -> channelID(Id,gapMax,0,1);
      const int chmin = p_IdHelper -> channelMin(chId);
      Amg::Vector3D posmin = tgc->channelPos(gapMax,0,chmin); // gapMin gives posmin!

      // caliculation based on max/min channels in a module
      //	  etamin = -logf(tan(atan(posmin.perp()/fabs(posmin.z()))/2.));
      //	  etamax = -logf(tan(atan(posmax.perp()/fabs(posmax.z()))/2.));

      // caliculation based on active sensitive area
      
      Amg::Vector3D posctr   = tgc->globalPosition();
      double activeheight = tgc->length();

      double etamin = -logf(tan(atan((posctr.perp()-0.5*activeheight)/fabs(posmin.z()))*0.5));
      double etamax = -logf(tan(atan((posctr.perp()+0.5*activeheight)/fabs(posmax.z()))*0.5));

      double zmin = posmin.z();
      double zmax = posmax.z();

      double rmin = posctr.perp()-0.5*activeheight;
      double rmax = posctr.perp()+0.5*activeheight;


      if (p_IdHelper->stationEta(Id) < 0) {
	 etamin = -etamin;
	 etamax = -etamax;
      }


      // caliculation based on max/min channels in a module
      //	  const Identifier chId1  = p_IdHelper -> channelID(Id,2,1,1);
      //	  chmax = p_IdHelper -> channelMax(chId1);
      //	  chmin = p_IdHelper -> channelMin(chId1);
      //	  posmin = tgc->channelPos(2,1,chmin);
      //	  posmax = tgc->channelPos(2,1,chmax);
      //	  phimin = atan2f(posmin.y(),posmin.x());
      //	  phimax = atan2f(posmax.y(),posmax.x());
      //	  if (phimin < 0) phimin += 2.*3.141592653589793238;
      //	  if (phimax < 0) phimax += 2.*3.141592653589793238;

      // caliculation based on active sensitive area
      double activelongside = tgc->longWidth()-tgc->frameXwidth()*2.;
      double phimin = atan2f(posctr.y(),posctr.x()) - atan2f(activelongside/2.,posctr.perp()+activeheight/2.);
      double phimax = atan2f(posctr.y(),posctr.x()) + atan2f(activelongside/2.,posctr.perp()+activeheight/2.);
      if (phimin < 0) phimin += 2.*M_PI;
      if (phimax < 0) phimax += 2.*M_PI;

      // get ROB id
      int subDetectorId = 0; // 0x67 (A side) or 0x68 (C side)
      int rodId = 0; // 1-12
      m_tgcCabling->getReadoutIDfromElementID(Id, subDetectorId, rodId);
      //int isAside = (subDetectorId==0x67) ? 0 : 1;
      //uint32_t robId = static_cast<uint32_t>(isAside*(maxRodId) + rodId -1); // 0-23
      // using Source ID :see Muon::TGC_Hid2RESrcID::getRodID()/getRobId()
      uint32_t robId =  ( ((0x0ff) & subDetectorId)<<16 ) | (rodId);
      // end part to get ROB id

      RegSelModule m( zmin, zmax, rmin, rmax, phimin, phimax, layerid, detid, robId, hashId );
      tgclut->addModule( m );

  }

  tgclut->initialise(); 
  //  tgclut->write("tgc.map");

  m_regionLUT = tgclut;

  return StatusCode::SUCCESS;
}
