/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetCnv_p2.h
// author: Ilija Vukotic
// date:   April 2008
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCNV_P2_H
#define tauEventTPCnv_TAUJETCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEventTPCnv/TauJet_p2.h"

class MsgStream;

class TauJetCnv_p2 : public T_AthenaPoolTPCnvBase<Analysis::TauJet, TauJet_p2>
{
    public:
        TauJetCnv_p2() {};
        virtual void persToTrans( const TauJet_p2    *persObj,
                                  Analysis :: TauJet *transObj,
                                  MsgStream          &msg );

        virtual void transToPers( const Analysis :: TauJet *transObj,
                                  TauJet_p2                *persObj,
                                  MsgStream                &msg );
};

#endif // tauEventTPCnv_TAUJETCNV_P2_H
