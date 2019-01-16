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

void MDTSimHitCollectionCnv_p3::transToPers(const MDTSimHitCollection* transCont, Muon::MDTSimHitCollection_p3* persCont, MsgStream &log)
{
  // for reasons of efficiency, set size before hand
  unsigned int size=transCont->size();
  persCont->m_MDTid.reserve(size);
  persCont->m_globalTime.reserve(size);
  persCont->m_driftRadius.reserve(size);
  persCont->m_stX.reserve(size);
  persCont->m_stY.reserve(size);
  persCont->m_stZ.reserve(size);
  persCont->m_stepLength.reserve(size);
  persCont->m_energyDeposit.reserve(size);
  persCont->m_particleEncoding.reserve(size);
  persCont->m_kineticEnergy.reserve(size);
  // persCont->m_meanTime.reserve(size);
  persCont->m_partLink.reserve(size);

  // make convertor to handle HepMcParticleLinks
  HepMcParticleLinkCnv_p2 hepMcPLCnv;
  HepMcParticleLink_p2 persLink; // will use this as a temp object inside the loop

  // loop through container, filling pers object
  MDTSimHitCollection::const_iterator it = transCont->begin(), itEnd = transCont->end();
  for (; it != itEnd; ++it) {
    persCont->m_MDTid.push_back(it->MDTid());
    persCont->m_globalTime.push_back(it->globalTime());
    persCont->m_driftRadius.push_back(it->driftRadius());
    persCont->m_stX.push_back(it->localPosition().x());
    persCont->m_stY.push_back(it->localPosition().y());
    persCont->m_stZ.push_back(it->localPosition().z());
    // persCont->m_meanTime.push_back(hit->m_meanTime);
    persCont->m_stepLength.push_back(it->stepLength());
    persCont->m_energyDeposit.push_back(it->energyDeposit());
    persCont->m_particleEncoding.push_back(it->particleEncoding());
    persCont->m_kineticEnergy.push_back(it->kineticEnergy());

    hepMcPLCnv.transToPers(&it->particleLink(),&persLink, log);
    persCont->m_partLink.push_back(persLink);

  }
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
                       link,
                       persCont->m_stepLength[i], persCont->m_energyDeposit[i], persCont->m_particleEncoding[i], persCont->m_kineticEnergy[i]);
  }
}

