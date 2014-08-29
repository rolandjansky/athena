/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonSimEvent/CSCSimHit.h"
#undef private
#undef protected
#include "MuonEventTPCnv/MuonSimEvent/CSCSimHitCollectionCnv_p1.h"
#include "MuonEventTPCnv/MuonSimEvent/CSCSimHitCollection_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

void CSCSimHitCollectionCnv_p1::transToPers(const CSCSimHitCollection* transCont, Muon::CSCSimHitCollection_p1* persCont, MsgStream &log)
{
    // for reasons of efficiency, set size before hand
    unsigned int size=transCont->size();
    persCont->m_CSCid.reserve(size);
    persCont->m_globalTime.reserve(size);
    persCont->m_energyDeposit.reserve(size);
    persCont->m_stX.reserve(size);
    persCont->m_stY.reserve(size);
    persCont->m_stZ.reserve(size);
    persCont->m_enX.reserve(size);
    persCont->m_enY.reserve(size);
    persCont->m_enZ.reserve(size);
    persCont->m_particleID.reserve(size);
    // persCont->m_meanTime.reserve(size);
    persCont->m_partLink.reserve(size);

    // make convertor to handle HepMcParticleLinks
    HepMcParticleLinkCnv_p1 hepMcPLCnv;
    HepMcParticleLink_p1 persLink; // will use this as a temp object inside the loop

  // loop through container, filling pers object  
    CSCSimHitCollection::const_iterator it = transCont->begin(), itEnd = transCont->end();
    for (; it != itEnd; ++it) {
        persCont->m_CSCid.push_back(it->m_CSCid);
        persCont->m_globalTime.push_back(it->m_globalTime);
        persCont->m_energyDeposit.push_back(it->m_energyDeposit);
        persCont->m_stX.push_back(it->m_hitStart.x());
        persCont->m_stY.push_back(it->m_hitStart.y());
        persCont->m_stZ.push_back(it->m_hitStart.z());
        persCont->m_enX.push_back(it->m_hitEnd.x());
        persCont->m_enY.push_back(it->m_hitEnd.y());
        persCont->m_enZ.push_back(it->m_hitEnd.z());
        persCont->m_particleID.push_back(it->m_particleID);
        // persCont->m_meanTime.push_back(hit->m_meanTime);
        
        hepMcPLCnv.transToPers(&(it->m_partLink),&persLink, log);   
        persCont->m_partLink.push_back(persLink);
    }
}


CSCSimHitCollection* CSCSimHitCollectionCnv_p1::createTransient(const Muon::CSCSimHitCollection_p1* persObj, MsgStream &log) {
   std::auto_ptr<CSCSimHitCollection> trans(new CSCSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
   persToTrans(persObj, trans.get(), log);
   return(trans.release());
}


void CSCSimHitCollectionCnv_p1::persToTrans(const Muon::CSCSimHitCollection_p1* persCont, CSCSimHitCollection* transCont, MsgStream &log) 
{
    HepMcParticleLinkCnv_p1 hepMcPLCnv;
    
    //transCont->reserve(persCont->m_CSCid.size());//done in createTransient
  for (unsigned int i = 0; i < persCont->m_CSCid.size(); i++) {
      Amg::Vector3D start(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);
      Amg::Vector3D end(persCont->m_enX[i], persCont->m_enY[i], persCont->m_enZ[i]);
      
      CSCSimHit transHit(persCont->m_CSCid[i], persCont->m_globalTime[i], persCont->m_energyDeposit[i], start, end, persCont->m_particleID[i]);
      // No way to pass through HMCPL in ctor! Seems a bit dumb.
      hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&(transHit.m_partLink), log);   
      transCont->push_back(transHit);
  }
}

