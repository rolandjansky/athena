/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauDetailsContainer_tlp2.h
/// 
/// Definition of persistent class 
/// for Analysis::TauDetailsContainer
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Ilija Vukotic

#ifndef tauEventTPCnv_TAU_DETAILS_CONTAINER_TLP2_H
#define tauEventTPCnv_TAU_DETAILS_CONTAINER_TLP2_H

#include <vector>
#include "tauEventTPCnv/TauDetailsContainer_p1.h"
#include "tauEventTPCnv/TauCommonDetails_p2.h"
#include "tauEventTPCnv/TauCommonExtraDetails_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertex_p1.h"
#include "TrkEventTPCnv/VxVertex/Vertex_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

//Pre declarations
class TauDetailsContainerCnv_tlp2;

///Persistent class for Analysis::TauDetailsContainer
class TauDetailsContainer_tlp2{
    friend class TauDetailsContainerCnv_tlp2;
public:
    ///Constructor 
    TauDetailsContainer_tlp2();
    
    ///Destructor
    virtual ~TauDetailsContainer_tlp2();
private:
    ///Tau details container
    std::vector<TauDetailsContainer_p1> m_tauDetailsContainers;

    ///Tau common details
    std::vector<TauCommonDetails_p2> m_tauCommonDetails;

    ///Tau extra common details
    std::vector<TauCommonExtraDetails_p1> m_tauCommonExtraDetails;

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
