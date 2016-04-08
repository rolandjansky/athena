/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauDetailsContainerCnv_tlp1.h
/// 
/// Definition of transient<->persistent converter 
/// for Analysis::TauDetailsContainer
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess  
/// @author Lukasz Janyst
/// 
/// Created by Lukasz Janyst on 2007-07-05.
///
/// $Id: TauDetailsContainerCnv_tlp1.h,v 1.7 2009-01-20 17:19:02 tburgess Exp $

#ifndef TAUEVENTTPCNV_TAUDETAILSCONTAINERCNV_TLP1_Ha
#define TAUEVENTTPCNV_TAUDETAILSCONTAINERCNV_TLP1_Ha

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TauDetailsContainer_tlp1.h"
#include "TauDetailsContainerCnv_p1.h"
#include "tauEventTPCnv/TauCommonDetailsCnv_p1.h"
#include "tauEventTPCnv/TauCommonExtraDetailsCnv_p1.h"
#include "tauEventTPCnv/Tau1P3PDetailsCnv_p1.h"
#include "tauEventTPCnv/Tau1P3PExtraDetailsCnv_p1.h"
#include "tauEventTPCnv/TauRecDetailsCnv_p1.h"
#include "tauEventTPCnv/TauRecExtraDetailsCnv_p1.h"
#include "tauEventTPCnv/tauAnalysisHelperObjectCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/VertexCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

///Tau details container converter
class TauDetailsContainerCnv_tlp1:
    public AthenaPoolTopLevelTPConverter<
    TauDetailsContainerCnv_p1, 
    TauDetailsContainer_tlp1>
{
public:
    ///Constructor
    TauDetailsContainerCnv_tlp1();

    ///Destructor
    virtual ~TauDetailsContainerCnv_tlp1() {}

    ///Set persistant storage
    virtual void setPStorage( TauDetailsContainer_tlp1 *storage );

protected:
    ///Common details converter
    TauCommonDetailsCnv_p1 m_tauCommonDetailsCnv;

    ///Extra common details converter
    TauCommonExtraDetailsCnv_p1 m_tauCommonExtraDetailsCnv;

    ///Tau1P3P details converter
    ///Deprecated - there for backwards compatability
    Tau1P3PDetailsCnv_p1 m_tau1P3PDetailsCnv;

    ///Tau1P3P extra details converter
    ///Deprecated - there for backwards compatability
    Tau1P3PExtraDetailsCnv_p1 m_tau1P3PExtraDetailsCnv;

    ///TauRec details converter
    ///Deprecated - there for backwards compatability
    TauRecDetailsCnv_p1 m_tauRecDetailsCnv;

    ///TauRec extra details converter
    ///Deprecated - there for backwards compatability
    TauRecExtraDetailsCnv_p1 m_tauRecExtraDetailsCnv;

    ///Tau analysis helper
    ///Deprecated - there for backwards compatability
    tauAnalysisHelperObjectCnv_p1 m_tauAnalysisHelperObjectCnv;

    ///Rec vertex converter
    RecVertexCnv_p1 m_recVertexCnv;
    
    ///Vertex converter
    VertexCnv_p1 m_vertexCnv;

    ///Fit quality converter
    FitQualityCnv_p1 m_fitQualityCnv;

    ///Error matrix converter
    ErrorMatrixCnv_p1 m_errorMatrixCnv;
};

#endif // TAUEVENTTPCNV_TAUDETAILSCONTAINERCNV_TLP1_Ha
