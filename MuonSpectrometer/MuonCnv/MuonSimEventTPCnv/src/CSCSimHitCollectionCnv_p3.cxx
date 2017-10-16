/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/CSCSimHit.h"
#include "MuonSimEventTPCnv/CSCSimHitCollectionCnv_p3.h"
#include "MuonSimEventTPCnv/CSCSimHitCollection_p3.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

void CSCSimHitCollectionCnv_p3::transToPers(const CSCSimHitCollection*, Muon::CSCSimHitCollection_p3*, MsgStream &/*log*/)
{
  throw std::runtime_error("CSCSimHitCollectionCnv_p3::transToPers is not supported in this release!");
}


CSCSimHitCollection* CSCSimHitCollectionCnv_p3::createTransient(const Muon::CSCSimHitCollection_p3* persObj, MsgStream &log) {
  std::auto_ptr<CSCSimHitCollection> trans(new CSCSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


void CSCSimHitCollectionCnv_p3::persToTrans(const Muon::CSCSimHitCollection_p3* persCont, CSCSimHitCollection* transCont, MsgStream &log)
{
  HepMcParticleLinkCnv_p2 hepMcPLCnv;

  //transCont->reserve(persCont->m_CSCid.size());//done in createTransient
  for (unsigned int i = 0; i < persCont->m_CSCid.size(); i++) {
    Amg::Vector3D start(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);
    Amg::Vector3D end(persCont->m_enX[i], persCont->m_enY[i], persCont->m_enZ[i]);

    HepMcParticleLink link;
    hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&link, log);

    transCont->Emplace (persCont->m_CSCid[i], persCont->m_globalTime[i], persCont->m_energyDeposit[i], start, end, persCont->m_particleID[i],
                        link.barcode(), persCont->m_kineticEnergy[i]);
  }
}
