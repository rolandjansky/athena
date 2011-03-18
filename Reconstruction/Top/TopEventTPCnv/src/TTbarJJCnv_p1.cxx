/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for TTbarJJ.
// 
// Andrei Gaponenko <agaponenko@lbl.gov>, 2008

#include <vector>
#include "TopEventTPCnv/TTbarJJCnv_p1.h"
#include "TopEventTPCnv/TTbarJetsBaseCnv_p1.h"

#include "TopEvent/TTbarJJ.h"
#include "TopEventTPCnv/TTbarJJ_p1.h"

#include "StoreGate/StoreGateSvc.h"

namespace {
  TTbarJetsBaseCnv_p1 baseCnv;
}

void TTbarJJCnv_p1::persToTrans( const TTbarJJ_p1* pers,
				       TopRec::TTbarJJ* trans, 
				       MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarJJCnv_p1::persToTrans() begin"<<endreq;
  baseCnv.persToTrans(pers, trans,  msg);
  trans->update4momentum();
  msg<<MSG::DEBUG<<"TTbarJJCnv_p1::persToTrans() end"<<endreq;
}

void TTbarJJCnv_p1::transToPers( const TopRec::TTbarJJ* trans, 
				       TTbarJJ_p1* pers, 
				       MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarJJCnv_p1::transToPers() begin"<<endreq;
  baseCnv.transToPers(trans, pers, msg);
  msg<<MSG::DEBUG<<"TTbarJJCnv_p1::transToPers() end"<<endreq;
}
