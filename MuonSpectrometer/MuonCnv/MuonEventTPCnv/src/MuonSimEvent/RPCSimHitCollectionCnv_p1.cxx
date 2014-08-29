/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonSimEvent/RPCSimHit.h"
#undef private
#undef protected
#include "MuonEventTPCnv/MuonSimEvent/RPCSimHitCollectionCnv_p1.h"
#include "MuonEventTPCnv/MuonSimEvent/RPCSimHitCollection_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

void RPCSimHitCollectionCnv_p1::transToPers(const RPCSimHitCollection* transCont, Muon::RPCSimHitCollection_p1* persCont, MsgStream &log)
{
    log<<MSG::INFO<<"Processing "<<endreq;
    // for reasons of efficiency, set size before hand
    unsigned int size=transCont->size();
    persCont->m_RPCid.reserve(size);
    persCont->m_globalTime.reserve(size);
    persCont->m_stX.reserve(size);
    persCont->m_stY.reserve(size);
    persCont->m_stZ.reserve(size);

    // persCont->m_meanTime.reserve(size);
    persCont->m_partLink.reserve(size);

    // make convertor to handle HepMcParticleLinks
    HepMcParticleLinkCnv_p1 hepMcPLCnv;
    HepMcParticleLink_p1 persLink; // will use this as a temp object inside the loop

  // loop through container, filling pers object  
    RPCSimHitCollection::const_iterator it = transCont->begin(), itEnd = transCont->end();
    for (; it != itEnd; ++it) {
        persCont->m_RPCid.push_back(it->m_RPCid);
        persCont->m_globalTime.push_back(it->m_globalTime);
        persCont->m_stX.push_back(it->m_localPosition.x());
        persCont->m_stY.push_back(it->m_localPosition.y());
        persCont->m_stZ.push_back(it->m_localPosition.z());

        hepMcPLCnv.transToPers(&(it->m_partLink),&persLink, log);   
        persCont->m_partLink.push_back(persLink);
    }
}


RPCSimHitCollection* RPCSimHitCollectionCnv_p1::createTransient(const Muon::RPCSimHitCollection_p1* persObj, MsgStream &log) {
   std::auto_ptr<RPCSimHitCollection> trans(new RPCSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
   persToTrans(persObj, trans.get(), log);
   return(trans.release());
}


void RPCSimHitCollectionCnv_p1::persToTrans(const Muon::RPCSimHitCollection_p1* persCont, RPCSimHitCollection* transCont, MsgStream &log) 
{
    HepMcParticleLinkCnv_p1 hepMcPLCnv;
    
    //transCont->reserve(persCont->m_RPCid.size());//done in createTransient
  for (unsigned int i = 0; i < persCont->m_RPCid.size(); i++) {
      Amg::Vector3D position(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);
      
      RPCSimHit transHit(persCont->m_RPCid[i], persCont->m_globalTime[i], position);
      // No way to pass through HMCPL in ctor! Seems a bit dumb.
      hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&(transHit.m_partLink), log);   
      transCont->push_back(transHit);
  }
}

