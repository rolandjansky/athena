/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauDetailsContainerCnv_tlp4.h
/// 
/// Definition of transient<->persistent converter 
/// for Analysis::TauDetailsContainer
/// 

#ifndef TAUEVENTTPCNV_TAUDETAILSCONTAINERCNV_TLP4_H
#define TAUEVENTTPCNV_TAUDETAILSCONTAINERCNV_TLP4_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TauDetailsContainer_tlp4.h"
#include "TauDetailsContainerCnv_p1.h"
#include "tauEventTPCnv/TauCommonDetailsCnv_p2.h"
#include "tauEventTPCnv/TauCommonExtraDetailsCnv_p1.h"
#include "tauEventTPCnv/TauPi0DetailsCnv_p2.h"
#include "tauEventTPCnv/TauPi0CandidateCnv_p1.h"
#include "tauEventTPCnv/TauPi0ClusterCnv_p1.h"
#include "tauEventTPCnv/TauShotCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/VertexCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

///Tau details container converter
class TauDetailsContainerCnv_tlp4:public AthenaPoolTopLevelTPConverter<TauDetailsContainerCnv_p1,TauDetailsContainer_tlp4> {
public:
    ///Constructor
    TauDetailsContainerCnv_tlp4();

    ///Destructor
    virtual ~TauDetailsContainerCnv_tlp4() {}

    ///Set persistant storage
    virtual void setPStorage( TauDetailsContainer_tlp4 *storage );

protected:
    ///Common details converter
    TauCommonDetailsCnv_p2 m_tauCommonDetailsCnv;

    ///Extra common details converter
    TauCommonExtraDetailsCnv_p1 m_tauCommonExtraDetailsCnv;

    ///TauPi0Details Converter
    TauPi0DetailsCnv_p2 m_TauPi0DetailsCnv;

    ///TauPi0Candidate Converter
    TauPi0CandidateCnv_p1 m_TauPi0CandidateCnv;
    
    ///TauPi0Cluster Converter
    TauPi0ClusterCnv_p1 m_TauPi0ClusterCnv;
    
    ///TauShot Converter
    TauShotCnv_p1 m_TauShotCnv;

    ///Rec vertex converter
    RecVertexCnv_p1 m_recVertexCnv;
    
    ///Vertex converter
    VertexCnv_p1 m_vertexCnv;

    ///Fit quality converter
    FitQualityCnv_p1 m_fitQualityCnv;

    ///Error matrix converter
    ErrorMatrixCnv_p1 m_errorMatrixCnv;
};

#endif 
