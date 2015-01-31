/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetContainerCnv_p3.h
// author: Lukasz Janyst
// date:   May 2008
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCONTAINERCNV_P3_H
#define tauEventTPCnv_TAUJETCONTAINERCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEventTPCnv/TauJetContainer_p3.h"

namespace Analysis
{
    class TauJetContainer;
}

class MsgStream;

class TauJetContainerCnv_p3 : public T_AthenaPoolTPCnvBase<Analysis::TauJetContainer, TauJetContainer_p3>
{
    public:
        TauJetContainerCnv_p3() {};
        virtual void persToTrans( const TauJetContainer_p3     *persObj,
                                  Analysis :: TauJetContainer  *transObj,
                                  MsgStream                    &msg );

        virtual void transToPers( const Analysis :: TauJetContainer *transObj,
                                  TauJetContainer_p3                *persObj,
                                  MsgStream                         &msg );
};

#endif
