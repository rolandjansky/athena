/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for TruthTrajectory.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007

#include "TrkTruthTPCnv/TruthTrajectoryCnv_p1.h"
#include "TrkTruthTPCnv/TruthTrajectory_p1.h"
#include "TrkTruthData/TruthTrajectory.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

namespace {
  const HepMcParticleLinkCnv_p1 particleLinkConverter;
}

void TruthTrajectoryCnv_p1::persToTrans( const Trk::TruthTrajectory_p1* pers,
					 TruthTrajectory* trans, 
					 MsgStream& msg ) const
{
  trans->resize(pers->size());
  for(Trk::TruthTrajectory_p1::size_type i=0; i<trans->size(); i++) {
    particleLinkConverter.persToTrans( &((*pers)[i]), &((*trans)[i]), msg);
  }
}

void TruthTrajectoryCnv_p1::transToPers( const TruthTrajectory* trans, 
					 Trk::TruthTrajectory_p1* pers, 
					 MsgStream& msg ) const
{
  pers->resize(trans->size());
  for(TruthTrajectory::size_type i=0; i<trans->size(); i++) {
    particleLinkConverter.transToPers( &((*trans)[i]), &((*pers)[i]), msg);
  }
}
