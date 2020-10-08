/**
 **   @file   TGC_RegSelCondAlg.cxx         
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
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonStation.h"

#include "MuonTGC_Cabling/MuonTGC_CablingSvc.h"

#include "TGC_RegSelCondAlg.h"



TGC_RegSelCondAlg::TGC_RegSelCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  MuonRegSelCondAlg( name, pSvcLocator )
{ 
  ATH_MSG_DEBUG( "TGC_RegSelCondAlg::TGC_RegSelCondAlg() " << name );
}



StatusCode TGC_RegSelCondAlg::initialize() {
  ATH_CHECK(MuonRegSelCondAlg::initialize());
  ATH_CHECK(m_mdtCablingKey.initialize());
  return StatusCode::SUCCESS;
}



std::unique_ptr<RegSelSiLUT> TGC_RegSelCondAlg::createTable( const EventContext& ctx, EventIDRange& id_range ) const { 

  SG::ReadCondHandle<MuonMDT_CablingMap> mdtCabling( m_mdtCablingKey, ctx );

  if( !mdtCabling.range( id_range ) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << mdtCabling.key());
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }   


  /// now get the TGC cabling service ...

  const MuonTGC_CablingSvc*   cabling = nullptr;

  if ( service( "MuonTGC_CablingSvc", cabling ).isFailure() ) { 
    ATH_MSG_ERROR( "Could not retrieve TGC cabling for " << name() );
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }

  /// now get the TGC detector manager ...

  const MuonGM::MuonDetectorManager* manager = nullptr;
  
  if ( (detStore()->retrieve( manager ).isFailure() ) ) { 
    ATH_MSG_ERROR( "Could not retrieve TGC Manager for " << name() );
    return std::unique_ptr<RegSelSiLUT>(nullptr);
  }

  const TgcIdHelper*  helper = manager->tgcIdHelper();
  
  std::vector<Identifier>::const_iterator  idfirst = helper->module_begin();
  std::vector<Identifier>::const_iterator  idlast =  helper->module_end();
 
  const IdContext ModuleContext = helper->module_context();

  ATH_MSG_DEBUG("createTable()");
  
  std::unique_ptr<RegSelSiLUT> lut = std::make_unique<RegSelSiLUT>();

  int maxRodId = 0;
  int maxSswId = 0;
  int maxSbloc = 0;
  int minChannelId = 0;
  int maxChannelId = 0;
  cabling->getReadoutIDRanges(maxRodId, maxSswId, maxSbloc, minChannelId, maxChannelId);

  for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++) {
   
    Identifier     Id = *i;
    IdentifierHash hashId;

    helper->get_hash( Id,hashId,&ModuleContext);
   
    ExpandedIdentifier exp_id;
    if (helper->get_expanded_id( Id, exp_id, &ModuleContext)) {
      ATH_MSG_DEBUG("Failed retrieving ExpandedIdentifier for PRD Identifier = " << Id.getString() << ". Skipping to the next PRD.");
      continue;
    }
      
    int detid   = ( exp_id[2]<0 ? -1 : 1 );
    int layerid = exp_id[1]+1;

    const MuonGM::TgcReadoutElement* tgc = manager->getTgcReadoutElement(Id);
    if (tgc == NULL)  {
      continue;
    }
            
    int gapMin = helper->gasGapMin(Id);
    int gapMax = helper->gasGapMax(Id);

    Identifier chId;
    chId = helper -> channelID(Id,gapMin,0,1);
    const int chmax = helper -> channelMax(chId);
    Amg::Vector3D posmax = tgc->channelPos(gapMin,0,chmax); // gapMax gives posmax!
    chId = helper -> channelID(Id,gapMax,0,1);
    const int chmin = helper -> channelMin(chId);
    Amg::Vector3D posmin = tgc->channelPos(gapMax,0,chmin); // gapMin gives posmin!

    // caliculation based on max/min channels in a module
    //  etamin = -logf(tan(atan(posmin.perp()/fabs(posmin.z()))/2.));
    //  etamax = -logf(tan(atan(posmax.perp()/fabs(posmax.z()))/2.));

    // caliculation based on active sensitive area
      
    Amg::Vector3D posctr = tgc->globalPosition();
    double activeheight  = tgc->length();

    double etamin = -logf(std::tan(std::atan((posctr.perp()-0.5*activeheight)/std::fabs(posmin.z()))*0.5));
    double etamax = -logf(std::tan(std::atan((posctr.perp()+0.5*activeheight)/std::fabs(posmax.z()))*0.5));

    double zmin = posmin.z();
    double zmax = posmax.z();

    double rmin = posctr.perp()-0.5*activeheight;
    double rmax = posctr.perp()+0.5*activeheight;


    if (helper->stationEta(Id) < 0) {
      etamin = -etamin;
      etamax = -etamax;
    }


    // caliculation based on active sensitive area
    double activelongside = tgc->longWidth()-tgc->frameXwidth()*2.;
    /// Should use std::atan2 etc, but atan2f is not definied in cmath, so for backwards 
    /// compatability we are forced to use the old versions 
    double phimin = atan2f(posctr.y(),posctr.x()) - atan2f(activelongside/2.,posctr.perp()+activeheight/2.);
    double phimax = atan2f(posctr.y(),posctr.x()) + atan2f(activelongside/2.,posctr.perp()+activeheight/2.);

    if (phimin < 0) phimin += 2.*M_PI;
    if (phimax < 0) phimax += 2.*M_PI;

    // get ROB id
    int subDetectorId = 0; // 0x67 (A side) or 0x68 (C side)
    int rodId = 0; // 1-12
    cabling->getReadoutIDfromElementID(Id, subDetectorId, rodId);
    //int isAside = (subDetectorId==0x67) ? 0 : 1;
    //uint32_t robId = static_cast<uint32_t>(isAside*(maxRodId) + rodId -1); // 0-23
    // using Source ID :see Muon::TGC_Hid2RESrcID::getRodID()/getRobId()
    uint32_t robId =  ( ((0x0ff) & subDetectorId)<<16 ) | (rodId);
    // end part to get ROB id

    RegSelModule m( zmin, zmax, rmin, rmax, phimin, phimax, layerid, detid, robId, hashId );
    lut->addModule( m );

  }

  lut->initialise();

  return lut;

}







