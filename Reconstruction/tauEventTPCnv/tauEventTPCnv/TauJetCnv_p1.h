/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetCnv_p1.h
// author: Kyle Cranmer, Sebastien Binet, Lukasz Janyst
// date:   April 2007
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCNV_P1_H
#define tauEventTPCnv_TAUJETCNV_P1_H

#include "tauEventTPCnv/TauJet_p1.h"

namespace Analysis
{
    class TauJetConainer;
}

class MsgStream;

class TauJetCnv_p1
{
public:
  TauJetCnv_p1() {};
  void persToTrans( const TauJet_p1    *persObj,
                    Analysis :: TauJet *transObj,
                    MsgStream          &msg ) const;

  void transToPers( const Analysis :: TauJet *transObj,
                    TauJet_p1                *persObj,
                    MsgStream                &msg ) const;
};

#endif // tauEventTPCnv_TAUJETCNV_P1_H
