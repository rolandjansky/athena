/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibTools/IdToFixedIdTool.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <iostream>
#include <string>
#include <sstream>

namespace MuonCalib {

IdToFixedIdTool::IdToFixedIdTool(const std::string& t,
				 const std::string& n,
				 const IInterface* p)
  : AthAlgTool(t,n,p),
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
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

  CHECK( m_idHelperTool.retrieve() );

  return status;
}

MuonFixedId IdToFixedIdTool::idToFixedId(const Identifier& id) const
{
  bool done;

  ATH_MSG_VERBOSE("IdToFixedId started");
  MuonFixedId fixedId;

  if( !(m_idHelperTool->isMuon(id)) ){
    ATH_MSG_DEBUG(" MuonIdentifier doesn't correspond to a muon ");
  } else {
    //setting the Muon specific fields
    done = fixedId.setStationName( fixedId.stationStringToFixedStationNumber( m_idHelperTool->mdtIdHelper().stationNameString( m_idHelperTool->mdtIdHelper().stationName( id ) ) ) );
    if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (general) ");
    if( m_idHelperTool->isMdt(id) ){
      done = fixedId.setTechnology(0) ; // MDT = 0;
      //setting the Mdt specific fields
      if(done) done = fixedId.setStationEta( m_idHelperTool->mdtIdHelper().stationEta( id ) );
      if(done) done = fixedId.setStationPhi( m_idHelperTool->mdtIdHelper().stationPhi( id ) );
      
    if(done) done = fixedId.setMdtTube( m_idHelperTool->mdtIdHelper().tube( id ) );
      if(done) done = fixedId.setMdtTubeLayer( m_idHelperTool->mdtIdHelper().tubeLayer( id ) );
      if(done) done = fixedId.setMdtMultilayer( m_idHelperTool->mdtIdHelper().multilayer( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (MDT) ");
    } else if ( m_idHelperTool->isRpc(id) ){
      done = fixedId.setTechnology(3) ; // RPC = 3;
      //setting the Rpc specific fields
      if(done) done = fixedId.setStationEta( m_idHelperTool->rpcIdHelper().stationEta( id ) );
      if(done) done = fixedId.setStationPhi( m_idHelperTool->rpcIdHelper().stationPhi( id ) );
      
      if(done) done = fixedId.setRpcDoubletR( m_idHelperTool->rpcIdHelper().doubletR( id ) );
      if(done) done = fixedId.setRpcDoubletZ( m_idHelperTool->rpcIdHelper().doubletZ( id ) );
      if(done) done = fixedId.setRpcDoubletPhi( m_idHelperTool->rpcIdHelper().doubletPhi( id ) );
      if(done) done = fixedId.setRpcGasGap( m_idHelperTool->rpcIdHelper().gasGap( id ) );
      if(done) done = fixedId.setRpcMeasuresPhi( m_idHelperTool->rpcIdHelper().measuresPhi( id ) );
      if(done) done = fixedId.setRpcStrip( m_idHelperTool->rpcIdHelper().strip( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (RPC) ");
    } else if ( m_idHelperTool->isCsc(id) ){
      done = fixedId.setTechnology(1) ; // CSC = 1;
      //setting the Csc specific fields
      if(done) done = fixedId.setStationEta( m_idHelperTool->cscIdHelper().stationEta( id ) );
      if(done) done = fixedId.setStationPhi( m_idHelperTool->cscIdHelper().stationPhi( id ) );

      if(done) done = fixedId.setCscChamberLayer( m_idHelperTool->cscIdHelper().chamberLayer( id ) );
      if(done) done = fixedId.setCscWireLayer( m_idHelperTool->cscIdHelper().wireLayer( id ) );
      if(done) done = fixedId.setCscMeasuresPhi( m_idHelperTool->cscIdHelper().measuresPhi( id ) );
      if(done) done = fixedId.setCscStrip( m_idHelperTool->cscIdHelper().strip( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (CSC) ");
    } else if ( m_idHelperTool->isTgc(id) ){
      done = fixedId.setTechnology(2) ; // TGC = 2;
      //setting the Tgc specific fields
      if(done) done = fixedId.setStationEta( m_idHelperTool->tgcIdHelper().stationEta( id ) );
      if(done) done = fixedId.setStationPhi( m_idHelperTool->tgcIdHelper().stationPhi( id ) );

      if(done) done = fixedId.setTgcGasGap( m_idHelperTool->tgcIdHelper().gasGap( id ) );
      if(done) done = fixedId.setTgcIsStrip( m_idHelperTool->tgcIdHelper().isStrip( id ) );
      if(done) done = fixedId.setTgcChannel( m_idHelperTool->tgcIdHelper().channel( id ) );
      if(!done)  ATH_MSG_INFO("Something went wrong in the conversion id->fid (TGC) ");
    }
  }
  return fixedId;
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
    tmp = m_idHelperTool->mdtIdHelper().channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
		     fid.eta(),
		     fid.phi(),
		     fid.mdtMultilayer(),
		     fid.mdtTubeLayer(),
		     fid.mdtTube() );
  } else if( fid.is_rpc() ){
    tmp = m_idHelperTool->rpcIdHelper().channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
		     fid.eta(),
		     fid.phi(), 
		     fid.rpcDoubletR(),
		     fid.rpcDoubletZ(),
		     fid.rpcDoubletPhi(),
		     fid.rpcGasGap(),
		     fid.rpcMeasuresPhi(),
		     fid.rpcStrip() );
  } else if((&(m_idHelperTool->cscIdHelper())) && fid.is_csc()){ 
     tmp = m_idHelperTool->cscIdHelper().channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
		      fid.eta(),
		      fid.phi(),
		      fid.cscChamberLayer(),
		      fid.cscWireLayer(),
		      fid.cscMeasuresPhi(),
		      fid.cscStrip() );
  } else if( fid.is_tgc() ){
    tmp = m_idHelperTool->tgcIdHelper().channelID( fid.stationNumberToFixedStationString( fid.stationName() ) ,
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
   Identifier athenaId = m_idHelperTool->mdtIdHelper().elementID(stnName,eta,phi);
   return athenaId;
}

void IdToFixedIdTool::print(const Identifier& id) const
{
  
  MuonFixedId tmp = idToFixedId( id );
  Identifier test = fixedIdToId( tmp );
  if(id != test) 
    ATH_MSG_WARNING("SOMETHING WENT WRONG PROCESSING FIXEDID<->ID");
      
  m_idHelperTool->mdtIdHelper().print(id);
}

void IdToFixedIdTool::print(const MuonFixedId& fid) const
{ 
  ATH_MSG_INFO("MuonFixedId conversion yields: " << fid);
}


}
