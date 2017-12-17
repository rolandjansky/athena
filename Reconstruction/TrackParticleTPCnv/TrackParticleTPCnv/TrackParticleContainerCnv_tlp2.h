/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLECONTAINER_CNV_TLP2_REC_H
#define TRACKPARTICLECONTAINER_CNV_TLP2_REC_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrackParticleContainer_tlp2.h"

//-----------------------------------------
// TrackParticle
//-----------------------------------------
#include "TrackParticleTPCnv/Particle/TrackParticleContainerCnv_p1.h"
#include "TrackParticleTPCnv/Particle/TrackParticleContainerCnv_p2.h"
#include "TrackParticleTPCnv/Particle/TrackParticleCnv_p1.h"
#include "TrackParticleTPCnv/Particle/TrackParticleCnv_p2.h"

//-----------------------------------------
// TrkParticleBase
//-----------------------------------------
#include "TrkEventTPCnv/TrkParticleBase/TrackParticleBaseCnv_p1.h"

//-----------------------------------------------------------------------------
// Trk Parameters
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkParameters/PerigeeCnv_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigeeCnv_p1.h"
#include "TrkEventTPCnv/TrkParameters/AtaSurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredAtaSurfaceCnv_p1.h"

//-----------------------------------------------------------------------------
// TrkNeutralParameters
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkNeutralParameters/NeutralParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkNeutralParameters/NeutralPerigeeCnv_p1.h"
#include "TrkEventTPCnv/TrkNeutralParameters/MeasuredNeutralPerigeeCnv_p1.h"
#include "TrkEventTPCnv/TrkNeutralParameters/NeutralAtaSurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkNeutralParameters/MeasuredNeutralAtaSurfaceCnv_p1.h"

//-----------------------------------------------------------------------------
// Trk Surfaces
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkSurfaces/PerigeeSurfaceCnv_p1.h"         
#include "TrkEventTPCnv/TrkSurfaces/BoundSurfaceCnv_p1.h"            
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p1.h"                 
#include "TrkEventTPCnv/TrkSurfaces/CylinderBoundsCnv_p1.h"          
#include "TrkEventTPCnv/TrkSurfaces/DiamondBoundsCnv_p1.h"           
#include "TrkEventTPCnv/TrkSurfaces/DiscBoundsCnv_p1.h"              
#include "TrkEventTPCnv/TrkSurfaces/RectangleBoundsCnv_p1.h"         
#include "TrkEventTPCnv/TrkSurfaces/TrapezoidBoundsCnv_p1.h"      
#include "TrkEventTPCnv/TrkSurfaces/RotatedTrapezoidBoundsCnv_p1.h"     
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"   

//-----------------------------------------------------------------------------
// TrkEventPrimitives
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkEventPrimitives/CovarianceMatrixCnv_p1.h"            

//-----------------------------------------------------------------------------
// TrkTrackSummary
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrackSummary/TrackSummaryCnv_p2.h"

//-----------------------------------------------------------------------------
// TrkTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/TrackCnv_p1.h"

//-----------------------------------------------------------------------------
// VxCandidate (for EL?)
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/VxVertex/VxCandidateCnv_p1.h"

//-----------------------------------------------------------------------------
// Element Link for track collection
//-----------------------------------------------------------------------------
#include "AthLinks/ElementLink.h" 
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxContainer.h"


//------------------------------------------------------------------------------
// TrackInfo
//------------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/TrackInfoCnv_p1.h" 

#include "TrackParticleTPCnv/DummyDetElementSurfaceCnv_p1.h"

class TrackParticleContainerCnv_tlp2 : public AthenaPoolTopLevelTPConverter< TrackParticleContainerCnv_p2, Rec::TrackParticleContainer_tlp2 >
{

public:

