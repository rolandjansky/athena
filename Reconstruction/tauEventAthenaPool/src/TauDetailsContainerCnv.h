/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauDetailsContainerCnv.h
// author: Lukasz Janyst
// date:   July 2007
//-----------------------------------------------------------------------------

#ifndef RECATHENAPOOL_TAUDETAILSCONTAINERCNV_H
#define RECATHENAPOOL_TAUDETAILSCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "tauEventTPCnv/TauDetailsContainer_tlp4.h"
#include "tauEvent/TauDetailsContainer.h"

typedef TauDetailsContainer_tlp4  TauDetailsContainer_PERS;

class TauDetailsContainerCnv: public T_AthenaPoolCustomCnv<
                                      Analysis :: TauDetailsContainer,
                                      TauDetailsContainer_PERS
                                   >

{
    friend class CnvFactory<TauDetailsContainerCnv>;
    protected:
        TauDetailsContainerCnv( ISvcLocator* svcloc );
        virtual TauDetailsContainer_PERS *createPersistent( Analysis :: TauDetailsContainer *transCont );
        virtual Analysis :: TauDetailsContainer *createTransient();
};

inline TauDetailsContainerCnv :: TauDetailsContainerCnv( ISvcLocator* svcLocator ):
    T_AthenaPoolCustomCnv<Analysis :: TauDetailsContainer, TauDetailsContainer_PERS>( svcLocator )
{
}

#endif // RECATHENAPOOL_TAUDETAILSCONTAINERCNV_H
