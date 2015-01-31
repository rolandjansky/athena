/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetContainerCnv_p5.h
// author: Lukasz Janyst
// date:   May 2008
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCONTAINERCNV_P5_H
#define tauEventTPCnv_TAUJETCONTAINERCNV_P5_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEventTPCnv/TauJetContainer_p5.h"

namespace Analysis
{
    class TauJetContainer;
}

class MsgStream;

class TauJetContainerCnv_p5 : public T_AthenaPoolTPCnvBase<Analysis::TauJetContainer, TauJetContainer_p5>
{
    public:
        TauJetContainerCnv_p5() {};
        virtual void persToTrans( const TauJetContainer_p5     *persObj,
                                  Analysis :: TauJetContainer  *transObj,
                                  MsgStream                    &msg );

        virtual void transToPers( const Analysis :: TauJetContainer *transObj,
                                  TauJetContainer_p5                *persObj,
                                  MsgStream                         &msg );
};

#endif
