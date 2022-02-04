/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/sTGCSimHit.h"
#include "MuonSimEventTPCnv/sTGCSimHitCollectionCnv_p3.h"
#include "MuonSimEventTPCnv/sTGCSimHitCollection_p3.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"


void sTGCSimHitCollectionCnv_p3::transToPers(const sTGCSimHitCollection* transCont, Muon::sTGCSimHitCollection_p3* persCont, MsgStream &log)
{
  // for reasons of efficiency, set size before hand
  unsigned int size=transCont->size();
  persCont->m_sTGCId.reserve(size);
  persCont->m_globalTime.reserve(size);
  // postStep Global position
  persCont->m_stX.reserve(size);
  persCont->m_stY.reserve(size);
  persCont->m_stZ.reserve(size);
  persCont->m_particleEncoding.reserve(size);
  persCont->m_depositEnergy.reserve(size);
  persCont->m_partLink.reserve(size);
  persCont->m_kineticEnergy.reserve(size);
  // preStep global position
  persCont->m_prestX.reserve(size);
  persCont->m_prestY.reserve(size);
  persCont->m_prestZ.reserve(size);

  // make convertor to handle HepMcParticleLinks
  HepMcParticleLinkCnv_p2 hepMcPLCnv;
  HepMcParticleLink_p2 persLink; // will use this as a temp object inside the loop

  // loop through container, filling pers object
  sTGCSimHitCollection::const_iterator it = transCont->begin(), itEnd = transCont->end();
  for (; it != itEnd; ++it) {
    persCont->m_sTGCId.push_back(it->sTGCId());
    persCont->m_globalTime.push_back(it->globalTime());
    // postStep Global position
    persCont->m_stX.push_back(it->globalPosition().x());
    persCont->m_stY.push_back(it->globalPosition().y());
    persCont->m_stZ.push_back(it->globalPosition().z());
    persCont->m_particleEncoding.push_back(it->particleEncoding());
    persCont->m_depositEnergy.push_back(it->depositEnergy());
    persCont->m_kineticEnergy.push_back(it->kineticEnergy());
    // preStep global position
    persCont->m_prestX.push_back(it->globalPrePosition().x());
    persCont->m_prestY.push_back(it->globalPrePosition().y());
    persCont->m_prestZ.push_back(it->globalPrePosition().z());

    hepMcPLCnv.transToPers(&it->particleLink(),&persLink, log);
    persCont->m_partLink.push_back(persLink);

  }
}


sTGCSimHitCollection* sTGCSimHitCollectionCnv_p3::createTransient(const Muon::sTGCSimHitCollection_p3* persObj, MsgStream &log) {
  std::unique_ptr<sTGCSimHitCollection> trans(new sTGCSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


void sTGCSimHitCollectionCnv_p3::persToTrans(const Muon::sTGCSimHitCollection_p3* persCont, sTGCSimHitCollection* transCont, MsgStream &log)
{
  HepMcParticleLinkCnv_p2 hepMcPLCnv;

  //transCont->reserve(persCont->m_sTGCId.size());//done in createTransient
  for (unsigned int i = 0; i < persCont->m_sTGCId.size(); i++) {
    //postStep position
    Amg::Vector3D position(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);
    // preStep position
    Amg::Vector3D preposition(persCont->m_prestX[i], persCont->m_prestY[i], persCont->m_prestZ[i]);
    // direction
    Amg::Vector3D direction(persCont->m_stX[i] - persCont->m_prestX[i],
                            persCont->m_stY[i] - persCont->m_prestY[i],
                            persCont->m_stZ[i] - persCont->m_prestZ[i]);
    // Normalize the direction vector if its length is greater than zero
    if (direction.mag() > 0.0001) direction /= direction.mag();

    HepMcParticleLink link;
    hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&link, log);

    transCont->Emplace(persCont->m_sTGCId[i], persCont->m_globalTime[i], position,
                       persCont->m_particleEncoding[i], direction, persCont->m_depositEnergy[i], link,
                       persCont->m_kineticEnergy[i], preposition);
  }
}
