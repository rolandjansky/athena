/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "StoreGate/StoreGateSvc.h"
#include "TopInputsTPCnv/TopOverlapVectorCnv_p1.h"

void TopOverlapVectorCnv_p1::persToTrans(const TopOverlapVector_p1* pers,
					 TopRec::TopOverlapVector* trans, 
					 MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TopOverlapVectorCnv_p1::persToTrans() begin [Pers: "
     <<typeid(TopOverlapVector_p1).name()
     <<", Trans: "<<typeid(TopRec::TopOverlapVector).name()
     <<"]"
     <<endreq;

  trans->resize(pers->size());
  for(TopOverlapVector_p1::size_type i=0; i < pers->size(); ++i) {
    m_cnv.persToTrans(&((*pers)[i]), &((*trans)[i]), msg);
  }

  msg<<MSG::DEBUG<<"TopOverlapVectorCnv_p1::persToTrans() end"<<endreq;
}

void TopOverlapVectorCnv_p1::transToPers(const TopRec::TopOverlapVector* trans, 
					 TopOverlapVector_p1* pers, 
					 MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TopOverlapVectorCnv_p1::transToPers() begin [Pers: "
     <<typeid(TopOverlapVector_p1).name()
     <<", Trans: "<<typeid(TopRec::TopOverlapVector).name()
     <<"]"
     <<endreq;

  pers->resize(trans->size());
  
  for(TopRec::TopOverlapVector::size_type i=0; i < trans->size(); ++i) {
    m_cnv.transToPers(&((*trans)[i]), &((*pers)[i]), msg);
  }
  
  msg<<MSG::DEBUG<<"TopOverlapVectorCnv_p1::transToPers() end"<<endreq;
}
