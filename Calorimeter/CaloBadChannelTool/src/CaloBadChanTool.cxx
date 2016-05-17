/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloBadChannelTool/CaloBadChanTool.h"

//#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

CaloBadChanTool::CaloBadChanTool(const std::string& type, const std::string& name, 
				 const IInterface* parent) :
  AthAlgTool( type, name, parent),
  m_larBCT("LArBadChanTool"),
  m_tileBCT("TileBadChanTool"),
  m_caloID(nullptr)
{
  declareInterface<ICaloBadChanTool>(this);
}


CaloBadChanTool::~CaloBadChanTool() {
}

StatusCode CaloBadChanTool::initialize()
{
  ATH_MSG_DEBUG ("in initialize()" );

  StatusCode sc=m_larBCT.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "Unable to get LArBadChanTool: no LAr bad channel info will be provided " );
  }
  else 
    ATH_MSG_DEBUG ( "LArBadChanTool retrieved" );


  sc=m_tileBCT.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "Unable to get TileBadChannelTool: no Tile bad channel info will be provided " );
  }
  else 
    ATH_MSG_DEBUG ( "TileBadChannelTool retrieved" );

  ATH_CHECK( detStore()->retrieve(m_caloID, "CaloCell_ID") );
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
