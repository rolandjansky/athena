/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_tlp2.h"

TrackParticleTruthCollectionContainerCnv_tlp2::TrackParticleTruthCollectionContainerCnv_tlp2()
{
    addMainTPConverter();
    //addTPConverter(&m_trackparttruthcollCnv );
    
}

void TrackParticleTruthCollectionContainerCnv_tlp2 :: setPStorage(
    TrackParticleTruthCollectionContainer_tlp2 *storage)
{
    setMainCnvPStorage(&storage->m_trackPartTruthCollConts);
}


T_TPCnv<TrackParticleTruthCollectionContainer,TrackParticleTruthCollectionContainer_tlp2>::T_TPCnv()
{}

T_TPCnv<TrackParticleTruthCollectionContainer,TrackParticleTruthCollectionContainer_tlp2>::~T_TPCnv()
{}

void T_TPCnv<TrackParticleTruthCollectionContainer,TrackParticleTruthCollectionContainer_tlp2>::
persToTrans (const TrackParticleTruthCollectionContainer_tlp2* pers,
             TrackParticleTruthCollectionContainer* trans,
             MsgStream& msg)
{
    setPStorage (const_cast<TrackParticleTruthCollectionContainer_tlp2*> (pers));
    m_mainConverter.pstoreToTrans (0, trans, msg);
}

void T_TPCnv<TrackParticleTruthCollectionContainer,TrackParticleTruthCollectionContainer_tlp2>::
transToPers (
    const TrackParticleTruthCollectionContainer* trans, 
    TrackParticleTruthCollectionContainer_tlp2* pers, 
    MsgStream& msg)
{
    this->setTLPersObject(pers) ;
    m_mainConverter.virt_toPersistent(trans, msg);
    this->clearTLPersObject();
}
