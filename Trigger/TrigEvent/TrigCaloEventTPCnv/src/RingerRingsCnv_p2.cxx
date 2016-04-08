/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/RingerRings.h"
#include "TrigCaloEventTPCnv/RingerRings_p2.h"
#include "TrigCaloEventTPCnv/RingerRingsCnv_p2.h"

void RingerRingsCnv_p2::transToPers(const RingerRings *trans, 
                                    RingerRings_p2 *pers,
				    MsgStream &log) {

  log << MSG::DEBUG << "RingerRingsCnv_p2::tranToPers" << endreq;
  
  if ( (!trans) || (!pers) )
    return;

  // Hack! Should be fixed by adding a const accessor on RingerRings
  //RingerRings *trans_non_const = const_cast<RingerRings *>(trans);
  
  pers->m_numberOfRings = trans->size();
  pers->m_rings.clear();
  pers->m_rings.reserve(trans->size());
  pers->m_rings.insert(pers->m_rings.end(),trans->rings().begin(),
	trans->rings().end() );
//  for (unsigned int i = 0; i < trans->size(); ++i) { 
//    pers->m_rings.push_back(trans->at(i));
//  }
}

void RingerRingsCnv_p2::persToTrans(const RingerRings_p2 *pers, 
                                    RingerRings *trans,
				    MsgStream &log) {

  log << MSG::DEBUG << "RingerRingsCnv_p2::persToTrans" << endreq;

  if ( (!pers) || (!trans) )
    return;

  trans->rings().clear();
  trans->rings().reserve(pers->m_rings.size() );
  trans->rings().insert(trans->rings().end(),pers->m_rings.begin(),
	pers->m_rings.end() );
//  for (unsigned int i = 0; i < pers->m_rings.size(); ++i){   
//    trans->rings().push_back(pers->m_rings[i]);
//  }
}

