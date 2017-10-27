/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/TRTUncompressedHit.h"
#include "Identifier/Identifier.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

#include "InDetSimEventTPCnv/InDetHits/TRT_Hit_p2.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCnv_p2.h"


void
TRT_HitCnv_p2::persToTrans(const TRT_Hit_p2* persObj, TRTUncompressedHit* transObj,
                           MsgStream &log)
{
  HepMcParticleLinkCnv_p2 HepMcPLCnv;
  HepMcParticleLink link;
  HepMcPLCnv.persToTrans(&(persObj->m_partLink),&link, log);

  *transObj = TRTUncompressedHit (persObj-> hitID,
                                  link.barcode(),
                                  persObj->particleEncoding,
                                  persObj->kineticEnergy,
                                  persObj->energyDeposit,
                                  persObj->preStepX,
                                  persObj->preStepY,
                                  persObj->preStepZ,
                                  persObj->postStepX,
                                  persObj->postStepY,
                                  persObj->postStepZ,
                                  persObj->globalTime);
}


void
TRT_HitCnv_p2::transToPers(const TRTUncompressedHit*, TRT_Hit_p2*, MsgStream &/*log*/)
{
  throw std::runtime_error("TRT_HitCnv_p2::transToPers is not supported in this release!");
}
