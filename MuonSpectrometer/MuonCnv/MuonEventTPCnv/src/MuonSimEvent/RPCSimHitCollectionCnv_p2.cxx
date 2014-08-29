/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonSimEvent/RPCSimHit.h"
#undef private
#undef protected
#include "MuonEventTPCnv/MuonSimEvent/RPCSimHitCollectionCnv_p2.h"
#include "MuonEventTPCnv/MuonSimEvent/RPCSimHitCollection_p2.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"

void RPCSimHitCollectionCnv_p2::transToPers(const RPCSimHitCollection* transCont, Muon::RPCSimHitCollection_p2* persCont, MsgStream &log)
{
    // for reasons of efficiency, set size before hand
    unsigned int size=transCont->size();
    persCont->m_RPCid.reserve(size);
    persCont->m_globalTime.reserve(size);
    persCont->m_stX.reserve(size);
    persCont->m_stY.reserve(size);
    persCont->m_stZ.reserve(size);
    persCont->m_partLink.reserve(size);
    persCont->m_stopX.reserve(size);
    persCont->m_stopY.reserve(size);
    persCont->m_stopZ.reserve(size);
    persCont->m_energyDeposit.reserve(size);
    persCont->m_stepLength.reserve(size);
    persCont->m_particleEncoding.reserve(size);
    persCont->m_kineticEnergy.reserve(size);

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
	persCont->m_stopX.push_back(it->m_localPostStepPosition.x());
	persCont->m_stopY.push_back(it->m_localPostStepPosition.y());
	persCont->m_stopZ.push_back(it->m_localPostStepPosition.z());
	persCont->m_energyDeposit.push_back(it->m_energyDeposit);
	persCont->m_stepLength.push_back(it->m_stepLength);
	persCont->m_particleEncoding.push_back(it->m_particleEncoding);
	persCont->m_kineticEnergy.push_back(it->m_kineticEnergy);
    }
}


RPCSimHitCollection* RPCSimHitCollectionCnv_p2::createTransient(const Muon::RPCSimHitCollection_p2* persObj, MsgStream &log) {
   std::auto_ptr<RPCSimHitCollection> trans(new RPCSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
   persToTrans(persObj, trans.get(), log);
   return(trans.release());
}


void RPCSimHitCollectionCnv_p2::persToTrans(const Muon::RPCSimHitCollection_p2* persCont, RPCSimHitCollection* transCont, MsgStream &log) 
{
    HepMcParticleLinkCnv_p1 hepMcPLCnv;
    
    //transCont->reserve(persCont->m_RPCid.size());//done in createTransient
  for (unsigned int i = 0; i < persCont->m_RPCid.size(); i++) {
      Amg::Vector3D position(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);
      Amg::Vector3D postPosition(persCont->m_stopX[i], persCont->m_stopY[i], persCont->m_stopZ[i]);
      
      
      RPCSimHit transHit(persCont->m_RPCid[i], persCont->m_globalTime[i], position, 0, postPosition, 
			 persCont->m_energyDeposit[i], persCont->m_stepLength[i], 
			 persCont->m_particleEncoding[i], persCont->m_kineticEnergy[i]);
      // No way to pass through HMCPL in ctor! Seems a bit dumb.
      hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&(transHit.m_partLink), log);   
      transCont->push_back(transHit);
  }
}

