/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/GenericMuonSimHit.h"
#include "MuonSimEventTPCnv/GenericMuonSimHitCollectionCnv_p2.h"
#include "MuonSimEventTPCnv/GenericMuonSimHitCollection_p2.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

void GenericMuonSimHitCollectionCnv_p2::transToPers(const GenericMuonSimHitCollection*, Muon::GenericMuonSimHitCollection_p2*, MsgStream &/*log*/)
{
  throw std::runtime_error("GenericMuonSimHitCollectionCnv_p2::transToPers is not supported in this release!");
}


GenericMuonSimHitCollection* GenericMuonSimHitCollectionCnv_p2::createTransient(const Muon::GenericMuonSimHitCollection_p2* persObj, MsgStream &log) {
  std::auto_ptr<GenericMuonSimHitCollection> trans(new GenericMuonSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


void GenericMuonSimHitCollectionCnv_p2::persToTrans(const Muon::GenericMuonSimHitCollection_p2* persCont, GenericMuonSimHitCollection* transCont, MsgStream &log)
{
  HepMcParticleLinkCnv_p2 hepMcPLCnv;

  //transCont->reserve(persCont->m_GenericId.size());//done in createTransient
  for (unsigned int i = 0; i < persCont->m_GenericId.size(); i++) {
    //pre
    Amg::Vector3D preposition(persCont->m_prestX[i], persCont->m_prestY[i], persCont->m_prestZ[i]);
    Amg::Vector3D loc_preposition(persCont->m_prelocX[i], persCont->m_prelocY[i], persCont->m_prelocZ[i]);
    //post
    Amg::Vector3D position(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);
    Amg::Vector3D loc_position(persCont->m_locX[i], persCont->m_locY[i], persCont->m_locZ[i]);
    Amg::Vector3D direction(persCont->m_ptX[i], persCont->m_ptY[i], persCont->m_ptZ[i]);

    HepMcParticleLink link;
    hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&link, log);

    transCont->Emplace(persCont->m_GenericId[i], persCont->m_globalTime[i], persCont->m_globalpreTime[i], position, loc_position, preposition, loc_preposition, persCont->m_particleEncoding[i], persCont->m_kineticEnergy[i], direction, persCont->m_depositEnergy[i], persCont->m_StepLength[i], link.barcode());
  }
}
