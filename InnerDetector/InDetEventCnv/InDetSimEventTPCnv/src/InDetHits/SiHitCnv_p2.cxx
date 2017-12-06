/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/SiHit.h"
#include "Identifier/Identifier.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

#include "InDetSimEventTPCnv/InDetHits/SiHit_p2.h"
#include "InDetSimEventTPCnv/InDetHits/SiHitCnv_p2.h"


void
SiHitCnv_p2::persToTrans(const SiHit_p2* persObj, SiHit* transObj, MsgStream &log)
{
  HepMcParticleLinkCnv_p2 HepMcPLCnv;
  HepMcParticleLink link;
  HepMcPLCnv.persToTrans(&(persObj->m_partLink),&link, log);

   *transObj = SiHit (HepGeom::Point3D<double> (persObj->m_stX,
                                                persObj->m_stY,
                                                persObj->m_stZ),
                      HepGeom::Point3D<double> (persObj->m_enX,
                                                persObj->m_enY,
                                                persObj->m_enZ),
                      persObj->m_energyLoss,
                      persObj->m_meanTime,
                      link.barcode(),
                      persObj->m_ID
                      );
}


void
SiHitCnv_p2::transToPers(const SiHit*, SiHit_p2*, MsgStream &/*log*/)
{
  throw std::runtime_error("SiHitCnv_p2::transToPers is not supported in this release!");
}
