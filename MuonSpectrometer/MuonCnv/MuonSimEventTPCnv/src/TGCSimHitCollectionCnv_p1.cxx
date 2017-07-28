/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/TGCSimHit.h"
#include "MuonSimEventTPCnv/TGCSimHitCollectionCnv_p1.h"
#include "MuonSimEventTPCnv/TGCSimHitCollection_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

void TGCSimHitCollectionCnv_p1::transToPers(const TGCSimHitCollection* transCont, Muon::TGCSimHitCollection_p1* persCont, MsgStream &log)
{
    // for reasons of efficiency, set size before hand
    unsigned int size=transCont->size();
    persCont->m_TGCid.reserve(size);
    persCont->m_globalTime.reserve(size);
    persCont->m_stX.reserve(size);
    persCont->m_stY.reserve(size);
    persCont->m_stZ.reserve(size);
    persCont->m_enX.reserve(size);
    persCont->m_enY.reserve(size);
    persCont->m_enZ.reserve(size);
    persCont->m_partLink.reserve(size);

    // make convertor to handle HepMcParticleLinks
    HepMcParticleLinkCnv_p1 hepMcPLCnv;
    HepMcParticleLink_p1 persLink; // will use this as a temp object inside the loop

  // loop through container, filling pers object  
    TGCSimHitCollection::const_iterator it = transCont->begin(), itEnd = transCont->end();
    for (; it != itEnd; ++it) {
        persCont->m_TGCid.push_back(it->TGCid());
        persCont->m_globalTime.push_back(it->globalTime());
        persCont->m_stX.push_back(it->localPosition().x());
        persCont->m_stY.push_back(it->localPosition().y());
        persCont->m_stZ.push_back(it->localPosition().z());
        persCont->m_enX.push_back(it->localDireCos().x());
        persCont->m_enY.push_back(it->localDireCos().y());
        persCont->m_enZ.push_back(it->localDireCos().z());
        
        hepMcPLCnv.transToPers(&it->particleLink(),&persLink, log);   
        persCont->m_partLink.push_back(persLink);
    }
}


TGCSimHitCollection* TGCSimHitCollectionCnv_p1::createTransient(const Muon::TGCSimHitCollection_p1* persObj, MsgStream &log) {
   std::auto_ptr<TGCSimHitCollection> trans(new TGCSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
   persToTrans(persObj, trans.get(), log);
   return(trans.release());
}


void TGCSimHitCollectionCnv_p1::persToTrans(const Muon::TGCSimHitCollection_p1* persCont, TGCSimHitCollection* transCont, MsgStream &log) 
{
    HepMcParticleLinkCnv_p1 hepMcPLCnv;
    
    //transCont->reserve(persCont->m_TGCid.size()); //done in createTransient
  for (unsigned int i = 0; i < persCont->m_TGCid.size(); i++) {
      Amg::Vector3D position(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);
      Amg::Vector3D localDireCos(persCont->m_enX[i], persCont->m_enY[i], persCont->m_enZ[i]);

      HepMcParticleLink link;
      hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&link, log);   

      transCont->Emplace(persCont->m_TGCid[i], persCont->m_globalTime[i], position, localDireCos,
                         link, 0, 0);
  }
}

