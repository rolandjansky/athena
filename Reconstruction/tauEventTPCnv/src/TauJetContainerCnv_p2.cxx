/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetConainerCnv_p2.cxx
// author: Ilija Vukotic
// date:   April 2008
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "tauEvent/TauJetContainer.h"
#undef private
#undef protected

#include "tauEventTPCnv/TauJetContainerCnv_p2.h"
#include "tauEventTPCnv/TauJetCnv_p2.h"


static TauJetCnv_p2 tauCnv;

void TauJetContainerCnv_p2 :: persToTrans( const TauJetContainer_p2    *pers,
                                           Analysis :: TauJetContainer *trans,
                                           MsgStream                   &msg )
{
    trans->clear();
    trans->m_ROIauthor = pers->m_ROIauthor;
    TauJetContainer_p2 :: const_iterator it = pers->begin();
    for( ; it != pers->end(); ++it )
    {
        Analysis :: TauJet *tau = new Analysis :: TauJet();
        tauCnv.persToTrans( &(*it), tau, msg );
        trans->push_back( tau );
    }
}

void TauJetContainerCnv_p2 :: transToPers( const Analysis :: TauJetContainer *trans,
                                           TauJetContainer_p2                *pers,
                                           MsgStream                         &msg )
{
    pers->reserve( trans->size() ); // prevent re-alloc
    pers->m_ROIauthor = trans->m_ROIauthor;
    Analysis :: TauJetContainer :: const_iterator it = trans->begin();
    for( ; it != trans->end(); ++it )
    {
      pers->push_back( TauJet_p2() );
      tauCnv.transToPers( (*it), &pers->back(), msg );
    }

}
