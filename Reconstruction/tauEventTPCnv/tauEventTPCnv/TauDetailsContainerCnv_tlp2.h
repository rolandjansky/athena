/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauDetailsContainerCnv_tlp2.h
/// 
/// Definition of transient<->persistent converter 
/// for Analysis::TauDetailsContainer
/// 

#ifndef TAUEVENTTPCNV_TAUDETAILSCONTAINERCNV_TLP2_H
#define TAUEVENTTPCNV_TAUDETAILSCONTAINERCNV_TLP2_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TauDetailsContainer_tlp2.h"
#include "TauDetailsContainerCnv_p1.h"
#include "tauEventTPCnv/TauCommonDetailsCnv_p2.h"
#include "tauEventTPCnv/TauCommonExtraDetailsCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/VertexCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

///Tau details container converter
class TauDetailsContainerCnv_tlp2:public AthenaPoolTopLevelTPConverter<TauDetailsContainerCnv_p1,TauDetailsContainer_tlp2> {
public:
    ///Constructor
    TauDetailsContainerCnv_tlp2();

    ///Destructor
    virtual ~TauDetailsContainerCnv_tlp2() {}

    ///Set persistant storage
    virtual void setPStorage( TauDetailsContainer_tlp2 *storage );

protected:
    ///Common details converter
    TauCommonDetailsCnv_p2 m_tauCommonDetailsCnv;

    ///Extra common details converter
    TauCommonExtraDetailsCnv_p1 m_tauCommonExtraDetailsCnv;

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
