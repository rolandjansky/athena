/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECATHENAPOOL_MUONSPSHOWERCONTAINERCNV_H 
#define RECATHENAPOOL_MUONSPSHOWERCONTAINERCNV_H 

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// RecTPCnv includes
#include "RecTPCnv/MuonSpShowerContainer_p1.h"

// muonEvent includes
#include "muonEvent/MuonSpShowerContainer.h"


// Forward declaration

// the latest persistent representation type of DataCollection:
typedef MuonSpShowerContainer_p1  MuonSpShowerContainer_PERS;

class MuonSpShowerContainerCnv : 
        public T_AthenaPoolCustomCnv<
            Rec::MuonSpShowerContainer, 
            MuonSpShowerContainer_PERS > 
{

  // make the factory for this converter our friend
    friend class CnvFactory<MuonSpShowerContainerCnv>;

protected:

  /** Create the converter from the service locator*/
    MuonSpShowerContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.*/
    virtual MuonSpShowerContainer_PERS*
        createPersistent( Rec::MuonSpShowerContainer* transCont );

  /** Build the transient representation from a persistent one*/
    virtual Rec::MuonSpShowerContainer* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline MuonSpShowerContainerCnv::MuonSpShowerContainerCnv( ISvcLocator* svcLocator ) :
        T_AthenaPoolCustomCnv< Rec::MuonSpShowerContainer, MuonSpShowerContainer_PERS>(svcLocator)
{
}

#endif //> RECATHENAPOOL_MUONCONTAINERCNV_H
