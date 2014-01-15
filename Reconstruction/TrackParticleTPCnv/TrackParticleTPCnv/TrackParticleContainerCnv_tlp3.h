/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLECONTAINER_CNV_TLP3_REC_H
#define TRACKPARTICLECONTAINER_CNV_TLP3_REC_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrackParticleContainer_tlp3.h"

//-----------------------------------------
// TrackParticle
//-----------------------------------------
#include "TrackParticleTPCnv/Particle/TrackParticleContainerCnv_p2.h"
#include "TrackParticleTPCnv/Particle/TrackParticleCnv_p3.h"

//-----------------------------------------------------------------------------
// Trk Parameters
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p2.h"

//-----------------------------------------------------------------------------
// TrkEventPrimitives
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkEventPrimitives/CovarianceMatrixCnv_p1.h"            

//-----------------------------------------------------------------------------
// TrkTrackSummary
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrackSummary/TrackSummaryCnv_p2.h"

//------------------------------------------------------------------------------
// TrackInfo
//------------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/TrackInfoCnv_p1.h" 

class TrackParticleContainerCnv_tlp3 : public AthenaPoolTopLevelTPConverter< TrackParticleContainerCnv_p2, Rec::TrackParticleContainer_tlp3 >
{

public:

    TrackParticleContainerCnv_tlp3(bool nosurf = false);
    virtual ~TrackParticleContainerCnv_tlp3() {}

//default methods  
    virtual void	setPStorage( Rec::TrackParticleContainer_tlp3 *storage );

// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
    virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

protected:

//TrackParticle
    TrackParticleCnv_p3                 m_trackParticleCnv;

    TrackParametersCnv_p2               m_trackParametersCnv;

// TrkTrackSummary
    TrackSummaryCnv_p2                  m_trackSummaryCnv;

// TrkEventPrimitives
    CovarianceMatrixCnv_p1              m_covarianceMatricesCnv;
    ErrorMatrixCnv_p1                   m_errorMatricesCnv;
};


template <>
class T_TPCnv<Rec::TrackParticleContainer, Rec::TrackParticleContainer_tlp3 >
    : public TrackParticleContainerCnv_tlp3{
public:
  // ??? Maybe this should be moved up to AthenaPoolTopLevelTPConverter...
    virtual void persToTrans (const Rec::TrackParticleContainer_tlp3* pers,
        Rec::TrackParticleContainer* trans,
        MsgStream& msg);
    virtual void transToPers (const Rec::TrackParticleContainer* trans,
        Rec::TrackParticleContainer_tlp3* pers,
        MsgStream& msg);
};


class TrackParticleContainerARACnv_tlp3
  : public TrackParticleContainerCnv_tlp3
{
public:
  TrackParticleContainerARACnv_tlp3()
    : TrackParticleContainerCnv_tlp3(true) {}
};


#endif
