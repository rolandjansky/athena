/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibTools/IdToFixedIdTool.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <iostream>
#include <string>
#include <sstream>

namespace MuonCalib {

IdToFixedIdTool::IdToFixedIdTool(const std::string& t,
				 const std::string& n,
				 const IInterface* p)
  : AthAlgTool(t,n,p),
    p_MuonMgr( 0 ),
    p_MdtIdHelper( 0 ),
    p_CscIdHelper( 0 ),
    p_RpcIdHelper( 0 ),
    p_TgcIdHelper( 0 ),
    m_print_level(0)
{
  declareInterface< IIdToFixedIdTool >(this) ;
  declareProperty("print_level" , m_print_level);
}

IdToFixedIdTool::~IdToFixedIdTool()
{
}

StatusCode IdToFixedIdTool::initialize() 
{
  ATH_MSG_VERBOSE("Initialisation started");
  
  StatusCode status = AlgTool::initialize();
  if ( status.isFailure() ) return status;
  
  StoreGateSvc* detStore=0;
  status = serviceLocator()->service("DetectorStore", detStore);
 
  if ( status.isSuccess() ) {
    status = detStore->retrieve( p_MuonMgr );
    if ( status.isFailure() ) {
      ATH_MSG_ERROR(" Cannot retrieve MuonDetDescrMgr ");
    } else {
      p_MdtIdHelper = p_MuonMgr->mdtIdHelper();
      p_CscIdHelper = p_MuonMgr->cscIdHelper();    
      p_RpcIdHelper = p_MuonMgr->rpcIdHelper();
      p_TgcIdHelper = p_MuonMgr->tgcIdHelper();
      ATH_MSG_INFO(" Retrieved IdHelpers: (muon, mdt, csc, rpc and tgc) ");
    }
  } else {
    ATH_MSG_ERROR(" MuonDetDescrMgr not found in DetectorStore ");
  }

  return status;
}

MuonFixedId IdToFixedIdTool::idToFixedId(const Identifier& id) const
{
  bool done;

  ATH_MSG_VERBOSE("IdToFixedId started");
  m_fixedId.clear();

  if( !(p_MdtIdHelper->is_muon(id)) ){
    ATH_MSG_DEBUG(" MuonIdentifier doesn't correspond to a muon ");
  } else {
    //setting the Muon specific fields
    done = m_fixedId.setStationName( m_fixedId.stationStringToFixedStationNumber( p_MdtIdHelper->stationNameString( p_MdtIdHelper->stationName( id ) ) ) );
    //    if(done) done = m_fixedId.setStationEta( p_MdtIdHelper->stationEta( id ) );
    //    if(done) done = m_fixedId.setStationPhi( p_MdtIdHelper->stationPhi( id ) );
    if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (general) ");
    if( p_MdtIdHelper->is_mdt(id) ){
      done = m_fixedId.setTechnology(0) ; // MDT = 0;
      //setting the Mdt specific fields
      if(done) done = m_fixedId.setStationEta( p_MdtIdHelper->stationEta( id ) );
      if(done) done = m_fixedId.setStationPhi( p_MdtIdHelper->stationPhi( id ) );
      
    if(done) done = m_fixedId.setMdtTube( p_MdtIdHelper->tube( id ) );
      if(done) done = m_fixedId.setMdtTubeLayer( p_MdtIdHelper->tubeLayer( id ) );
      if(done) done = m_fixedId.setMdtMultilayer( p_MdtIdHelper->multilayer( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (MDT) ");
    } else if ( p_RpcIdHelper->is_rpc(id) ){
      done = m_fixedId.setTechnology(3) ; // RPC = 3;
      //setting the Rpc specific fields
      if(done) done = m_fixedId.setStationEta( p_RpcIdHelper->stationEta( id ) );
      if(done) done = m_fixedId.setStationPhi( p_RpcIdHelper->stationPhi( id ) );
      
      if(done) done = m_fixedId.setRpcDoubletR( p_RpcIdHelper->doubletR( id ) );
      if(done) done = m_fixedId.setRpcDoubletZ( p_RpcIdHelper->doubletZ( id ) );
      if(done) done = m_fixedId.setRpcDoubletPhi( p_RpcIdHelper->doubletPhi( id ) );
      if(done) done = m_fixedId.setRpcGasGap( p_RpcIdHelper->gasGap( id ) );
      if(done) done = m_fixedId.setRpcMeasuresPhi( p_RpcIdHelper->measuresPhi( id ) );
      if(done) done = m_fixedId.setRpcStrip( p_RpcIdHelper->strip( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (RPC) ");
    } else if ( p_CscIdHelper->is_csc(id) ){
      done = m_fixedId.setTechnology(1) ; // CSC = 1;
      //setting the Csc specific fields
      if(done) done = m_fixedId.setStationEta( p_CscIdHelper->stationEta( id ) );
      if(done) done = m_fixedId.setStationPhi( p_CscIdHelper->stationPhi( id ) );

      if(done) done = m_fixedId.setCscChamberLayer( p_CscIdHelper->chamberLayer( id ) );
      if(done) done = m_fixedId.setCscWireLayer( p_CscIdHelper->wireLayer( id ) );
      if(done) done = m_fixedId.setCscMeasuresPhi( p_CscIdHelper->measuresPhi( id ) );
      if(done) done = m_fixedId.setCscStrip( p_CscIdHelper->strip( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (CSC) ");
    } else if ( p_TgcIdHelper->is_tgc(id) ){
      done = m_fixedId.setTechnology(2) ; // TGC = 2;
      //setting the Tgc specific fields
      if(done) done = m_fixedId.setStationEta( p_TgcIdHelper->stationEta( id ) );
      if(done) done = m_fixedId.setStationPhi( p_TgcIdHelper->stationPhi( id ) );

      if(done) done = m_fixedId.setTgcGasGap( p_TgcIdHelper->gasGap( id ) );
      if(done) done = m_fixedId.setTgcIsStrip( p_TgcIdHelper->isStrip( id ) );
      if(done) done = m_fixedId.setTgcChannel( p_TgcIdHelper->channel( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (TGC) ");
    }
  }
  return m_fixedId;
}

StatusCode IdToFixedIdTool::finalize()
{
  return StatusCode::SUCCESS;
}

Identifier IdToFixedIdTool::fixedIdToId(const MuonFixedId& fid) const
{
  ATH_MSG_VERBOSE("FixedIdToId started     ");
  Identifier tmp;
  if( fid.is_mdt() ){
    tmp = p_MdtIdHelper->channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
		     fid.eta(),
		     fid.phi(),
		     fid.mdtMultilayer(),
		     fid.mdtTubeLayer(),
		     fid.mdtTube() );
  } else if( fid.is_rpc() ){
    tmp = p_RpcIdHelper->channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
		     fid.eta(),
		     fid.phi(), 
		     fid.rpcDoubletR(),
		     fid.rpcDoubletZ(),
		     fid.rpcDoubletPhi(),
		     fid.rpcGasGap(),
		     fid.rpcMeasuresPhi(),
		     fid.rpcStrip() );
  } else if( fid.is_csc() ){ 
     tmp = p_CscIdHelper->channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
		      fid.eta(),
		      fid.phi(),
		      fid.cscChamberLayer(),
		      fid.cscWireLayer(),
		      fid.cscMeasuresPhi(),
		      fid.cscStrip() );
  } else if( fid.is_tgc() ){
    tmp = p_TgcIdHelper->channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
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
     std::cout <<"IdToFixedIdTool::regionKeyToId : invalid region key "<<region<<std::endl;
     return Identifier(0);
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
   Identifier athenaId = p_MdtIdHelper->elementID(stnName,eta,phi);
   return athenaId;
}

void IdToFixedIdTool::print(const Identifier& id) const
{
  
  MuonFixedId tmp = idToFixedId( id );
  Identifier test = fixedIdToId( tmp );
  if(id != test) 
    ATH_MSG_WARNING("SOMETHING WENT WRONG PROCESSING FIXEDID<->ID");
      
  p_MdtIdHelper->print(id);
}

void IdToFixedIdTool::print(const MuonFixedId& fid) const
{ 
  ATH_MSG_INFO("MuonFixedId conversion yields: " << fid);
}


}
