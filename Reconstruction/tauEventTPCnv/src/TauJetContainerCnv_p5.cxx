/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetConainerCnv_p5.cxx
// author: Lukasz Janyst
// date:   May 2008
//-----------------------------------------------------------------------------

#include "tauEvent/TauJetContainer.h"
#include "tauEventTPCnv/TauJetContainerCnv_p5.h"
#include "tauEventTPCnv/TauJetCnv_p5.h"


static TauJetCnv_p5 tauCnv;

void TauJetContainerCnv_p5 :: persToTrans( const TauJetContainer_p5    *pers,
                                           Analysis :: TauJetContainer *trans,
                                           MsgStream                   &msg )
{
    trans->clear();
    trans->setROIAuthor (pers->m_ROIauthor);
    TauJetContainer_p5 :: const_iterator it = pers->begin();
    for( ; it != pers->end(); ++it )
    {
        Analysis :: TauJet *tau = new Analysis :: TauJet();
        tauCnv.persToTrans( &(*it), tau, msg );
        trans->push_back( tau );
    }
}

void TauJetContainerCnv_p5 :: transToPers( const Analysis :: TauJetContainer *trans,
                                           TauJetContainer_p5                *pers,
                                           MsgStream                         &msg )
{
    pers->reserve( trans->size() ); // prevent re-alloc
    pers->m_ROIauthor = trans->getROIAuthor();
    Analysis :: TauJetContainer :: const_iterator it = trans->begin();
    for( ; it != trans->end(); ++it )
    {
      pers->push_back( TauJet_p5() );
      tauCnv.transToPers( (*it), &pers->back(), msg );
    }

}
