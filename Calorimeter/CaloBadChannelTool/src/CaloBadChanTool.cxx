/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloBadChannelTool/CaloBadChanTool.h"

//#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

CaloBadChanTool::CaloBadChanTool(const std::string& type, const std::string& name, 
				 const IInterface* parent) :
  AlgTool( type, name, parent),
  m_larBCT("LArBadChanTool"),
  m_tileBCT("TileBadChanTool"),
  m_log(NULL)
{
  declareInterface<ICaloBadChanTool>(this);
}


CaloBadChanTool::~CaloBadChanTool() {
  delete m_log;
}

StatusCode CaloBadChanTool::initialize()
{
  StatusCode sc;
  m_log= new MsgStream(msgSvc(),name());

  (*m_log) <<MSG::DEBUG <<"in initialize()" <<endreq;

  sc=m_larBCT.retrieve();
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "Unable to get LArBadChanTool: no LAr bad channel info will be provided " << endreq;
  }
  else 
    (*m_log) << MSG::DEBUG << "LArBadChanTool retrieved" << endreq;


  sc=m_tileBCT.retrieve();
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "Unable to get TileBadChannelTool: no Tile bad channel info will be provided " << endreq;
  }
  else 
    (*m_log) << MSG::DEBUG << "TileBadChannelTool retrieved" << endreq;

  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if (!sc.isSuccess() || 0 == detStore)  {
    (*m_log) <<MSG::ERROR <<"Could not find DetStore" <<endreq;
    return StatusCode::FAILURE;
  }
  sc = detStore->retrieve(m_caloID, "CaloCell_ID");
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Could not get Calo_ID helper !" << endreq;
    return StatusCode::FAILURE;
  }
	
  return StatusCode::SUCCESS;

}

CaloBadChannel CaloBadChanTool::caloStatus(Identifier id) const{
  if (m_tileBCT && m_caloID->is_tile(id)) {
    //(*m_log) << MSG::VERBOSE << "Calling Tile BadChannelTool ... not yet implemeted" << endreq;
    return m_tileBCT->caloStatus(id);
  }
  else if(m_larBCT && m_caloID->is_lar(id)) {
   // (*m_log) << MSG::VERBOSE << "Calling LAr BadChannelTool ... not yet implemeted" << endreq;
    return m_larBCT->caloStatus(id);
  }
  else {
      CaloBadChannel empty;
      return empty;
  }
}
