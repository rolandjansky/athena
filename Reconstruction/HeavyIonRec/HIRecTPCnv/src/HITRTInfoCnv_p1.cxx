/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIRecTPCnv/HITRTInfoCnv_p1.h"

void HITRTInfoCnv_p1::persToTrans( const HITRTInfo_p1* pers,
				   HITRTInfo* trans, 
				   MsgStream& msg ) 
{
  if (msg.level() <= MSG::DEBUG)
    msg << MSG::DEBUG << "Loading HITRTInfo from persistent state.[start]"  << endmsg;
  trans->m_OccupancyPerPart.reserve(pers->infos.size());
  for( unsigned int i(0); i<pers->infos.size(); ++i )
    trans->m_OccupancyPerPart.push_back( pers->infos[i] );
  if (msg.level() <= MSG::DEBUG)
    msg << MSG::DEBUG << "Loading HITRTInfo from persistent state.[end]"  << endmsg;
  return;
}

void HITRTInfoCnv_p1::transToPers( const HITRTInfo* trans, 
				   HITRTInfo_p1* pers, 
				   MsgStream& msg ) 
{
  if (msg.level() <= MSG::DEBUG)
    msg << MSG::DEBUG << "Creating persistent state of HITRTInfo.[start]"    << endmsg;
  pers->infos.reserve(trans->m_OccupancyPerPart.size());
  
  for( unsigned int i(0); i<trans->m_OccupancyPerPart.size(); ++i )
    {
      pers->infos.push_back( trans->m_OccupancyPerPart[i] );
      if (msg.level() <= MSG::DEBUG)
        msg << MSG::DEBUG << "Creating persistent state of HITRTInfo.[" << i << "] : " << trans->m_OccupancyPerPart[i] << endmsg;
    }
  if (msg.level() <= MSG::DEBUG)
    msg << MSG::DEBUG << "Creating persistent state of HITRTInfo.[end]"    << endmsg;
  return;
}
