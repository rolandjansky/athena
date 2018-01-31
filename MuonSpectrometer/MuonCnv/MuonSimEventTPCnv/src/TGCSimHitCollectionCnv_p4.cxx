/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/TGCSimHit.h"
#include "MuonSimEventTPCnv/TGCSimHitCollectionCnv_p4.h"
#include "MuonSimEventTPCnv/TGCSimHitCollection_p4.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

void TGCSimHitCollectionCnv_p4::transToPers(const TGCSimHitCollection*, Muon::TGCSimHitCollection_p4*, MsgStream &/*log*/)
{
  throw std::runtime_error("TGCSimHitCollectionCnv_p3::transToPers is not supported in this release!");
}


TGCSimHitCollection* TGCSimHitCollectionCnv_p4::createTransient(const Muon::TGCSimHitCollection_p4* persObj, MsgStream &log) {
  std::auto_ptr<TGCSimHitCollection> trans(new TGCSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


void TGCSimHitCollectionCnv_p4::persToTrans(const Muon::TGCSimHitCollection_p4* persCont, TGCSimHitCollection* transCont, MsgStream &log)
{
  HepMcParticleLinkCnv_p2 hepMcPLCnv;

  //transCont->reserve(persCont->m_TGCid.size()); //done in createTransient
  for (unsigned int i = 0; i < persCont->m_TGCid.size(); i++) {
    Amg::Vector3D position(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);
    Amg::Vector3D localDireCos(persCont->m_enX[i], persCont->m_enY[i], persCont->m_enZ[i]);

    HepMcParticleLink link;
    hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&link, log);

    transCont->Emplace(persCont->m_TGCid[i], persCont->m_globalTime[i], position, localDireCos,
                       link.barcode(),
                       persCont->m_energyDeposit[i], persCont->m_stepLength[i],
                       persCont->m_particleEncoding[i], persCont->m_kineticEnergy[i]);
  }
}
