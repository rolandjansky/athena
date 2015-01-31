/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauDetailsContainerCnv_tlp2.cxx
///
/// Implementation of transient<->persistent converter
/// for Analysis::TauDetailsContainer

#include "tauEventTPCnv/TauDetailsContainerCnv_tlp2.h"

TauDetailsContainerCnv_tlp2 :: TauDetailsContainerCnv_tlp2(){
    addMainTPConverter();
    addTPConverter(&m_recVertexCnv );
    addTPConverter(&m_vertexCnv );
    addTPConverter(&m_fitQualityCnv );
    addTPConverter(&m_errorMatrixCnv );
    addTPConverter(&m_tauCommonDetailsCnv );
    addTPConverter(&m_tauCommonExtraDetailsCnv );

}

void TauDetailsContainerCnv_tlp2 :: setPStorage(TauDetailsContainer_tlp2 *storage){
    setMainCnvPStorage(&storage->m_tauDetailsContainers);
    m_recVertexCnv.setPStorage(&storage->m_recVertices);
    m_vertexCnv.setPStorage(&storage->m_vertices);
    m_fitQualityCnv.setPStorage(&storage->m_fitQualities);
    m_errorMatrixCnv.setPStorage(&storage->m_errorMatrices);
    m_tauCommonDetailsCnv.setPStorage(&storage->m_tauCommonDetails);
    m_tauCommonExtraDetailsCnv.setPStorage(&storage->m_tauCommonExtraDetails);
}

