/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParticleCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "Particle/TrackParticle.h"
#include "TrackParticleTPCnv/Particle/TrackParticleCnv_p1.h"
#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

void  TrackParticleCnv_p1::persToTrans(const Rec::TrackParticle_p1 * persObj, Rec::TrackParticle * transObj, MsgStream &log)
{
 fillTransFromPStore( &m_trackParticleBaseCnv, persObj->m_trackParticleBase, transObj, log );
 const Amg::Vector3D& mom = transObj->definingParameters().momentum();
 transObj->setPx (mom[Trk::px]);
 transObj->setPy (mom[Trk::py]);
 transObj->setPz (mom[Trk::pz]);
 double mpi = Trk::ParticleMasses().mass[Trk::pion];
 transObj->setE (std::sqrt ((long double)mom.mag2() + (long double)mpi*mpi));
}

void  TrackParticleCnv_p1::transToPers(const Rec::TrackParticle * transObj, Rec::TrackParticle_p1 * persObj, MsgStream &log)
{
 persObj->m_trackParticleBase = baseToPersistent( &m_trackParticleBaseCnv, transObj, log );
}
