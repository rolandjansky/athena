/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for TruthTrajectory.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007
// Olivier Arnaez <olivier.arnaez@cern.ch>, 2015

#include "TrkTruthTPCnv/TruthTrajectoryCnv_p2.h"
#include "TrkTruthTPCnv/TruthTrajectory_p2.h"
#include "TrkTruthData/TruthTrajectory.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

namespace {
  HepMcParticleLinkCnv_p2 particleLinkConverter;
}

void TruthTrajectoryCnv_p2::persToTrans( const Trk::TruthTrajectory_p2* pers,
                                         TruthTrajectory* trans,
                                         MsgStream& msg )
{
  trans->resize(pers->size());
  for(Trk::TruthTrajectory_p2::size_type i=0; i<trans->size(); i++) {
    particleLinkConverter.persToTrans( &((*pers)[i]), &((*trans)[i]), msg);
  }
}

void TruthTrajectoryCnv_p2::transToPers( const TruthTrajectory*,
                                         Trk::TruthTrajectory_p2*,
                                         MsgStream& /*msg*/ )
{
  throw std::runtime_error("TruthTrajectoryCnv_p2::transToPers is not supported in this release!");
}
