/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RecTPCnv/MuonSpShowerContainerCnv_p1.h"
#include "RecTPCnv/MuonSpShowerCnv_p1.h"


void 
MuonSpShowerContainerCnv_p1::persToTrans(   const MuonSpShowerContainer_p1* pers, 
                                            Rec::MuonSpShowerContainer* trans, 
                                            MsgStream& msg ) 
{
    using namespace Rec;
    trans->clear();
    std::vector<MuonSpShower_p1*>::const_iterator it    = pers->m_showers.begin(), itEnd = pers->m_showers.end();
    for ( ; it!=itEnd; ++it ) {
        MuonSpShower* shower = m_cnv.createTransient( *it, msg );
        trans->push_back( shower );
    }
    return;
}

void 
MuonSpShowerContainerCnv_p1::transToPers(   const Rec::MuonSpShowerContainer* trans, 
                                            MuonSpShowerContainer_p1* pers, 
                                            MsgStream& msg) 
{
    pers->m_ownshowers = true;
    Rec::MuonSpShowerContainer::const_iterator it    = trans->begin(), itEnd = trans->end();
    for ( ; it!=itEnd; ++it ) {
        MuonSpShower_p1* shower_p1 = m_cnv.createPersistent( *it, msg );
        pers->m_showers.push_back( shower_p1 );
    }
    return;
}

