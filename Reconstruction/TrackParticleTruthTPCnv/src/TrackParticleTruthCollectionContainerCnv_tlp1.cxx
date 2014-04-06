/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_tlp1.h"

TrackParticleTruthCollectionContainerCnv_tlp1::TrackParticleTruthCollectionContainerCnv_tlp1()
{
    addMainTPConverter();
    //addTPConverter(&m_trackparttruthcollCnv );
    
}

void TrackParticleTruthCollectionContainerCnv_tlp1 :: setPStorage(
    TrackParticleTruthCollectionContainer_tlp1 *storage)
{
    setMainCnvPStorage(&storage->m_trackPartTruthCollConts);
}


T_TPCnv<TrackParticleTruthCollectionContainer,TrackParticleTruthCollectionContainer_tlp1>::T_TPCnv()
{}

T_TPCnv<TrackParticleTruthCollectionContainer,TrackParticleTruthCollectionContainer_tlp1>::~T_TPCnv()
{}

void T_TPCnv<TrackParticleTruthCollectionContainer,TrackParticleTruthCollectionContainer_tlp1>::
persToTrans (const TrackParticleTruthCollectionContainer_tlp1* pers,
             TrackParticleTruthCollectionContainer* trans,
             MsgStream& msg)
{
    setPStorage (const_cast<TrackParticleTruthCollectionContainer_tlp1*> (pers));
    m_mainConverter.pstoreToTrans (0, trans, msg);
}

void T_TPCnv<TrackParticleTruthCollectionContainer,TrackParticleTruthCollectionContainer_tlp1>::
transToPers (
    const TrackParticleTruthCollectionContainer* trans, 
    TrackParticleTruthCollectionContainer_tlp1* pers, 
    MsgStream& msg)
{
    this->setTLPersObject(pers) ;
    m_mainConverter.virt_toPersistent(trans, msg);
    this->clearTLPersObject();
}
