/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/RPCSimHit.h"
#include "MuonSimEventTPCnv/RPCSimHitCollectionCnv_p3.h"
#include "MuonSimEventTPCnv/RPCSimHitCollection_p3.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

void RPCSimHitCollectionCnv_p3::transToPers(const RPCSimHitCollection*, Muon::RPCSimHitCollection_p3*, MsgStream &/*log*/)
{
  throw std::runtime_error("RPCSimHitCollectionCnv_p3::transToPers is not supported in this release!");
}


RPCSimHitCollection* RPCSimHitCollectionCnv_p3::createTransient(const Muon::RPCSimHitCollection_p3* persObj, MsgStream &log) {
  std::auto_ptr<RPCSimHitCollection> trans(new RPCSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


void RPCSimHitCollectionCnv_p3::persToTrans(const Muon::RPCSimHitCollection_p3* persCont, RPCSimHitCollection* transCont, MsgStream &log)
{
  HepMcParticleLinkCnv_p2 hepMcPLCnv;

  //transCont->reserve(persCont->m_RPCid.size());//done in createTransient
  for (unsigned int i = 0; i < persCont->m_RPCid.size(); i++) {
    Amg::Vector3D position(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);
    Amg::Vector3D postPosition(persCont->m_stopX[i], persCont->m_stopY[i], persCont->m_stopZ[i]);


    HepMcParticleLink link;
    hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&link, log);

    transCont->Emplace(persCont->m_RPCid[i], persCont->m_globalTime[i], position,
                       link.barcode(), postPosition,
                       persCont->m_energyDeposit[i], persCont->m_stepLength[i],
                       persCont->m_particleEncoding[i], persCont->m_kineticEnergy[i]);
  }
}
