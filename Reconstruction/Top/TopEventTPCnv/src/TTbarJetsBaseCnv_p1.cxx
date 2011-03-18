/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for TTbarJetsBase.
// 
// Andrei Gaponenko <agaponenko@lbl.gov>, 2008

#include <vector>
#include "DataModelAthenaPool/VectorElementLinkCnv_p1.h"

#include "TopEventTPCnv/TTbarJetsBaseCnv_p1.h"

#include "TopEvent/TTbarJetsBase.h"
#include "TopEventTPCnv/TTbarJetsBase_p1.h"

#include "StoreGate/StoreGateSvc.h"

namespace {
  VectorElementLinkCnv_p1<TopRec::JetVector> jetVecCnv;
}

void TTbarJetsBaseCnv_p1::persToTrans( const TTbarJetsBase_p1* pers,
				       TopRec::TTbarJetsBase* trans, 
				       MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarJetsBaseCnv_p1::persToTrans() begin"<<endreq;
  jetVecCnv.persToTrans(&pers->m_jets, &trans->m_jets,  msg);
  msg<<MSG::DEBUG<<"TTbarJetsBaseCnv_p1::persToTrans() end"<<endreq;
}

void TTbarJetsBaseCnv_p1::transToPers( const TopRec::TTbarJetsBase* trans, 
				       TTbarJetsBase_p1* pers, 
				       MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TTbarJetsBaseCnv_p1::transToPers() begin"<<endreq;
  jetVecCnv.transToPers(&trans->jets(), &pers->m_jets, msg);
  msg<<MSG::DEBUG<<"TTbarJetsBaseCnv_p1::transToPers() end"<<endreq;
}
