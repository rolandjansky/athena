/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/MDTSimHit.h"
#include "MuonSimEventTPCnv/MDTSimHitCollectionCnv_p3.h"
#include "MuonSimEventTPCnv/MDTSimHitCollection_p3.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

void MDTSimHitCollectionCnv_p3::transToPers(const MDTSimHitCollection*, Muon::MDTSimHitCollection_p3*, MsgStream &/*log*/)
{
  throw std::runtime_error("MDTSimHitCollectionCnv_p3::transToPers is not supported in this release!");
}


MDTSimHitCollection* MDTSimHitCollectionCnv_p3::createTransient(const Muon::MDTSimHitCollection_p3* persObj, MsgStream &log) {
  std::auto_ptr<MDTSimHitCollection> trans(new MDTSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


void MDTSimHitCollectionCnv_p3::persToTrans(const Muon::MDTSimHitCollection_p3* persCont, MDTSimHitCollection* transCont, MsgStream &log)
{
  HepMcParticleLinkCnv_p2 hepMcPLCnv;

  //transCont->reserve(persCont->m_MDTid.size());//done in createTransient
  for (unsigned int i = 0; i < persCont->m_MDTid.size(); i++) {
    Amg::Vector3D position(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);

    HepMcParticleLink link;
    hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&link, log);

    transCont->Emplace(persCont->m_MDTid[i], persCont->m_globalTime[i], persCont->m_driftRadius[i], position,
                       link.barcode(),
                       persCont->m_stepLength[i], persCont->m_energyDeposit[i], persCont->m_particleEncoding[i], persCont->m_kineticEnergy[i]);
  }
}
