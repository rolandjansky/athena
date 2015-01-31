/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauDetailsContainerCnv_tlp1.cxx
///
/// Implementation of transient<->persistent converter
/// for Analysis::TauDetailsContainer
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created by Lukasz Janyst on 2007-07-05.
///
/// $Id: TauDetailsContainerCnv_tlp1.cxx,v 1.9 2009-05-08 14:32:44 tburgess Exp $

#include "tauEventTPCnv/TauDetailsContainerCnv_tlp1.h"

TauDetailsContainerCnv_tlp1 :: TauDetailsContainerCnv_tlp1()
{
    addMainTPConverter();
    addTPConverterForReading(&m_tau1P3PDetailsCnv );
    addTPConverterForReading(&m_tau1P3PExtraDetailsCnv );
    addTPConverterForReading(&m_tauRecDetailsCnv );
    addTPConverterForReading(&m_tauRecExtraDetailsCnv );
    addTPConverterForReading(&m_tauAnalysisHelperObjectCnv );
    addTPConverter(&m_recVertexCnv );
    addTPConverter(&m_vertexCnv );
    addTPConverter(&m_fitQualityCnv );
    addTPConverter(&m_errorMatrixCnv );
    addTPConverter(&m_tauCommonDetailsCnv );
    addTPConverter(&m_tauCommonExtraDetailsCnv );

}

void TauDetailsContainerCnv_tlp1 :: setPStorage(
    TauDetailsContainer_tlp1 *storage)
{
    setMainCnvPStorage(&storage->m_tauDetailsContainers);
    m_tau1P3PDetailsCnv.setPStorage(&storage->m_tau1P3PDetails);
    m_tau1P3PExtraDetailsCnv.setPStorage(&storage->m_tau1P3PExtraDetails);
    m_tauRecDetailsCnv.setPStorage(&storage->m_tauRecDetails);
    m_tauRecExtraDetailsCnv.setPStorage(&storage->m_tauRecExtraDetails);
    m_tauAnalysisHelperObjectCnv.setPStorage(
	&storage->m_tauAnalysisHelperObjects);
    m_recVertexCnv.setPStorage(&storage->m_recVertices);
    m_vertexCnv.setPStorage(&storage->m_vertices);
    m_fitQualityCnv.setPStorage(&storage->m_fitQualities);
    m_errorMatrixCnv.setPStorage(&storage->m_errorMatrices);
    m_tauCommonDetailsCnv.setPStorage(
	&storage->m_tauCommonDetails);
    m_tauCommonExtraDetailsCnv.setPStorage(
	&storage->m_tauCommonExtraDetails);
}

