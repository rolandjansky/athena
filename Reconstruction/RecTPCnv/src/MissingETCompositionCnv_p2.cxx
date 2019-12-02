/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "DataModelAthenaPool/NavigableCnv_p2.h"

#include "RecTPCnv/MissingETCompositionCnv_p2.h"
#include "MissingETEvent/MissingETComposition.h"
typedef NavigableCnv_p2<MissingETComposition, Weight_p1> NavigableCnv_t;

// specializing template for Weight_p1, filling weights

static const NavigableCnv_t    navCnv;

namespace MissingETHandlers
{
  template <>
    Weight::Weight(Weight_p1& we) {
        this->setWpx(we.w[0]); this->setWpy(we.w[1]); this->setWet(we.w[2]); this->setStatusWord(we.statusWord);
    }

  template <>
    Weight::Weight(const Weight_p1& we) {
        this->setWpx(we.w[0]); this->setWpy(we.w[1]); this->setWet(we.w[2]); this->setStatusWord(we.statusWord);
    }    
    
}

void
MissingETCompositionCnv_p2::transToPers(const MissingETComposition* transObj, MissingETComposition_p2* persObj, MsgStream &reporter ) const {
    // reporter << MSG::DEBUG << "MissingETCompositionCnv_p2 toPers()" << endmsg;
    navCnv.transToPers( transObj, &persObj->m_nav, reporter );
    // reporter << MSG::DEBUG << "MissingETCompositionCnv_p2 toPers() done ..." << endmsg;
}

void
MissingETCompositionCnv_p2::persToTrans(const MissingETComposition_p2* persObj, MissingETComposition* transObj, MsgStream &reporter ) const {
    // reporter << MSG::DEBUG << "MissingETCompositionCnv toTrans()" << endmsg;
    navCnv.persToTrans( &(persObj->m_nav), transObj, reporter );
    // reporter << MSG::DEBUG << "MissingETCompositionCnv_p2 toTrans() done ..." << endmsg;
}

