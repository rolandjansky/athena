/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DataModelAthenaPool/NavigableCnv_p2.h"

#include "RecTPCnv/MissingETCompositionCnv_p1.h"
#include "MissingETEvent/MissingETComposition.h"
typedef NavigableCnv_p2<MissingETComposition, MissingETComposition_p1::Weight_p1> NavigableCnv_t;

// specializing template for Weight_p1, filling weights

static NavigableCnv_t    navCnv;

namespace MissingETHandlers
{
  template <>
    Weight::Weight(MissingETComposition_p1::Weight_p1& w) { 
    this->setWpx(w.wx); this->setWpy(w.wy); this->setWet(w.wet); this->setStatusWord(w.statusWord);
  }

  template <>
    Weight::Weight(const MissingETComposition_p1::Weight_p1& w) { 
    this->setWpx(w.wx); this->setWpy(w.wy); this->setWet(w.wet); this->setStatusWord(w.statusWord);
  }
}

void
MissingETCompositionCnv_p1::transToPers(const MissingETComposition* transObj,
					MissingETComposition_p1* persObj,
					MsgStream &reporter )
{
  reporter << MSG::DEBUG << "MissingETCompositionCnv_p1 toPers()" << endreq;
  
  navCnv.transToPers( transObj, &persObj->m_nav, reporter );
  
  reporter << MSG::DEBUG << "MissingETCompositionCnv_p1 toPers() done ..." << endreq;
}

void
MissingETCompositionCnv_p1::persToTrans(const MissingETComposition_p1* persObj,
					MissingETComposition* transObj,
					MsgStream &reporter )
{
  reporter << MSG::DEBUG << "MissingETCompositionCnv toTrans()" << endreq;
  
  navCnv.persToTrans( &(persObj->m_nav), transObj, reporter );
  
  reporter << MSG::DEBUG << "MissingETCompositionCnv_p1 toTrans() done ..." << endreq;
}

