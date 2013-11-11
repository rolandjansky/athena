/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetCnv_p1.h
// author: Kyle Cranmer, Sebastien Binet, Lukasz Janyst
// date:   April 2007
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCNV_P1_H
#define tauEventTPCnv_TAUJETCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEventTPCnv/TauJet_p1.h"

namespace Analysis
{
    class TauJetConainer;
}

class MsgStream;

class TauJetCnv_p1 : public T_AthenaPoolTPCnvBase<Analysis::TauJet, TauJet_p1>
{
    public:
        TauJetCnv_p1() {};
        virtual void persToTrans( const TauJet_p1    *persObj,
                                  Analysis :: TauJet *transObj,
                                  MsgStream          &msg );

        virtual void transToPers( const Analysis :: TauJet *transObj,
                                  TauJet_p1                *persObj,
                                  MsgStream                &msg );
};

#endif // tauEventTPCnv_TAUJETCNV_P1_H
