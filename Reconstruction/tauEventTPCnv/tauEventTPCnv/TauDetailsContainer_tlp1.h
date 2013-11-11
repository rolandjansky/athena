/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauDetailsContainer_tlp1.h
/// 
/// Definition of persistent class 
/// for Analysis::TauDetailsContainer
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess  
/// @author Lukasz Janyst
/// 
/// Created by Lukasz Janyst on 2007-07-01.
///
/// $Id: TauDetailsContainer_tlp1.h,v 1.5 2009-01-20 17:19:02 tburgess Exp $

#ifndef tauEventTPCnv_TAU_DETAILS_CONTAINER_TLP1_H
#define tauEventTPCnv_TAU_DETAILS_CONTAINER_TLP1_H

#include <vector>
#include "tauEventTPCnv/TauDetailsContainer_p1.h"
#include "tauEventTPCnv/TauCommonDetails_p1.h"
#include "tauEventTPCnv/TauCommonExtraDetails_p1.h"
#include "tauEventTPCnv/Tau1P3PDetails_p1.h"
#include "tauEventTPCnv/Tau1P3PExtraDetails_p1.h"
#include "tauEventTPCnv/TauRecDetails_p1.h"
#include "tauEventTPCnv/TauRecExtraDetails_p1.h"
#include "tauEventTPCnv/tauAnalysisHelperObject_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertex_p1.h"
#include "TrkEventTPCnv/VxVertex/Vertex_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

//Pre declarations
class TauDetailsContainerCnv_tlp1;

///Persistent class for Analysis::TauDetailsContainer
class TauDetailsContainer_tlp1
{
    friend class TauDetailsContainerCnv_tlp1;
public:
    ///Constructor 
    TauDetailsContainer_tlp1();
    
    ///Destructor
    virtual ~TauDetailsContainer_tlp1();
private:
    ///Tau details container
    std::vector<TauDetailsContainer_p1> m_tauDetailsContainers;

    ///Tau common details
    std::vector<TauCommonDetails_p1> m_tauCommonDetails;

    ///Tau extra common details
    std::vector<TauCommonExtraDetails_p1> m_tauCommonExtraDetails;

    ///Tau 1P3P details
    ///Deprecated - there for backwards compatability
    std::vector<Tau1P3PDetails_p1> m_tau1P3PDetails;

    ///Tau 1P3P details
    ///Deprecated - there for backwards compatability
    std::vector<Tau1P3PExtraDetails_p1> m_tau1P3PExtraDetails;

    ///Tau Rec Details
    ///Deprecated - there for backwards compatability
    std::vector<TauRecDetails_p1> m_tauRecDetails;

    ///Tau Rec Extra details
    ///Deprecated - there for backwards compatability
    std::vector<TauRecExtraDetails_p1> m_tauRecExtraDetails;

    /// Tau analysis helper
    /// Deprecated - there for backwards compatability
    std::vector<tauAnalysisHelperObject_p1> m_tauAnalysisHelperObjects;

    ///Rec vertex
    std::vector<Trk::RecVertex_p1> m_recVertices;

    ///Vertex
    std::vector<Trk::Vertex_p1> m_vertices;

    ///Fit quality
    std::vector<Trk::FitQuality_p1> m_fitQualities;

    ///Error matrix
    std::vector<Trk::HepSymMatrix_p1> m_errorMatrices;
};

#endif // tauEventTPCnv_TAU_DETAILS_CONTAINER_TLP1_H
