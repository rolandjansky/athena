/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/RingerRings.h"
#include "TrigCaloEventTPCnv/RingerRings_p2.h"
#include "TrigCaloEventTPCnv/RingerRingsCnv_p2.h"

void RingerRingsCnv_p2::transToPers(const RingerRings *trans, 
                                    RingerRings_p2 *pers,
				    MsgStream &log) const {

  log << MSG::DEBUG << "RingerRingsCnv_p2::tranToPers" << endmsg;
  
  if ( (!trans) || (!pers) )
    return;

  pers->m_numberOfRings = trans->size();
  pers->m_rings.clear();
  pers->m_rings.reserve(trans->size());
  pers->m_rings.insert(pers->m_rings.end(),trans->rings().begin(),
	trans->rings().end() );
}

void RingerRingsCnv_p2::persToTrans(const RingerRings_p2 *pers, 
                                    RingerRings *trans,
				    MsgStream &log) const {

  log << MSG::DEBUG << "RingerRingsCnv_p2::persToTrans" << endmsg;

  if ( (!pers) || (!trans) )
    return;

  trans->rings().clear();
  trans->rings().reserve(pers->m_rings.size() );
  trans->rings().insert(trans->rings().end(),pers->m_rings.begin(),
	pers->m_rings.end() );
}

