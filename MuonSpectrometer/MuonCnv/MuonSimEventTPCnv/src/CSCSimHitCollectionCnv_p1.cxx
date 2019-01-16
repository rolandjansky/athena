/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/CSCSimHit.h"
#include "MuonSimEventTPCnv/CSCSimHitCollectionCnv_p1.h"
#include "MuonSimEventTPCnv/CSCSimHitCollection_p1.h"
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
        persCont->m_CSCid.push_back(it->CSCid());
        persCont->m_globalTime.push_back(it->globalTime());
        persCont->m_energyDeposit.push_back(it->energyDeposit());
        persCont->m_stX.push_back(it->getHitStart().x());
        persCont->m_stY.push_back(it->getHitStart().y());
        persCont->m_stZ.push_back(it->getHitStart().z());
        persCont->m_enX.push_back(it->getHitEnd().x());
        persCont->m_enY.push_back(it->getHitEnd().y());
        persCont->m_enZ.push_back(it->getHitEnd().z());
        persCont->m_particleID.push_back(it->particleID());
        // persCont->m_meanTime.push_back(hit->m_meanTime);
        
        hepMcPLCnv.transToPers(&(it->particleLink()),&persLink, log);   
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

      HepMcParticleLink link;
      hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&link, log);   

      transCont->Emplace (persCont->m_CSCid[i], persCont->m_globalTime[i], persCont->m_energyDeposit[i], start, end, persCont->m_particleID[i],
                         link,-1.);
  }
}