    TrackParticleContainerCnv_tlp2();
    virtual ~TrackParticleContainerCnv_tlp2() {}

//default methods  
    virtual void	setPStorage( Rec::TrackParticleContainer_tlp2 *storage );

// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
    virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

protected:

//TrackParticle
    TrackParticleCnv_p2                 m_trackParticleCnv_p2;

// TrkParameters   
    TrackParametersCnv_p1               m_trackParametersCnv;
    PerigeeCnv_p1                       m_perigeeCnv;
    AtaDiscCnv_p1                       m_ataDiscsCnv;
    AtaCylinderCnv_p1                   m_ataCylindersCnv;
    AtaPlaneCnv_p1                      m_ataPlanesCnv;
    AtaStraightLineCnv_p1               m_ataStraightLinesCnv;
    MeasuredPerigeeCnv_p1               m_measuredPerigeeCnv;
    MeasuredAtaDiscCnv_p1               m_measuredAtaDiscsCnv;
    MeasuredAtaCylinderCnv_p1           m_measuredAtaCylindersCnv;
    MeasuredAtaPlaneCnv_p1              m_measuredAtaPlanesCnv;
    MeasuredAtaStraightLineCnv_p1       m_measuredAtaStraightLinesCnv;

// TrkNeutralParameters
    NeutralParametersCnv_p1               m_neutralParametersCnv;
    NeutralPerigeeCnv_p1                  m_neutralPerigeeCnv;
    NeutralAtaDiscCnv_p1                  m_neutralAtaDiscsCnv;
    NeutralAtaCylinderCnv_p1              m_neutralAtaCylindersCnv;
    NeutralAtaPlaneCnv_p1                 m_neutralAtaPlanesCnv;
    NeutralAtaStraightLineCnv_p1          m_neutralAtaStraightLinesCnv;
    MeasuredNeutralPerigeeCnv_p1          m_measuredNeutralPerigeeCnv;
    MeasuredNeutralAtaDiscCnv_p1          m_measuredNeutralAtaDiscsCnv;
    MeasuredNeutralAtaCylinderCnv_p1      m_measuredNeutralAtaCylindersCnv;
    MeasuredNeutralAtaPlaneCnv_p1         m_measuredNeutralAtaPlanesCnv;
    MeasuredNeutralAtaStraightLineCnv_p1  m_measuredNeutralAtaStraightLinesCnv;

    PerigeeSurfaceCnv_p1                m_perigeeSurfacesCnv;
    DiscSurfaceCnv_p1                   m_discSurfacesCnv;
    PlaneSurfaceCnv_p1                  m_planeSurfacesCnv;
    CylinderSurfaceCnv_p1               m_cylinderSurfacesCnv;
    StraightLineSurfaceCnv_p1           m_straightLineSurfacesCnv;
    DetElementSurfaceCnv_p1             m_detElSurfCnv;

    SurfaceCnv_p1                       m_surfacesCnv;
    CylinderBoundsCnv_p1                m_cylinderBoundsCnv;
    DiamondBoundsCnv_p1                 m_diamondBoundsCnv;
    DiscBoundsCnv_p1                    m_discBoundsCnv;
    RectangleBoundsCnv_p1               m_rectangleBoundsCnv;
    TrapezoidBoundsCnv_p1               m_trapesoidBoundsCnv;
    RotatedTrapezoidBoundsCnv_p1        m_rotatedTrapesoidBoundsCnv;

// TrkTrackSummary
    TrackSummaryCnv_p2                  m_trackSummaryCnv;

// TrkEventPrimitives
    CovarianceMatrixCnv_p1              m_covarianceMatricesCnv;
    ErrorMatrixCnv_p1                   m_errorMatricesCnv;

};


template <>
class T_TPCnv<Rec::TrackParticleContainer, Rec::TrackParticleContainer_tlp2 >
    : public TrackParticleContainerCnv_tlp2{
public:
  // ??? Maybe this should be moved up to AthenaPoolTopLevelTPConverter...
    virtual void persToTrans (const Rec::TrackParticleContainer_tlp2* pers,
        Rec::TrackParticleContainer* trans,
        MsgStream& msg);
    virtual void transToPers (const Rec::TrackParticleContainer* trans,
        Rec::TrackParticleContainer_tlp2* pers,
        MsgStream& msg);
};


class TrackParticleContainerARACnv_tlp2
  : public T_TPCnv<Rec::TrackParticleContainer, Rec::TrackParticleContainer_tlp2 >
{
public:
  TrackParticleContainerARACnv_tlp2();

private:
  DummyDetElementSurfaceCnv_p1   m_dumDetElSurfCnv;
};


#endif
