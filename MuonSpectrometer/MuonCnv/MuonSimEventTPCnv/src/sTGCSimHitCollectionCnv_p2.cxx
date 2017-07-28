/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/sTGCSimHit.h"
#include "MuonSimEventTPCnv/sTGCSimHitCollectionCnv_p2.h"
#include "MuonSimEventTPCnv/sTGCSimHitCollection_p2.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"


void sTGCSimHitCollectionCnv_p2::transToPers(const sTGCSimHitCollection* transCont, Muon::sTGCSimHitCollection_p2* persCont, MsgStream &log)
{
  // for reasons of efficiency, set size before hand
  unsigned int size=transCont->size();
  persCont->m_sTGCId.reserve(size);
  persCont->m_globalTime.reserve(size);
  // postStep Global
  persCont->m_stX.reserve(size);
  persCont->m_stY.reserve(size);
  persCont->m_stZ.reserve(size);
  // postStep Local
  persCont->m_particleEncoding.reserve(size);
  persCont->m_ptX.reserve(size);
  persCont->m_ptY.reserve(size);
  persCont->m_ptZ.reserve(size);
  persCont->m_depositEnergy.reserve(size);
  persCont->m_partLink.reserve(size);

  // make convertor to handle HepMcParticleLinks
  HepMcParticleLinkCnv_p2 hepMcPLCnv;
  HepMcParticleLink_p2 persLink; // will use this as a temp object inside the loop

  // loop through container, filling pers object
  sTGCSimHitCollection::const_iterator it = transCont->begin(), itEnd = transCont->end();
  for (; it != itEnd; ++it) {
    persCont->m_sTGCId.push_back(it->sTGCId());
    persCont->m_globalTime.push_back(it->globalTime());
    // postStep Global
    persCont->m_stX.push_back(it->globalPosition().x());
    persCont->m_stY.push_back(it->globalPosition().y());
    persCont->m_stZ.push_back(it->globalPosition().z());
    // postStep Local
    persCont->m_particleEncoding.push_back(it->particleEncoding());
    persCont->m_ptX.push_back(it->globalDirection().x());
    persCont->m_ptY.push_back(it->globalDirection().y());
    persCont->m_ptZ.push_back(it->globalDirection().z());
    persCont->m_depositEnergy.push_back(it->depositEnergy());

    hepMcPLCnv.transToPers(&it->particleLink(),&persLink, log);
    persCont->m_partLink.push_back(persLink);

  }
}


sTGCSimHitCollection* sTGCSimHitCollectionCnv_p2::createTransient(const Muon::sTGCSimHitCollection_p2* persObj, MsgStream &log) {
  std::auto_ptr<sTGCSimHitCollection> trans(new sTGCSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


void sTGCSimHitCollectionCnv_p2::persToTrans(const Muon::sTGCSimHitCollection_p2* persCont, sTGCSimHitCollection* transCont, MsgStream &log)
{
  HepMcParticleLinkCnv_p2 hepMcPLCnv;

  //transCont->reserve(persCont->m_sTGCId.size());//done in createTransient
  for (unsigned int i = 0; i < persCont->m_sTGCId.size(); i++) {
    //post
    Amg::Vector3D position(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);
    Amg::Vector3D direction(persCont->m_ptX[i], persCont->m_ptY[i], persCont->m_ptZ[i]);

    HepMcParticleLink link;
    hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&link, log);

    transCont->Emplace(persCont->m_sTGCId[i], persCont->m_globalTime[i], position, persCont->m_particleEncoding[i], direction, persCont->m_depositEnergy[i], link);
  }
}
