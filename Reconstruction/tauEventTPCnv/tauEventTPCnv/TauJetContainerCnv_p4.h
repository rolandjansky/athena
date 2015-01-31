/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetContainerCnv_p4.h
// author: Lukasz Janyst
// date:   May 2008
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCONTAINERCNV_P4_H
#define tauEventTPCnv_TAUJETCONTAINERCNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEventTPCnv/TauJetContainer_p4.h"

namespace Analysis
{
    class TauJetContainer;
}

class MsgStream;

class TauJetContainerCnv_p4 : public T_AthenaPoolTPCnvBase<Analysis::TauJetContainer, TauJetContainer_p4>
{
    public:
        TauJetContainerCnv_p4() {};
        virtual void persToTrans( const TauJetContainer_p4     *persObj,
                                  Analysis :: TauJetContainer  *transObj,
                                  MsgStream                    &msg );

        virtual void transToPers( const Analysis :: TauJetContainer *transObj,
                                  TauJetContainer_p4                *persObj,
                                  MsgStream                         &msg );
};

#endif
