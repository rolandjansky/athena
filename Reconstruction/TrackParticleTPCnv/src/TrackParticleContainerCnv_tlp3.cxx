/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTPCnv/TrackParticleContainerCnv_tlp3.h"
#include "CxxUtils/checker_macros.h"

TrackParticleContainerCnv_tlp3::TrackParticleContainerCnv_tlp3(bool nosurf /*= false*/)
  : m_trackParametersCnv (nosurf)
{
    addMainTPConverter();

    addTPConverter( &m_trackParticleCnv);
    addTPConverter( &m_trackParametersCnv);
    addTPConverter( &m_trackSummaryCnv);
    addTPConverter( &m_covarianceMatricesCnv);
    addTPConverter( &m_errorMatricesCnv);
}

void TrackParticleContainerCnv_tlp3::setPStorage( Rec::TrackParticleContainer_tlp3 *storage )
{ 
    setMainCnvPStorage( &storage->m_trackParticleContainer);
    m_trackParticleCnv.      setPStorage(&storage->m_trackParticle);
    m_trackParametersCnv.     setPStorage(&storage->m_trackParameters);
    m_trackSummaryCnv.        setPStorage(&storage->m_trackSummaries);
    m_covarianceMatricesCnv.  setPStorage(&storage->m_hepSymMatrices);
    m_errorMatricesCnv.       setPStorage(&storage->m_hepSymMatrices);

}


void T_TPCnv<Rec::TrackParticleContainer, Rec::TrackParticleContainer_tlp3 >::
persToTrans (const Rec::TrackParticleContainer_tlp3* pers, Rec::TrackParticleContainer* trans, MsgStream& msg)
{
    // FIXME: TPConverter uses the same non-const member m_pStorage
    // for both reading and writing, but we want it to be const
    // in the former case.
    Rec::TrackParticleContainer_tlp3* pers_nc ATLAS_THREAD_SAFE =
      const_cast<Rec::TrackParticleContainer_tlp3*> (pers);
    setPStorage (pers_nc);
    m_mainConverter.pstoreToTrans (0, trans, msg);
}

void T_TPCnv<Rec::TrackParticleContainer, Rec::TrackParticleContainer_tlp3 >::
transToPers (const Rec::TrackParticleContainer* trans, Rec::TrackParticleContainer_tlp3* pers, MsgStream& msg)  {
  // std::cout<<"TrackParticleContainer::transToPers - about to call this->setTLPersObject( pers )"<<std::endl;
    this->setTLPersObject( pers );
    m_mainConverter.virt_toPersistent(trans, msg);
    this->clearTLPersObject();
}

