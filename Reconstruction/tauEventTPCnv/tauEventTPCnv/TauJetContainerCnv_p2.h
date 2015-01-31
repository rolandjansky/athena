/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetContainerCnv_p2.h
// author: Ilija Vukotic
// date:   April 2008
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCONTAINERCNV_P2_H
#define tauEventTPCnv_TAUJETCONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEventTPCnv/TauJetContainer_p2.h"

namespace Analysis
{
    class TauJetContainer;
}

class MsgStream;

class TauJetContainerCnv_p2 : public T_AthenaPoolTPCnvBase<Analysis::TauJetContainer, TauJetContainer_p2>
{
    public:
        TauJetContainerCnv_p2() {};
        virtual void persToTrans( const TauJetContainer_p2     *persObj,
                                  Analysis :: TauJetContainer  *transObj,
                                  MsgStream                    &msg );

        virtual void transToPers( const Analysis :: TauJetContainer *transObj,
                                  TauJetContainer_p2                *persObj,
                                  MsgStream                         &msg );
};

#endif
