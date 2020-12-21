/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloBadChannelTool/CaloBadChanTool.h"

//#include "GaudiKernel/MsgStream.h"

CaloBadChanTool::CaloBadChanTool(const std::string& type, const std::string& name,
				 const IInterface* parent) :
  AthAlgTool( type, name, parent),
  m_caloID(nullptr)
{
  declareInterface<ICaloBadChanTool>(this);
}


CaloBadChanTool::~CaloBadChanTool() {
}

StatusCode CaloBadChanTool::initialize()
{
  ATH_MSG_DEBUG ("in initialize()" );

  StatusCode sc=m_larBCKey.initialize();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "Unable to initialize LAr bad channels key: no LAr bad channel info will be provided " );
  }
  else
    ATH_MSG_DEBUG ( "LAr bad channels key initialized" );


  sc=m_tileBCT.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "Unable to get TileBadChannelTool: no Tile bad channel info will be provided " );
  }
  else
    ATH_MSG_DEBUG ( "TileBadChannelTool retrieved" );

  ATH_CHECK( detStore()->retrieve(m_caloID, "CaloCell_ID") );
  return StatusCode::SUCCESS;
}

CaloBadChannel
CaloBadChanTool::caloStatus(const EventContext& ctx, Identifier id) const
{
  if (m_tileBCT && m_caloID->is_tile(id)) {
    return m_tileBCT->caloStatus(ctx,id);
  } else if (m_caloID->is_lar(id)) {
    SG::ReadCondHandle<LArBadChannelCont> bch{ m_larBCKey, ctx };
    const LArBadChannelCont* bcCont{ *bch };
    if (bcCont) {
      CaloBadChannel::BitWord res = 0;
      LArBadChannel lbc = bcCont->offlineStatus(id);

      if (lbc.reallyNoisy() || lbc.sporadicBurstNoise()) {
        CaloBadChannel::setBit(CaloBadChannel::noisyBit, res);
      }
      if (lbc.deadReadout() || lbc.deadPhys()) {
        CaloBadChannel::setBit(CaloBadChannel::deadBit, res);
      } else if (!lbc.good()) {
        CaloBadChannel::setBit(CaloBadChannel::affectedBit, res);
      }
      return CaloBadChannel(res);
    } else {
      CaloBadChannel empty;
      return empty;
    }
  } else {
    CaloBadChannel empty;
    return empty;
  }
}
