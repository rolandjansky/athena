/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "StoreGate/StoreGateSvc.h"
#include "TopInputsTPCnv/TopOverlapTriggerVectorCnv_p1.h"

void TopOverlapTriggerVectorCnv_p1::persToTrans(const TopOverlapTriggerVector_p1* pers,
					 	TopRec::TopOverlapTriggerVector* trans, 
					 	MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TopOverlapTriggerVectorCnv_p1::persToTrans() begin [Pers: "
     <<typeid(TopOverlapTriggerVector_p1).name()
     <<", Trans: "<<typeid(TopRec::TopOverlapTriggerVector).name()
     <<"]"
     <<endreq;

  trans->resize(pers->size());
  for(TopOverlapTriggerVector_p1::size_type i=0; i < pers->size(); ++i) {
    //m_cnv.persToTrans(&((*pers)[i]), &((*trans)[i]), msg);
    m_cnv.persToTrans( & pers->at(i), & trans->at(i), msg);
  }

  msg<<MSG::DEBUG<<"TopOverlapTriggerVectorCnv_p1::persToTrans() end"<<endreq;
}

void TopOverlapTriggerVectorCnv_p1::transToPers(const TopRec::TopOverlapTriggerVector* trans, 
					 TopOverlapTriggerVector_p1* pers, 
					 MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TopOverlapTriggerVectorCnv_p1::transToPers() begin [Pers: "
     <<typeid(TopOverlapTriggerVector_p1).name()
     <<", Trans: "<<typeid(TopRec::TopOverlapTriggerVector).name()
     <<"]"
     <<endreq;

  pers->resize(trans->size());
  
  for(TopRec::TopOverlapTriggerVector::size_type i=0; i < trans->size(); ++i) {
    //m_cnv.transToPers(&((*trans)[i]), &((*pers)[i]), msg);
    m_cnv.transToPers( & trans->at(i), & pers->at(i), msg);
  }
  
  msg<<MSG::DEBUG<<"TopOverlapTriggerVectorCnv_p1::transToPers() end"<<endreq;
}
