/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/MDTSimHit.h"
#include "MuonSimEventTPCnv/MDTSimHitCollectionCnv_p1.h"
#include "MuonSimEventTPCnv/MDTSimHitCollection_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

void MDTSimHitCollectionCnv_p1::transToPers(const MDTSimHitCollection* transCont, Muon::MDTSimHitCollection_p1* persCont, MsgStream &log)
{
    // for reasons of efficiency, set size before hand
    unsigned int size=transCont->size();
    persCont->m_MDTid.reserve(size);
    persCont->m_globalTime.reserve(size);
    persCont->m_driftRadius.reserve(size);
    persCont->m_stX.reserve(size);
    persCont->m_stY.reserve(size);
    persCont->m_stZ.reserve(size);
    // persCont->m_meanTime.reserve(size);
    persCont->m_partLink.reserve(size);

    // make convertor to handle HepMcParticleLinks
    HepMcParticleLinkCnv_p1 hepMcPLCnv;
    HepMcParticleLink_p1 persLink; // will use this as a temp object inside the loop

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
        
        hepMcPLCnv.transToPers(&it->particleLink(),&persLink, log);   
        persCont->m_partLink.push_back(persLink);
    }
}


MDTSimHitCollection* MDTSimHitCollectionCnv_p1::createTransient(const Muon::MDTSimHitCollection_p1* persObj, MsgStream &log) {
   std::auto_ptr<MDTSimHitCollection> trans(new MDTSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
   persToTrans(persObj, trans.get(), log);
   return(trans.release());
}


void MDTSimHitCollectionCnv_p1::persToTrans(const Muon::MDTSimHitCollection_p1* persCont, MDTSimHitCollection* transCont, MsgStream &log) 
{
    HepMcParticleLinkCnv_p1 hepMcPLCnv;
    
    //transCont->reserve(persCont->m_MDTid.size());//done in createTransient
  for (unsigned int i = 0; i < persCont->m_MDTid.size(); i++) {
      Amg::Vector3D position(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);

      HepMcParticleLink link;
      hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&link, log);   

      transCont->Emplace(persCont->m_MDTid[i], persCont->m_globalTime[i], persCont->m_driftRadius[i], position,
                         link,-9999999.,-1.,0,-1.);
  }
}

