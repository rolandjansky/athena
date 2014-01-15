/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParticleCnv_p1.h
// author: Andreas Wildauer
//
//-----------------------------------------------------------------------------

#ifndef TRACKPARTICLE_CNV_P1_REC_H
#define TRACKPARTICLE_CNV_P1_REC_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkParticleBase/TrackParticleBase.h"
#include "Particle/TrackParticle.h"
#include "TrackParticleTPCnv/Particle/TrackParticle_p1.h"

class MsgStream;

class TrackParticleCnv_p1: 
     public T_AthenaPoolTPPolyCnvBase< Trk::TrackParticleBase, Rec::TrackParticle, Rec::TrackParticle_p1>
{
    public:
        TrackParticleCnv_p1() : m_trackParticleBaseCnv(0) {}
        virtual void persToTrans( const Rec :: TrackParticle_p1 *persObj,
                                        Rec :: TrackParticle    *transObj,
                                        MsgStream                        &log );
        virtual void transToPers( const Rec :: TrackParticle    *transObj,
                                        Rec :: TrackParticle_p1 *persObj,
                                        MsgStream                        &log );
   
    private:
        ITPConverterFor<Trk::TrackParticleBase>*  m_trackParticleBaseCnv;
        // the 4 datamembers from P4XYZ are transient I think
};

#endif // TRACKPARTICLE_CNV_P1_REC_H
