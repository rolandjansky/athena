/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauDetailsContainerCnv_tlp4.cxx
///
/// Implementation of top-level transient<->persistent converter
/// for Analysis::TauDetailsContainer

#include "tauEventTPCnv/TauDetailsContainerCnv_tlp4.h"

TauDetailsContainerCnv_tlp4 :: TauDetailsContainerCnv_tlp4(){
    addMainTPConverter();
    addTPConverter(&m_recVertexCnv );
    addTPConverter(&m_vertexCnv );
    addTPConverter(&m_fitQualityCnv );
    addTPConverter(&m_errorMatrixCnv );
    addTPConverter(&m_tauCommonDetailsCnv );
    addTPConverter(&m_tauCommonExtraDetailsCnv );
    addTPConverter(&m_PanTauDetailsCnv);
    addTPConverter(&m_TauPi0ClusterCnv);
    addTPConverter(&m_TauPi0CandidateCnv);
    addTPConverter(&m_TauShotCnv);
    addTPConverter(&m_TauPi0DetailsCnv);

}

void TauDetailsContainerCnv_tlp4 :: setPStorage(TauDetailsContainer_tlp4 *storage){
    setMainCnvPStorage(&storage->m_tauDetailsContainers);
    m_recVertexCnv.setPStorage(&storage->m_recVertices);
    m_vertexCnv.setPStorage(&storage->m_vertices);
    m_fitQualityCnv.setPStorage(&storage->m_fitQualities);
    m_errorMatrixCnv.setPStorage(&storage->m_errorMatrices);
    m_tauCommonDetailsCnv.setPStorage(&storage->m_tauCommonDetails);
    m_tauCommonExtraDetailsCnv.setPStorage(&storage->m_tauCommonExtraDetails);
    m_PanTauDetailsCnv.setPStorage(&storage->m_PanTauDetails);
    m_TauPi0ClusterCnv.setPStorage(&storage->m_TauPi0Cluster);
    m_TauPi0CandidateCnv.setPStorage(&storage->m_TauPi0Candidate);
    m_TauShotCnv.setPStorage(&storage->m_TauShot);
    m_TauPi0DetailsCnv.setPStorage(&storage->m_TauPi0Details);
}

T_TPCnv<Analysis::TauDetailsContainer, TauDetailsContainer_tlp4 >::T_TPCnv(){}

T_TPCnv<Analysis::TauDetailsContainer, TauDetailsContainer_tlp4 >::~T_TPCnv(){}

void T_TPCnv<Analysis::TauDetailsContainer,TauDetailsContainer_tlp4 >::persToTrans (const TauDetailsContainer_tlp4* pers,
	     Analysis::TauDetailsContainer* trans, MsgStream& msg){
    setPStorage (const_cast<TauDetailsContainer_tlp4*> (pers));
    m_mainConverter.pstoreToTrans (0, trans, msg);
}

void T_TPCnv<Analysis::TauDetailsContainer, TauDetailsContainer_tlp4 >::transToPers (const Analysis::TauDetailsContainer* trans, TauDetailsContainer_tlp4* pers, MsgStream& msg){
    this->setTLPersObject(pers) ;
    m_mainConverter.virt_toPersistent(trans, msg);
    this->clearTLPersObject();
}
