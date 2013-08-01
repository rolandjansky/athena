/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetContainerCnv.h
// author: Kyle Cranmer, Sebastien Binet, Lukasz Janyst
// date:   April 2007
//-----------------------------------------------------------------------------

#ifndef RECATHENAPOOL_TAUJETCONTAINERCNV_H
#define RECATHENAPOOL_TAUJETCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "tauEventTPCnv/TauJetContainer_p1.h"
#include "tauEventTPCnv/TauJetContainer_p2.h"
#include "tauEventTPCnv/TauJetContainer_p3.h"
#include "tauEventTPCnv/TauJetContainer_p4.h"
#include "tauEventTPCnv/TauJetContainer_p5.h"
#include "tauEvent/TauJetContainer.h"

typedef TauJetContainer_p5  TauJetContainer_PERS;

class TauJetContainerCnv: public T_AthenaPoolCustomCnv<
                                      Analysis :: TauJetContainer,
                                      TauJetContainer_PERS
                                   >

{
    friend class CnvFactory<TauJetContainerCnv>;
    protected:
        TauJetContainerCnv( ISvcLocator* svcloc );
        virtual TauJetContainer_PERS *createPersistent( Analysis :: TauJetContainer *transCont );
        virtual Analysis :: TauJetContainer *createTransient();
};

inline TauJetContainerCnv :: TauJetContainerCnv( ISvcLocator* svcLocator ):
    T_AthenaPoolCustomCnv<Analysis :: TauJetContainer, TauJetContainer_PERS>( svcLocator )
{
}

#endif //> RECATHENAPOOL_TAUJETCONTAINERCNV_H
