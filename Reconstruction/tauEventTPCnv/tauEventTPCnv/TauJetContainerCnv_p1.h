/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetContainerCnv_p1.h
// author: Kyle Cranmer, Sebastien Binet, Lukasz Janyst
// date:   April 2007
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCONTAINERCNV_P1_H
#define tauEventTPCnv_TAUJETCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEventTPCnv/TauJetContainer_p1.h"

namespace Analysis
{
    class TauJetContainer;
}

class MsgStream;

class TauJetContainerCnv_p1 : public T_AthenaPoolTPCnvBase<Analysis::TauJetContainer, TauJetContainer_p1>
{
    public:
        TauJetContainerCnv_p1() {};
        virtual void persToTrans( const TauJetContainer_p1     *persObj,
                                  Analysis :: TauJetContainer  *transObj,
                                  MsgStream                    &msg );

        virtual void transToPers( const Analysis :: TauJetContainer *transObj,
                                  TauJetContainer_p1                *persObj,
                                  MsgStream                         &msg );
};

#endif // tauEventTPCnv_TAUJETCONTAINERCNV_P1_H
