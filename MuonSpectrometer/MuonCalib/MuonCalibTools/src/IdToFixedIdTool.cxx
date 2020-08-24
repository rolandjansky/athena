/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibTools/IdToFixedIdTool.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <iostream>
#include <string>
#include <sstream>

namespace MuonCalib {

IdToFixedIdTool::IdToFixedIdTool(const std::string& t,
				 const std::string& n,
				 const IInterface* p)
  : AthAlgTool(t,n,p),
    m_print_level(0)
{
  declareInterface< IIdToFixedIdTool >(this) ;
  declareProperty("print_level" , m_print_level);
}

StatusCode IdToFixedIdTool::initialize() 
{
  ATH_MSG_VERBOSE("Initialisation started");
  
  ATH_CHECK(AlgTool::initialize());

  ATH_CHECK( m_idHelperSvc.retrieve() );

  return StatusCode::SUCCESS;
}

MuonFixedId IdToFixedIdTool::idToFixedId(const Identifier& id) const
{
  bool done;

  ATH_MSG_VERBOSE("IdToFixedId started");
  MuonFixedId fixedId;

  if( !(m_idHelperSvc->isMuon(id)) ){
    ATH_MSG_DEBUG(" MuonIdentifier doesn't correspond to a muon ");
  } else {
    //setting the Muon specific fields
    done = fixedId.setStationName( fixedId.stationStringToFixedStationNumber( m_idHelperSvc->mdtIdHelper().stationNameString( m_idHelperSvc->mdtIdHelper().stationName( id ) ) ) );
    if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (general) ");
    if( m_idHelperSvc->isMdt(id) ){
      done = fixedId.setTechnology(0) ; // MDT = 0;
      //setting the Mdt specific fields
      if(done) done = fixedId.setStationEta( m_idHelperSvc->mdtIdHelper().stationEta( id ) );
      if(done) done = fixedId.setStationPhi( m_idHelperSvc->mdtIdHelper().stationPhi( id ) );
      
    if(done) done = fixedId.setMdtTube( m_idHelperSvc->mdtIdHelper().tube( id ) );
      if(done) done = fixedId.setMdtTubeLayer( m_idHelperSvc->mdtIdHelper().tubeLayer( id ) );
      if(done) done = fixedId.setMdtMultilayer( m_idHelperSvc->mdtIdHelper().multilayer( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (MDT) ");
    } else if ( m_idHelperSvc->isRpc(id) ){
      done = fixedId.setTechnology(3) ; // RPC = 3;
      //setting the Rpc specific fields
      if(done) done = fixedId.setStationEta( m_idHelperSvc->rpcIdHelper().stationEta( id ) );
      if(done) done = fixedId.setStationPhi( m_idHelperSvc->rpcIdHelper().stationPhi( id ) );
      
      if(done) done = fixedId.setRpcDoubletR( m_idHelperSvc->rpcIdHelper().doubletR( id ) );
      if(done) done = fixedId.setRpcDoubletZ( m_idHelperSvc->rpcIdHelper().doubletZ( id ) );
      if(done) done = fixedId.setRpcDoubletPhi( m_idHelperSvc->rpcIdHelper().doubletPhi( id ) );
      if(done) done = fixedId.setRpcGasGap( m_idHelperSvc->rpcIdHelper().gasGap( id ) );
      if(done) done = fixedId.setRpcMeasuresPhi( m_idHelperSvc->rpcIdHelper().measuresPhi( id ) );
      if(done) done = fixedId.setRpcStrip( m_idHelperSvc->rpcIdHelper().strip( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (RPC) ");
    } else if ( m_idHelperSvc->isCsc(id) ){
      done = fixedId.setTechnology(1) ; // CSC = 1;
      //setting the Csc specific fields
      if(done) done = fixedId.setStationEta( m_idHelperSvc->cscIdHelper().stationEta( id ) );
      if(done) done = fixedId.setStationPhi( m_idHelperSvc->cscIdHelper().stationPhi( id ) );

      if(done) done = fixedId.setCscChamberLayer( m_idHelperSvc->cscIdHelper().chamberLayer( id ) );
      if(done) done = fixedId.setCscWireLayer( m_idHelperSvc->cscIdHelper().wireLayer( id ) );
      if(done) done = fixedId.setCscMeasuresPhi( m_idHelperSvc->cscIdHelper().measuresPhi( id ) );
      if(done) done = fixedId.setCscStrip( m_idHelperSvc->cscIdHelper().strip( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (CSC) ");
    } else if ( m_idHelperSvc->isTgc(id) ){
      done = fixedId.setTechnology(2) ; // TGC = 2;
      //setting the Tgc specific fields
      if(done) done = fixedId.setStationEta( m_idHelperSvc->tgcIdHelper().stationEta( id ) );
      if(done) done = fixedId.setStationPhi( m_idHelperSvc->tgcIdHelper().stationPhi( id ) );

      if(done) done = fixedId.setTgcGasGap( m_idHelperSvc->tgcIdHelper().gasGap( id ) );
      if(done) done = fixedId.setTgcIsStrip( m_idHelperSvc->tgcIdHelper().isStrip( id ) );
      if(done) done = fixedId.setTgcChannel( m_idHelperSvc->tgcIdHelper().channel( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (TGC) ");
    }
  }
  return fixedId;
}

Identifier IdToFixedIdTool::fixedIdToId(const MuonFixedId& fid) const
{
  ATH_MSG_VERBOSE("FixedIdToId started     ");
  Identifier tmp;
  if( fid.is_mdt() ){
    tmp = m_idHelperSvc->mdtIdHelper().channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
		     fid.eta(),
		     fid.phi(),
		     fid.mdtMultilayer(),
		     fid.mdtTubeLayer(),
		     fid.mdtTube() );
  } else if( fid.is_rpc() ){
    tmp = m_idHelperSvc->rpcIdHelper().channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
		     fid.eta(),
		     fid.phi(), 
		     fid.rpcDoubletR(),
		     fid.rpcDoubletZ(),
		     fid.rpcDoubletPhi(),
		     fid.rpcGasGap(),
		     fid.rpcMeasuresPhi(),
		     fid.rpcStrip() );
  } else if( m_idHelperSvc->hasCSC() && fid.is_csc()){ 
     tmp = m_idHelperSvc->cscIdHelper().channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
		      fid.eta(),
		      fid.phi(),
		      fid.cscChamberLayer(),
		      fid.cscWireLayer(),
		      fid.cscMeasuresPhi(),
		      fid.cscStrip() );
  } else if( fid.is_tgc() ){
    tmp = m_idHelperSvc->tgcIdHelper().channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
		     fid.eta(),
		     fid.phi(),
		     fid.tgcGasGap(),
		     fid.tgcIsStrip(),
		     fid.tgcChannel() );
  }
  return tmp;
}

Identifier IdToFixedIdTool::regionKeyToId(std::string region) const
{
  // unpack the region key string used by the calibration framework and return
  // an Athena Identifier
  int eta,phi,ml;
  std::string stnName(region,0,3);
  std::string::size_type separator1=region.find_first_of("_");
  std::string::size_type separator2=region.find_last_of("_");
  if((separator1==std::string::npos)||(separator2==std::string::npos)||(separator1==separator2)) {
     ATH_MSG_WARNING("IdToFixedIdTool::regionKeyToId : invalid region key "<<region);
     return Identifier();
  }    
  std::string::size_type separator3=region.find( "_", separator1+1);
  if(separator3!=separator2){
    // format STN_ETA_PHI_ML
    std::string stnPhi(region,separator1+1,separator2-separator1-1);
    std::string stnEta(region,separator2+1,separator3-separator2-1);
    std::istringstream is1,is2;
    is1.str(stnEta);
    is1 >> eta;
    is2.str(stnPhi);
    is2 >> phi;
    std::string Ml(region,separator3+1,region.size()-separator3-1);
    std::istringstream is3;
    is3.str(Ml);
    is3 >> ml;
  } else {
    // format STN_ETA_PHI
    std::string stnPhi(region,separator1+1,separator2-separator1-1);
    std::string stnEta(region,separator2+1,region.size()-separator2-1);
    std::istringstream is1,is2;
    is1.str(stnEta);
    is1 >> eta;
    is2.str(stnPhi);
    is2 >> phi;
    ml=0;
   } 
   // figure out what to do here when ml!=0 ...
   Identifier athenaId = m_idHelperSvc->mdtIdHelper().elementID(stnName,eta,phi);
   return athenaId;
}

void IdToFixedIdTool::print(const Identifier& id) const
{
  
  MuonFixedId tmp = idToFixedId( id );
  Identifier test = fixedIdToId( tmp );
  if(id != test) 
    ATH_MSG_WARNING("SOMETHING WENT WRONG PROCESSING FIXEDID<->ID");
      
  m_idHelperSvc->mdtIdHelper().print(id);
}

void IdToFixedIdTool::print(const MuonFixedId& fid) const
{ 
  ATH_MSG_INFO("MuonFixedId conversion yields: " << fid);
}


}
