/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/GenericMuonSimHit.h"
#include "MuonSimEventTPCnv/GenericMuonSimHitCollectionCnv_p1.h"
#include "MuonSimEventTPCnv/GenericMuonSimHitCollection_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

void GenericMuonSimHitCollectionCnv_p1::transToPers(const GenericMuonSimHitCollection* transCont, Muon::GenericMuonSimHitCollection_p1* persCont, MsgStream &log)
{
  // for reasons of efficiency, set size before hand
  unsigned int size=transCont->size();
  persCont->m_GenericId.reserve(size);
  persCont->m_globalTime.reserve(size);
  persCont->m_globalpreTime.reserve(size);
  // preStep Global
  persCont->m_prestX.reserve(size);
  persCont->m_prestY.reserve(size);
  persCont->m_prestZ.reserve(size);
  // preStep Local
  persCont->m_prelocX.reserve(size);
  persCont->m_prelocY.reserve(size);
  persCont->m_prelocZ.reserve(size);
  // postStep Global
  persCont->m_stX.reserve(size);
  persCont->m_stY.reserve(size);
  persCont->m_stZ.reserve(size);
  // postStep Local
  persCont->m_locX.reserve(size);
  persCont->m_locY.reserve(size);
  persCont->m_locZ.reserve(size);
  persCont->m_kineticEnergy.reserve(size);
  persCont->m_particleEncoding.reserve(size);
  persCont->m_ptX.reserve(size);
  persCont->m_ptY.reserve(size);
  persCont->m_ptZ.reserve(size);
  persCont->m_depositEnergy.reserve(size);
  persCont->m_StepLength.reserve(size);	
  persCont->m_partLink.reserve(size);

  // make convertor to handle HepMcParticleLinks
  HepMcParticleLinkCnv_p1 hepMcPLCnv;
  HepMcParticleLink_p1 persLink; // will use this as a temp object inside the loop

  // loop through container, filling pers object  
  GenericMuonSimHitCollection::const_iterator it = transCont->begin(), itEnd = transCont->end();
  for (; it != itEnd; ++it) {
    persCont->m_GenericId.push_back(it->GenericId());
    persCont->m_globalTime.push_back(it->globalTime());
    persCont->m_globalpreTime.push_back(it->globalpreTime());
    // preStep Global
    persCont->m_prestX.push_back(it->globalPrePosition().x());
    persCont->m_prestY.push_back(it->globalPrePosition().y());
    persCont->m_prestZ.push_back(it->globalPrePosition().z());
    // preStep Local
    persCont->m_prelocX.push_back(it->localPrePosition().x());
    persCont->m_prelocY.push_back(it->localPrePosition().y());
    persCont->m_prelocZ.push_back(it->localPrePosition().z());
    // postStep Global
    persCont->m_stX.push_back(it->globalPosition().x());
    persCont->m_stY.push_back(it->globalPosition().y());
    persCont->m_stZ.push_back(it->globalPosition().z());
    // postStep Local
    persCont->m_locX.push_back(it->localPosition().x());
    persCont->m_locY.push_back(it->localPosition().y());
    persCont->m_locZ.push_back(it->localPosition().z());
    persCont->m_kineticEnergy.push_back(it->kineticEnergy());
    persCont->m_particleEncoding.push_back(it->particleEncoding()); 
    persCont->m_ptX.push_back(it->globalDirection().x());
    persCont->m_ptY.push_back(it->globalDirection().y());
    persCont->m_ptZ.push_back(it->globalDirection().z());
    persCont->m_depositEnergy.push_back(it->depositEnergy());
    persCont->m_StepLength.push_back(it->StepLength());
		
    hepMcPLCnv.transToPers(&it->particleLink(),&persLink, log);   
    persCont->m_partLink.push_back(persLink);

  }
}


GenericMuonSimHitCollection* GenericMuonSimHitCollectionCnv_p1::createTransient(const Muon::GenericMuonSimHitCollection_p1* persObj, MsgStream &log) {
  std::auto_ptr<GenericMuonSimHitCollection> trans(new GenericMuonSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


void GenericMuonSimHitCollectionCnv_p1::persToTrans(const Muon::GenericMuonSimHitCollection_p1* persCont, GenericMuonSimHitCollection* transCont, MsgStream &log) 
{
  HepMcParticleLinkCnv_p1 hepMcPLCnv;
    
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

    transCont->Emplace(persCont->m_GenericId[i], persCont->m_globalTime[i], persCont->m_globalpreTime[i], position, loc_position, preposition, loc_preposition, persCont->m_particleEncoding[i], persCont->m_kineticEnergy[i], direction, persCont->m_depositEnergy[i], persCont->m_StepLength[i], link);
  }
}

