/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for TTbarJJTop.
// 
// Andrei Gaponenko <agaponenko@lbl.gov>, 2009


#include <vector>
#include <algorithm>
#include <iterator>

#include "TopEventTPCnv/TTbarJJTopCnv_p1.h"
#include "TopEventTPCnv/TTbarJetsBaseCnv_p1.h"

#include "TopEvent/TTbarJJTop.h"
#include "TopEventTPCnv/TTbarJJTop_p1.h"

#include "StoreGate/StoreGateSvc.h"

namespace {
  TTbarJetsBaseCnv_p1 baseCnv;
}

void TTbarJJTopCnv_p1::persToTrans( const TTbarJJTop_p1* pers,
				    TopRec::TTbarJJTop* trans, 
				    MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarJJTopCnv_p1::persToTrans() begin"<<endreq;

  TopRec::TTbarJJ base;
  baseCnv.persToTrans(pers, &base,  msg);

  TopRec::IndexSet indexes;
  std::copy(pers->m_htop1_indexes.begin(),  pers->m_htop1_indexes.end(), std::inserter(indexes, indexes.begin()) );

  *trans = TopRec::TTbarJJTop(base, indexes);

  msg<<MSG::DEBUG<<"TTbarJJTopCnv_p1::persToTrans() end"<<endreq;
}

void TTbarJJTopCnv_p1::transToPers( const TopRec::TTbarJJTop* trans, 
				    TTbarJJTop_p1* pers, 
				    MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarJJTopCnv_p1::transToPers() begin"<<endreq;
  baseCnv.transToPers(trans, pers, msg);
  pers->m_htop1_indexes = trans->top1().jetIndexes();
  msg<<MSG::DEBUG<<"TTbarJJTopCnv_p1::transToPers() end"<<endreq;
}
